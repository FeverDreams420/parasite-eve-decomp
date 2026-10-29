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
void Battle_CycleTarget(s8 mode) {
    if (g_BattleTargetCount >= 2) {
        if (mode != 1 && mode != 2) {
            if (g_FieldPadPressed & 0x10) {
                Battle_MarkActiveEntities(g_BattleTargetList, g_BattleTargetIndex);
                g_BattleTargetIndex = (g_BattleTargetIndex + 1) % g_BattleTargetCount;
                Battle_InitFadeVars();
                PLAY_TABLE_SOUND(0x44E)
            }
            if (g_FieldPadPressed & 0x40) {
                Battle_MarkActiveEntities(g_BattleTargetList, g_BattleTargetIndex);
                g_BattleTargetIndex = (g_BattleTargetIndex + g_BattleTargetCount - 1) % g_BattleTargetCount;
                Battle_InitFadeVars();
                PLAY_TABLE_SOUND(0x44E)
            }
        }
    } else if (!g_BattleTargetCount) mode = 8;
    switch (mode) {
    case 0: case 1: case 2: case 3: {
        s8 outOfRange;
        BattleEntity *actor;
        Render_AnimationFrame();
        if ((g_ActiveActor->stateFlags & AYA_DARKNESS_MASK) == AYA_DARKNESS)
            outOfRange = g_BattleTargetList[g_BattleTargetIndex].dist > AYA_DARKNESS_RANGE;
        else
            outOfRange = g_ActiveActor->action->range < g_BattleTargetList[g_BattleTargetIndex].dist;
        actor = g_BattleTargetList[g_BattleTargetIndex].actor;
        Battle_DrawStatusOverlay(&actor->renderObject, mode, outOfRange,
            ((EnemyCombatant *)actor->core)->lowAndEntityId.bytes.entityId);
        break;
    }
    case 4: case 5: case 6: case 7: {
        BattleEntity *actor = g_BattleTargetList[g_BattleTargetIndex].actor;
        Battle_DrawStatusOverlay(&actor->renderObject, mode, 0,
            ((EnemyCombatant *)actor->core)->lowAndEntityId.bytes.entityId);
        break;
    }
    case 8: Battle_DrawStatusOverlay(0, 8, 0, 0); break;
    }
    Battle_StepPlayerTurn(g_BattleTargetList, g_BattleTargetIndex, mode);
}

u8 g_BattleHitPending;
u8 g_BattleRemainingActions;
u8 g_BattleActionAnimModes[4];
u8 g_BattleActionQueueCount;

void Battle_SetupEntityTarget(BattleEntity *actor) {
    BattleEntity *target;
    g_BattleHitPending = 0;
    g_BattleRemainingActions = g_ActiveActor->action->turnWord & 15;
    g_BattleActionAnimModes[0] = g_ActiveActor->action->animMode[0];
    g_BattleActionAnimModes[1] = g_ActiveActor->action->animMode[1];
    g_BattleActionAnimModes[2] = g_ActiveActor->action->animMode[2];
    g_BattleActionAnimModes[3] = g_ActiveActor->action->animMode[3];
    target = 0;
    if ((actor->entityFlags & 0x40000000) && !((EnemyCombatant *)actor->core)->deathPersist) {
        for (target = (BattleEntity *)g_FieldActorListHead; target; target = target->next) {
            EnemyCombatant *enemy;
            if (target == D_8009CD10 || target == actor) continue;
            enemy = target->core;
            if (enemy && enemy->hpAlive > 0 &&
                /* Retail tests actor here, not the candidate target. */
                target->teamId == actor->teamId && !(actor->entityFlags & 0x40000000)) break;
        }
    } else if ((actor->entityFlags & 0x6000) && ((EnemyCombatant *)actor->core)->field04.bytes.kind == 3) {
        for (target = (BattleEntity *)g_FieldActorListHead; target; target = target->next) {
            EnemyCombatant *enemy;
            if (target == D_8009CD10 || target == actor) continue;
            enemy = target->core;
            if (enemy && enemy->hpAlive > 0 &&
                target->parent == actor && enemy->field04.bytes.kind == 1) break;
        }
    } else if (!(actor->entityFlags & 0x4000)) target = actor;
    if (target) {
        unsigned int mode = g_ActiveActor->action->turnWord & 0xC0;
        if (mode == 0xC0 || mode == 0x40) {
            BattleInitSlot *slot;
            g_BattleRemainingActions = 0;
            slot = &g_BattleActionQueue[g_BattleActionQueueCount];
            slot->actor = target;
            slot->command = 2;
            slot->actionGeneration = g_BattleActiveTurnSlot;
            g_BattleActionQueueCount++;
        } else {
            while (g_BattleRemainingActions) {
                BattleInitSlot *slot;
                s16 actionGeneration = g_BattleActiveTurnSlot;
                g_BattleRemainingActions--;
                slot = &g_BattleActionQueue[g_BattleActionQueueCount];
                slot->actor = target;
                slot->command = 1;
                slot->actionGeneration = actionGeneration;
                g_BattleActionQueueCount++;
            }
        }
        Battle_DispatchEntityEffect();
        g_ActiveActor->stateFlags |= 0x200000;
    }
}

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

inline static void Mark(BattleEntity *entity)
{
  int kind = ((Combatant *)entity->core)->field04.bytes.kind;
  if (kind == 1)
  {
    entity = entity->parent;
  }
  else
    if (kind == 4)
  {
    BattleEntity *active;
    entity = (BattleEntity *)g_FieldActorListHead;
    if (!entity)
      return;
    active = D_8009CD10;
    while (entity != 0)
    {
      if (((entity != active) && ((entity->core) != 0)) && ((((Combatant *)entity->core)->field04.bytes.kind) == 4))
      {
        entity->renderObject.flags_9C |= 0x20;
      }
      entity = entity->next;
    }

    return;
  }
  entity->renderObject.flags_9C |= 0x20;
}

inline static void MarkGroup(BattleEntity *entity, int group_kind, BattleEntity *active)
{
  int kind = ((Combatant *)entity->core)->field04.bytes.kind;
  if (kind == 1) entity = entity->parent;
  else if (kind == group_kind) {
    entity = (BattleEntity *)g_FieldActorListHead;
    while (entity != 0) {
      if (entity != active && entity->core != 0 && ((Combatant *)entity->core)->field04.bytes.kind == group_kind)
        entity->renderObject.flags_9C |= 0x20;
      entity = entity->next;
    }
    return;
  }
  entity->renderObject.flags_9C |= 0x20;
}

inline static void MarkKinds(BattleEntity *entity, int group_kind, int linked_kind, BattleEntity *active)
{
  int kind = ((Combatant *)entity->core)->field04.bytes.kind;
  if (kind == linked_kind) entity = entity->parent;
  else if (kind == group_kind) {
    entity = (BattleEntity *)g_FieldActorListHead;
    while (entity != 0) {
      if (entity != active && entity->core != 0 && ((Combatant *)entity->core)->field04.bytes.kind == group_kind)
        entity->renderObject.flags_9C |= 0x20;
      entity = entity->next;
    }
    return;
  }
  entity->renderObject.flags_9C |= 0x20;
}

/* MASPSX_FORCE_G0 keeps the battle words absolute, as retail; the COMMON
 * fade level supplies the small-data metadata of the one $gp-relative byte. */
u8 g_BattleFadeLevel;