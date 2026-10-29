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



int Battle_RollEscapeChance(void) {
    BattleEntity *entity = (BattleEntity *)g_FieldActorListHead;
    s16 highestRank = 0;
    /* Retail narrows the probability after every multiplier. */
    s8 result = 0;
    for (; entity; entity = entity->next) {
        EnemyCombatant *enemy;
        if (entity == D_8009CD10) continue;
        enemy = entity->core;
        if (!enemy || enemy->hpAlive <= 0) continue;
        if (highestRank < (s8)enemy->field04.bytes.rank)
            highestRank = (s8)enemy->field04.bytes.rank;
        if (enemy->statusFlags2 & 0x40000) {
            result = -1;
            break;
        }
    }
    if (!result) {
        int difference = (s8)(g_ActiveActor->field04.bytes.rank - highestRank);
        if (difference >= 2) result = 80;
        else if (difference == 1) result = 40;
        else if (difference == 0) result = 40;
        else if (difference == -1) result = 25;
        else result = 15;
        if ((s16)g_ActiveActor->curHP * 10 < (s16)g_ActiveActor->maxHP)
            result = result * 3 / 2;
        switch (((unsigned int)g_ActiveActor->stateFlags >> 25) & 7) {
        case 1: result = result * 3 / 2; break;
        case 2: result *= 2; break;
        case 3: result *= 3; break;
        case 4: result *= 4; break;
        case 5: result = 100; break;
        }
        {
            int chance = result;
            result = rand() % 100 < chance;
        }
    }
    if (result <= 0) {
        Combatant *player;
        int attempts;
        g_BattleSaveOverlayActive = 1;
        if (result == -1)
            g_CurItemEffectData = g_EscapeBlockedTexts + Save_GetMetadataWindowIndex() * 14;
        else
            g_CurItemEffectData = g_EscapeFailedTexts + Save_GetMetadataWindowIndex() * 15;
        player = g_ActiveActor;
        attempts = ((unsigned int)player->stateFlags >> 25) & 7;
        if (attempts < 5)
            player->stateFlags = (player->stateFlags & 0xF1FFFFFF) |
                (((attempts + 1) & 7) << 25);
    }
    return result;
}