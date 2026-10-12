/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "pe1/menu_state.h"
#include "pe1/menu_widget.h"
#include "pe1/battle_entity_anim.h"
#include "common.h"
#include "pe1/battle.h"
#include "pe1/inventory_slots.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/aya.h"
#include "pe1/player_entity.h"
#include "pe1/battle_cmd.h"
#include "pe1/battle_runtime.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
u8 *D_8009D1E0;
int g_MenuCommandResult;
int D_8009D018;

/* Active-list commands from the item menus and the pending command result
 * (g_MenuCommandResult) that the menu loop polls and clears. */

#include "pe1/battle_modifiers.h"
void Battle_DispatchSpecialAction(int);

void Inv_SetActiveList(int mode, int *slot) {
    Combatant *active;
    int *selection;
    BattleEntity *entity;
    int value;
    int present;
    selection = slot;
    entity = D_8009D254;
    active = 0;
    if (entity) {
        value = (int)entity->core;
        present = value != 0;
            active = (Combatant *)(value & -present);
    }
    switch (mode) {
    case 0:
        g_MenuCommandResult = *selection + 3;
        break;
    case 1:
        g_MenuCommandResult = *selection + 0x183;
        break;
    case 2:
        if (active) {
            u8 *out = D_8009D1E0;
            if (out) {
                *(BattleAction *)out = *(BattleAction *)(active->action);
                Inv_BuildWeaponList(0, (WeaponListOutput *)D_8009D1E0);
            }
            if (Menu_GetBattleEquipMode()) g_MenuCommandResult = 0x197;
            else Battle_DispatchSpecialAction(0x197);
        }
        break;
    case 3:
        if (active) {
            u8 *out = D_8009D1E0;
            if (out) {
                *(BattleAttributes *)out = *active->attributes;
                Inv_BuildArmorList((BattleAttributes *)D_8009D1E0);
            }
            if (Menu_GetBattleEquipMode()) g_MenuCommandResult = 0x198;
            else Battle_DispatchSpecialAction(0x198);
        }
        break;
    case 5:
        if (active) {
            register BattleAction *data asm("$5") = active->action;
            ItemDataRecord *item = *(ItemDataRecord **)selection;
            register u32 word asm("$3") = data->attackWord;
            register int mask asm("$4") = -0x400;
            int kind;
            int encoded;
            u32 flags2;
            register u32 shifted asm("$2");
            word = (word & mask) | (item->ammo & 0x3FF);
            data->attackWord = word;
                    item = *(ItemDataRecord **)selection;
            kind = item->kind;
            data = active->action;
            if (kind != 0 && (unsigned)kind < 8) {
                asm volatile("" ::: "memory");
                encoded = kind - 4;
                if (encoded <= 0) encoded = 1;
            } else {
                            item = *(ItemDataRecord **)selection;
                kind = item->kind;
                if ((unsigned)kind < 19) encoded = 0;
                else encoded = kind - 18;
            }
            flags2 = data->attackWord;
            shifted = 0xFFCFFFFF;
            flags2 &= shifted;
            shifted = encoded & 3;
            shifted <<= 20;
            flags2 |= shifted;
            data->attackWord = flags2;
        }
        break;
    case 8: g_MenuCommandResult = 0x199; break;
    case 9: g_MenuCommandResult = -1; break;
    case 10: g_MenuCommandResult = 1000; break;
    case 11: g_MenuCommandResult = 1; break;
    case 12:
        D_8009D018 = 4;
        Inv_SelectActiveList(0);
        BattleCmd_LoadWeaponModifiers();
        g_MenuCommandResult = 2;
        break;
    default: break;
    }
    if (g_MenuCommandResult) MenuWidget_InitPool();
}

int Menu_GetCommandResult(void) {
    return g_MenuCommandResult;
}

void Menu_ClearCommandResult(void) {
    g_MenuCommandResult = 0;
}

/* g_PlayerEntity points at the player-side field actor; its core is the
 * Combatant record used by the command paths below. */

void BattleCmd_SyncActiveAmmo(void) {
    BattleEntity *entity;
    Combatant *current;
    ItemDataRecord *entry;
    int saved;

    entity = (BattleEntity *)g_PlayerEntity;
    if (entity != 0) {
        current = entity->core;
        if (current != 0) {
            g_AyaHpCurrent[0] = current->curHP;
            if (current->action != 0) {
                saved = Inv_IsActiveListOverrideSelected();
                Inv_SelectActiveList(0);
                entry = Inv_LookupActiveListData(g_AyaEquippedWeaponSlot[0]);
                if (entry != 0) {
                    entry->ammo = current->action->attackWord & 0x3FF;
                }
                Inv_SelectActiveList(saved);
            }
        }
    }
}

void BattleCmd_SetCurrentHP(int arg0) {
    Combatant *current;

    if (g_PlayerEntity != 0) {
        current = ((BattleEntity *)g_PlayerEntity)->core;
        if (current != 0) {
            current->curHP = arg0;
        }
    }

    g_AyaHpCurrent[0] = arg0;
}

/* MASPSX_FORCE_G0 keeps the player pointer absolute, as retail; the COMMON
 * stack top supplies the small-data metadata of the one $gp-relative word. */
BattleCmdEntry *g_BattleCmdStackTop;

int BattleCmd_GetRemainingAmmo(int *out) {
    BattleCmdEntry *cursor;
    BattleEntity *entity;
    Combatant *current;
    int remaining = 0;

    entity = (BattleEntity *)g_PlayerEntity;
    if (entity != 0) {
        if (entity->core != 0) {
            current = entity->core;
            remaining = (s16)(current->exp_or_acc >> 16);
            cursor = g_BattleCmdStack;

            while (cursor < g_BattleCmdStackTop) {
                /* retail compares the whole first word, not the opcode byte */
                if (cursor->header.word == 1) {
                    remaining -= cursor->payload.ammo_spend.amount;
                }
                cursor++;
            }

            if (out != 0) {
                *out = (s16)(current->maxAtk >> 16);
            }
        }
    }

    return remaining;
}

void BattleCmd_SetCurrentMP(int arg0) {
    Combatant *current;

    if (g_PlayerEntity != 0) {
        current = ((BattleEntity *)g_PlayerEntity)->core;
        if (current != 0) {
            current->exp_or_acc = arg0 << 16;
        }
    }
}

void BattleCmd_CommitAndSyncAmmo(int item) {
    BattleEntity *entity;
    Combatant *current;
    ItemDataRecord *entry;
    int saved;

    Battle_ApplyDamage(item);
    entity = (BattleEntity *)g_PlayerEntity;
    if (entity != 0) {
        current = entity->core;
        if (current != 0) {
            g_AyaHpCurrent[0] = current->curHP;
            if (current->action != 0) {
                saved = Inv_IsActiveListOverrideSelected();
                Inv_SelectActiveList(0);
                entry = Inv_LookupActiveListData(g_AyaEquippedWeaponSlot[0]);
                if (entry != 0) {
                    entry->ammo = current->action->attackWord & 0x3FF;
                }
                Inv_SelectActiveList(saved);
            }
        }
    }
}

/* Applies a weapon change, then writes the loaded ammo back to the item. */
void BattleCmd_ChangeWeaponAndSync(int arg0) {
    Combatant *current;
    ItemDataRecord *entry;
    int saved;

    Battle_ApplySpellEffect(arg0, D_8009D254);
    if (D_8009D254 != 0) {
        current = D_8009D254->core;
        if (current != 0) {
            g_AyaHpCurrent[0] = current->curHP;
            if (current->action != 0) {
                saved = Inv_IsActiveListOverrideSelected();
                Inv_SelectActiveList(0);
                entry = Inv_LookupActiveListData(g_AyaEquippedWeaponSlot[0]);
                if (entry != 0) {
                    entry->ammo = current->action->attackWord & 0x3FF;
                }
                Inv_SelectActiveList(saved);
            }
        }
    }
}

int Aya_HasParasiteSpell(int spell)
{
    return (g_AyaParasiteSpellFlags[0] >> spell) & 1;
}
