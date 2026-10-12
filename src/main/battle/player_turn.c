/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "pe1/battle_runtime.h"
#include "pe1/gte_types.h"
#include "pe1/battle_main.h"

/* COMMON supplies the small-data sizes of the turn queue words to MASPSX;
 * every other word stays absolute. */
u8 D_8009CE39;
u8 D_8009CE3B;
u8 D_8009CE3C;
u8 D_8009D1D4;
int D_8009D200;
s16 D_8009D27C;

int Battle_CalcDistToPlayer(BattleEntity *actor, BattleEntity *player);
s16 Battle_CalcAngleToTarget(RenderObjectEntity *object, void *target);
int Battle_StepCharacterAction(BattleInitSlot *slot);

#define CORE_HEALTH(entity) (((EnemyCombatant *)(entity)->core)->hpAlive)

void Battle_UpdatePlayerTurn(void)
{
    BattleInitSlot *slot = &D_800BE830[D_8009D1D4];
    BattleEntity *player = D_8009D254;
    BattleEntity *actor;
    Combatant *active;
    int angle;
    int category;
    u8 step;
    int diff;

    if (player->actionMode != 12) {
        active = D_8009D278;
        if (active->stateFlags & 0x200000) {
            BattleEntity *target = slot->actor;
            if (target->core != 0 && CORE_HEALTH(target) > 0) {
                diff = target->renderObject.target_y;
                diff -= D_8009D27C;
                angle = ratan2(diff, Battle_CalcDistToPlayer(target, player));
                if (angle < -0xAB) category = 0;
                else if (angle < 0xE4) category = 1;
                else category = 2;
                Entity_SetActionMode(D_8009D254, *(category + D_8009D278->targetAngleActionModes));
                D_8009D278->stateFlags &= ~0x200000;
            }
        }

        {
            BattleEntity *angle_actor = slot->actor;
            D_8009D254->facingAngle = Battle_CalcAngleToTarget(&angle_actor->renderObject, &D_8009D254->posX);
            step = Battle_StepCharacterAction(slot);
            if (step == 0) return;
            if (step != 1) return;
        }

        {
            if (slot->actor != D_800BE830[D_8009D1D4 + 1].actor ||
                CORE_HEALTH(slot->actor) <= 0) {
                u8 count = D_8009CE39;
                u8 delta = D_8009CE3B;
                register Combatant *effect asm("$4");
                                effect = D_8009D278;
                D_8009CE39 = count + delta;
                asm volatile("" : : );
                if (effect->action->attackWord & 0x3FF)
                    Battle_StartEnemyAttackEffect(slot->actor);
            }
        }

        D_8009D1D4++;
        if (D_8009D1D4 != D_8009CE3C) return;
        if (D_8009D278->action->actionCode.actionId != 8)
            Pm_SendCmd(D_8009D200, 0, 0, 2, 0, 0);
        {
            int sound = D_8009D2FC;
            if (sound != -1)
                Pm_SendCmd(sound, 0, 0, 2, 0, 0);
        }
        return;
    }

    if (player->animLastFrame != player->animPrev.parts.integer) return;
    actor = slot->actor;
    diff = actor->renderObject.target_y;
    diff -= D_8009D27C;
    angle = ratan2(diff, Battle_CalcDistToPlayer(actor, player));
    if (angle < -0xAB) category = 0;
    else if (angle < 0xE4) category = 1;
    else category = 2;
    Entity_SetActionMode(D_8009D254, *(category + D_8009D278->targetAngleActionModes));
}

#include "common.h"
#include "pe1/battle.h"

#define U8_AT(ptr, offset) (*(u8 *)((u8 *)(ptr) + (offset)))
#define S16_AT(ptr, offset) (*(s16 *)((u8 *)(ptr) + (offset)))
#define U16_AT(ptr, offset) (*(u16 *)((u8 *)(ptr) + (offset)))
#define U32_AT(ptr, offset) (*(u32 *)((u8 *)(ptr) + (offset)))
#define ACTION_AT(index) ((u16)D_800BE830[(u8)(index)].command)

void Battle_ApplyDamage(int action);

void Battle_ApplyPlayerHit(void) {
    u32 committed;
    register int mask asm("$5");
    unsigned int index;
    Combatant *actor;
    int flags;
    u16 action;

    committed = 0x200000;
    if (D_8009D278->stateFlags & committed) {
        unsigned int stateFlags;
        Entity_SetActionMode(D_8009D254, 0xE);
        Asset_Find08Alt(0x4B3, 0, D_8009D254->posX.parts.integer,
                        D_8009D254->posY.parts.integer, D_8009D254->posZ.parts.integer);

        mask = ~0x200000;
        actor = D_8009D278;
        stateFlags = actor->stateFlags;
        flags = D_8009D1A0;
        D_8009D1A0 = flags | 0x100;
        actor->stateFlags = stateFlags & mask;
    }

    if (D_8009D254->animLastFrame == D_8009D254->animPrev.parts.integer) {
        Entity_SetActionMode(D_8009D254, D_8009D278->actionMode12);
        actor = D_8009D278;
        index = D_8009D1D4;
        actor->stateFlags |= committed;

        Battle_ApplyDamage((s16)ACTION_AT(index) - 3);
        Scene_LoadRoomAssets(0x55, D_8009D254);

        {
            unsigned int nextIndex = D_8009D1D4 + 1;
            action = ACTION_AT(nextIndex);
            D_8009D1D4 = nextIndex;
        }

        if (action - 3U >= 0x194U || D_8009D254->actionMode < 4) {
            D_8009D1A0 = D_8009D1A0 & ~0x100;
        }
    }
}
