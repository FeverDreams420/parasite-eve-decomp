#include "common.h"
#include "pe1/battle.h"
#include "pe1/gte_types.h"
#include "pe1/battle_runtime.h"
#include "pe1/battle_entity_anim.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#define NULL ((void *)0)
int Battle_CalcDistToPlayer(BattleEntity *actor, BattleEntity *player);

/* COMMON supplies the target count's small-data size to MASPSX; every
 * other word stays absolute. */
s8 g_BattleTargetIndex;

void Battle_BuildTargetList(void) {
    s32 entityFlags;
    register s32 nextIndex asm("$5");
    register s32 finalCount asm("$2");
    s32 pad_[2];
    s32 distance;
    s16 angle;
    EnemyCombatant *candidateCore;
    BattleEntity *player;
    BattleEntity *candidate;

    candidate = (BattleEntity *)D_8009D20C;
    g_BattleTargetIndex = 0;
    if (candidate != NULL) {
        do {
            player = D_8009D254;
            if (candidate != player) {
                candidateCore = candidate->core;
                if (candidateCore != NULL) {
                    entityFlags = candidate->entityFlags;
                    if (((entityFlags & 0x2040) != 0x40) && !(entityFlags & 0x4000) &&
                        (candidateCore->hpAlive > 0)) {
                        g_BattleTargetList[g_BattleTargetIndex].actor = candidate;
                        distance = Battle_CalcDistToPlayer(candidate, player);
                        g_BattleTargetList[g_BattleTargetIndex].dist = distance;
                        angle = ratan2(
                            candidate->renderObject.target_x -
                                D_8009D254->posX.parts.integer,
                            candidate->renderObject.target_z -
                                D_8009D254->posZ.parts.integer);
                        nextIndex = g_BattleTargetIndex + 1;
                        g_BattleTargetList[g_BattleTargetIndex].angle = angle;
                        g_BattleTargetIndex = nextIndex;
                    }
                }
            }
            candidate = candidate->next;
        } while (candidate != NULL);
    }
    if (g_BattleTargetIndex >= 2) {
        Battle_SortTargets((BattleTargetWords *)g_BattleTargetList, 0, (s8) (g_BattleTargetIndex - 1));
    }
    finalCount = g_BattleTargetIndex;
    __asm__("" : "=r"(finalCount) : "0"(finalCount));
    g_BattleTargetList[finalCount].actor = NULL;
}
