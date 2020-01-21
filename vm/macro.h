/* file contain macros (duh)int
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
#define BROKEN_HEART (1 << (8 *sizeof(unsigned int) - 2))
#define BREAK_MY_HEART(objRef) ((objRef)->size | BROKEN_HEART)
#define HEART_IS_BROKEN(objRef) (((objRef)->size & BROKEN_HEART) == BROKEN_HEART)
#define FORWARD_MASK (11 << (8 * sizeof(unsigned int)-2))
#define SET_FORWARDPOINTER(objRef, offset) (((objRef)->size & (FORWARD_MASK | MSB)) | offset)
#define GET_FORWARDPOINTER(objRef) ((objRef)->size & ~FORWARD_MASK)