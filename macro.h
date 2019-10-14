#define EQSTRING(param, testString) !strcmp(param, testString)
#define IMMEDIATE(x) ((x) & 0x00FFFFFF)
#define OPCODE(x) (((unsigned int) x) >> 24)

