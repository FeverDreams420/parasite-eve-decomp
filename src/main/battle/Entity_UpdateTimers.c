/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section --expand-div */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/battle_runtime.h"
#include "pe1/inventory.h"
#include "pe1/item_ids.h"
#include "pe1/menu_flags.h"
#include "pe1/game_timers.h"
#include "pe1/render_prim.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
unsigned char D_8009CE34;

void Battle_ApplyDamage(s32);
void Battle_SubActionStep(void);
s32 Inv_CountByValue(s32);

void Entity_UpdateTimers(void) {
    s32 status_item_0;
    s32 status_item_1;
    s32 status_item_2;
    s32 status_item_3;
    s16 current_hp;
    s32 accumulator;
    s32 status_group;
    s32 status_group_again;
    s32 consumed_item;
    u8 consumed_item_snapshot;
    s32 hp_threshold;
    s32 max_hp;
    u16 timer46;
    u16 global_timer;
    u16 timer40;
    u16 timer42;
    u16 timer44;
    u8 effect_timer;
    Combatant *combatant;
    s32 *state_flags;

    combatant = D_8009D278;
    state_flags = &combatant->stateFlags;
    if (((*state_flags) & AYA_POISON_MASK) == AYA_POISON) {
        Battle_SubActionStep();
        timer40 = D_8009D278->statusTimer40 - D_8009D278->statusStep3E;
        D_8009D278->statusTimer40 = timer40;
        if ((timer40 << 0x10) <= 0) {
            (*state_flags) = ((*state_flags) & ~AYA_POISON_MASK);
        }
    }
    if (((*state_flags) & AYA_CONFUSION_MASK) == AYA_CONFUSION) {
        timer42 = D_8009D278->statusTimer42 - D_8009D278->statusStep3C;
        D_8009D278->statusTimer42 = timer42;
        if ((timer42 << 0x10) <= 0) {
            (*state_flags) = ((*state_flags) & ~AYA_CONFUSION_MASK);
            D_8009D2E8 &= ~FIELD_MOVE_CONFUSED;
        }
    }
    if (((*state_flags) & AYA_DARKNESS_MASK) == AYA_DARKNESS) {
        timer44 = D_8009D278->statusTimer44 - D_8009D278->statusStep3C;
        D_8009D278->statusTimer44 = timer44;
        if ((timer44 << 0x10) <= 0) {
            (*state_flags) = ((*state_flags) & ~AYA_DARKNESS_MASK);
        }
    }
    status_group = (*state_flags) & AYA_STIFFNESS_MASK;
    if ((status_group == AYA_STIFFNESS_SLOW) || (status_group == AYA_STIFFNESS_LOCK)) {
        timer46 = D_8009D278->statusTimer46 - D_8009D278->statusStep3C;
        D_8009D278->statusTimer46 = timer46;
        if ((timer46 << 0x10) <= 0) {
            if (((*state_flags) & AYA_STIFFNESS_MASK) == AYA_STIFFNESS_LOCK && D_8009D254->actionMode == 0x11) {
                Entity_SetActionMode(D_8009D254, D_8009D278->actionMode12);
            }
            (*state_flags) = ((*state_flags) & ~AYA_STIFFNESS_MASK);
        } else if (((*state_flags) & AYA_STIFFNESS_MASK) == AYA_STIFFNESS_LOCK) {
            D_8009D254->motionX = 0;
            D_8009D254->motionY = 0;
            D_8009D254->motionZ = 0;
        }
    }
    if ((*state_flags) & AYA_HASTE) {
        global_timer = D_8009D228 - 1;
        D_8009D228 = global_timer;
        if ((global_timer << 0x10) <= 0) {
            (*state_flags) = ((*state_flags) & ~AYA_HASTE);
        }
    }
    if (((*state_flags) & AYA_BARRIER) && D_8009D278->exp_or_acc <= 0x10000) {
        D_8009D278->exp_or_acc = 0x10000;
        (*state_flags) = ((*state_flags) & ~AYA_BARRIER);
    }
    if ((*state_flags) & AYA_GENE_HEAL) {
        if (D_8009CDDC != 0) {
            current_hp = (s16) D_8009D278->curHP;
            if (current_hp < (s16) D_8009D278->maxHP) {
                D_8009D278->curHP = (s16) (current_hp + 1);
                D_8009D278->hpMirror = (u16) (D_8009D278->hpMirror + 1);
            }
        }
        accumulator = D_8009D278->exp_or_acc -
                    (D_8009D278->maxAtk /
                     (D_8009D278->field04.fieldId04 * 0x1E));
        D_8009D278->exp_or_acc = accumulator;
        if (accumulator <= 0x10000) {
            D_8009D278->exp_or_acc = 0x10000;
            (*state_flags) = ((*state_flags) & ~AYA_GENE_HEAL);
        }
    }
    if ((*state_flags) & 0x01000000) {
        effect_timer = D_8009CE34 - 1;
        D_8009CE34 = effect_timer;
        if ((effect_timer << 0x18) <= 0) {
            (*state_flags) = ((*state_flags) & 0xFEFFFFFF);
        }
    }
    if (D_8009D278->attributes->effectFlags & EFFECT_AUTO_MEDICINE) {
        consumed_item = 0;
        if ((s16) D_8009D278->curHP > 0) {
            do {
                consumed_item_snapshot = consumed_item & 0xFF;
                max_hp = (s16) D_8009D278->maxHP;
                hp_threshold = max_hp / 5;
                if ((s16) D_8009D278->curHP < hp_threshold) {
                    if (Inv_CountByValue(ITEM_FULL_RECOVER) != 0) {
                        consumed_item = 1;
                        Battle_ApplyDamage(ITEM_FULL_RECOVER);
                        Inv_FindItemById(ITEM_FULL_RECOVER);
                        continue;
                    }
                    if (Inv_CountByValue(ITEM_MEDICINE_4) != 0) {
                        consumed_item = 1;
                        Battle_ApplyDamage(ITEM_MEDICINE_4);
                        Inv_FindItemById(ITEM_MEDICINE_4);
                        continue;
                    }
                    if (Inv_CountByValue(ITEM_MEDICINE_3) != 0) {
                        consumed_item = 1;
                        Battle_ApplyDamage(ITEM_MEDICINE_3);
                        Inv_FindItemById(ITEM_MEDICINE_3);
                        continue;
                    }
                    if (Inv_CountByValue(ITEM_MEDICINE_2) != 0) {
                        consumed_item = 1;
                        Battle_ApplyDamage(ITEM_MEDICINE_2);
                        Inv_FindItemById(ITEM_MEDICINE_2);
                        continue;
                    }
                    consumed_item_snapshot = consumed_item & 0xFF;
                    if (Inv_CountByValue(ITEM_MEDICINE_1) != 0) {
                        consumed_item = 1;
                        Battle_ApplyDamage(ITEM_MEDICINE_1);
                        Inv_FindItemById(ITEM_MEDICINE_1);
                        continue;
                    }
                }
                break;
            } while (1);
            if (consumed_item_snapshot != 0) {
                Scene_LoadRoomAssets(0x56, D_8009D254);
                Asset_Find08Alt(0x4B4, 0, D_8009D254->posX.parts.integer, D_8009D254->posY.parts.integer, D_8009D254->posZ.parts.integer);
            }
        }
    }
    if (D_8009D278->attributes->effectFlags & EFFECT_AUTO_CURE) {
        if (((*state_flags) & AYA_POISON_MASK) == AYA_POISON) {
            if (Inv_CountByValue(ITEM_CURE_P) != 0) {
                Battle_ApplyDamage(ITEM_CURE_P);
                status_item_0 = ITEM_CURE_P;
                Inv_FindItemById(status_item_0);
                Scene_LoadRoomAssets(0x57, D_8009D254);
                Asset_Find08Alt(0x4B5, 0, D_8009D254->posX.parts.integer, D_8009D254->posY.parts.integer, D_8009D254->posZ.parts.integer);
            } else if (Inv_CountByValue(ITEM_FULL_CURE) != 0) {
                Battle_ApplyDamage(ITEM_FULL_CURE);
                status_item_0 = ITEM_FULL_CURE;
                Inv_FindItemById(status_item_0);
                Scene_LoadRoomAssets(0x57, D_8009D254);
                Asset_Find08Alt(0x4B5, 0, D_8009D254->posX.parts.integer, D_8009D254->posY.parts.integer, D_8009D254->posZ.parts.integer);
            }
        }
        if (((*state_flags) & AYA_CONFUSION_MASK) == AYA_CONFUSION) {
            if (Inv_CountByValue(ITEM_CURE_C) != 0) {
                Battle_ApplyDamage(ITEM_CURE_C);
                status_item_1 = ITEM_CURE_C;
                Inv_FindItemById(status_item_1);
                Scene_LoadRoomAssets(0x57, D_8009D254);
                Asset_Find08Alt(0x4B5, 0, D_8009D254->posX.parts.integer, D_8009D254->posY.parts.integer, D_8009D254->posZ.parts.integer);
            } else if (Inv_CountByValue(ITEM_FULL_CURE) != 0) {
                Battle_ApplyDamage(ITEM_FULL_CURE);
                status_item_1 = ITEM_FULL_CURE;
                Inv_FindItemById(status_item_1);
                Scene_LoadRoomAssets(0x57, D_8009D254);
                Asset_Find08Alt(0x4B5, 0, D_8009D254->posX.parts.integer, D_8009D254->posY.parts.integer, D_8009D254->posZ.parts.integer);
            }
        }
        if (((*state_flags) & AYA_DARKNESS_MASK) == AYA_DARKNESS) {
            if (Inv_CountByValue(ITEM_CURE_D) != 0) {
                Battle_ApplyDamage(ITEM_CURE_D);
                status_item_2 = ITEM_CURE_D;
                Inv_FindItemById(status_item_2);
                Scene_LoadRoomAssets(0x57, D_8009D254);
                Asset_Find08Alt(0x4B5, 0, D_8009D254->posX.parts.integer, D_8009D254->posY.parts.integer, D_8009D254->posZ.parts.integer);
            } else if (Inv_CountByValue(ITEM_FULL_CURE) != 0) {
                Battle_ApplyDamage(ITEM_FULL_CURE);
                status_item_2 = ITEM_FULL_CURE;
                Inv_FindItemById(status_item_2);
                Scene_LoadRoomAssets(0x57, D_8009D254);
                Asset_Find08Alt(0x4B5, 0, D_8009D254->posX.parts.integer, D_8009D254->posY.parts.integer, D_8009D254->posZ.parts.integer);
            }
        }
        status_group_again = (*state_flags) & AYA_STIFFNESS_MASK;
        if ((status_group_again == AYA_STIFFNESS_SLOW) || (status_group_again == AYA_STIFFNESS_LOCK)) {
            if (Inv_CountByValue(ITEM_CURE_M) != 0) {
                Battle_ApplyDamage(ITEM_CURE_M);
                status_item_3 = ITEM_CURE_M;
                Inv_FindItemById(status_item_3);
                Scene_LoadRoomAssets(0x57, D_8009D254);
                Asset_Find08Alt(0x4B5, 0, D_8009D254->posX.parts.integer, D_8009D254->posY.parts.integer, D_8009D254->posZ.parts.integer);
            } else if (Inv_CountByValue(ITEM_FULL_CURE) != 0) {
                Battle_ApplyDamage(ITEM_FULL_CURE);
                status_item_3 = ITEM_FULL_CURE;
                Inv_FindItemById(status_item_3);
                Scene_LoadRoomAssets(0x57, D_8009D254);
                Asset_Find08Alt(0x4B5, 0, D_8009D254->posX.parts.integer, D_8009D254->posY.parts.integer, D_8009D254->posZ.parts.integer);
            }
        }
        if (((*state_flags) & 0x1000) && (Inv_CountByValue(ITEM_FULL_CURE) != 0)) {
            Battle_ApplyDamage(ITEM_FULL_CURE);
            Inv_FindItemById(ITEM_FULL_CURE);
            Scene_LoadRoomAssets(0x57, D_8009D254);
            Asset_Find08Alt(0x4B5, 0, D_8009D254->posX.parts.integer, D_8009D254->posY.parts.integer, D_8009D254->posZ.parts.integer);
        }
    }
}
