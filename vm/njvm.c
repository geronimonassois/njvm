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
#define RETURN_STACK_SIZE 10000

int stack[STACK_SIZE];
int* static_variables;
int return_value;

int no_of_static_variables;
int no_of_instructions;

unsigned int* memory;
unsigned int stack_pointer;
unsigned int return_stack_pointer;
unsigned int frame_pointer;

unsigned int debug_flag = 0;

/*Program Memory counter*/
unsigned int program_counter;

/*
 *
 */
const char *instructions[32]={
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
        "du"
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
        dup
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
void catch_param(char param[]){
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
            debug(param);
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
    load_program_to_memory(program_file_path);
    while(program_counter < no_of_instructions){
        opcode_instruction_pointer[OPCODE(memory[program_counter])](SIGN_EXTEND(IMMEDIATE(memory[program_counter])));
    }
}


/*
 * Debug function
 * @program_file_path: path to binary
 * */
void debug(char* program_file_path){
    printf(ANSI_COLOR_GREEN "\nDEBUG MODE\n\n" ANSI_COLOR_RESET);
    stack_pointer = 0;
    program_counter = 0;
    load_program_to_memory(program_file_path);
    printf(ANSI_COLOR_BLUE "Program loaded into memory!" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "\n\nTo start program execution press Return!"ANSI_COLOR_RESET);
    while(program_counter < no_of_instructions){
        if(fgetc(stdin) == '\n'){
            opcode_instruction_pointer[OPCODE(memory[program_counter])](SIGN_EXTEND(IMMEDIATE(memory[program_counter])));
            system("clear");
            print_assambler_instructions_debug(program_counter, program_counter+1);
        }
    }
}


/*
 * exception handling
 * */
void exception(char* message, const char *func, int line){
    fprintf (stderr, ANSI_COLOR_RED "%s: \n" ANSI_COLOR_RESET, message);
    fprintf (stderr, ANSI_COLOR_RED "FUNCTION: \t%s\n" ANSI_COLOR_RESET, func);
    fprintf (stderr, ANSI_COLOR_RED "LINE: \t\t%d\n" ANSI_COLOR_RESET, line);
    exit(1);
}

int pop_Stack(){
    if(stack_pointer <= 0){
        exception("Stackunderflow Exception", __func__, __LINE__);
    }
    return stack[--stack_pointer];
}

void push_Stack(int immediate){
    if(stack_pointer == STACK_SIZE-1){
        exception("Stackoverflow Exception bei Funktion:", __func__, __LINE__);
    }
    stack[stack_pointer++] = immediate;
}

void pop_Global(int immediate){
    if(immediate <= 0){
        exception("Global Stackunderflow Exception", __func__, __LINE__);
    }
    int var = pop_Stack();
    static_variables[immediate] = var;
}

void push_Global(int immediate){
    if(immediate < no_of_static_variables){
        exception("Global Stackoverflow Exception bei Funktion:", __func__, __LINE__);
    }
    int var = static_variables[immediate];
    push_Stack(var);
}

void pop_local(int memory_Adress){
    if(memory_Adress <= 0){
        exception("Stackunderflow Exception", __func__, __LINE__);
    }
    int var = pop_Stack();
    stack[memory_Adress] = var;
}

void push_local(int memory_Adress){
    if(memory_Adress == STACK_SIZE-1){
        exception("Stackoverflow Exception bei Funktion:", __func__, __LINE__);
    }
    int val = stack[memory_Adress];
    push_Stack(val);
}

int halt (int immediate){
    printf("%s\n", STOP);
    exit(1);
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
        exception("Divide by Zero Exception", __func__, __LINE__);
    }
    push_Stack(numerator / divident);
    program_counter++;
    return 0;
}

int mod (int immediate){
    int modulent = pop_Stack();
    int numerator = pop_Stack();
    if(modulent == 0){
        exception("Modulo by Zero Exception", __func__, __LINE__);
    }
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
        exception("Assamble Stackframe Stackoverflow Exception bei Funktion:", __func__, __LINE__);
    }
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
    if(first_val > second_val){
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
    if(first_val >= second_val){
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
    if(first_val < second_val){
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
    if(first_val <= second_val){
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
        exception("Jump address is invalid", __func__, __LINE__);
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

int call(int immediate){
    push_Stack(program_counter+1);
    jmp(immediate);
    return 0;
}

int ret(int immediate){
    unsigned int return_adress = pop_Stack();
    program_counter = return_adress;
    return 0;
}

int drop(int immediate){
    for(int i = 0; i < immediate; i++){
        pop_Stack();
    }
    program_counter++;
    return 0;
}

int pushr(int immediate){
    push_Stack(return_value);
    program_counter++;
    return 0;
}

int popr(int immediate){
    return_value = pop_Stack();
    program_counter++;
    return 0;
}

int dup(int immediate){
    push_Stack(stack[stack_pointer]);
    program_counter++;
    return 0;
}

void check_file_format(FILE *fp){
    char head[4];
    if(fread(&head[0], sizeof(char), 4, fp) != 4){
        exception("could not read number of requested bytes", __func__, __LINE__);
    }
    if(strncmp(head, "NJBF", 4) != 0){
        exception("No valid ninja binary file", __func__, __LINE__);
    }
}

void check_file_version_no(FILE *fp){
    int version_no;
    if(fread(&version_no, sizeof(int), 1, fp) != 1){
        exception("could not read number of requested bytes", __func__, __LINE__);
    }
    if(version_no != VERSION_NO){
        exception("The source file version number is incorrect!", __func__, __LINE__);
    }
}

void allocate_memory_for_instructions(FILE *fp){
    if(fread(&no_of_instructions, sizeof(int), 1, fp) != 1){
        exception("could not read number of requested bytes", __func__, __LINE__);
    }
    memory = malloc(no_of_instructions * sizeof(unsigned int));
    if(memory == NULL){
        exception("memory allocation failed", __func__, __LINE__);
    }
}


void allocate_memory_for_static_variables(FILE *fp){
    if(fread(&no_of_static_variables, sizeof(int), 1, fp) != 1){
        exception("could not read number of requested bytes", __func__, __LINE__);
    }
    static_variables = malloc(no_of_static_variables * sizeof(int));
    if(static_variables == NULL){
        exception("memory allocation failed", __func__, __LINE__);
    }
}


void read_instructions_into_memory(FILE *fp){
    if(fread(memory, sizeof(int), no_of_instructions, fp) != no_of_instructions){
        exception("could not read number of requested bytes", __func__, __LINE__);
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


void print_assambler_instructions_debug(int current_asm, int next_asm){
    char buf[80];
    for(int i = 0; i < no_of_instructions; i++){
        int temp = SIGN_EXTEND(IMMEDIATE(memory[i]));
        if(i == current_asm){
            sprintf(buf, ANSI_COLOR_RED "%d [current instruction]" ANSI_COLOR_RESET, temp);
        }else if(i == next_asm){
            sprintf(buf, ANSI_COLOR_BLUE"%d [next instruction]" ANSI_COLOR_RESET, temp);
        }else{
            sprintf(buf, "%d", temp);
        }
        printf("%d\t%s\t%s\n", i, instructions[OPCODE(memory[i])], buf);
    }
}

