/* CC1_FLAGS: -G1 */
/* MASPSX_FLAGS: -G1 */

#include "pe1/battle.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/player_entity.h"

void Battle_HaltOnPositiveX(void) {
    Combatant *state;
    BattleEntity *actor;
    Combatant *state2;
    BattleEntity *actor2;
    int flags;

    state = g_ActiveActor;
    if ((s16)state->curHP > 0) {
        actor = (BattleEntity *)g_PlayerEntity;
        actor->motionX = 0;
        actor->motionY = 0;
        actor->motionZ = 0;
        Entity_SetActionMode(actor, 0x12);

        state2 = g_ActiveActor;
        actor2 = (BattleEntity *)g_PlayerEntity;
        flags = state2->stateFlags;
        D_8009CE30 = 0;
        flags |= 0x2000;
        state2->stateFlags = flags;
        actor2->entityFlags &= ~0x100;
        Battle_FlushScriptSounds();
    }
}
