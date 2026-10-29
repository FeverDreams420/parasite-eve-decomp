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

void Battle_DispatchSpecialAction(int arg0) {
    BattleEntity *actor;

    if ((g_GameStateFlags & 2) == 0) {
        actor = (BattleEntity *)g_PlayerEntity;
        g_ActiveActor = actor->core;
        Asset_Find08Alt(0x453, 1, actor->posX.parts.integer,
                       actor->posY.parts.integer, actor->posZ.parts.integer);
    }

    switch (arg0) {
    case 0x197:
        Battle_CopyPadStateToRecord();
        Battle_SetupEnemyAnims();
        g_ActiveActor->actionMode12 = 0xD;
        break;
    case 0x198:
        Battle_SyncEnemyAttributes();
        break;
    }
}

void Battle_DispatchInit(int arg0) {
    if (g_GameStateFlags & 2) {
        return;
    }

    g_ActiveActor = ((BattleEntity *)g_PlayerEntity)->core;

    switch (arg0) {
    case 0x197:
        Battle_CopyPadStateToRecord();
        Battle_SetupEnemyAnims();
        break;
    case 0x198:
        Battle_SyncEnemyAttributes();
        break;
    default:
        break;
    }
}

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