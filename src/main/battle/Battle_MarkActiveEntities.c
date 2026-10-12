/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
#include "pe1/battle.h"
#include "pe1/battle_runtime.h"

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
    entity = (BattleEntity *)D_8009D20C;
    if (!entity)
      return;
    active = D_8009D254;
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
    entity = (BattleEntity *)D_8009D20C;
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
    entity = (BattleEntity *)D_8009D20C;
    while (entity != 0) {
      if (entity != active && entity->core != 0 && ((Combatant *)entity->core)->field04.bytes.kind == group_kind)
        entity->renderObject.flags_9C |= 0x20;
      entity = entity->next;
    }
    return;
  }
  entity->renderObject.flags_9C |= 0x20;
}

void Battle_MarkActiveEntities(BattleTarget *table, int selected_index)
{
  int mode;
  s16 center;
  s16 lower;
  s16 upper;
  int center_copy;
  int low_copy, high_copy;
  int inside;
  int linked_kind;
  int group_kind;
  int range_group;
  BattleEntity *cached_active;
  unsigned char slot;
  BattleEntity *active;
  if (D_8009D2B0 == 0)
  {
    return;
  }
  mode = (D_8009D278->action->turnWord >> 6) & 3;
  g_BattleFadeStep = 0;
  switch (mode)
  {
    case 0:
      Mark(table[(s8) selected_index].actor);
      break;

    case 2:
      center = table[(s8) selected_index].angle;

      if (center < (-0x600))
    {
      lower = center + 0x200;
      upper = center + 0xE00;
    }
    else
      if (center < 0x600)
    {
      lower = center - 0x200;
      upper = center + 0x200;
    }
    else
    {
      lower = center - 0xE00;
      upper = center - 0x200;
    }
      if (table[0].actor == 0)
    {
      return;
    }
      slot = 0;
      high_copy = upper;
      low_copy = lower;
      center_copy = center;
      inside = center < 0x600;
      range_group = 4;
      active = D_8009D254;
      cached_active = active;
      for (; table[slot].actor != 0; slot++)
    {
      s16 angle;
      if ((center_copy >= (-0x600)) && inside)
      {
        angle = table[slot].angle;
        if ((angle < low_copy) || (angle > high_copy))
        {
          continue;
        }
      }
      else
      {
        angle = table[slot].angle;
        if ((angle < high_copy) && (angle > low_copy))
        {
          continue;
        }
      }
      MarkGroup(table[slot].actor, range_group, cached_active);
    }

      break;

    case 1:

    case 3:
      if (table[0].actor == 0)
    {
      return;
    }
      slot = 0;
      linked_kind = 1;
      group_kind = 4;
      /* Retail materializes both kinds before loading the active actor. */
            active = D_8009D254;
      for (; table[slot].actor != 0; slot++)
    {
      MarkKinds(table[slot].actor, group_kind, linked_kind, active);
    }

      break;

  }

}
