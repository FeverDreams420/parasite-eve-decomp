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

unsigned char g_BattleActionQueueCount;
unsigned char g_BattleActionIndex;
unsigned char g_BattleRemainingActions;
signed char g_BattleActiveTurnSlot;
signed char g_BattleItemMenuState;

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



void Battle_Init(void)
{
    u8 index;
    BattleInitSlot *records;
    BattleAction **action;
    BattleAction *config0;
    BattleAction *config1;
    BattleAction *config2;
    BattleAction *config3;
    BattleAction *config4;
    BattleAction *config5;

    Pm_StopAll();
    index = 0;
    records = g_BattleActionQueue;
    g_BattleAttackPmSlot = -1;
    g_BattleImpactPmSlot = -1;
    g_BattleEffectPmSlot = -1;
    g_BattleAilmentPmSlot = -1;

    do {
        g_BattleActionQueue[index].actor = 0;
        records[index].actionGeneration = 0;
        g_BattleActionQueue[index].command = 0;
        index++;
    } while (index < 45);

    action = &g_ActiveActor->action;
    config0 = *action;
    g_BattleTargetIndex = 0;
    g_BattleTargetCycleMode = 0;
    g_BattleHitPending = 0;
    config1 = *action;
    g_BattleActiveTurnSlot = (config0->turnWord >> 4) & 3;
    config2 = *action;
    g_BattleRemainingActions = config1->turnWord & 0xF;
    g_BattleActionAnimModes[0] = config2->animMode[0];
    config3 = *action;
    g_BattleActionAnimModes[1] = config3->animMode[1];
    config4 = *action;
    g_BattleActionAnimModes[2] = config4->animMode[2];
    config5 = *action;
    g_BattleActionAnimModes[3] = config5->animMode[3];

    Draw_ResetTextboxes();
    g_BattleSaveOverlayActive = 0;
    g_BattleResultFlags &= ~0x300;
    Battle_InitFadeVars();
}