/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
#include "pe1/battle_runtime.h"
#include "pe1/inventory.h"
#include "pe1/random.h"
#include "pe1/gte_types.h"

/* COMMON supplies small-data metadata to stock MASPSX. Existing retail
 * data/linker symbols provide storage; these do not allocate new globals. */
u8 D_8009D274, D_8009D1D4, D_8009D294;
s8 g_EvasionStatus, g_EvasionReaction;

/* Apply the evasion outcome Battle_CheckEvasion left in g_EvasionStatus and
 * g_EvasionReaction to one target: a resolved hit sets the hit state, the
 * weapon category and the reaction in the core flags; an unresolved one
 * opens the floating panel.
 * The steal path reaches this through jump cross-jumping into the
 * single-target copy, as in retail. */
static inline void ApplyEvasionOutcome(BattleEntity *target)
{
    EnemyCombatant *core = target->core;
    if (g_EvasionStatus == 1)
    {
        EnemyCoreFlagBits *bits = (EnemyCoreFlagBits *)&core->coreFlags;
        bits->hitState = 1;
        bits->category = D_8009D278->action->attackWord >> 20;
        bits->reaction = ((BattleEvasionReaction *)&g_EvasionReaction)->value;
    }
    else if (g_EvasionStatus == 0)
    {
        core->panelC_val = -1;
        core->panelC_timer = 0x1E;
        core->panelC_x = target->renderObject.projected_target_x;
        core->panelC_y = target->renderObject.projected_target_y - 0x14;
    }
}

/* Resolve the timed hit for stealing, chained targets, cone attacks or a single
 * target. */
void Battle_ResolveHitOnTimer(void)
{
    BattleAction *action;
    BattleInitSlot *selectedSlot;
    BattleEntity *target;
    s16 actionId;
    s32 stealMode;
    u8 hitTimer;
    hitTimer = D_8009D274 + 1;
    action = D_8009D278->action;
    selectedSlot = &D_800BE830[D_8009D1D4];
    D_8009D274 = hitTimer;
    if ((hitTimer & 0xFF) != action->hitTrigger)
        return;
    actionId = action->actionCode.actionId;
    if (actionId == 8)
    {
        Battle_CheckEvasion(selectedSlot->actor, (BattleEvasionOutcome *)&g_EvasionStatus, 0);
        if (g_EvasionStatus == 1)
        {
            BattleEntity *thief = selectedSlot->actor;
            Asset_Find08Alt(0x46D, 0, thief->renderObject.target_x, thief->renderObject.target_y, thief->renderObject.target_z);
            if (D_8009D278->action->turnWord & 0x6000)
            {
                stealMode = (((EnemyCombatant *)selectedSlot->actor->core)->statusFlags2 >> 0xC) & 3;
                switch (stealMode)
                {
                    /* Retail switch shares the steal body with case 0. */
                case 0:
                    if (!(rand() & 1))
                    {
                    case 2:
                        if (Inv_AddItem(((EnemyCombatant *)selectedSlot->actor->core)->stealItemId) != 0)
                        {
                            EnemyCombatant *enemy = selectedSlot->actor->core;
                            D_8009D1CE = 1;
                            D_8009D1F8 = Inv_GetItemEffectData(enemy->stealItemId, 2);
                        }
                        else
                        {
                            D_8009D1CE = 1;
                            D_8009D1F8 = Inv_GetItemEffectData(((EnemyCombatant *)selectedSlot->actor->core)->stealItemId, 1);
                            ((EnemyCombatant *)selectedSlot->actor->core)->stealItemId = 0;
                        }
                    }
                    break;
                }
            }
        }
        ApplyEvasionOutcome(selectedSlot->actor);
    }
    else if (actionId == 6)
    {
        s16 reaction;
        Battle_CheckEvasion(selectedSlot->actor, (BattleEvasionOutcome *)&g_EvasionStatus, 0);
        ApplyEvasionOutcome(selectedSlot->actor);
        reaction = g_EvasionReaction;
        if (g_EvasionStatus == 1)
        {
            for (target = (BattleEntity *)D_8009D20C; target != 0; target = target->next)
            {
                EnemyCombatant *core;
                u32 flags;
                if (target == D_8009D254)
                    continue;
                core = target->core;
                if (core == 0)
                    continue;
                flags = target->entityFlags;
                if (((flags & 0x2040) != 0x40) && !(flags & 0x4000) && (core->hpAlive > 0) && (target != selectedSlot->actor))
                {
                    Battle_CheckEvasion(target, (BattleEvasionOutcome *)&g_EvasionStatus, reaction);
                    ApplyEvasionOutcome(target);
                }
            }
        }
    }
    else if (((action->turnWord & 0xC0) == 0x80) && !(D_8009D278->stateFlags & 0x100000))
    {
        u16 playerAngle = D_8009D254->facingAngle;
        s16 centered;
        s32 lowerBound;
        s32 upperBound;
        upperBound = playerAngle - 0x800;
        centered = upperBound;
        if (centered < -0x600)
        {
            upperBound = playerAngle + 0x600;
            lowerBound = playerAngle - 0x600;
        }
        else if (centered < 0x600)
        {
            lowerBound = playerAngle - 0xA00;
            upperBound = playerAngle - 0x600;
        }
        else
        {
            lowerBound = playerAngle - 0x1600;
            upperBound = playerAngle - 0xA00;
        }
        target = (BattleEntity *)D_8009D20C;
        if (target != 0)
        {
            s32 upper = (s16)upperBound;
            s32 lower = (s16)lowerBound;
            s32 facing = centered;
            int upperRange = facing < 0x600;
            do
            {
                BattleEntity *player = D_8009D254;
                EnemyCombatant *core;
                u32 flags;
                s32 angle;
                if (target == player)
                    continue;
                core = target->core;
                if (core == 0)
                    continue;
                flags = target->entityFlags;
                if (((flags & 0x2040) == 0x40) || (flags & 0x4000) || (core->hpAlive <= 0))
                    continue;
                angle = ratan2(target->renderObject.target_x - player->posX.parts.integer, target->renderObject.target_z - player->posZ.parts.integer);
                if (facing >= -0x600 && upperRange)
                {
                    if ((s16)angle < lower || upper < (s16)angle)
                        continue;
                }
                else
                {
                    if ((s16)angle < upper && lower < (s16)angle)
                        continue;
                }
                Battle_CheckEvasion(target, (BattleEvasionOutcome *)&g_EvasionStatus, 0);
                ApplyEvasionOutcome(target);
            } while ((target = target->next) != 0);
        }
    }
    else if (selectedSlot->actor != 0)
    {
        Battle_CheckEvasion(selectedSlot->actor, (BattleEvasionOutcome *)&g_EvasionStatus, 0);
        ApplyEvasionOutcome(selectedSlot->actor);
    }
    D_8009D294 = 0;
    D_8009D274 = 0;
}
