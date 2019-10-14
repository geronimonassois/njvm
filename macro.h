/* file contain macros (duh)
 * @EQSTRING tests strings on equality returns bool value
 * @IMMEDIATE bitcuts opcode, returns immediate value
 * @OPCODE bitcuts immediate, returns opcode value
 */

#define EQSTRING(param, testString) !strcmp(param, testString)
#define IMMEDIATE(x) ((x) & 0x00FFFFFF)
#define OPCODE(x) (((unsigned int) x) >> 24)

