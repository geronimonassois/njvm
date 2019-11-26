#include "debugger.h"
#include "ncurses.h"
#include <stdlib.h>

void quit()
{
    endwin();
}


/*
 * Debug function
 * @program_file_path: path to binary
 * */
void debug(char* program_file_path){
    initscr();			/* Start curses mode 		  */
    curs_set(1);
    raw();
    printw("Hello World !!!");	/* Print Hello World		  */
    refresh();			/* Print it on to the real screen */
    getch();			/* Wait for user input */
    endwin();			/* End curses mode		  */

    /*
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
     */
}

