#ifndef PE1_FIELD_OWNER_ACTOR_H
#define PE1_FIELD_OWNER_ACTOR_H

/* Queries on the actor that owns the running field effect (D_800F32D0). */

/* Sends positional sound command (arg0, arg1) at the owner actor's position
 * and returns the sound handle. */
int func_800D3F64(int arg0, int arg1);
/* Owner actor state progress; 0x80 when it has no state or above 0x40. */
int func_800D3FD8(void);

#endif /* PE1_FIELD_OWNER_ACTOR_H */
