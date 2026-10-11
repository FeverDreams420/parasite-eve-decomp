#ifndef PE1_EXT_B0CEC88E0001
#define PE1_EXT_B0CEC88E0001
extern RenderObjectEntity D_800B0CEC;
/* D_800B0D88 is D_800B0CEC.flags_9C. Retail reloads the player's flags only
 * after this store, so the store must look like it may alias them: a
 * non-small view keeps the address in a register, and separate read and
 * write views keep GCC from sharing one LUI between the load and the store. */
extern struct { u16 value; char pad[14]; } D_800B0D88_read __asm__("D_800B0D88");
extern struct { u16 value; char pad[14]; } D_800B0D88_write __asm__("D_800B0D88");
#endif /* PE1_EXT_B0CEC88E0001 */
