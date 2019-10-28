#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "constants.h"
#include "macro.h"
#include "program1.h"
#include "instructions.h"

/* Njvm program stack */
#define STACK_SIZE 10000
unsigned int stack[STACK_SIZE];
unsigned int stackpointer;

/*Program Memory counter*/
unsigned int program_counter;

/*
 *
 */
const char *instructions[11]={
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
        "wrchr"
};

void exception(char* message){
    printf(message);
    exit(1);
}

typedef void (*instructionPtr)(int);

/*
 * ASM Instructions
 */
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

int pop_Stack(){
    if(stackpointer < 0){
        exception("Stackunderflow Exception");
    }
    printf("pops stack at: %d\n",stackpointer); //-!
    return stack[--stackpointer];
}

void push_Stack(int immediate){
    if(stackpointer == STACK_SIZE-1){
        exception("Stackoverflow Exception");
    }
    printf("pushes to Stack-Adress: %d\n", stackpointer); //-!
    stack[stackpointer++] = immediate;
}

/*
 * Instruction Pointer Array
 */
instructionPtr opcode_instruction_pointer[] = {
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
    wrchr
};

void halt (int immediate){
    exit(1);
}

void pushc (int immediate){
    push_Stack(immediate);
    program_counter++;
}

void add (int immediate){
    int num1 = pop_Stack();
    int num2 = pop_Stack();
    push_Stack(num1+num2);
    program_counter++;
}

void sub (int immediate){
    int num1 = pop_Stack();
    int num2 = pop_Stack();
    push_Stack(num2-num1);
    program_counter++;
}

void mul (int immediate){
    push_Stack(pop_Stack()*pop_Stack());
    program_counter++;
}

void divi (int immediate){
    int divident = pop_Stack();
    int numerator = pop_Stack();
    if (divident == 0 || numerator == 0){
        exception("Divide by Zero Exception");
    }
    push_Stack(numerator / divident);
    program_counter++;
}

void mod (int immediate){
    int modulent = pop_Stack();
    int numerator = pop_Stack();
    pushc(numerator % modulent);
    program_counter++;
}

void rdint (int immediate){
    int number;
    scanf("%d", &number);
    push_Stack(number);
    program_counter++;
}

void wrint (int immediate){
    printf("%d", pop_Stack());
    program_counter++;
}

void rdchr (int immediate){
    char character;
    scanf("%c", &character);
    push_Stack(character);
    program_counter++;
}


void wrchr (int immediate){
    printf("%c", pop_Stack());
    program_counter++;
}

void load_program_to_memory(char* program_file_path){
    FILE *fp;
    fp = fopen(program_file_path, "r");
    if(fp == NULL){
        exception("Could not open file!");
    }
    fprintf(fp, " ");
    fclose(fp);
    printf("%s", program_file_path);
}


/* catches argv parameters
 * @param[] string Parameter
 * if wrong param -> Error message to exit
 */
void catch_param(char param[]){
    if(EQSTRING(param, "--version")){
        printf(VERSION);
    }else if(EQSTRING(param, "--help")){
        printf(HELP);
    }else{
        printf("unknown command line argument '%s', try '%s --help'\n", param, __FILE__);
        exit(1);
    }
}

/* Main Program flow function */
void run(char* program_file_path){
    stackpointer = 0;
    program_counter = 0;
    //print_assambler_instructions(PROGRAM_1_INSTRUCTION_COUNT, program_1_memory);
    /*
    while(program_counter <= PROGRAM_1_INSTRUCTION_COUNT){
        opcode_instruction_pointer[OPCODE(program_1_memory[program_counter])](SIGN_EXTEND(IMMEDIATE(program_1_memory[program_counter])));
    }
     */
    load_program_to_memory(program_file_path);
}

/* Prints assambler instructions
 * @count number of assambler instructions
 * @memory array of assambler instructions
 */
void print_assambler_instructions(unsigned int count, unsigned int memory[]){
    char buf[30];
    for(int i = 0; i < count; i++){
        int temp = IMMEDIATE(memory[i]);
        sprintf(buf, "%i", temp);
        printf("%d\t%s\t%s\n", i, instructions[OPCODE(memory[i])], ((temp > 0) ? buf : ""));
    }
}

/* calls for argv check and program run function */
int main(int argc, char *argv[]){
    for(int i=1; i < argc-1; i++){
        catch_param(argv[i]);
    }
    printf("%s\n", START);
    run(argv[argc-1]);
    printf("%s\n", STOP);
    return 0;
}
