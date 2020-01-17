//
// Created by geronimonassois on 06.11.19.
//

#ifndef NJVM_NJVM_H
#define NJVM_NJVM_H

#include "bigint/src/support.h"

/*
 * ASM Instruction Prototypes
 * */

// Aufgabe 1
int halt (int);
int pushc (int);
int add (int);
int sub (int);
int mul (int);
int divi (int);
int mod (int);
int rdint (int);
int wrint (int);
int rdchr (int);
int wrchr (int);

// Aufgabe 2
int pushg(int);
int popg(int);
int asf(int);
int rsf(int);
int pushl(int);
int popl(int);

// Aufgabe 3
int eq(int);
int ne(int);
int lt(int);
int le(int);
int gt(int);
int ge(int);
int jmp(int);
int brf(int);
int brt(int);

// Aufgabe 4
int call(int);
int ret(int);
int drop(int);
int pushr(int);
int popr(int);
int dup(int);

/*
 * Structures
 */
typedef enum { false, true } boolean;

typedef struct {
    boolean isObjectReference;
    union{
        ObjRef objRef;
        int number;
    } u;
} StackSlot;


/*
 * Functions
 * */
void print_Stack(void);
void exception(char* , const char*, int);
void check_file_format(FILE*);
void check_file_version_no(FILE*);
void allocate_memory_for_instructions(FILE*);
void allocate_memory_for_static_variables(FILE*);
void allocate_memory_for_stack(void);
void read_instructions_into_memory(FILE*);
void load_program_to_memory(char*);
void catch_param(char* );
void run(char*);
void debug(char*);
void print_assambler_instructions(void);
void print_menu(void);
void print_memory(void);
ObjRef pop_Stack_Object(void);
void push_Stack_Object(ObjRef);
int pop_Number(void);
void push_Number(int immediate);

#endif //NJVM_NJVM_H
