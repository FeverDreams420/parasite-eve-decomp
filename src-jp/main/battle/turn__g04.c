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

static inline void FinishAction(void) {
    u8 i;
    if ((unsigned int)((u16)g_BattleActionQueue[0].command - 3) < 404 && D_8009CD10->actionMode >= 4)
        g_GameStateFlags |= 0x100;
    if ((unsigned int)((u16)g_BattleActionQueue[0].command - 387) >= 21) {
        for (i=0; i<g_BattleActionQueueCount; i++) {
            if ((unsigned int)((u16)g_BattleActionQueue[i].command - 1) < 2) {
                Battle_DispatchEntityEffect();
                break;
            }
        }
    }
    g_BattleGaugeCapLatch = 0;
    g_ActiveActor->hpAlive = 0;
    g_ActiveActor->stateFlags |= 0x200000;
    BattleCmd_ResetTableCursor();
}

u8 g_BattleActionQueueCount, g_BattleRemainingActions;
u8 g_BattleSlotCancelPending, g_AyaActionStep;
s8 g_BattleTargetCycleMode;
u16 g_BattlePendingCommand;
s8 g_BattleTargetIndex;

/* COMMON supplies small-data metadata to MASPSX. */
u8 g_BattleActionQueueCount, g_BattleSlotCancelPending;
u8 g_BattleRemainingActions;

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
void Battle_FillActionQueue(BattleTarget *target);
int Battle_StepEnemyTurn(s8 mode);
void Battle_AdvanceTurnSlot(void);
void Battle_InitFadeVars(void);
void Battle_MarkActiveEntities(BattleTarget *table, int selected_index);


int Battle_HandleItemMenu(void) {
    u8 active=1;
    u8 opened=0, cancelled=0;
    if (!g_BattleActiveTurnSlot) {
        g_BattleRemainingActions=0;
        active=0;
        FinishAction();
        Battle_MarkActiveEntities(g_BattleTargetList,g_BattleTargetIndex);
        PLAY_TABLE_SOUND(0x44C)
        return active;
    } else {
        if ((g_FieldPadPressed&0x2000) && !MenuWidget_FindByModeAndSelectedBase(1,0) ) {
            cancelled=1;
            if (g_BattleItemMenuState==1) MenuWidget_InitPool();
            else if (g_BattleItemMenuState==2) BattleCmd_UndoPending();
            if (--g_BattleActiveTurnSlot) {
                g_BattleRemainingActions=g_ActiveActor->action->turnWord&15;
            } else {
                active=0;
                FinishAction();
                Battle_MarkActiveEntities(g_BattleTargetList,g_BattleTargetIndex);
                PLAY_TABLE_SOUND(0x44C)
                return active;
            }
            PLAY_TABLE_SOUND(0x44C)
        }
        if (!g_BattleItemMenuState) {
            if (g_BattleActiveTurnSlot) {
                int mode;
                if (g_ActiveActor->action->actionCode.actionId==8)mode=3;
                else {
                    unsigned int actionMode=(g_ActiveActor->action->turnWord>>6)&3;
                    switch(actionMode) {
                    case 1: mode=1; break;
                    case 3: mode=2; break;
                    default: mode=0; break;
                    }
                }
                Battle_CycleTarget(mode);
                if ((g_FieldPadPressed&0x200)&&!cancelled) {
                    if(g_BattleTargetCount) {
                        Battle_FillActionQueue(&g_BattleTargetList[g_BattleTargetIndex]);
                        if(!g_BattleRemainingActions) {
                            g_BattleActiveTurnSlot--;
                            g_BattleRemainingActions=g_ActiveActor->action->turnWord&15;
                        }
                    }
                    PLAY_TABLE_SOUND(0x44C)
                }
            }
            if(g_BattleRemainingActions==(g_ActiveActor->action->turnWord&15) && (g_FieldPadPressed&0x80) && g_BattleActiveTurnSlot && !cancelled) {
                g_BattleItemMenuState=1;
                Battle_MarkActiveEntities(g_BattleTargetList,g_BattleTargetIndex);
                PLAY_TABLE_SOUND(0x44C)
                Inv_OpenAyaItemList(1);
                opened=1;
                Render_BeginSceneLoad();
            }
            if ((g_FieldPadPressed&0x400)&&!opened&&!cancelled) {
                Battle_MarkActiveEntities(g_BattleTargetList,g_BattleTargetIndex);
                if(g_BattleActionQueueCount) {Battle_AdvanceTurnSlot();Battle_InitFadeVars();}
                else active=0;
                PLAY_TABLE_SOUND(0x44D)
            }
        } else {
            g_BattleItemMenuState=Battle_StepEnemyTurn(g_BattleItemMenuState);
            if(g_BattleItemMenuState==-1) {
                g_BattleItemMenuState=0;
                active=0;
                FinishAction();
            } else if(!g_BattleItemMenuState) Battle_InitFadeVars();
        }
    }
    return active;
}