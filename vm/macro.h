/* file contain macros (duh)int
 * @EQSTRING tests strings on equality returns bool value
 * @IMMEDIATE bitcuts opcode, returns immediate value
 * @OPCODE bitcuts immediate, returns opcode value
 */

#define EQSTRING(param, testString) !strcmp(param, testString)
#define IMMEDIATE(x) ((x) & 0x00FFFFFFu)
#define OPCODE(x) (((unsigned int) x) >> 24u)
#define SIGN_EXTEND(i) ((i) & 0x00800000u ? (i) | 0xFF000000u : (i))
#define MSB (1u << (unsigned)(8 *sizeof(unsigned int) - 1))
#define IS_PRIM(objRef) (((objRef)->size & MSB) == 0)

#define GET_REFS(objRef) ((ObjRef *)(objRef)->data)

#define BROKEN_HEART (1u << (8 *sizeof(unsigned int) - 2))
#define BREAK_MY_HEART(offset) (BROKEN_HEART | offset)
#define HEART_IS_BROKEN(objRef) (((objRef)->size & BROKEN_HEART) == BROKEN_HEART)
#define FORWARD_MASK (1u << (8 * sizeof(unsigned int)-2))
#define SET_FORWARDPOINTER(objRef, offset) (((objRef)->size & (FORWARD_MASK)) | offset)
#define GET_FORWARDPOINTER(objRef) ((objRef)->size & ~FORWARD_MASK)
#define GET_SIZE(objRef) ((objRef)->size & ~FORWARD_MASK)