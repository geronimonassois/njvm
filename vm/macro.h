/* file contain macros (duh)
 * @EQSTRING tests strings on equality returns bool value
 * @IMMEDIATE bitcuts opcode, returns immediate value
 * @OPCODE bitcuts immediate, returns opcode value
 */

#define EQSTRING(param, testString) !strcmp(param, testString)
#define IMMEDIATE(x) ((x) & 0x00FFFFFF)
#define OPCODE(x) (((unsigned int) x) >> 24)
#define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xFF000000 : (i))
#define MSB (1 << (8 *sizeof(unsigned int) - 1))
#define IS_PRIM(objRef) (((objRef)->size & MSB) == 0)
#define GET_SIZE(objRef) ((objRef)->size & ~MSB)
#define GET_REFS(objRef) ((ObjRef *)(objRef)->data)