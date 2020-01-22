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

// Aufgabe 2
void pushg(int);
void popg(int);
void asf(int);
void rsf(int);
void pushl(int);
void popl(int);

// Aufgabe 3
void eq(int);
void ne(int);
void lt(int);
void le(int);
void gt(int);
void ge(int);
void jmp(int);
void brf(int);
void brt(int);

// Aufgabe 4
void call(int);
void ret(int);
void drop(int);
void pushr(int);
void popr(int);
void dup(int);

//Aufgabe 7
void new(int);
void getf(int);
void putf(int);
void newa(int);
void getfa(int);
void putfa(int);
void getsz(int);
void pushn(int);
void refeq(int);
void refne(int);

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
void garbage_collector_recursive_exception(short);
void shit_debug_func(unsigned int); // Bitte rausnehmen
void relocate_Objects(void);
void relocate_big_int(void);
void scanning(void);
void allocate_program_memory(void);
void setup_big_int(void);
void exception(char* , const char*, int);
void check_file_format(FILE*);
void check_file_version_no(FILE*);
void allocate_memory_for_instructions(FILE*);
void allocate_memory_for_static_variables(FILE*);
void allocate_memory_for_stack(void);
void allocate_memory_for_heap(void);
ObjRef heap_alloc(unsigned int);
void flip(void);
void garbage_collector(void);
ObjRef relocate(ObjRef);
ObjRef copy_object(ObjRef);
void scan(void);
void read_instructions_into_memory(FILE*);
void load_program_to_memory(char*);
void catch_param(int, char*[]);
void run(char*);
void debug(char*);
void print_assambler_instructions(void);
void print_menu(void);
void print_memory(void);
ObjRef pop_Stack_Object(void);
void push_Stack_Object(ObjRef);
int pop_Number(void);
void push_Number(int immediate);


void print_heap(ObjRef);

#endif //NJVM_NJVM_H
