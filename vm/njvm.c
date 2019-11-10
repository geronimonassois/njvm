#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "njvm.h"
#include "macro.h"
#include "constants.h"
#include "instructions.h"

/* Njvm program stack */
#define STACK_SIZE 10000
int stack[STACK_SIZE];
int* global_variables;
int* local_variables;
unsigned int *memory;
int no_of_static_variables;
int *static_variables;
unsigned int sp;
unsigned int fp;
int no_of_instructions;

/*Program Memory counter*/
unsigned int program_counter;

/*
 *
 */
const char *instructions[17]={
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
        "popl"
};


/*
 * exception handling
 * */
void exception(char* message){
    printf(message);
    exit(1);
}

/*
 * Typedef for instruction pointer
 * */
typedef void (*instructionPtr)(int);

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
    popl
};

int pop_Stack(){
    if(sp < 0){
        exception("Stackunderflow Exception");
    }
    printf("pops stack at: %d\n",sp); //-!
    return stack[--sp];
}

void push_Stack(int immediate){
    if(sp == STACK_SIZE-1){
        exception("Stackoverflow Exception");
    }
    printf("pushes to Stack-Adress: %d\n", sp); //-!
    stack[sp++] = immediate;
}

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

/*
 * TO-DO
 * implement functions
 * debug function pointer problem
 */
void pushg(int immediate){

}

int popg(void){
    return 1;
}

void pushl(int immediate) {

}

int popl(void){
    return 1;
}

void asf(){

}

void rsf(){

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
    if(fread(&memory, sizeof(int), no_of_instructions, fp) != no_of_instructions){
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
        exception("Could not open file!");
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
    sp = 0;
    program_counter = 0;
    /*
    while(program_counter <= PROGRAM_1_INSTRUCTION_COUNT){
        opcode_instruction_pointer[OPCODE(program_1_memory[program_counter])](SIGN_EXTEND(IMMEDIATE(program_1_memory[program_counter])));
    }
     */
    load_program_to_memory(program_file_path);
    print_assambler_instructions();
}

/* Prints assambler instructions
 * @count: number of assambler instructions
 * @memory: array of assambler instructions
 */
void print_assambler_instructions(void){
    char buf[30];
    for(int i = 0; i < no_of_instructions; i++){
        int temp = IMMEDIATE((memory[i]));
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
