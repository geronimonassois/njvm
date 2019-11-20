#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "njvm.h"
#include "macro.h"
#include "constants.h"
#include "instructions.h"

/* Njvm program stack */
#define STACK_SIZE 10000

int stack[STACK_SIZE];
int* global_variables;
int* local_variables;
int* static_variables;
extern int errno;

int no_of_static_variables;
int no_of_instructions;

unsigned int* memory;
unsigned int stack_pointer;
unsigned int frame_pointer;


/*Program Memory counter*/
unsigned int program_counter;

/*
 *
 */
const char *instructions[26]={
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
        "brt"
};


/*
 * exception handling
 * */
void exception(char* message){
    fprintf(stderr, "%s:\t %s\n",message, strerror(errno));
    exit(1);
}

/*
 * Typedef for instruction pointer
 * */
typedef int (*instructionPtr)(int);

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
    wrchr,

    pushg,
    popg,
    asf,
    rsf,
    pushl,
    popl,

    eq,
    ne,
    lt,
    le,
    gt,
    ge,
    jmp,
    brf,
    brt
};

int pop_Stack(){
    if(stack_pointer <= 0){
        exception("Stackunderflow Exception");
    }
    return stack[--stack_pointer];
}

void push_Stack(int immediate){
    if(stack_pointer == STACK_SIZE-1){
        exception("Stackoverflow Exception");
    }
    printf("pushes to Stack-Adress: %d\n", stack_pointer); //-!
    stack[stack_pointer++] = immediate;
}

int halt (int immediate){
    exit(1);
    return 0;
}

int pushc (int immediate){
    push_Stack(immediate);
    program_counter++;
    return 0;
}

int add (int immediate){
    int num1 = pop_Stack();
    int num2 = pop_Stack();
    push_Stack(num1+num2);
    program_counter++;
    return 0;
}

int sub (int immediate){
    int num1 = pop_Stack();
    int num2 = pop_Stack();
    push_Stack(num2-num1);
    program_counter++;
    return 0;
}

int mul (int immediate){
    push_Stack(pop_Stack()*pop_Stack());
    program_counter++;
    return 0;
}

int divi (int immediate){
    int divident = pop_Stack();
    int numerator = pop_Stack();
    if (divident == 0 || numerator == 0){
        exception("Divide by Zero Exception");
    }
    push_Stack(numerator / divident);
    program_counter++;
    return 0;
}

int mod (int immediate){
    int modulent = pop_Stack();
    int numerator = pop_Stack();
    pushc(numerator % modulent);
    program_counter++;
    return 0;
}

int rdint (int immediate){
    int number;
    scanf("%d", &number);
    push_Stack(number);
    program_counter++;
    return 0;
}

int wrint (int immediate){
    printf("%d", pop_Stack());
    program_counter++;
    return 0;
}

int rdchr (int immediate){
    char character;
    scanf("%c", &character);
    push_Stack(character);
    program_counter++;
    return 0;
}


int wrchr (int immediate){
    printf("%c", pop_Stack());
    program_counter++;
    return 0;
}

int pushg(int immediate){
    int var = pop_Stack();
    static_variables[immediate] = var;
    program_counter++;

    return 0;
}

int popg(int immediate){
    int var = static_variables[immediate];
    static_variables[immediate] = 0;
    push_Stack(var);
    program_counter++;

    return 0;
}

int pushl(int immediate) {
    int var = stack[frame_pointer+immediate];
    push_Stack(var);
    program_counter++;
    return 0;
}

int popl(int immediate){
    int var = pop_Stack();
    stack[frame_pointer+immediate] = var;
    program_counter++;

    return 0;
}

int asf(int immediate){
    push_Stack(frame_pointer);
    frame_pointer = stack_pointer;
    stack_pointer += immediate;
    program_counter++;

    return 0;
}

int rsf(int immediate){
    stack_pointer = frame_pointer;
    frame_pointer = pop_Stack();
    program_counter++;
    return 0;
}

int eq(int immediate){
    int first_val = pop_Stack();
    int second_val = pop_Stack();
    if(first_val == second_val){
        program_counter++;
        push_Stack(1);
        return 1;
    }
    program_counter++;
    push_Stack(0);
    return 0;
}

int ne(int immediate){
    int first_val = pop_Stack();
    int second_val = pop_Stack();
    if(first_val != second_val){
        program_counter++;
        push_Stack(1);
        return 1;
    }
    program_counter++;
    push_Stack(0);
    return 0;
}

int lt(int immediate){
    int first_val = pop_Stack();
    int second_val = pop_Stack();
    if(first_val < second_val){
        program_counter++;
        push_Stack(1);
        return 1;
    }
    program_counter++;
    push_Stack(0);
    return 0;
}

int le(int immediate){
    int first_val = pop_Stack();
    int second_val = pop_Stack();
    if(first_val <= second_val){
        program_counter++;
        push_Stack(1);
        return 1;
    }
    program_counter++;
    push_Stack(0);
    return 0;
}

int gt(int immediate){
    int first_val = pop_Stack();
    int second_val = pop_Stack();
    if(first_val > second_val){
        program_counter++;
        push_Stack(1);
        return 1;
    }
    program_counter++;
    push_Stack(0);
    return 0;
}

int ge(int immediate){
    int first_val = pop_Stack();
    int second_val = pop_Stack();
    if(first_val >= second_val){
        program_counter++;
        push_Stack(1);
        return 1;
    }
    program_counter++;
    push_Stack(0);
    return 0;
}

int jmp(int immediate){
    if(immediate < 0 || immediate > no_of_instructions){
        exception("Jump address is invalid");
    }
    program_counter = immediate;
    return 0;
}

int brf(int immediate){
    int eval = pop_Stack();
    if(eval == 0){
        jmp(immediate);
        return 0;
    }
    program_counter++;
    return 0;
}

int brt(int immediate){
    int eval = pop_Stack();
    if(eval == 1){
        jmp(immediate);
        return 0;
    }
    program_counter++;
    return 0;
}


void check_file_format(FILE *fp){
    char head[4];
    if(fread(&head[0], sizeof(char), 4, fp) != 4){
        exception("could not read number of requested bytes");
    }
    if(strncmp(head, "NJBF", 4) != 0){
        exception("No valid ninja binary file");
    }
}

void check_file_version_no(FILE *fp){
    int version_no;
    if(fread(&version_no, sizeof(int), 1, fp) != 1){
        exception("could not read number of requested bytes");
    }
    if(version_no != VERSION_NO){
        exception("The source file version number is incorrect!");
    }
}

void allocate_memory_for_instructions(FILE *fp){
    if(fread(&no_of_instructions, sizeof(int), 1, fp) != 1){
        exception("could not read number of requested bytes");
    }
    memory = malloc(no_of_instructions * sizeof(unsigned int));
    if(memory == NULL){
        exception("memory allocation failed");
    }
}


void allocate_memory_for_static_variables(FILE *fp){
    if(fread(&no_of_static_variables, sizeof(int), 1, fp) != 1){
        exception("could not read number of requested bytes");
    }
    static_variables = malloc(no_of_static_variables * sizeof(int));
    if(static_variables == NULL){
        exception("memory allocation failed");
    }
}


void read_instructions_into_memory(FILE *fp){
    if(fread(memory, sizeof(int), no_of_instructions, fp) != no_of_instructions){
        exception("could not read number of requested bytes");
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
        exception("Error opening file");
    }
    check_file_format(fp);
    check_file_version_no(fp);
    allocate_memory_for_instructions(fp);
    allocate_memory_for_static_variables(fp);
    read_instructions_into_memory(fp);
    fclose(fp);
}


/* catches argv parameters
 * @param[]: string Parameter
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

/*
 * Main Program flow function
 * @program_file_path: path to binary
 * */
void run(char* program_file_path){
    stack_pointer = 0;
    program_counter = 0;
    load_program_to_memory(program_file_path);
    while(program_counter < no_of_instructions){
        opcode_instruction_pointer[OPCODE(memory[program_counter])](SIGN_EXTEND(IMMEDIATE(memory[program_counter])));
    }
    //print_assambler_instructions();
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
        printf("%d\t%s\t%s\n", i, instructions[OPCODE(memory[i])], ((temp > 0) ? buf : " "));
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
