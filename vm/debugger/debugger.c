/*
#include "debugger.h"
#include "../constants.h"






void debug(char* program_file_path){
    char input;
    stack_pointer = 0;
    program_counter = 0;
    load_program_to_memory(program_file_path);
    system("clear");
    printf("\n");
    print_assambler_instructions_debug(program_counter,program_counter+1);
    print_menu();
    while(program_counter < no_of_instructions){
        switch (input = getchar()){
            case 'x' :
                system("clear");
                printf("\n");
                print_assambler_instructions_debug(program_counter,program_counter+1);
                print_menu();
                opcode_instruction_pointer[OPCODE(memory[program_counter])](SIGN_EXTEND(IMMEDIATE(memory[program_counter])));
                break;
            case 'm' :
                system("clear");
                print_memory();
                print_menu();
                printf("\n");

        }
    }
    return;
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

void print_menu(void){
    printf("\n" \
    ANSI_COLOR_GREEN"[x]" ANSI_COLOR_RESET  " execute next instruction\n" \
    ANSI_COLOR_GREEN"[<linenumber>]" ANSI_COLOR_RESET " set breakpoint\n"\
    ANSI_COLOR_GREEN"[m]"ANSI_COLOR_RESET " view memory"
    );
    printf(ANSI_COLOR_RED"\n\nCommand: "ANSI_COLOR_RESET);
}


void print_memory(void){
    char buf[80];
    for(int i = 0; i < STACK_SIZE; i++){
        sprintf(buf, ANSI_COLOR_RED "%d [current instruction]" ANSI_COLOR_RESET, stack[i]);
        printf("%d\t%s\t%s\n", i, instructions[OPCODE(memory[i])], buf);
    }
}

*/