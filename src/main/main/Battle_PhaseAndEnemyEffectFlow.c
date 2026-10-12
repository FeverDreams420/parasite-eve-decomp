/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: --expand-div -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/battle.h"
#include "pe1/gte_types.h"
#include "pe1/inventory.h"
#include "pe1/akao.h"
#include "pe1/entity_animation.h"
#include "pe1/pm.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/battle_main.h"
#include "pe1/battle_runtime.h"

#define NULL ((void *)0)
typedef struct { s32 words[4]; } Copy16;
typedef struct { u8 bytes[0x10]; } __attribute__((packed)) PackedCopy16;
typedef struct { u8 bytes[8]; } __attribute__((packed)) PackedCopy8;
typedef struct { u8 bytes[0x10]; } LargeSymbol;
typedef union { s32 value; } FlagValue;

int BattleCmd_CommitAmmoAndUpdate(); /* no prototype: retail calls it with and without the action */
s16 Battle_CalcAngleToTarget(RenderObjectEntity *object, void *target);
int Battle_CalcDistToPlayer(BattleEntity *actor, BattleEntity *player);
void Battle_DispatchEntityEffect(void);
void Battle_HaltOnPositiveX(void);
int Battle_StepAyaAction(void);
int Battle_StepCharacterAction(BattleInitSlot *slot);
void Battle_UpdateEntityFacing(BattleEntity *entity);
void Pm_StopAll(void);

/* COMMON supplies the small-data sizes of the turn queue words to MASPSX;
 * every other word stays absolute. */
u8 D_8009CE38[4];
u8 D_8009CE39;
u8 D_8009CE3A;
u8 D_8009CE3B;
u8 D_8009CE3C;
u8 D_8009D1D4;
s32 D_8009D208;
s32 D_8009D258;
s16 D_8009D27C;



void Battle_AdvancePhase(void) {
    Tbl20 sp18;
    BattleActionSoundTable sp30;
    BattleEntity *temp_v1_2;
    BattleEntity *var_a2;
    int targetFound;
    s16 temp_v0;
    s32 var_a2_2;
    s32 var_a3_2;
    s32 var_v0_5;
    s32 temp_a0_3;
    s32 temp_a0_4;
    BattleEntity *facingTarget;
    s32 temp_s0;
    s32 action;
    s32 command_value;
    s32 actor_flags;
    BattleEntity *character_state;
    s32 system_flags;
    FlagValue cleanup_flags;
    FlagValue loop_limit;
    s32 temp_v0_2;
    BattleEntity *soundActor;
    s32 temp_v1_3;
    s32 temp_v1_4;
    s32 var_a0_2;
    s32 var_s0_2;
    s32 var_s0_4;
    s32 var_v0_2;
    s32 var_v1;
    EnemyActionEffect *effect;
    s8 var_a0;
    u16 temp_a0;
    u16 var_a0_3;
    u32 temp_a2;
    u32 end_slot;
    u32 var_v1_2;
    u8 temp_v0_6;
    s32 var_s0;
    u8 var_s0_3;
    s32 var_v0_3;
    BattleEntity *queuedActor;
    BattleAction *queuedAction;
    BattleInitSlot *temp_s2;
    void *temp_t0;
    EnemyCombatant *temp_v0_4;
    BattleEntity *temp_v1_6;
    EnemyCombatant *temp_v1_8;
    void *var_a0_4;
    s32 var_a1;
    int firstSlot;
    int lastSlot;
    Combatant *actor;
    Combatant *cleanupActor;
    Combatant *aya_actor;
    BattleEntity *aya_player;
    u8 *copy_dst;
    const u8 *copy_src;
    const u8 *copy_end;

    var_a0 = D_8009D2A0;
    temp_a2 = D_8009D1D4;
    temp_s2 = &D_800BE830[temp_a2 & 0xFF];
    if (var_a0 == 0) {
        var_s0 = temp_a2;
        if ((temp_a2 & 0xFF) < (u8) D_8009CE3C) {
            do {
                temp_a0 = (u16)D_800BE830[var_s0 & 0xFF].command;
                if (temp_a0 - 3U < 0x180U) {
                    Inv_AddItem((s16) temp_a0 - 3);
                }
                var_s0 += 1;
            } while ((var_s0 & 0xFFU) < (u8) D_8009CE3C);
        }
        D_8009CE3C = 0;
        D_8009D1D4 = 0;
        D_8009D1A0 = D_8009D1A0 & ~0x100;
        return;
    }
    if (D_8009D278->stateFlags & 0x200000) {
        sp18 = D_8001074C;
        copy_dst = (u8 *)sp30.soundId;
        copy_src = (const u8 *)D_80010760.soundId;
        copy_end = copy_src + 0x20;
        if (((u32)copy_dst | (u32)copy_src) & 3) {
            do {
                *(PackedCopy16 *)copy_dst = *(const PackedCopy16 *)copy_src;
                copy_src += 0x10;
                copy_dst += 0x10;
            } while (copy_src != copy_end);
            *(PackedCopy8 *)copy_dst = *(const PackedCopy8 *)copy_src;
            command_value = temp_s2->command;
        }
        else
        {
        goto aligned_copy;
block_found:
        var_a2 = temp_v1_2;
        targetFound = 1;
        goto block_search_done;
aligned_copy:
        do {
            *(Copy16 *)copy_dst = *(const Copy16 *)copy_src;
            copy_src += 0x10;
            copy_dst += 0x10;
        } while (copy_src != copy_end);
copy_tail:
        *(PackedCopy8 *)copy_dst = *(const PackedCopy8 *)copy_src;
        command_value = temp_s2->command;
        }
after_command:
        action = command_value - 0x183;
        if (temp_s2->actor != D_8009D254) {
            temp_v0 = Battle_CalcAngleToTarget(&temp_s2->actor->renderObject, &D_8009D254->posX);
            var_a0_2 = D_8009D1D4;
            firstSlot = var_a0_2 & 0xFF;

            D_8009D254->facingAngle = temp_v0;
            targetFound = 0;
            if (D_800BE830[firstSlot].command - 0x183 == 0x13) {
                var_a2 = NULL;
                var_s0 = var_a0_2;
                lastSlot = firstSlot + 7;
                if (firstSlot < lastSlot) {
                    loop_limit.value = lastSlot;
                    do {
                        temp_v1_2 = D_800BE830[var_s0 & 0xFF].actor;
                        if ((temp_v1_2 == NULL) || (((EnemyCombatant *)temp_v1_2->core)->hpAlive <= 0) || (*(s32 *)&temp_v1_2->entityFlags & 0x4000)) {
                            var_s0 += 1;
                        } else {
                            goto block_found;
                        }
                    } while ((var_s0 & 0xFF) < loop_limit.value);
                }
block_search_done:
                if (!(targetFound & 0xFF)) {
                    var_v0_3 = D_8009D1D4 + 7;
                temp_v1_3 = var_v0_3 & 0xFF;
                temp_v1_3 <<= 3;
                temp_v1_3 = *(u16 *)((u8 *)&D_800BE830[0].command + temp_v1_3);
                D_8009D1D4 = var_v0_3;
                temp_v1_3 -= 3;
                temp_v1_3 = temp_v1_3 < 0x194U;
                if (!temp_v1_3 || ((u8) D_8009D254->actionMode < 4U)) {
                    D_8009D1A0 = D_8009D1A0 & ~0x100;
                }
                Entity_SetActionMode(((void *)D_8009D254), D_8009D278->actionMode12);
                D_8009D278->stateFlags |= 0x200000;
                return;
                }
                if (var_a2 != NULL) {
                    var_s0 = D_8009D1D4;
                    temp_v1_3 = var_s0 & 0xFF;
                    var_a0_2 = temp_v1_3 + 7;
                    if (temp_v1_3 < var_a0_2) {
                        var_a1 = var_a0_2;
                        do {
                            queuedActor = D_800BE830[temp_v1_3].actor;
                            if ((((EnemyCombatant *)queuedActor->core)->hpAlive <= 0) || (queuedActor == NULL)) {
                                D_800BE830[temp_v1_3].actor = var_a2;
                            }
                            var_s0 += 1;
                            temp_v1_3 = var_s0 & 0xFF;
                        } while (temp_v1_3 < var_a1);
                    }
                }
            }
            else
            {
            temp_v1_6 = D_800BE830[firstSlot].actor;
            if ((temp_v1_6 == NULL) || (((EnemyCombatant *)temp_v1_6->core)->hpAlive <= 0) || (*(s32 *)&temp_v1_6->entityFlags & 0x4000)) {
                var_v0_3 = D_8009D1D4 + 1;
block_35:
                temp_v1_3 = var_v0_3 & 0xFF;
                temp_v1_3 <<= 3;
                temp_v1_3 = *(u16 *)((u8 *)&D_800BE830[0].command + temp_v1_3);
                D_8009D1D4 = var_v0_3;
                temp_v1_3 -= 3;
                temp_v1_3 = temp_v1_3 < 0x194U;
                if (!temp_v1_3 || ((u8) D_8009D254->actionMode < 4U)) {
                    D_8009D1A0 = D_8009D1A0 & ~0x100;
                }
                Entity_SetActionMode(((void *)D_8009D254), D_8009D278->actionMode12);
                D_8009D278->stateFlags |= 0x200000;
                return;
            }
        }
            }
block_39:
        Pm_StopAll();
        if (action == 6) {
            BattleEntity *attackTarget;
            queuedAction = D_8009D278->action;
            if (!(*(s32 *)&queuedAction->attackWord & 0x3FF)) {
                BattleCmd_CommitAmmoAndUpdate(queuedAction);
            }
            D_8009D278->stateFlags |= 0x100000;
            attackTarget = temp_s2->actor;
            temp_s0 = attackTarget->renderObject.target_y - D_8009D27C;
            temp_v0_2 = ratan2(temp_s0, Battle_CalcDistToPlayer(attackTarget, ((void *)D_8009D254)));
            if (temp_v0_2 < -0xAB) {
                var_v1 = 0;
            } else {
                var_v1 = 2;
                if (temp_v0_2 < 0xE4) {
                    var_v1 = 1;
                }
            }
            Entity_SetActionMode(((void *)D_8009D254), *(var_v1 + D_8009D278->targetAngleActionModes));
            {
                s32 *stateFlags = &D_8009D278->stateFlags;
                *stateFlags &= 0xFFDFFFFF;
            }
            D_8009CE39 = 0x58;
            D_8009CE38[0] = 0;
            D_8009CE3A = 0x18;
            D_8009CE3B = 0;
        } else {
            Entity_SetActionMode(((void *)D_8009D254), 0xFU);
        }
        if (action != 0x13) {
            D_8009D258 = Scene_LoadRoomAssets(sp18.bytes[action], temp_s2->actor);
        }
        if ((action - 7U < 2U) || (action == 0xA)) {
            soundActor = temp_s2->actor;
            var_a0_3 = sp30.soundId[action];
            var_a2_2 = soundActor->renderObject.target_x;
            var_a3_2 = soundActor->renderObject.target_y;
            var_v0_5 = soundActor->renderObject.target_z;
        } else {
            var_a0_3 = sp30.soundId[action];
            var_a2_2 = D_8009D254->posX.parts.integer;
            var_a3_2 = D_8009D254->posY.parts.integer;
            var_v0_5 = D_8009D254->posZ.parts.integer;
        }
        ((int (*)(int, int, int, int, int))Akao_SendPositionalCmdStereo)(var_a0_3, 0, var_a2_2, var_a3_2, var_v0_5);
        var_a1 = 0xFFDFFFFF;
        var_a0_4 = ((void *)D_8009D278);
        actor_flags = ((Combatant *)var_a0_4)->stateFlags;
        D_8009D1A0 = D_8009D1A0 | 0x100;
        actor_flags &= var_a1;
        ((Combatant *)var_a0_4)->stateFlags = actor_flags;
    }
block_55:
    actor = ((void *)D_8009D278);
    actor_flags = actor->stateFlags;
    if (actor_flags & 0x80000) {
        if ((Battle_StepAyaAction() << 0x18) != 0) {
            end_slot = D_8009D1D4;
            aya_actor = ((void *)D_8009D278);
            aya_player = ((void *)D_8009D254);
            aya_actor->exp_or_acc = 0;
            aya_player->entityFlags &= ~0x100;
            var_s0_4 = end_slot - 7;
            var_a2_2 = var_s0_4 & 0xFF;
            if (var_a2_2 < end_slot) {
                do {
                    facingTarget = D_800BE830[var_a2_2].actor;
                    if (facingTarget != ((void *)D_8009D254)) {
                        Battle_UpdateEntityFacing(facingTarget);
                    }
                    var_s0_4 += 1;
                    var_a2_2 = var_s0_4 & 0xFF;
                } while ((unsigned)var_a2_2 < (u8) D_8009D1D4);
            }
            var_a0_4 = D_8009D20C;
            if (var_a0_4 != NULL) {
                do {
                    if (var_a0_4 != D_8009D254) {
                        temp_v0_4 = ((BattleEntity *)var_a0_4)->core;
                        if (temp_v0_4 != NULL) {
                            effect = temp_v0_4->effect;
                            if (effect != NULL) {
                                effect->state = 4;
                                temp_v1_8 = ((BattleEntity *)var_a0_4)->core;
                                temp_v1_8->coreFlags &= 0xC0FFFFFF;
                            }
                        }
                    }
                    var_a0_4 = ((BattleEntity *)var_a0_4)->next;
                } while (var_a0_4 != NULL);
            }
            D_8009D278->stateFlags &= 0xFFF7FFFF;
            Battle_HaltOnPositiveX();
        }
    } else if (actor_flags & 0x100000) {
        temp_v0 = Battle_CalcAngleToTarget(&temp_s2->actor->renderObject, ((void *)D_8009D254) + 0x28);
        D_8009D254->facingAngle = temp_v0;
        Battle_StepCharacterAction(temp_s2);
        if (g_PmSlotTable[D_8009D258].header.state - 1U >= 2U) {
            var_a2_2 = 0xFFEFFFFF;
            temp_a0_3 = -0x101;
            cleanupActor = ((void *)D_8009D278);
            character_state = ((void *)D_8009D254);
            cleanupActor->exp_or_acc = 0;
            *(s32 *)&character_state->entityFlags &= temp_a0_3;
            system_flags = D_8009D1A0;
            cleanup_flags.value = cleanupActor->stateFlags;
            system_flags &= temp_a0_3;
            cleanup_flags.value &= var_a2_2;
            D_8009D1A0 = system_flags;
            cleanup_flags.value |= 0x200000;
            cleanupActor->stateFlags = cleanup_flags.value;
            Battle_HaltOnPositiveX();
        }
    } else {
        if (D_8009D254->animLastFrame == D_8009D254->animPrev.parts.integer) {
            Entity_SetActionMode(((void *)D_8009D254), actor->actionMode12);
        }
        if (((g_PmSlotTable[D_8009D258].header.state - 1U >= 2U) || (temp_s2->command == 0x196)) && (D_8009D278->stateFlags |= 0x200000, Battle_ApplySpellEffect(temp_s2->command - 0x183, temp_s2->actor), (temp_s2->command != 0x196))) {
            temp_v0_6 = D_8009D1D4 + 1;
            D_8009D1D4 = temp_v0_6;
            if (((u16)D_800BE830[temp_v0_6 & 0xFF].command - 3U >= 0x194U) || ((u8) D_8009D254->actionMode < 4U)) {
                D_8009D1A0 = D_8009D1A0 & ~0x100;
            }
            temp_a0_4 = (u8)D_8009CE3C;
            for (var_s0_4 = D_8009D1D4; (var_s0_4 & 0xFFU) < (unsigned)temp_a0_4; var_s0_4++) {
                if ((u16)D_800BE830[var_s0_4 & 0xFF].command - 1U < 2U) {
                    Battle_DispatchEntityEffect();
                    return;
                }
            }
        }
    }
}

void Battle_StartEnemyAttackEffect(BattleEntity *entity)
{
    Combatant *core = (Combatant *)entity->core;
    u32 *status = (u32 *)&core->statusFlags2;
    int effect;

    if ((D_8009D278->action->turnWord & 0x400) && ((*status & 3) != 1)) {
        effect = Scene_LoadRoomAssets(7, entity);
        D_8009D208 = effect;
        Pm_SendCmd(effect, 0, 0, 2, 0, 0);
        Asset_Find08Alt(0x484, 0, entity->renderObject.target_x,
                        entity->renderObject.target_y, entity->renderObject.target_z);
        return;
    }
    if ((D_8009D278->action->turnWord & 0x300) == 0x300 &&
        ((*status & 0x3C000) != 0x14000)) {
        D_8009D208 = Scene_LoadRoomAssets(90, entity);
        Asset_Find08Alt(0x488, 0, entity->renderObject.target_x,
                        entity->renderObject.target_y, entity->renderObject.target_z);
        return;
    }
    if ((D_8009D278->action->turnWord & 0x100) &&
        ((*status & 0xC000) != 0x4000)) {
        D_8009D208 = Scene_LoadRoomAssets(88, entity);
        Asset_Find08Alt(0x488, 0, entity->renderObject.target_x,
                        entity->renderObject.target_y, entity->renderObject.target_z);
        return;
    }
    if ((D_8009D278->action->turnWord & 0x200) &&
        ((*status & 0x30000) != 0x10000)) {
        D_8009D208 = Scene_LoadRoomAssets(89, entity);
        Asset_Find08Alt(0x486, 0, entity->renderObject.target_x,
                        entity->renderObject.target_y, entity->renderObject.target_z);
        return;
    }
    if ((D_8009D278->action->turnWord & 0x800) &&
        ((*status & 0xC) != 4)) {
        effect = Scene_LoadRoomAssets(7, entity);
        D_8009D208 = effect;
        Pm_SendCmd(effect, 0, 0, 1, 0, 0);
        Asset_Find08Alt(0x482, 0, entity->renderObject.target_x,
                        entity->renderObject.target_y, entity->renderObject.target_z);
        return;
    }
    if ((D_8009D278->action->turnWord & 0x1000) &&
        ((*status & 0x30) != 0x10)) {
        effect = Scene_LoadRoomAssets(7, entity);
        D_8009D208 = effect;
        Pm_SendCmd(effect, 0, 0, 0, 0, 0);
        Asset_Find08Alt(0x480, 0, entity->renderObject.target_x,
                        entity->renderObject.target_y, entity->renderObject.target_z);
    }
}
