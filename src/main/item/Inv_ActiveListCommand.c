#include "pe1/menu_state.h"
#include "pe1/menu_widget.h"
#include "pe1/battle_entity_anim.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
#include "common.h"
#include "pe1/battle.h"
#include "pe1/inventory_slots.h"

/* Active-list commands from the item menus and the pending command result
 * (g_MenuCommandResult) that the menu loop polls and clears. */

/* COMMON supplies small-data metadata to MASPSX; the extern words stay
 * absolute. */
u8 *D_8009D1E0;
int g_MenuCommandResult;
int D_8009D018;
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
