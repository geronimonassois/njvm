//
// Created by geronimonassois on 06.11.19.
//

#ifndef NJVM_NJVM_H
#define NJVM_NJVM_H

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
 * Functions
 * */
void check_file_format(FILE*);
void check_file_version_no(FILE*);
void allocate_memory_for_instructions(FILE*);
void allocate_memory_for_static_variables(FILE*);
void read_instructions_into_memory(FILE*);
void load_program_to_memory(char*);
void catch_param(char* );
void run(char*);
void print_assambler_instructions(void);
void print_assambler_instructions_debug(int, int);
int pop_Stack(void);
void push_Stack(int);

#endif //NJVM_NJVM_H
