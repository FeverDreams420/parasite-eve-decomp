/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/battle.h"
#include "pe1/battle_cmd.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/battle_hud_colors.h"
#include "pe1/battle_reset_stats.h"
#include "pe1/battle_runtime.h"
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
s8 D_8009CE30;
u16 D_8009D298;
u8 D_8009CE38[4];
u8 D_8009CE39;
u8 D_8009CE3A;
u8 D_8009CE3B;
s8 D_8009CE40;
u8 D_8009CE60;
u8 D_8009D1DC;
int D_8009D200;
s32 D_8009D208;
s32 D_8009D258;
u8 D_8009D294;
u8 D_8009D2D8;
unsigned char g_BattleActionQueueCount;
s8 g_BattleTargetIndex;

void Battle_BeginPlayerAction(void) {
    BattleEntity *player;
    Combatant *actor;
    int mask;
    int flags;
    u32 *entityFlags;
    s32 *stateFlags;

    mask = -0x101;
    D_8009D2E8 |= 1;
    player = D_8009D254;
    entityFlags = &player->entityFlags;
    flags = *entityFlags;
    actor = D_8009D278;
    flags &= mask;
    *entityFlags = flags;
    stateFlags = &actor->stateFlags;
    *stateFlags |= 0x10000;
    D_8009D298 = 0;
    Entity_SetActionMode(player, 0x12);
    Battle_FlushScriptSounds();
}

void Battle_ReturnToIdle(void) {
    Combatant *state;
    BattleEntity *actor;
    int mask_state;
    int flags;
    s32 *stateFlags;

    mask_state = 0xFFFEFFFF;
    D_8009D2E8 &= -2;
    state = g_ActiveActor;
    actor = (BattleEntity *)g_PlayerEntity;
    stateFlags = &state->stateFlags;
    flags = *stateFlags;
    flags &= mask_state;
    *stateFlags = flags;
    actor->motionX = 0;
    actor->motionY = 0;
    actor->motionZ = 0;
    Entity_SetActionMode(actor, state->actionMode12);
}

void Battle_HaltOnPositiveX(void) {
    Combatant *state;
    BattleEntity *actor;
    Combatant *state2;
    BattleEntity *actor2;
    int flags;

    state = g_ActiveActor;
    if ((s16)state->curHP > 0) {
        actor = (BattleEntity *)g_PlayerEntity;
        actor->motionX = 0;
        actor->motionY = 0;
        actor->motionZ = 0;
        Entity_SetActionMode(actor, 0x12);

        state2 = g_ActiveActor;
        actor2 = (BattleEntity *)g_PlayerEntity;
        flags = state2->stateFlags;
        D_8009CE30 = 0;
        flags |= 0x2000;
        state2->stateFlags = flags;
        actor2->entityFlags &= ~0x100;
        Battle_FlushScriptSounds();
    }
}

void Battle_InitEntityColors(void) {
    s32 flags;
    s32 *stateFlags;

    Entity_SetActionMode(D_8009D254, D_8009D278->actionMode12);
    stateFlags = &D_8009D278->stateFlags;
    flags = *stateFlags;
    D_8009CE30 = 0;
    *stateFlags = flags & ~0x2000;
    D_8009D298 = 0;
    D_800B0158[0] = 0xFF;
    D_800B0159[0] = 0x3D;
    D_800B015A[0] = 0x81;
    D_800B0160[0] = 0x83;
    D_800B0161[0] = 0x13;
    D_800B0162[0] = 1;
    D_800B0168[0] = 0xFF;
    D_800B0169[0] = 0x3D;
    D_800B016A[0] = 0x81;
    D_800B0170[0] = 0x83;
    D_800B0171[0] = 0x13;
    D_800B0172[0] = 1;
    D_800B01A0[0] = 0xFF;
    D_800B01A1[0] = 0x3D;
    D_800B01A2[0] = 0x81;
    D_800B01A8[0] = 0x83;
    D_800B01A9[0] = 0x13;
    D_800B01AA[0] = 1;
    D_800B01B0[0] = 0xFF;
    D_800B01B1[0] = 0x3D;
    D_800B01B2[0] = 0x81;
    D_800B01B8[0] = 0x83;
    D_800B01B9[0] = 0x13;
    D_800B01BA[0] = 1;
}

unsigned char g_BattleActionQueueCount;
unsigned char g_BattleActionIndex;
unsigned char g_BattleRemainingActions;
signed char g_BattleActiveTurnSlot;
signed char g_BattleItemMenuState;

void Save_ResetGlobalFlags(void) {
    g_BattleActionQueueCount = 0;
    g_BattleActionIndex = 0;
    g_BattleRemainingActions = 0;
    g_BattleActiveTurnSlot = 0;
    g_BattleItemMenuState = 0;
}

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
    records = D_800BE830;
    D_8009D200 = -1;
    D_8009D2FC = -1;
    D_8009D258 = -1;
    D_8009D208 = -1;

    do {
        D_800BE830[index].actor = 0;
        records[index].actionGeneration = 0;
        D_800BE830[index].command = 0;
        index++;
    } while (index < 45);

    action = &D_8009D278->action;
    config0 = *action;
    g_BattleTargetIndex = 0;
    D_8009CE40 = 0;
    D_8009D294 = 0;
    config1 = *action;
    D_8009D2D8 = (config0->turnWord >> 4) & 3;
    config2 = *action;
    D_8009D1DC = config1->turnWord & 0xF;
    D_8009CE38[0] = config2->animMode[0];
    config3 = *action;
    D_8009CE39 = config3->animMode[1];
    config4 = *action;
    D_8009CE3A = config4->animMode[2];
    config5 = *action;
    D_8009CE3B = config5->animMode[3];

    Draw_ResetTextboxes();
    D_8009D1CE = 0;
    D_8009D1AC &= ~0x300;
    Battle_InitFadeVars();
    D_8009CE60 = 0;
}

unsigned char g_BattleActionQueueCount;

signed char Pad_GetMenuPressedBitOrDisabled(void) {
    if (g_ActiveActor->stateFlags & 0x10000) {
        return -1;
    }
    return (signed char)g_BattleActionQueueCount;
}

int Battle_IsActive(void) {
    if (g_BattleActionQueueCount != 0) {
        return 0;
    }
    if ((g_GameStateFlags & 2) == 0 || (g_ActiveActor->stateFlags & 0x10000) == 0) {
        return 1;
    }
    return 0;
}

int Pad_IsMenuConfirmAvailable(void) {
    if (g_BattleActionQueueCount != 0) {
        return 0;
    }
    if ((g_GameStateFlags & 2) == 0 || (g_ActiveActor->stateFlags & 0x10000) == 0) {
        return 1;
    }
    return 0;
}

/* MASPSX_FORCE_G0 keeps the battle pointers and the effect result absolute,
 * as retail; the COMMON asset result supplies the small-data metadata of the
 * one $gp-relative word. */
int D_8009D200;

void Battle_DispatchEntityEffect(void) {
    int action_id;
    BattleAction *action;
    BattleAction *action2;
    int action_id2;
    BattleEntity *next_entity;
    int next_id;
    int result;
    u32 word10;

    action = D_8009D278->action;
    action_id = action->actionCode.actionId;

    if (action_id == 6) {
        if ((action->attackWord & 0x3FF) == 0) {
            BattleCmd_CommitAmmoAndUpdate(action);
        }
        D_8009D200 = Scene_LoadRoomAssets(3, D_8009D254);
        return;
    }

    if (action_id == 8) {
        D_8009D2FC = Scene_LoadRoomAssets(6, D_8009D254);
        return;
    }

    if ((action->attackWord & 0x3FF) == 0) {
        BattleCmd_CommitAmmoAndUpdate(action);
    }

    action2 = D_8009D278->action;
    action_id2 = action2->actionCode.actionId;

    if (action_id2 == 5 || ((word10 = action2->turnWord) & 0x1F00) != 0) {
        result = Scene_LoadRoomAssets(0, D_8009D254);
        next_entity = D_8009D254;
        next_id = 5;
    } else {
        if ((action_id2 == 2) || ((word10 & 0xC0) == 0x80)) {
            result = Scene_LoadRoomAssets(1, D_8009D254);
        } else {
            result = Scene_LoadRoomAssets(2, D_8009D254);
        }
        next_entity = D_8009D254;
        next_id = 4;
    }

    D_8009D200 = result;
    D_8009D2FC = Scene_LoadRoomAssets(next_id, next_entity);
}

void Battle_CheckEvasion(BattleEntity *actor, BattleEvasionOutcome *outcome,
                         int attackType)
{
    BattleAction *action;
    int range;
    int distance;
    int roll;
    int threshold;
    int special;
    int hit;

    outcome->reaction = 0;
    if (!(actor->entityFlags & 0x4000)) {
    if (D_8009D278->stateFlags & 0x100000) {
        outcome->status = 1;
        outcome->reaction = 2;
        return;
    }

    outcome->status = 0;
    range = AYA_DARKNESS_RANGE;
    if ((D_8009D278->stateFlags & AYA_DARKNESS_MASK) != AYA_DARKNESS) {
        range = D_8009D278->action->range;
    }
    distance = Battle_CalcDistToPlayer(actor, D_8009D254) * 1000 / range;
    {
        int random = rand();
        int quotient = random / 100;
        roll = random - quotient * 100;
    }

    action = D_8009D278->action;
    if (action->actionCode.actionId == 8) {
        if (distance >= 1001) return;
        threshold = ((action->turnWord >> 17) & 1) * 20;
        if (distance >= 801) threshold += 5;
        else if (distance >= 501) threshold += 20;
        else threshold += 50;
        if ((s16)roll < threshold) outcome->reaction = 3;
        outcome->status = 1;
        return;
    }

    if (action->actionCode.actionId == 6 && attackType != 0) {
        outcome->status = 1;
        switch (attackType) {
        case 2:
            outcome->reaction = 1;
            break;
        case 3:
            outcome->reaction = 2;
            break;
        default:
            outcome->reaction = 4;
            break;
        }
        return;
    }

    if (distance >= 1801) return;
    if (distance >= 1501) {
    hit = (s16)roll < 25;
    if (!hit) return;
    outcome->status = 1;
    outcome->reaction = 1;
    return;
    }
below_1501:
    if (distance >= 1201) {
    hit = (s16)roll < 50;
    if (!hit) return;
    outcome->status = 1;
    outcome->reaction = 1;
    return;
    }
below_1201:
    if (distance >= 1001) {
    hit = (s16)roll < 80;
    if (!hit) return;
    outcome->status = 1;
    outcome->reaction = 1;
    return;

    }
close_range:
    special = (D_8009D278->action->turnWord >> 17) & 1;
    threshold = special * 20;
    if (distance >= 801) threshold += 5;
    else if (distance >= 501) threshold += 20;
    else threshold += 50;
    outcome->reaction = (s16)roll < threshold ? 3 : 2;
    outcome->status = 1;
    return;

    }
invalid:
    outcome->status = -1;
done:
    return;
}

#define NULL ((void *)0)

void Battle_BuildTargetList(void) {
    s32 entityFlags;
    register s32 nextIndex asm("$5");
    register s32 finalCount asm("$2");
    s32 pad_[2];
    s32 distance;
    s16 angle;
    EnemyCombatant *candidateCore;
    BattleEntity *player;
    BattleEntity *candidate;

    candidate = (BattleEntity *)g_FieldActorListHead;
    g_BattleTargetIndex = 0;
    if (candidate != NULL) {
        do {
            player = D_8009D254;
            if (candidate != player) {
                candidateCore = candidate->core;
                if (candidateCore != NULL) {
                    entityFlags = candidate->entityFlags;
                    if (((entityFlags & 0x2040) != 0x40) && !(entityFlags & 0x4000) &&
                        (candidateCore->hpAlive > 0)) {
                        g_BattleTargetList[g_BattleTargetIndex].actor = candidate;
                        distance = Battle_CalcDistToPlayer(candidate, player);
                        g_BattleTargetList[g_BattleTargetIndex].dist = distance;
                        angle = ratan2(
                            candidate->renderObject.target_x -
                                D_8009D254->posX.parts.integer,
                            candidate->renderObject.target_z -
                                D_8009D254->posZ.parts.integer);
                        nextIndex = g_BattleTargetIndex + 1;
                        g_BattleTargetList[g_BattleTargetIndex].angle = angle;
                        g_BattleTargetIndex = nextIndex;
                    }
                }
            }
            candidate = candidate->next;
        } while (candidate != NULL);
    }
    if (g_BattleTargetIndex >= 2) {
        Battle_SortTargets((BattleTargetWords *)g_BattleTargetList, 0, (s8) (g_BattleTargetIndex - 1));
    }
    finalCount = g_BattleTargetIndex;
    __asm__("" : "=r"(finalCount) : "0"(finalCount));
    g_BattleTargetList[finalCount].actor = NULL;
}

void Battle_SortTargets(BattleTargetWords *records, signed char first, signed char last)
{
    signed char pivot;
    signed char scan;

    if (first < last) {
        Battle_SwapRecords(records, first, (signed char)((first + last) / 2));
        pivot = first;
        for (scan = pivot; scan <= last; scan++) {
            if (g_BattleTargetList[(signed char)scan].dist < g_BattleTargetList[first].dist) {
                pivot++;
                Battle_SwapRecords(records, (signed char)pivot, (signed char)scan);
            }
        }
        Battle_SwapRecords(records, first, pivot);
        Battle_SortTargets(records, first, (signed char)(pivot - 1));
        Battle_SortTargets(records, (signed char)(pivot + 1), last);
    }
}

void Battle_SwapRecords(BattleTargetWords *records, int from_arg, int to_arg) {
    BattleTargetWords tmp;
    int from_index;
    int to_index;

    from_index = (signed char)from_arg;
    to_index = (signed char)to_arg;

    tmp = records[from_index];
    records[from_index] = records[to_index];
    records[to_index] = tmp;
}

/* Copy the action the battle menu chose into the player's action record. */
void Battle_CopyPadStateToRecord(void) {
    BattleAction *record = D_8009D278->action;
    BattleAttackBits *attack = (BattleAttackBits *)&record->attackWord;
    BattleTurnBits *turn = (BattleTurnBits *)&record->turnWord;

    record->powerBonus = D_800A76D8.powerBonus;
    record->range = D_800A76D8.range;
    record->field04 = D_800A76D8.field04;
    record->actionCode.actionId = D_800A76D8.actionCode.actionId;
    record->hitTrigger = D_800A76D8.hitTrigger;
    attack->count = D_800A76E4.count;
    attack->power = D_800A76E4.power;
    attack->category = D_800A76E4.category;
    turn->actions = D_800A76E8.actions;
    turn->slot = D_800A76E8.slot;
    turn->mode = D_800A76E8.mode;
    turn->element8 = D_800A76E8.element8;
    turn->element9 = D_800A76E8.element9;
    turn->element10 = D_800A76E8.element10;
    turn->element11 = D_800A76E8.element11;
    turn->element12 = D_800A76E8.element12;
    turn->hitState = D_800A76E8.hitState;
    turn->flag15 = D_800A76E8.flag15;
    turn->flag16 = D_800A76E8.flag16;
    turn->flag17 = D_800A76E8.flag17;
}

/* Copy the enemy attributes the menu record carries into the active actor. */
void Battle_SyncEnemyAttributes(void) {
    BattleAttributes *source = (BattleAttributes *)&D_800A76D8;
    BattleAttributes *attributes = g_ActiveActor->attributes;
    BattleEffectBits *effects = (BattleEffectBits *)&attributes->effectFlags;

    attributes->parameterWord.fields.first = source->parameterWord.fields.first;
    attributes->parameterWord.fields.second = source->parameterWord.fields.second;
    attributes->parameterWord.fields.third = source->parameterWord.fields.third;
    attributes->parameterWord.fields.reserved = source->parameterWord.fields.reserved;
    effects->resistPoison = D_800A76DC.resistPoison;
    effects->resistConfusion = D_800A76DC.resistConfusion;
    effects->resistDarkness = D_800A76DC.resistDarkness;
    effects->resistStiffness = D_800A76DC.resistStiffness;
    effects->resistSteal = D_800A76DC.resistSteal;
    effects->armorMode = D_800A76DC.armorMode;
    effects->mod14 = D_800A76DC.mod14;
    effects->mod16 = D_800A76DC.mod16;
    effects->mod11 = D_800A76DC.mod11;
    effects->mod17 = D_800A76DC.mod17;
    effects->mod12 = D_800A76DC.mod12;
    effects->autoMedicine = D_800A76DC.autoMedicine;
    effects->autoCure = D_800A76DC.autoCure;
    effects->mod13 = D_800A76DC.mod13;
    effects->mod15 = D_800A76DC.mod15;
}

/* MASPSX_FORCE_G0 keeps the game-state flags absolute, as retail; the COMMON
 * script sound cursor and count supply the small-data metadata of the two
 * $gp-relative bytes. */
u8 D_8009D1D4;
u8 g_BattleActionQueueCount;

void Battle_FlushScriptSounds(void) {
    unsigned int index;
    int flags;
    int mask;
    int item;

    index = D_8009D1D4;
    while ((index & 0xFF) < g_BattleActionQueueCount) {
        item = (u16)D_800BE830[index & 0xFF].command;
        if ((unsigned int)(item - 3) < 0x180) {
            Inv_AddItem((short)item - 3);
        }

        index++;
    }

    flags = g_GameStateFlags;
    mask = -0x101;
    g_BattleActionQueueCount = 0;
    D_8009D1D4 = 0;
    g_GameStateFlags = flags & mask;
}

void Battle_StepScriptEntry(void)
{
    BattleInitSlot *entry;
    BattleEntity *entity;
    int command;

    if (D_8009D1D4 < g_BattleActionQueueCount) {
        entry = &D_800BE830[D_8009D1D4];
        entity = D_8009D254;
        entity->motionX = 0;
        entity->motionY = 0;
        entity->motionZ = 0;
        if (entity->actionMode >= 4) {
            command = entry->command;
            if (command < 3) {
                Battle_UpdatePlayerTurn();
            } else if (command < 0x183) {
                Battle_ApplyPlayerHit();
            } else if (command < 0x197) {
                Battle_AdvancePhase();
            } else if (command < 0x199) {
                Entity_SetActionMode(entity, 13);
                D_8009D2E8 |= 1;
                D_8009D1D4++;
                D_8009D254->entityFlags |= 0x100;
            } else {
                if ((s8)Battle_RollEscapeChance() == 1) {
                    Entity_SetActionMode(D_8009D254, D_8009D278->actionMode12);
                    D_8009D28C = 4;
                }
                D_8009D1D4++;
            }
        }
    } else {
        D_8009D1D4 = 0;
        g_BattleActionQueueCount = 0;
    }
}
