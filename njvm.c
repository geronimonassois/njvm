#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "constants.h"
#include "macro.h"
#include "program1.h"
#include "instructions.h"

/* Njvm program stack */
unsigned int stack[10000];

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

/* catches argv parameters
 * @param[] string Parameter
 * if wrong param -> Error message to exit
 */
void catch_param(char param[]){
    if(EQSTRING(param, "--version")){
        printf(VERSION);
    }else if(EQSTRING(param, "--help")){
        printf(HELP);
    }else if(EQSTRING(param, "--prog1")) {
        run();
    }else{
        printf("unknown command line argument '%s', try '%s --help'\n", param, __FILE__);
        exit(1);
    }
}

/* Main Program flow function
 * todo v1:
 * call functionpointer over Opcode with imidiate value as param
 * todo v2:
 * get Prog mem from assambler dat and malloc memory (stack usw)
 */
void run(){
    print_instructions(PROGRAM_1_INSTRUCTION_COUNT, program_1_memory);
}

/* Prints assambler instructions
 * @count number of assambler instructions
 * @memory array of assambler instructions
 */
void print_instructions(unsigned int count, unsigned int memory[]){
    char buf[30];
    for(int i = 0; i < count; i++){
        int temp = IMMEDIATE(memory[i]);
        sprintf(buf, "%i", temp);
        printf("%d\t%s\t%s\n", i, instructions[OPCODE(memory[i])], ((temp > 0) ? buf : ""));
    }
}

/* calls for argv check and program run function */
int main(int argc, char *argv[]){
    for(int i=1; i < argc; i++){
        catch_param(argv[i]);
    }
    printf("%s\n", START);
    run();
    printf("%s\n", STOP);
    return 0;
}