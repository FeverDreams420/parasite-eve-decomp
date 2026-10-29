/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "pe1/akao.h"
#include "pe1/asset_find.h"
#include "pe1/battle.h"
#include "pe1/battle_cmd.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/battle_update.h"
#include "pe1/menu_inventory.h"
#include "pe1/player_entity.h"
#include "pe1/random.h"
#include "pe1/render_object.h"
#include "pe1/save.h"
#include "pe1/scene_transition.h"
extern struct BattleEntity *D_8009CD10;


s8 g_BattleTargetIndex;

#define PLAY_TABLE_SOUND(command) \
    { if (g_AkaoBgmHandle[0]) Akao_SendTableCommand(g_AkaoBgmHandle[0], command, 0, 0x80, 0x7F); }


u8 g_BattleHitPending;
u8 g_BattleRemainingActions;
u8 g_BattleActionAnimModes[4];
u8 g_BattleActionQueueCount;

s8 g_BattleActiveTurnSlot, g_BattleItemMenuState, g_BattleTargetIndex;
u8 g_BattleRemainingActions, g_BattleSlotCancelPending, g_BattleActionQueueCount;

u8 g_BattleActionQueueCount, g_BattleRemainingActions;
u8 g_BattleSlotCancelPending, g_AyaActionStep;
s8 g_BattleTargetCycleMode;
u16 g_BattlePendingCommand;
s8 g_BattleTargetIndex;

static inline void StoreSlot(BattleInitSlot *slot, BattleEntity *actor, s16 kind, s16 actionGeneration) {
    slot->actor = actor;
    slot->command = kind;
    slot->actionGeneration = actionGeneration;
}

static inline void SetSlot(int index, BattleEntity *actor, s16 kind, s16 actionGeneration) {
    StoreSlot(&g_BattleActionQueue[index], actor, kind, actionGeneration);
}

/* COMMON supplies small-data metadata to MASPSX. */
u8 g_BattleActionQueueCount, g_BattleSlotCancelPending;
u8 g_BattleRemainingActions;

/* Step the turn index back to the first slot of the action that ends at
 * initial. This revision has no slot-zero guard: the first slot always
 * starts a new action generation. The halfword parameter makes the rewind
 * compute its own initial - 1 instead of reusing the one from the command
 * lookup. */
static inline void RewindTurn(s16 initial)
{
    g_BattleActionQueueCount = initial - 1;
    while (g_BattleActionQueue[g_BattleActionQueueCount].actionGeneration ==
           g_BattleActionQueue[g_BattleActionQueueCount - 1].actionGeneration)
        g_BattleActionQueueCount--;
}

/* Shared level and direction for the pulsing target highlight. Retail linker
 * objects own the storage; these retain the matching COMMON metadata. */
u8 g_BattleFadeLevel;
s8 g_BattleFadeStep;

/* Fade the targeted actor; group members (kind 4) fade together, linked parts
 * (kind 1) fade their parent. The level is evaluated at the call. */
#define FADE_TARGET(target, level)                                                       \
    {                                                                                    \
        BattleEntity *entity = (target);                                                 \
        BattleEntity *actor;                                                             \
        int kind = ((EntityAnimEventCore *)entity->core)->kind;               \
        if (kind == 1) {                                                                 \
            Render_FadeEntityColor(&entity->parent->renderObject, (level), (level),      \
                                   (level));                                             \
        } else if (kind == 4) {                                                          \
            for (actor = (BattleEntity *)g_FieldActorListHead; actor != 0; actor = actor->next) {                  \
                if (actor != D_8009CD10 && actor->core != 0 &&                           \
                    ((Combatant *)actor->core)->field04.bytes.kind == 4) {        \
                    Render_FadeEntityColor(&actor->renderObject, g_BattleFadeLevel,      \
                                           g_BattleFadeLevel, g_BattleFadeLevel);        \
                }                                                                        \
            }                                                                            \
        } else {                                                                         \
            Render_FadeEntityColor(&entity->renderObject, (level), (level), (level));    \
        }                                                                                \
    }


/* MASPSX_FORCE_G0 keeps the battle globals absolute; the COMMON fade step
 * g_BattleFadeStep supplies the small-data metadata of the one $gp-relative byte. */
s8 g_BattleFadeStep;

/* MASPSX_FORCE_G0 keeps the battle words absolute, as retail; the COMMON
 * fade level supplies the small-data metadata of the one $gp-relative byte. */
u8 g_BattleFadeLevel;
void Battle_CycleTarget(s8 mode);
void Battle_InitFadeVars(void);



int Battle_StepEnemyTurn(s8 mode) {
    if (mode == 1) {
        int command = g_MenuFrameCommand;
        s16 queuedCommand = g_MenuFrameCommand;
        if (command > 0) {
            if (command < 387) {
                SetSlot(g_BattleActionQueueCount, D_8009CD10, queuedCommand, g_BattleActiveTurnSlot);
                g_BattleActionQueueCount++;
                g_BattleActiveTurnSlot--;
                g_BattleTargetCycleMode = 0;
                mode = 0;
            } else if (command < 407) {
                switch (command) {
                case 393:
                    Battle_InitFadeVars();
                    g_BattleTargetCycleMode = 4;
                    mode++;
                    break;
                case 394:
                    Battle_InitFadeVars();
                    g_BattleTargetCycleMode = 5;
                    mode++;
                    break;
                case 395:
                    Battle_InitFadeVars();
                    g_BattleTargetCycleMode = 6;
                    mode++;
                    break;
                case 397:
                    Battle_InitFadeVars();
                    g_BattleTargetCycleMode = 7;
                    mode++;
                    break;
                case 406:
                    Battle_InitFadeVars();
                    g_AyaActionStep = 0;
                    for (mode = 0; (u8)mode < 7; mode++) {
                        int index = g_BattleActionQueueCount;
                        int randomValue = rand();
                        SetSlot(index, g_BattleTargetList[randomValue % g_BattleTargetCount].actor,
                                g_MenuFrameCommand, g_BattleActiveTurnSlot);
                        g_BattleActionQueueCount++;
                    }
                    g_BattleActiveTurnSlot--;
                    g_BattleTargetCycleMode = 0;
                    mode = 0;
                    break;
                default:
                    SetSlot(g_BattleActionQueueCount, D_8009CD10, g_MenuFrameCommand, g_BattleActiveTurnSlot);
                    g_BattleActiveTurnSlot--;
                    g_BattleActionQueueCount++;
                    g_BattleTargetCycleMode = 0;
                    mode = 0;
                    break;
                }
            } else if (command < 409) {
                SetSlot(g_BattleActionQueueCount, D_8009CD10, queuedCommand, g_BattleActiveTurnSlot);
                if (!(g_GameStateFlags & 2)) {
                    g_ActiveActor = D_8009CD10->core;
                    Asset_Find08Alt(0x453, 1, D_8009CD10->posX.parts.integer,
                                    D_8009CD10->posY.parts.integer,
                                    D_8009CD10->posZ.parts.integer);
                }
                switch (command) {
                case 407:
                    Battle_CopyPadStateToRecord();
                    Battle_SetupEnemyAnims();
                    g_ActiveActor->actionMode12 = 13;
                    break;
                case 408:
                    Battle_SyncEnemyAttributes();
                    break;
                }
                g_BattleActionQueueCount++;
                g_BattleActiveTurnSlot = 0;
                g_BattleTargetCycleMode = 0;
                mode = -1;
            } else {
                SetSlot(g_BattleActionQueueCount, D_8009CD10, queuedCommand, g_BattleActiveTurnSlot);
                g_BattleActionQueueCount++;
                g_BattleActiveTurnSlot = 0;
                g_BattleTargetCycleMode = 0;
                mode = -1;
            }
            if (!g_BattleTargetCycleMode)
                PLAY_TABLE_SOUND(0x44C)
            g_BattlePendingCommand = g_MenuFrameCommand;
        } else if (command == -1) {
            mode = 0;
        }
    } else if (mode == 2) {
        Battle_CycleTarget(g_BattleTargetCycleMode);
        if (g_FieldPadPressed & 0x200) {
            SetSlot(g_BattleActionQueueCount, g_BattleTargetList[g_BattleTargetIndex].actor, g_BattlePendingCommand,
                    g_BattleActiveTurnSlot);
            g_BattleActiveTurnSlot--;
            g_BattleActionQueueCount++;
            g_BattleTargetCycleMode = 0;
            mode = 0;
            PLAY_TABLE_SOUND(0x44C)
        } else if (g_FieldPadPressed & 0x400) {
            mode = 0;
            BattleCmd_UndoPending();
            g_BattleTargetIndex = 0;
            PLAY_TABLE_SOUND(0x44D)
        }
    }
    return mode;
}

void Battle_AdvanceTurnSlot(void)
{
    int savedIndex = g_BattleActionQueueCount;
    int kind;
    s16 i;
    s8 generation;
    g_BattleTargetIndex = 0;
    kind = g_BattleActionQueue[savedIndex - 1].command;
    if (kind == 1) {
        if (g_BattleRemainingActions == (g_ActiveActor->action->turnWord & 15)) {
            g_BattleActiveTurnSlot++;
            RewindTurn(savedIndex);
        } else {
            RewindTurn(savedIndex);
        }
        g_BattleRemainingActions = g_ActiveActor->action->turnWord & 15;
    } else if (kind == 2) {
        int mode = g_ActiveActor->action->turnWord & 0xC0;
        if (mode == 0xC0) {
            g_BattleActiveTurnSlot++;
            g_BattleActionQueueCount = savedIndex - (u8)g_BattleTargetCount;
        } else if (mode == 0x40) {
            int word;
            g_BattleActiveTurnSlot++;
            word = g_ActiveActor->action->turnWord;
            g_BattleActionQueueCount = savedIndex - (((word & 15) * 3) >> 1);
        }
        g_BattleRemainingActions = g_ActiveActor->action->turnWord & 15;
    } else if (kind < 0x197) {
        g_BattleActiveTurnSlot++;
        BattleCmd_UndoPending();
        do {
            g_BattleActionQueueCount--;
        } while (g_BattleActionQueue[g_BattleActionQueueCount].actionGeneration ==
                 g_BattleActionQueue[g_BattleActionQueueCount - 1].actionGeneration);
        g_BattleRemainingActions = g_ActiveActor->action->turnWord & 15;
    }
    generation = g_BattleActiveTurnSlot;
    for (i = 0; i < 45; i++) {
        if (g_BattleActionQueue[i].actionGeneration == generation) {
            g_BattleActionQueue[i].actor = 0;
            (&g_BattleActionQueue[i])->actionGeneration = 0;
            g_BattleActionQueue[i].command = 0;
        }
    }
}