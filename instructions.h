#define HALT  0
#define PUSHC 1
#define ADD   2
#define SUB   3
#define MUL   4
#define DIV   5
#define MOD   6
#define RDINT 7
#define WRINT 8
#define RDCHR 9
#define WRCHR 10

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
