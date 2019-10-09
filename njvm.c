#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "constants.h"
#include "macro.h"

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
}

void

void run(){
    unsigned int memory[100]{
        0x01000003,
        0x01000004,
        0x02000000,
        0x0100000A,
        0x01000006,
        0x03000000,
        0x04000000,
        0x08000000,
        0x0100000A,
        0x0A000000,
        0x00000000,
    };
}

int main(int argc, char *argv[]){
    for(int i=1; i < argc; i++){
        catch_param(argv[i]);
    }
    printf("%s\n", START);
    printf("%s\n", STOP);
    return 0;
}