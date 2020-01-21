// Libs
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// VM
#include "njvm.h"
#include "macro.h"
#include "constants.h"
#include "instructions.h"
#include "bigint/src/bigint.h"
#include "bigint/src/support.h"
//#include "debugger/debugger.h"

#define MEMORY_SLOT_SIZE 1024

// TODO
// Prog Count Umschreiben
// Holy shit bug, bei Aufruf von Prog + Ausgabe 11111



ObjRef BIG_NULL;
ObjRef BIG_ONE;


StackSlot *stack;
ObjRef *static_variables;
ObjRef return_value;

int no_of_static_variables;
int no_of_instructions;

unsigned int* memory;

unsigned char* heap_start;
unsigned char* heap_start_start;
unsigned char* heap_limit;
unsigned char* heap_max;
unsigned int stack_size = 64;
unsigned int heap_size = 8192;
unsigned char* heap_pointer;
unsigned int stack_pointer;
unsigned int frame_pointer;

unsigned int debug_flag = 0;

/*Program Memory counter*/
unsigned int program_counter;

/*
 *
 */
const char *instructions[]={
        "halt",
        "pushc",
        "add",
        "sub",
        "mul",
        "div",
        "mod",
        "rdint",
        "wrint",
        "rdchr",
        "wrchr",

        "pushg",
        "popg",
        "asf",
        "rsf",
        "pushl",
        "popl",

        "eq",
        "ne",
        "lt",
        "le",
        "gt",
        "ge",
        "jmp",
        "brf",
        "brt",

        "call",
        "ret",
        "drop",
        "pushr",
        "popr",
        "du",

        "new",
        "getf",
        "putf",
        "newa",
        "getfa",
        "putfa",
        "getsz",
        "pushn",
        "refeq",
        "refne"
};


/*
 * Typedef for instruction pointer
 * */
typedef int (*instructionPtr)(int);

/*
 * Instruction Pointer Array
 */
instructionPtr opcode_instruction_pointer[] = {
        // Aufgabe 1
        halt,
        pushc,
        add,
        sub,
        mul,
        divi,
        mod,
        rdint,
        wrint,
        rdchr,
        wrchr,

        // Aufgabe 2
        pushg,
        popg,
        asf,
        rsf,
        pushl,
        popl,

        // Aufgabe 3
        eq,
        ne,
        lt,
        le,
        gt,
        ge,
        jmp,
        brf,
        brt,

        // Aufgabe 4
        call,
        ret,
        drop,
        pushr,
        popr,
        dup,

        //Aufgabe 7
        new,
        getf,
        putf,
        newa,
        getfa,
        putfa,
        getsz,
        pushn,
        refeq,
        refne
};



/* calls for argv check and program run function */
int main(int argc, char *argv[]){
    catch_param(argc, argv);
    return 0;
}


/* catches argv parameters
 * @param[]: string Parameter
 * if wrong param -> Error message to exit
 */
void catch_param(int param_count, char *params[]){
    for(int i=1; i < param_count; i++) {
        if (EQSTRING(params[i], "--version")) {
            printf(VERSION);
        } else if (EQSTRING(params[i], "--help")) {
            printf(HELP);
        } else if (EQSTRING(params[i], "--stack")) {
            stack_size = atoi(params[++i]);
        } else if (EQSTRING(params[i], "--heap")) {
            heap_size = atoi(params[++i]);
        } else if (EQSTRING(params[i], "--gcpurge")) {
            //TODO
        } else if (EQSTRING(params[i], "--gcstats")) {
            //TODO
        } else if (EQSTRING(params[i], "--debug")) {
            debug_flag = 1;
        } else if (*params[0] == '-' && *params[1] == '-') {
            printf("unknown command line argument '%s', try '%s --help'\n", params[i], __FILE__);
            exit(1);
        } else {
            if (debug_flag == 1) {
                //debug(param);
            } else {
                run(params[i]);
            }
        }
    }
}


/*
 * Main Program flow function
 * @program_file_path: path to binary
 * */
void run(char* program_file_path){
    printf("%s\n", START);
    stack_pointer = 0;
    program_counter = 0;
    allocate_memory_for_stack();
    allocate_memory_for_heap();
    bigFromInt(0);
    BIG_NULL = bip.res;
    bigFromInt(1);
    BIG_ONE = bip.res;
    load_program_to_memory(program_file_path);

    /*
    ObjRef o = newPrimObject(6);
    o->size = BREAK_MY_HEART(o);
    //o->size = SET_FORWARDPOINTER(o, 13);
    printf("%d\n", HEART_IS_BROKEN(o));
    getchar();
     */

    while(program_counter < no_of_instructions){
        unsigned int memory_on_prog_count = memory[program_counter];
        program_counter ++;
        opcode_instruction_pointer[OPCODE(memory_on_prog_count)](SIGN_EXTEND(IMMEDIATE(memory_on_prog_count)));
    }
}


ObjRef newPrimObject(int numBytes){
    ObjRef primObject = heap_alloc(sizeof(unsigned int) + numBytes);
    primObject->size = (unsigned int) numBytes;
    return primObject;
}

ObjRef newCompoundObject(int numObjRefs){
    ObjRef compObject;
    compObject = heap_alloc(sizeof(unsigned int) + (numObjRefs * sizeof(ObjRef)));
    compObject->size = (MSB | numObjRefs);
    for(int i = 0; i < numObjRefs; i++){
        GET_REFS(compObject)[i] = NULL;
    }
    return compObject;
}


void fatalError(char *msg){
    printf("Error: %s\n", msg);
    exception("BigInt fatal error: ", __func__ , __LINE__);
}

/*
 * exception handling
 * */
void exception(char* message, const char* func, int line){
    fprintf (stdout, ANSI_COLOR_RED "%s: \n" ANSI_COLOR_RESET, message);
    fprintf (stdout, ANSI_COLOR_RED "FUNCTION: \t%s\n" ANSI_COLOR_RESET, func);
    fprintf (stdout, ANSI_COLOR_RED "LINE: \t\t%d\n" ANSI_COLOR_RESET, line);
    fprintf (stdout, ANSI_COLOR_RED "PROGRAM COUNTER: \t\t%d\n" ANSI_COLOR_RESET, program_counter);
    exit(1);
}

ObjRef pop_Stack_Object(void){
    stack_pointer--;
    if(stack_pointer < 0){
        exception("Stackunderflow Exception at function: ", __func__, __LINE__);
    } else if (!stack[stack_pointer].isObjectReference){
        exception("Type Mismatch ->\nreference: Object\nactual: Number", __func__, __LINE__);
    }
    return stack[stack_pointer].u.objRef;
}

void push_Stack_Object(ObjRef objRef){
    if(stack_pointer >= stack_size-1){
        exception("Stackoverflow Exception", __func__, __LINE__);
    }
    stack[stack_pointer].isObjectReference = true;
    stack[stack_pointer].u.objRef = objRef;
    stack_pointer++;
}

int pop_Stack_Number(void){
    stack_pointer--;
    if(stack_pointer < 0){
        exception("Stackunderflow Exception: ", __func__, __LINE__);
    } else if (stack[stack_pointer].isObjectReference){
        exception("Type Mismatch ->\nreference: Number\nactual: Object", __func__, __LINE__);
    }
    return stack[stack_pointer].u.number;
}

void push_Stack_Number(int immediate){
    if(stack_pointer >= stack_size-1){
        exception("Stackoverflow Exception: ", __func__, __LINE__);
    }
    stack[stack_pointer].isObjectReference = false;
    stack[stack_pointer].u.number = immediate;
    stack_pointer++;
}


void pop_Global(int immediate){
    if(immediate < 0){
        exception("Global Stackunderflow Exception", __func__, __LINE__);
    }
    ObjRef var = pop_Stack_Object();
    static_variables[immediate] = var;
}


void push_Global(int immediate){
    if(immediate >= no_of_static_variables){
        exception("Global Stackoverflow Exception", __func__, __LINE__);
    }
    ObjRef memory_Adress = static_variables[immediate];
    push_Stack_Object(memory_Adress);
}


void pop_local(int memory_Adress) {
    if (memory_Adress <= 0) {
        exception("Stackunderflow Exception", __func__, __LINE__);
    }
    ObjRef var = pop_Stack_Object();
    stack[memory_Adress].isObjectReference = true;
    stack[memory_Adress].u.objRef = var;
}

void push_local(int memory_Adress){
    if(memory_Adress == stack_size-1){
        exception("Stackoverflow Exception", __func__, __LINE__);
    }
    stack[memory_Adress].isObjectReference = true;
    ObjRef val = stack[memory_Adress].u.objRef;
    push_Stack_Object(val);
}

int halt (int immediate){
    printf("%s\n", STOP);
    exit(1);
}

int pushc (int immediate){
    bigFromInt(immediate);
    push_Stack_Object(bip.res);
    //program_counter++;
    return 0;
}

int add (int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    bigAdd();
    push_Stack_Object(bip.res);
    //program_counter++;
    return 0;
}

int sub (int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    bigSub();
    push_Stack_Object(bip.res);
    //program_counter++;
    return 0;
}

int mul (int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    bigMul();
    push_Stack_Object(bip.res);
    //program_counter++;
    return 0;
}

int divi (int immediate){
    bip.op2 = pop_Stack_Object();

    // TODO exceptions rausnehmen -> durch bigint lib integriert
    /*
    if (bigToInt() == 0){
        exception("Divide by Zero Exception", __func__, __LINE__);
    }
     */
    bip.op1 = pop_Stack_Object();
    bigDiv();
    push_Stack_Object(bip.res);
    //program_counter++;
    return 0;
}

// TODO EXC
int mod (int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    bigDiv();
    push_Stack_Object(bip.rem);
    //program_counter++;
    return 0;
}

int rdint (int immediate){
    bigRead(stdin);
    push_Stack_Object(bip.res);
    //program_counter++;
    return 0;
}

int wrint (int immediate){
    bip.op1 = pop_Stack_Object();
    bigPrint(stdout);
    //program_counter++;
    return 0;
}

int rdchr (int immediate){
    char character;
    scanf("%c", &character);
    bigFromInt((unsigned int) character);
    push_Stack_Object(bip.res);
    //program_counter++;
    return 0;
}


int wrchr (int immediate){
    bip.op1 = pop_Stack_Object();
    printf("%c", (char) bigToInt());
    //program_counter++;
    return 0;
}

int pushg(int immediate){
    push_Global(immediate);
    //program_counter++;
    return 0;
}

int popg(int immediate){
    pop_Global(immediate);
    //program_counter++;
    return 0;
}

int pushl(int immediate) {
    push_local(frame_pointer+immediate);
    //program_counter++;
    return 0;
}

int popl(int immediate){
    pop_local(frame_pointer+immediate);
    //program_counter++;
    return 0;
}

int asf(int immediate){
    if((stack_pointer + immediate) > stack_size-1){
        exception("Assamble Stackframe Stackoverflow Exception", __func__, __LINE__);
    }
    push_Stack_Number(frame_pointer);
    frame_pointer = stack_pointer;
    for(int i = 0; i < immediate; i++){
        stack[stack_pointer + i].isObjectReference = true;
        stack[stack_pointer + i].u.objRef = NULL;
    }
    stack_pointer += immediate;
    //program_counter++;
    return 0;
}

int rsf(int immediate){
    stack_pointer = frame_pointer;
    frame_pointer = pop_Stack_Number();
    //program_counter++;
    return 0;
}

int eq(int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    if(bigCmp() == 0){
        push_Stack_Object(BIG_ONE);
        //program_counter++;
        return 1;
    }
    //program_counter++;
    push_Stack_Object(BIG_NULL);
    return 0;
}


int ne(int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    if(bigCmp() != 0){
        //program_counter++;
        push_Stack_Object(BIG_ONE);
        return 1;
    }
    //program_counter++;
    push_Stack_Object(BIG_NULL);
    return 0;
}

int lt(int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    if(bigCmp() < 0){
        //program_counter++;
        push_Stack_Object(BIG_ONE);
        return 1;
    }
    //program_counter++;
    push_Stack_Object(BIG_NULL);
    return 0;
}

int le(int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    if(bigCmp() <= 0){
        //program_counter++;
        push_Stack_Object(BIG_ONE);
        return 1;
    }
    //program_counter++;
    push_Stack_Object(BIG_NULL);
    return 0;
}

int gt(int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    if(bigCmp() > 0){
        //program_counter++;
        push_Stack_Object(BIG_ONE);
        return 1;
    }
    //program_counter++;
    push_Stack_Object(BIG_NULL);
    return 0;
}

int ge(int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    if(bigCmp() >= 0){
        //program_counter++;
        push_Stack_Object(BIG_ONE);
        return 1;
    }
    //program_counter++;
    push_Stack_Object(BIG_NULL);
    return 0;
}

int jmp(int immediate){
    if(immediate < 0 || immediate > no_of_instructions){
        exception("Jump address is invalid", __func__, __LINE__);
    }
    program_counter = immediate;
    return 0;
}

int brf(int immediate){
    ObjRef eval = pop_Stack_Object();
    bip.op2 = eval;
    bip.op1 = BIG_NULL;
    if(bigCmp() == 0){
        jmp(immediate);
        return 0;
    }
    //program_counter++;
    return 0;
}

int brt(int immediate){
    ObjRef eval = pop_Stack_Object();
    bip.op2 = eval;
    bip.op1 = BIG_NULL;
    if(bigCmp() != 0){
        jmp(immediate);
        return 0;
    }
    //program_counter++;
    return 0;
}

int call(int immediate){
    push_Stack_Number(program_counter);
    jmp(immediate);
    return 0;
}

int ret(int immediate){
    unsigned int return_adress = pop_Stack_Number();
    program_counter = return_adress;
    return 0;
}

int drop(int immediate){
    for(int i = 0; i < immediate; i++){
        pop_Stack_Object();
    }
    //program_counter++;
    return 0;
}

int pushr(int immediate){
    push_Stack_Object(return_value);
    //program_counter++;
    return 0;
}

int popr(int immediate){
    return_value = pop_Stack_Object();
    //program_counter++;
    return 0;
}

int dup(int immediate){
    ObjRef objRef = pop_Stack_Object();
    push_Stack_Object(objRef);
    push_Stack_Object(objRef);
    //program_counter++;
    return 0;
}

int new(int immediate){
    if(immediate < 0){
        exception("compount object size exception: ", __func__, __LINE__);
    } else {
        push_Stack_Object(newCompoundObject(immediate));
    }
    //program_counter++;
    return 0;
}


int getf(int immediate){
    bip.op2 = pop_Stack_Object();
    if(GET_SIZE(bip.op2)-1 < immediate){
        exception("index out of bounds exception: ", __func__, __LINE__);
    } else if (immediate < 0){
        exception("index less than 0 exception: ", __func__, __LINE__);
    }
    push_Stack_Object(GET_REFS(bip.op2)[immediate]);
    //program_counter++;
    return 0;
}


int putf(int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    if(GET_SIZE(bip.op1)-1 < immediate){
        exception("index out of bounds exception: ", __func__, __LINE__);
    } else if (immediate < 0){
        exception("index less than 0 exception: ", __func__, __LINE__);
    }
    GET_REFS(bip.op1)[immediate] = bip.op2;
    //program_counter++;
    return 0;
}


int newa(int immediate){
    bip.op1 = pop_Stack_Object();
    if(!IS_PRIM(bip.op1)){
        exception("Type Mismatch ->\nreference: primitive object\nactual: compound object", __func__, __LINE__);
    } else {
        push_Stack_Object(newCompoundObject(bigToInt()));
    }
    //program_counter++;
    return 0;
}


int getfa(int immediate){

    // index
    bip.op1 = pop_Stack_Object();

    // target object
    bip.op2 = pop_Stack_Object();

    if(IS_PRIM(bip.op2)){
        exception("Type Mismatch ->\nreference: primitive object\nactual: compound object", __func__, __LINE__);
    } else {
        push_Stack_Object(GET_REFS(bip.op2)[bigToInt()]);
    }
    //program_counter++;
    return 0;
}


int putfa(int immediate){
    // object to be placed at given index
    bip.op2 = pop_Stack_Object();

    // index
    bip.op1 = pop_Stack_Object();

    // target
    bip.rem = pop_Stack_Object();

    if(IS_PRIM(bip.rem)){
        exception("Type Mismatch ->\nreference: primitive object\nactual: compound object", __func__, __LINE__);
    } else {
        GET_REFS(bip.rem)[bigToInt()] = bip.op2;
    }
    //program_counter++;
    return 0;
}


int getsz(int immediate){
    bip.op1 = pop_Stack_Object();
    if(IS_PRIM(bip.op1)){
        bigFromInt(-1);
        push_Stack_Object(bip.res);
    } else {
        bigFromInt(GET_SIZE(bip.op1));
        push_Stack_Object(bip.res);
    }
    //program_counter++;
    return 0;
}


int pushn(int immediate){
    push_Stack_Object(NULL);
    //program_counter++;
    return 0;
}


int refeq(int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    if(GET_REFS(bip.op2) == GET_REFS(bip.op1)){
        push_Stack_Object(BIG_ONE);
    } else {
        push_Stack_Object(BIG_NULL);
    }
    //program_counter++;
    return 0;
}


int refne(int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    if(GET_REFS(bip.op2) != GET_REFS(bip.op1)){
        push_Stack_Object(BIG_ONE);
    } else {
        push_Stack_Object(BIG_NULL);
    }
    //program_counter++;
    return 0;
}


void check_file_format(FILE *fp){
    char head[4];
    if(fread(&head[0], sizeof(char), 4, fp) != 4){
        exception("Could not read number of requested bytes", __func__, __LINE__);
    }
    if(strncmp(head, "NJBF", 4) != 0){
        exception("No valid ninja binary file", __func__, __LINE__);
    }
}

void check_file_version_no(FILE *fp){
    int version_no;
    if(fread(&version_no, sizeof(int), 1, fp) != 1){
        exception("Could not read number of requested bytes", __func__, __LINE__);
    }
    if(version_no != VERSION_NO){
        exception("Source file version number is incorrect", __func__, __LINE__);
    }
}

void allocate_memory_for_instructions(FILE *fp){
    if(fread(&no_of_instructions, sizeof(int), 1, fp) != 1){
        exception("Could not read number of requested bytes", __func__, __LINE__);
    }
    memory = malloc(no_of_instructions * sizeof(unsigned int));
    if(memory == NULL){
        exception("Memory allocation failed", __func__, __LINE__);
    }
}


void allocate_memory_for_static_variables(FILE *fp){
    if(fread(&no_of_static_variables, sizeof(int), 1, fp) != 1){
        exception("could not read number of requested bytes", __func__, __LINE__);
    }
    static_variables = malloc(no_of_static_variables * sizeof(ObjRef));
    if(static_variables == NULL){
        exception("Memory allocation failed", __func__, __LINE__);
    }
}


void allocate_memory_for_stack(void){
    stack = calloc(MEMORY_SLOT_SIZE, stack_size);
    if(stack == NULL){
        exception("Stack memory allocation failed", __func__, __LINE__);
    }
}

// funktioniert super
void allocate_memory_for_heap(void){
    heap_start_start = calloc(MEMORY_SLOT_SIZE, heap_size);
    if(heap_start_start == NULL){
        exception("Heap memory allocation failed", __func__, __LINE__);
    }
    heap_start = heap_start_start;
    heap_pointer = heap_start_start;
    heap_limit = &heap_start_start[heap_size*MEMORY_SLOT_SIZE / 2];
    heap_max = &heap_start_start[heap_size*MEMORY_SLOT_SIZE-1];
}

// 38592 43712

ObjRef heap_alloc(unsigned int size){
    ObjRef heap_address_for_object = (ObjRef)heap_pointer;
    if((heap_pointer+size) > heap_limit){
        garbage_collector();

        if((heap_pointer+size) > heap_limit){
            exception("Out of memory exception: ", __func__, __LINE__);
        }

        heap_address_for_object = (ObjRef)heap_pointer;
    }
    // TODO check this please
    heap_pointer += size;
    return (ObjRef)heap_address_for_object;
}


void read_instructions_into_memory(FILE *fp){
    if(fread(memory, sizeof(int), no_of_instructions, fp) != no_of_instructions){
        exception("Could not read number of requested bytes", __func__, __LINE__);
    }
}

/*
 * loading binary into memory
 * @program_file_path: path to binary
 * TO-DO: make single "check_file_header"-function!
 * */
void load_program_to_memory(char* program_file_path){
    FILE *fp;
    fp = fopen(program_file_path, "r");
    if(fp == NULL){
        exception("Error opening file", __func__, __LINE__);
    }
    check_file_format(fp);
    check_file_version_no(fp);
    allocate_memory_for_instructions(fp);
    allocate_memory_for_static_variables(fp);
    read_instructions_into_memory(fp);
    fclose(fp);
}


/* Prints assambler instructions
 * @count: number of assambler instructions
 * @memory: array of assambler instructions
 */
void print_assambler_instructions(void){
    char buf[30];
    for(int i = 0; i < no_of_instructions; i++){
        int temp = SIGN_EXTEND(IMMEDIATE(memory[i]));
        sprintf(buf, "%d", temp);
        printf("%d\t%s\t%s\n", i, instructions[OPCODE(memory[i])], buf);
    }
}


void print_Stack(void){
    int i=0;
    while(i < stack_pointer){
        if(stack[i].isObjectReference){
            printf("Obj -> stack[%d]: ", i);
            bip.op1 = stack[i].u.objRef;
            bigPrint(stdout);
            printf("\n");
        } else {
            printf("Num -> stack[%d]: %d\n", i, stack[i].u.number);
        }
        i++;
    }
    printf("\nsp: %d\nfp: %d\n\n", stack_pointer, frame_pointer);
}


void garbage_collector(void){
    printf(ANSI_COLOR_RED"\nSTARTING GARBAGE COLLECTION\n"ANSI_COLOR_RESET);
    static int isRunning = 0;

    if (isRunning){
        exception("recursive garbage collection", __func__, __LINE__);
    }

    isRunning = 1;

    flip();

    for(int i=0; i < stack_pointer; i++){
        if(stack[i].isObjectReference){
            stack[i].u.objRef = relocate(stack[i].u.objRef);
        }
    }


    for(int i = 0; i < no_of_static_variables; i++){
        static_variables[i] = relocate(static_variables[i]);
    }

    return_value = relocate(return_value);
    BIG_NULL = relocate(BIG_NULL);
    BIG_ONE = relocate(BIG_ONE);
    bip.op1 = relocate(bip.op1);
    bip.op2 = relocate(bip.op2);
    bip.res = relocate(bip.res);
    bip.rem = relocate(bip.rem);


    unsigned char* scan = heap_start;
    ObjRef object;
    while(scan != heap_pointer){
        object = (ObjRef)scan;
        if (!IS_PRIM(object)){
            for(int i = 0; i < GET_SIZE(object); i++){
                GET_REFS(object)[i] = relocate(GET_REFS(object)[i]);
            }
            scan += (GET_SIZE(object) * sizeof(ObjRef)) + sizeof(unsigned int);
        } else {
            scan += GET_SIZE(object) + sizeof(unsigned int);
        }
    }
    isRunning = 0;
}

void flip(void){
    // unsigned char* heap_temp = heap_start;

    if(heap_limit == heap_max){
        heap_limit = heap_start;
        heap_start = heap_start_start;
    } else {
        heap_start = heap_limit;
        heap_limit = heap_max;
    }
    heap_pointer = heap_start;

}


ObjRef relocate(ObjRef orig){
    ObjRef copy = NULL;
    if(orig == NULL){
        copy = NULL;
    } else if (HEART_IS_BROKEN(orig)){
        copy = (ObjRef)(heap_start + GET_FORWARDPOINTER(orig));
    } else {
        copy = copy_object(orig);
    }
    return copy;
}


ObjRef copy_object(ObjRef orig){
    unsigned char*  temp_address;
    unsigned int size;


    if(IS_PRIM(orig)){
        size =(sizeof(unsigned int)+GET_SIZE(orig));
    } else {
        size = (sizeof(unsigned int)+(GET_SIZE(orig) * sizeof(ObjRef)));
    }
    temp_address = (unsigned char*)heap_alloc(size);
    int offset = (unsigned int)(temp_address - heap_start);
    memcpy(temp_address, orig, size);
    orig->size = BREAK_MY_HEART(offset);

    return (ObjRef)temp_address;
}


void print_heap(ObjRef obj){
    printf("obj size: %d\n", GET_SIZE(obj));
}
