//
// Created by geronimonassois on 06.11.19.
//

#ifndef NJVM_NJVM_H
#define NJVM_NJVM_H

/*
 * ASM Instruction Prototypes
 * */
int pop_Stack(void);
void push_Stack(int);
void halt (int);
void pushc (int);
int pop_Stack(void);
void push_Stack(int);
void halt (int);
void pushc (int);
void add (int);
void sub (int);
void mul (int);
void divi (int);
void mod (int);
void rdint (int);
void wrint (int);
void rdchr (int);
void wrchr (int);
void pushg(int);
int popg(void);
void asf(void);
void rsf(void);
void pushl(int);
int popl(void);

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

#endif //NJVM_NJVM_H
