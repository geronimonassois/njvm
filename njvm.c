#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "constants.h"
#include "macro.h"
#include "program1.h"
#include "instructions.h"

unsigned int stack[1000];

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

void run(){
    print_instructions(PROGRAM_1_INSTRUCTION_COUNT, program_1_memory);
}

void print_instructions(unsigned int count, unsigned int memory[]){
    char buf[30];
    for(int i = 0; i < count; i++){
        int temp = IMMEDIATE(memory[i]);
        sprintf(buf, "%i", temp);
        printf("%d\t%s\t%s\n", i, instructions[OPCODE(memory[i])], ((temp > 0) ? buf : ""));
    }
}

int main(int argc, char *argv[]){
    for(int i=1; i < argc; i++){
        catch_param(argv[i]);
    }
    printf("%s\n", START);
    run();
    printf("%s\n", STOP);
    return 0;
}