// todo BITTE DIE BIT / BYTEZAHLEN in GCSTATS kontrollieren, bin müde
// TODO B bei njlisp out of memory testen

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


StackSlot *stack;
ObjRef *static_variables;
ObjRef return_value;

int no_of_static_variables;
int no_of_instructions;

unsigned char* heap_write_memory_start;
unsigned char* heap_start;
unsigned char* heap_write_memory_end;
unsigned char* heap_max;
unsigned char* heap_pointer;

unsigned int stack_pointer;
unsigned int frame_pointer;
unsigned int program_counter;
unsigned int stack_size = 64; // ---- todo hier * 1024 / stackslot
unsigned int heap_size = 8192;
unsigned int* memory;

unsigned int heap_obj_count = 0;
unsigned int obj_alive = 0;
unsigned int debug_flag = 0;
unsigned short gcpurge = 0;
unsigned short gcstats = 0;

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


typedef void (*instructionPtr)(int);


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


int main(int argc, char *argv[]){
    catch_param(argc, argv);
    return 0;
}


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
            gcpurge = 1;
        } else if (EQSTRING(params[i], "--gcstats")) {
            gcstats = 1;
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

void run(char* program_file_path){
    printf("%s\n", START);
    allocate_program_memory();
    stack_pointer = 0;
    program_counter = 0;
    load_program_to_memory(program_file_path);
    unsigned int memory_on_program_counter;
    while(program_counter < no_of_instructions){
        memory_on_program_counter = memory[program_counter++];
        opcode_instruction_pointer[OPCODE(memory_on_program_counter)](SIGN_EXTEND(IMMEDIATE(memory_on_program_counter)));
    }
}


void allocate_program_memory(){
    allocate_memory_for_stack();
    allocate_memory_for_heap();
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


void exception(char* message, const char* func, int line){
    fprintf (stdout, ANSI_COLOR_RED "%s: \n" ANSI_COLOR_RESET, message);
    fprintf (stdout, ANSI_COLOR_RED "FUNCTION: \t%s\n" ANSI_COLOR_RESET, func);
    fprintf (stdout, ANSI_COLOR_RED "LINE: \t\t%d\n" ANSI_COLOR_RESET, line);
    fprintf (stdout, ANSI_COLOR_RED "PROGRAM COUNTER: \t\t%d\n" ANSI_COLOR_RESET, program_counter);
    exit(1);
}


ObjRef pop_Stack_Object(){
    stack_pointer--;
    if(stack_pointer < 0){
        exception("Stackunderflow Exception at function: ", __func__, __LINE__);
    } else if (!stack[stack_pointer].isObjectReference){
        exception("Type Mismatch ->\nreference: Object\nactual: Number", __func__, __LINE__);
    }
    return stack[stack_pointer].u.objRef;
}

// TODO Bitte das - 1 HINTER STACK_SIZE WEG
// MUSS GEÄNDERT WERDEN, DA GRÖßE NICHT RICHTIG GESETZT WEIRD, falscher Vergleich
void push_Stack_Object(ObjRef objRef){
    if(stack_pointer* sizeof(StackSlot) >= (stack_size*MEMORY_SLOT_SIZE)){ // todo lola
        exception("Stackoverflow Exception", __func__, __LINE__);
    }
    stack[stack_pointer].isObjectReference = true;
    stack[stack_pointer].u.objRef = objRef;
    stack_pointer++;
}


int pop_Stack_Number(){
    stack_pointer--;
    if(stack_pointer < 0){
        exception("Stackunderflow Exception: ", __func__, __LINE__);
    } else if (stack[stack_pointer].isObjectReference){
        exception("Type Mismatch ->\nreference: Number\nactual: Object", __func__, __LINE__);
    }
    return stack[stack_pointer].u.number;
}


void push_Stack_Number(int immediate){
    if(stack_pointer* sizeof(StackSlot) >= (stack_size*MEMORY_SLOT_SIZE)){ // todo lola
        exception("Stackoverflow Exception: ", __func__, __LINE__);
    }
    stack[stack_pointer].isObjectReference = false;
    stack[stack_pointer].u.number = immediate;
    stack_pointer++;
}

// todo eventuell über/unterhalb abfangen
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

// todo eventuell über/unterhalb abfangen
void pop_local(unsigned int memory_Adress) {
    if (memory_Adress <= 0) {
        exception("Stackunderflow Exception", __func__, __LINE__);
    }
    ObjRef var = pop_Stack_Object();
    stack[memory_Adress].isObjectReference = true;
    stack[memory_Adress].u.objRef = var;
}


void push_local(unsigned int memory_Adress){
    if(stack_pointer* sizeof(StackSlot) == (stack_size*MEMORY_SLOT_SIZE)){ // todo lola
        exception("Stackoverflow Exception", __func__, __LINE__);
    }
    ObjRef val = stack[memory_Adress].u.objRef;
    push_Stack_Object(val);
}


void halt (int immediate){
    garbage_collector();
    printf("%s\n", STOP);
    exit(1);
}


void pushc (int immediate){
    bigFromInt(immediate);
    push_Stack_Object(bip.res);
}


void add (int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    bigAdd();
    push_Stack_Object(bip.res);
}


void sub (int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    bigSub();
    push_Stack_Object(bip.res);
}


void mul (int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    bigMul();
    push_Stack_Object(bip.res);
}


void divi (int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    bigDiv();
    push_Stack_Object(bip.res);
}


void mod (int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    bigDiv();
    push_Stack_Object(bip.rem);
}


void rdint (int immediate){
    bigRead(stdin);
    push_Stack_Object(bip.res);
}


void wrint (int immediate){
    bip.op1 = pop_Stack_Object();
    bigPrint(stdout);
}


void rdchr (int immediate){
    char character;
    scanf("%c", &character);
    bigFromInt((int) character);
    push_Stack_Object(bip.res);
}


void wrchr (int immediate){
    bip.op1 = pop_Stack_Object();
    printf("%c", (char) bigToInt());
}


void pushg(int immediate){
    push_Global(immediate);
}


void popg(int immediate){
    pop_Global(immediate);
}


void pushl(int immediate) {
    push_local(frame_pointer+ immediate);
}


void popl(int immediate){
    pop_local(frame_pointer+ immediate);
}


void asf(int immediate){
    if((stack_pointer + immediate)* sizeof(StackSlot) == (stack_size*MEMORY_SLOT_SIZE)){ // todo lola
        exception("Assamble Stackframe Stackoverflow Exception", __func__, __LINE__);
    }
    push_Stack_Number((int)frame_pointer);
    frame_pointer = stack_pointer;
    for(int i = 0; i < immediate; i++){
        stack[stack_pointer + i].isObjectReference = true;
        stack[stack_pointer + i].u.objRef = NULL;
    }
    stack_pointer += immediate;
}


void rsf(int immediate){
    stack_pointer = frame_pointer;
    frame_pointer = pop_Stack_Number();
}


void eq(int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    if(bigCmp() == 0){
        bigFromInt(1);
    } else {
        bigFromInt(0);
    }
    push_Stack_Object(bip.res);
}


void ne(int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    if(bigCmp() != 0){
        bigFromInt(1);
    } else {
        bigFromInt(0);
    }
    push_Stack_Object(bip.res);
}


void lt(int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    if(bigCmp() < 0){
        bigFromInt(1);
    } else {
        bigFromInt(0);
    }
    push_Stack_Object(bip.res);
}


void le(int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    if(bigCmp() <= 0){
        bigFromInt(1);
    } else {
        bigFromInt(0);
    }
    push_Stack_Object(bip.res);
}


void gt(int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    if(bigCmp() > 0){
        bigFromInt(1);
    } else {
        bigFromInt(0);
    }
    push_Stack_Object(bip.res);
}


void ge(int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    if(bigCmp() >= 0){
        bigFromInt(1);
    } else {
        bigFromInt(0);
    }
    push_Stack_Object(bip.res);
}


void jmp(int immediate){
    if(immediate < 0 || immediate > no_of_instructions){
        exception("Jump address is invalid", __func__, __LINE__);
    }
    program_counter = (unsigned int)immediate;
}


void brf(int immediate){
    bip.op1 = pop_Stack_Object();
    if(bigToInt() == 0){
        jmp(immediate);
    }
}


void brt(int immediate){
    bip.op1 = pop_Stack_Object();
    if(bigToInt() != 0){
        jmp(immediate);
    }
}


void call(int immediate){
    push_Stack_Number((int)program_counter);
    jmp(immediate);
}


void ret(int immediate){
    unsigned int return_adress = pop_Stack_Number();
    program_counter = return_adress;
}


void drop(int immediate){
    for(int i = 0; i < immediate; i++){
        pop_Stack_Object();
    }
}


void pushr(int immediate){
    push_Stack_Object(return_value);
}


void popr(int immediate){
    return_value = pop_Stack_Object();
}


void dup(int immediate){
    ObjRef objRef = pop_Stack_Object();
    push_Stack_Object(objRef);
    push_Stack_Object(objRef);
}


void new(int immediate){
    if(immediate < 0){
        exception("compount object size exception: ", __func__, __LINE__);
    } else {
        push_Stack_Object(newCompoundObject(immediate));
    }
}


void getf(int immediate){
    bip.op1 = pop_Stack_Object();
    if(IS_PRIM(bip.op1)){
        exception("Getfield for Prim obj: ", __func__, __LINE__);
    } else if(GET_SIZE(bip.op1) <= immediate){
        exception("index out of bounds exception: ", __func__, __LINE__);
    } else if (immediate < 0){
        exception("index less than 0 exception: ", __func__, __LINE__);
    }
    push_Stack_Object(GET_REFS(bip.op1)[immediate]);
}


void putf(int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    if(GET_SIZE(bip.op1) <= immediate){
        exception("index out of bounds exception: ", __func__, __LINE__);
    } else if (immediate < 0){
        exception("index less than 0 exception: ", __func__, __LINE__);
    }
    GET_REFS(bip.op1)[immediate] = bip.op2;
}


void newa(int immediate){
    bip.op1 = pop_Stack_Object();
    if(!IS_PRIM(bip.op1)){
        exception("Type Mismatch ->\nreference: primitive object\nactual: compound object", __func__, __LINE__);
    } else {
        push_Stack_Object(newCompoundObject(bigToInt()));
    }
}


void getfa(int immediate){
    bip.op1 = pop_Stack_Object();       // index
    bip.op2 = pop_Stack_Object();       // target object
    if(IS_PRIM(bip.op2)){
        exception("Type Mismatch ->\nreference: primitive object\nactual: compound object", __func__, __LINE__);
    } else {
        push_Stack_Object(GET_REFS(bip.op2)[bigToInt()]);
    }
}


void putfa(int immediate){
    bip.op2 = pop_Stack_Object();       // object to be placed at given index
    bip.op1 = pop_Stack_Object();       // index
    bip.rem = pop_Stack_Object();       // target
    if(IS_PRIM(bip.rem)){
        exception("Type Mismatch ->\nreference: primitive object\nactual: compound object", __func__, __LINE__);
    } else {
        GET_REFS(bip.rem)[bigToInt()] = bip.op2;
    }
}


void getsz(int immediate){
    bip.op1 = pop_Stack_Object();
    if(IS_PRIM(bip.op1)){
        bigFromInt(-1);
    } else {
        bigFromInt(GET_SIZE(bip.op1));
    }
    push_Stack_Object(bip.res);
}


void pushn(int immediate){
    push_Stack_Object(NULL);
}


void refeq(int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    if(bip.op2 == bip.op1){
        bigFromInt(1);
    } else {
        bigFromInt(0);
    }
    push_Stack_Object(bip.res);
}


void refne(int immediate){
    bip.op2 = pop_Stack_Object();
    bip.op1 = pop_Stack_Object();
    if(bip.op2 != bip.op1){
        bigFromInt(1);
    } else {
        bigFromInt(0);
    }
    push_Stack_Object(bip.res);
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


void allocate_memory_for_heap(void){
    heap_start = calloc(MEMORY_SLOT_SIZE, heap_size);
    if(heap_start == NULL){
        exception("Heap memory allocation failed", __func__, __LINE__);
    }
    heap_start = heap_start;
    heap_pointer = heap_start;
    heap_write_memory_end = &heap_start[heap_size*MEMORY_SLOT_SIZE / 2];
    heap_max = &heap_start[heap_size*MEMORY_SLOT_SIZE-1];
}


ObjRef heap_alloc(unsigned int size){
    if(gcstats){
        heap_obj_count++;
    }
    ObjRef heap_address_for_object = (ObjRef)heap_pointer;
    if((heap_pointer+size) > heap_write_memory_end){
        garbage_collector();
        if((heap_pointer+size) > heap_write_memory_end){
            exception("Out of memory exception: ", __func__, __LINE__);
        }
        heap_address_for_object = (ObjRef)heap_pointer;
    }
    heap_pointer += size;
    return (ObjRef)heap_address_for_object;
}


void read_instructions_into_memory(FILE *fp){
    if(fread(memory, sizeof(int), no_of_instructions, fp) != no_of_instructions){
        exception("Could not read number of requested bytes", __func__, __LINE__);
    }
}


void load_program_to_memory(char* program_file_path){
    FILE *file_pointer;
    file_pointer = fopen(program_file_path, "r");
    if(file_pointer == NULL){
        exception("Error opening file", __func__, __LINE__);
    }
    check_file_format(file_pointer);
    check_file_version_no(file_pointer);
    allocate_memory_for_instructions(file_pointer);
    allocate_memory_for_static_variables(file_pointer);
    read_instructions_into_memory(file_pointer);
    fclose(file_pointer);
}


void garbage_collector(void){
    if(gcstats){
        printf("\n------------------------------------------------------------------------------------\n");
        printf("Es Wurden: %d Objekte seit dem letzten collect angelegt\n",heap_obj_count);
        printf("Es wurde: %ld Speicher genutzt\n",(heap_write_memory_end - heap_pointer));
    }
    static short isRunning = 0;
    garbage_collector_recursive_exception(isRunning);
    isRunning = 1;
    flip();
    relocate_Objects();
    relocate_big_int();
    scanning();
    if(gcpurge){
        if(heap_write_memory_end == heap_max){
            null_unused_heap(heap_start,heap_write_memory_start);
        } else{
            null_unused_heap(heap_write_memory_end,heap_max);
        }
    }
    if(gcstats){
        printf("Es ist: %ld Speicher nach dem collecten frei\n",(heap_write_memory_end - heap_pointer));
        printf("Es leben noch: %d Objekte nach dem collecten\n", obj_alive + no_of_static_variables);
        printf("------------------------------------------------------------------------------------\n");
        heap_obj_count = 0;
        obj_alive = 0;
    }
    isRunning = 0;
}


void garbage_collector_recursive_exception(short running){
    if (running){
        exception("recursive garbage collection", __func__, __LINE__);
    }
}


void relocate_Objects(){
    for(int i=0; i < stack_pointer; i++){
        if(stack[i].isObjectReference){
            stack[i].u.objRef = relocate(stack[i].u.objRef);
            if(gcstats){
                obj_alive ++;
            }
        }
    }
    for(int i = 0; i < no_of_static_variables; i++){
        static_variables[i] = relocate(static_variables[i]);
    }
    return_value = relocate(return_value);
}


void relocate_big_int(){
    bip.op1 = relocate(bip.op1);
    bip.op2 = relocate(bip.op2);
    bip.res = relocate(bip.res);
    bip.rem = relocate(bip.rem);
}


void scanning(){
    unsigned char* scan = heap_write_memory_start;
    ObjRef object;
    while(scan != heap_pointer){
        object = (ObjRef)scan;
        if (!IS_PRIM(object)){
            if(gcstats){
                obj_alive ++;
            }
            for(int i = 0; i < GET_SIZE(object); i++){
                GET_REFS(object)[i] = relocate(GET_REFS(object)[i]);
            }
            scan += (GET_SIZE(object) * sizeof(ObjRef)) + sizeof(unsigned int);
        } else {
            scan += GET_SIZE(object) + sizeof(unsigned int);
        }
    }
}

void null_unused_heap(unsigned char* unused_memory_start, unsigned char* unused_memory_end){
    unsigned char* nulling_memory = unused_memory_start;
    while(nulling_memory < unused_memory_end){
        *nulling_memory = NULL;
        nulling_memory ++;
    }
}



void flip(){
    if(heap_write_memory_end == heap_max){
        heap_write_memory_end = heap_write_memory_start;
        heap_write_memory_start = heap_start;
    } else {
        heap_write_memory_start = heap_write_memory_end;
        heap_write_memory_end = heap_max;
    }
    heap_pointer = heap_write_memory_start;
}


ObjRef relocate(ObjRef orig){
    ObjRef copy;
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
    unsigned int offset = (unsigned int)(temp_address - heap_start);
    memcpy(temp_address, orig, size);
    orig->size = BREAK_MY_HEART(offset);
    return (ObjRef)temp_address;
}