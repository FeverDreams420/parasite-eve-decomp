/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/battle.h"
#include "pe1/battle_cmd.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/battle_reset_stats.h"
#include "pe1/battle_runtime.h"
#include "pe1/battle_setup.h"
#include "pe1/battle_status.h"
#include "pe1/game_state.h"
#include "pe1/gte_types.h"
#include "pe1/inventory.h"
#include "pe1/menu_flags.h"
#include "pe1/player_entity.h"
#include "pe1/pm.h"
#include "pe1/random.h"
#include "pe1/textbox.h"
extern unsigned int D_8009CDA8;
extern struct BattleEntity *D_8009CD10;

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
s8 g_BattleHaltTimer;
u16 g_BattleHitActionPending;
u8 g_BattleActionAnimModes[4];
s8 g_BattleTargetCycleMode;
u8 g_BattleSlotCancelPending;
u8 g_BattleRemainingActions;
int g_BattleAttackPmSlot;
s32 g_BattleAilmentPmSlot;
s32 g_BattleEffectPmSlot;
u8 g_BattleHitPending;
unsigned char g_BattleActionQueueCount;
s8 g_BattleTargetIndex;

void Battle_BeginPlayerAction(void) {
    BattleEntity *player;
    Combatant *actor;
    int mask;
    int flags;
    u32 *entityFlags;
    s32 *stateFlags;

    mask = -0x101;
    D_8009CDA8 |= 1;
    player = D_8009CD10;
    entityFlags = &player->entityFlags;
    flags = *entityFlags;
    actor = g_ActiveActor;
    flags &= mask;
    *entityFlags = flags;
    stateFlags = &actor->stateFlags;
    *stateFlags |= 0x10000;
    g_BattleHitActionPending = 0;
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
    D_8009CDA8 &= -2;
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
        g_BattleHaltTimer = 0;
        flags |= 0x2000;
        state2->stateFlags = flags;
        actor2->entityFlags &= ~0x100;
        Battle_FlushScriptSounds();
    }
}

void Battle_InitEntityColors(void) {
    s32 flags;
    s32 *stateFlags;

    Entity_SetActionMode(D_8009CD10, g_ActiveActor->actionMode12);
    stateFlags = &g_ActiveActor->stateFlags;
    flags = *stateFlags;
    g_BattleHaltTimer = 0;
    *stateFlags = flags & ~0x2000;
    g_BattleHitActionPending = 0;
    g_BattlePeGauges[0][1].r0 = 0xFF;
    g_BattlePeGauges[0][1].g0 = 0x3D;
    g_BattlePeGauges[0][1].b0 = 0x81;
    g_BattlePeGauges[0][1].r1 = 0x83;
    g_BattlePeGauges[0][1].g1 = 0x13;
    g_BattlePeGauges[0][1].b1 = 1;
    g_BattlePeGauges[0][1].r2 = 0xFF;
    g_BattlePeGauges[0][1].g2 = 0x3D;
    g_BattlePeGauges[0][1].b2 = 0x81;
    g_BattlePeGauges[0][1].r3 = 0x83;
    g_BattlePeGauges[0][1].g3 = 0x13;
    g_BattlePeGauges[0][1].b3 = 1;
    g_BattlePeGauges[1][1].r0 = 0xFF;
    g_BattlePeGauges[1][1].g0 = 0x3D;
    g_BattlePeGauges[1][1].b0 = 0x81;
    g_BattlePeGauges[1][1].r1 = 0x83;
    g_BattlePeGauges[1][1].g1 = 0x13;
    g_BattlePeGauges[1][1].b1 = 1;
    g_BattlePeGauges[1][1].r2 = 0xFF;
    g_BattlePeGauges[1][1].g2 = 0x3D;
    g_BattlePeGauges[1][1].b2 = 0x81;
    g_BattlePeGauges[1][1].r3 = 0x83;
    g_BattlePeGauges[1][1].g3 = 0x13;
    g_BattlePeGauges[1][1].b3 = 1;
}

unsigned char g_BattleActionQueueCount;
unsigned char g_BattleActionIndex;
unsigned char g_BattleRemainingActions;
signed char g_BattleActiveTurnSlot;
signed char g_BattleItemMenuState;

void Save_ResetGlobalFlags(void) {
    g_BattleActionQueueCount = 0;
    g_BattleActionIndex = 0;
    g_BattleRemainingActions = 0;
    g_BattleActiveTurnSlot = 0;
    g_BattleItemMenuState = 0;
}

unsigned char g_BattleActionQueueCount;

/* MASPSX_FORCE_G0 keeps the battle pointers and the effect result absolute,
 * as retail; the COMMON asset result supplies the small-data metadata of the
 * one $gp-relative word. */
int g_BattleAttackPmSlot;

#define NULL ((void *)0)


/* MASPSX_FORCE_G0 keeps the game-state flags absolute, as retail; the COMMON
 * script sound cursor and count supply the small-data metadata of the two
 * $gp-relative bytes. */
u8 g_BattleActionIndex;
u8 g_BattleActionQueueCount;