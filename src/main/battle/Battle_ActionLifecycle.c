/* CC1_FLAGS: -G2 */
/* MASPSX_FLAGS: -G2 */

#include "pe1/battle.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/battle_reset_stats.h"
#include "pe1/battle_runtime.h"
#include "pe1/player_entity.h"

void Battle_BeginPlayerAction(void) {
    BattleEntity *player;
    Combatant *actor;
    int mask;
    int flags;
    u32 *entityFlags;
    s32 *stateFlags;

    mask = -0x101;
    D_8009D2E8 |= 1;
    player = D_8009D254;
    entityFlags = &player->entityFlags;
    flags = *entityFlags;
    actor = D_8009D278;
    flags &= mask;
    *entityFlags = flags;
    stateFlags = &actor->stateFlags;
    *stateFlags |= 0x10000;
    D_8009D298 = 0;
    Entity_SetActionMode(player, 0x12);
    Battle_FlushScriptSounds();
}

void Battle_ReturnToIdle(void) {
    Combatant *state;
    BattleEntity *actor;
    int mask_state;
    int flags;
    s32 *stateFlags;

    mask_state = 0xFFFEFFFF;
    D_8009D2E8 &= -2;
    state = g_ActiveActor;
    actor = (BattleEntity *)g_PlayerEntity;
    stateFlags = &state->stateFlags;
    flags = *stateFlags;
    flags &= mask_state;
    *stateFlags = flags;
    actor->motionX = 0;
    actor->motionY = 0;
    actor->motionZ = 0;
    Entity_SetActionMode(actor, state->actionMode12);
}
