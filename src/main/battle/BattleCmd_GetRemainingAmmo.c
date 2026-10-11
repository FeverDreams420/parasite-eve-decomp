/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "pe1/battle_cmd.h"

/* MASPSX_FORCE_G0 keeps the player pointer absolute, as retail; the COMMON
 * stack top supplies the small-data metadata of the one $gp-relative word. */
extern struct FieldActor *g_PlayerEntity;
extern BattleCmdEntry g_BattleCmdStack[];
BattleCmdEntry *g_BattleCmdStackTop;

int BattleCmd_GetRemainingAmmo(int *out) {
    BattleCmdEntry *cursor;
    void **entity;
    void *current;
    int remaining = 0;

    entity = (void **)g_PlayerEntity;
    if (entity != 0) {
        if (entity[0] != 0) {
            current = entity[0];
            remaining = *(short *)((char *)current + 0xA);
            cursor = g_BattleCmdStack;

            while (cursor < g_BattleCmdStackTop) {
                /* retail compares the whole first word, not the opcode byte */
                if (cursor->header.word == 1) {
                    remaining -= cursor->payload.ammo_spend.amount;
                }
                cursor++;
            }

            if (out != 0) {
                *out = *(short *)((char *)current + 0x2A);
            }
        }
    }

    return remaining;
}
