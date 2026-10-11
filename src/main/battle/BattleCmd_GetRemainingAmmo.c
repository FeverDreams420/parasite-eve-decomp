/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "pe1/battle.h"
#include "pe1/battle_cmd.h"
#include "pe1/player_entity.h"

/* MASPSX_FORCE_G0 keeps the player pointer absolute, as retail; the COMMON
 * stack top supplies the small-data metadata of the one $gp-relative word. */
BattleCmdEntry *g_BattleCmdStackTop;

int BattleCmd_GetRemainingAmmo(int *out) {
    BattleCmdEntry *cursor;
    BattleEntity *entity;
    Combatant *current;
    int remaining = 0;

    entity = (BattleEntity *)g_PlayerEntity;
    if (entity != 0) {
        if (entity->core != 0) {
            current = entity->core;
            remaining = (s16)(current->exp_or_acc >> 16);
            cursor = g_BattleCmdStack;

            while (cursor < g_BattleCmdStackTop) {
                /* retail compares the whole first word, not the opcode byte */
                if (cursor->header.word == 1) {
                    remaining -= cursor->payload.ammo_spend.amount;
                }
                cursor++;
            }

            if (out != 0) {
                *out = (s16)(current->maxAtk >> 16);
            }
        }
    }

    return remaining;
}
