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

#define STACK_SIZE 64
#define MEMORY_SLOT_SIZE 1024


ObjRef BIG_NULL;
ObjRef BIG_ONE;



StackSlot *stack;
StackSlot *static_variables;
ObjRef return_value;

int no_of_static_variables;
int no_of_instructions;

unsigned int* memory;
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
    for(int i=1; i < argc; i++){
        catch_param(argv[i]);
    }
    return 0;
}


/* catches argv parameters
 * @param[]: string Parameter
 * if wrong param -> Error message to exit
 */
void catch_param(char* param){
    if(EQSTRING(param, "--version")){
        printf(VERSION);
    }else if(EQSTRING(param, "--help")) {
        printf(HELP);
    }else if(EQSTRING(param, "--debug")) {
        debug_flag = 1;
    }else if(param[0] == '-' && param[1] == '-'){
        printf("unknown command line argument '%s', try '%s --help'\n", param, __FILE__);
        exit(1);
    }else{
        if(debug_flag == 1){
            //debug(param);
        }else{
            run(param);
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
    bigFromInt(0);
    BIG_NULL = bip.res;
    bigFromInt(1);
    BIG_ONE = bip.res;
    load_program_to_memory(program_file_path);

    /*
    print_assambler_instructions();
    getchar();
     */

    while(program_counter < no_of_instructions){
        opcode_instruction_pointer[OPCODE(memory[program_counter])](SIGN_EXTEND(IMMEDIATE(memory[program_counter])));
    }
}



ObjRef newPrimObject(int numBytes){
    ObjRef primObject = malloc(sizeof(unsigned int) + numBytes);
    primObject->size = (unsigned int) numBytes;
    return primObject;
}

ObjRef newCompoundObject(int numObjRefs){
    ObjRef compObject;
    compObject = malloc(sizeof(unsigned int) + (numObjRefs * sizeof(ObjRef*)));
    compObject->size = (MSB | numObjRefs);
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
    //print_assambler_instructions();
    exit(1);
}

ObjRef pop_Stack_Object(void){
    stack_pointer--;
    if(stack_pointer <= 0){
        exception("Stackunderflow Exception at function: ", __func__, __LINE__);
    } else if (!stack[stack_pointer].isObjectReference){
        exception("Type Mismatch ->\nreference: Object\nactual: Number", __func__, __LINE__);
    }
    return stack[stack_pointer].u.objRef;
}

void push_Stack_Object(ObjRef objRef){
    if(stack_pointer >= STACK_SIZE-1){
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
    if(stack_pointer >= STACK_SIZE-1){
        exception("Stackoverflow Exception: ", __func__, __LINE__);
    }
    stack[stack_pointer].isObjectReference = false;
    stack[stack_pointer].u.number = immediate;
    stack_pointer++;
}


void pop_Global(int immediate){
    if(immediate <= 0){
        exception("Global Stackunderflow Exception", __func__, __LINE__);
    }
    int var = pop_Stack_Number();
    static_variables[immediate].u.number = var;
}


void push_Global(int immediate){
    if(immediate < no_of_static_variables){
        exception("Global Stackoverflow Exception", __func__, __LINE__);
    }
    int memory_Adress = static_variables[immediate].u.number;
    push_Stack_Number(memory_Adress);
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
    if(memory_Adress == STACK_SIZE-1){
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
    program_counter++;
    return 0;
}

int add (int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    bigAdd();
    push_Stack_Object(bip.res);
    program_counter++;
    return 0;
}

int sub (int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    bigSub();
    push_Stack_Object(bip.res);
    program_counter++;
    return 0;
}

int mul (int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    bigMul();
    push_Stack_Object(bip.res);
    program_counter++;
    return 0;
}

int divi (int immediate){
    bip.op2 = pop_Stack_Object();
    /*
    if (bigToInt() == 0){
        exception("Divide by Zero Exception", __func__, __LINE__);
    }
     */
    bip.op1 = pop_Stack_Object();
    bigDiv();
    push_Stack_Object(bip.res);
    program_counter++;
    return 0;
}

int mod (int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = bip.op2;
    if(bigToInt() == 0){
        exception("Modulo by Zero Exception", __func__, __LINE__);
    }
    bip.op1 = pop_Stack_Object();
    bigDiv();
    push_Stack_Object(bip.rem);
    program_counter++;
    return 0;
}

int rdint (int immediate){
    bigRead(stdin);
    push_Stack_Object(bip.res);
    program_counter++;
    return 0;
}

int wrint (int immediate){
    bip.op1 = pop_Stack_Object();
    bigPrint(stdout);
    program_counter++;
    return 0;
}

int rdchr (int immediate){
    char character;
    scanf("%c", &character);
    bigFromInt((unsigned int) character);
    push_Stack_Object(bip.res);
    program_counter++;
    return 0;
}


int wrchr (int immediate){
    bip.op1 = pop_Stack_Object();
    printf("%c", (char) bigToInt());
    program_counter++;
    return 0;
}

int pushg(int immediate){
    push_Global(immediate);
    program_counter++;
    return 0;
}

int popg(int immediate){
    pop_Global(immediate);
    program_counter++;
    return 0;
}

int pushl(int immediate) {
    push_local(frame_pointer+immediate);
    program_counter++;
    return 0;
}

int popl(int immediate){
    pop_local(frame_pointer+immediate);
    program_counter++;
    return 0;
}

int asf(int immediate){
    if((stack_pointer + immediate) > STACK_SIZE-1){
        exception("Assamble Stackframe Stackoverflow Exception", __func__, __LINE__);
    }
    push_Stack_Number(frame_pointer);
    frame_pointer = stack_pointer;
    stack_pointer += immediate;
    program_counter++;
    return 0;
}

int rsf(int immediate){
    stack_pointer = frame_pointer;
    frame_pointer = pop_Stack_Number();
    program_counter++;
    return 0;
}

int eq(int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    if(bigCmp() == 0){
        push_Stack_Object(BIG_ONE);
        program_counter++;
        return 1;
    }
    program_counter++;
    push_Stack_Object(BIG_NULL);
    return 0;
}


int ne(int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    if(bigCmp() != 0){
        program_counter++;
        push_Stack_Object(BIG_ONE);
        return 1;
    }
    program_counter++;
    push_Stack_Object(BIG_NULL);
    return 0;
}

int lt(int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    if(bigCmp() < 0){
        program_counter++;
        push_Stack_Object(BIG_ONE);
        return 1;
    }
    program_counter++;
    push_Stack_Object(BIG_NULL);
    return 0;
}

int le(int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    if(bigCmp() <= 0){
        program_counter++;
        push_Stack_Object(BIG_ONE);
        return 1;
    }
    program_counter++;
    push_Stack_Object(BIG_NULL);
    return 0;
}

int gt(int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    if(bigCmp() > 0){
        program_counter++;
        push_Stack_Object(BIG_ONE);
        return 1;
    }
    program_counter++;
    push_Stack_Object(BIG_NULL);
    return 0;
}

int ge(int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    if(bigCmp() >= 0){
        program_counter++;
        push_Stack_Object(BIG_ONE);
        return 1;
    }
    program_counter++;
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
    program_counter++;
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
    program_counter++;
    return 0;
}

int call(int immediate){
    push_Stack_Number(program_counter+1);
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
    program_counter++;
    return 0;
}

int pushr(int immediate){
    push_Stack_Object(return_value);
    program_counter++;
    return 0;
}

int popr(int immediate){
    return_value = pop_Stack_Object();
    program_counter++;
    return 0;
}

int dup(int immediate){
    ObjRef objRef = pop_Stack_Object();
    push_Stack_Object(objRef);
    push_Stack_Object(objRef);
    program_counter++;
    return 0;
}

int new(int immediate){
    if(immediate < 0){
        exception("compount object size exception: ", __func__, __LINE__);
    } else {
        push_Stack_Object(newCompoundObject(immediate));
    }
    program_counter++;
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
    program_counter++;
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
    program_counter++;
    return 0;
}


int newa(int immediate){
    bip.op1 = pop_Stack_Object();
    if(!IS_PRIM(bip.op1)){
        exception("Type Mismatch ->\nreference: primitive object\nactual: compound object", __func__, __LINE__);
    } else {
        push_Stack_Object(newCompoundObject(bigToInt()));
    }
    program_counter++;
    return 0;
}


int getfa(int immediate){

    // index
    bip.op1 = pop_Stack_Object();
    unsigned int i = bigToInt();

    // target object
    bip.op2 = pop_Stack_Object();

    if(IS_PRIM(bip.op2)){
        exception("Type Mismatch ->\nreference: primitive object\nactual: compound object", __func__, __LINE__);
    } else {
        push_Stack_Object(GET_REFS(bip.op2)[bigToInt()]);
    }
    program_counter++;
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
    program_counter++;
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
    program_counter++;
    return 0;
}


int pushn(int immediate){
    push_Stack_Object(NULL);
    program_counter++;
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
    program_counter++;
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
    program_counter++;
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
    stack = calloc(MEMORY_SLOT_SIZE, STACK_SIZE);
    if(stack == NULL){
        exception("Memory allocation failed", __func__, __LINE__);
    }
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