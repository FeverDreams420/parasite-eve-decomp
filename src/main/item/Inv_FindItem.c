#include "pe1/menu_state.h"
#include "pe1/menu_sound.h"
#include "pe1/aya.h"
#include "pe1/save.h"
#include "pe1/inventory_state.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

#include "pe1/inventory_slots.h"


SavedMenuMode g_SavedMenuMode;

unsigned int Menu_GetActiveMode(void);
void Window_SetBoundsByMode(int mode);

/* Equips the inventory item whose equipment record has this ID: weapons
 * (kinds 1..8) and armor (kind 9). Returns nonzero when nothing was equipped. */
int Inv_EquipItemById(unsigned int itemId) {
    int result = itemId;
    ItemDataRecord *record = D_800C0E20.equipment;

    if (record < D_800C0E20.equipment + 128) {
        while (1) {
            if (record->itemId == result) {
                break;
            }
            record++;
            if (record >= D_800C0E20.equipment + 128) {
                break;
            }
        }
        if (record < D_800C0E20.equipment + 128) {
            int value;
            int index;
            int count;
            s16 *item;
            s16 *end;
            g_InvItemPtr = D_800C0E20.slots;
            count = Inv_GetAyaSlotLimit();
            g_InvSelectionBits = g_AyaItemSelectionBits;
            g_InvSelectionBitWords = 2;
            value = record - D_800C0E20.equipment + 0x100;
            item = g_InvItemPtr;
            g_InvSlotLimit = count;
            end = item + count;
            if (item < end) {
                while (*item != value) {
                    item++;
                    if (item >= end) {
                        break;
                    }
                }
                if (item < g_InvItemPtr + g_InvSlotLimit) {
                    index = item - g_InvItemPtr;
                } else {
                    index = -1;
                }
            } else {
                index = -1;
            }
            result = index;
            if (result >= 0) {
                ItemDataRecord *entry;
                int kind;
                if (result < g_InvSlotLimit) {
                    entry = Inv_LookupItemRecordScaled(g_InvItemPtr[result]);
                } else {
                    entry = 0;
                }
                kind = 0;
                if (entry != 0) {
                    kind = entry->kind;
                }
                if (kind != 0) {
                    /* Retail tests kind < 9 first, then 9; the inner test is
                     * threaded into those two branches. */
                    if (kind < 9 || kind == 9) {
                        if (kind < 9) {
                            g_AyaEquippedWeaponSlot[0] = result;
                        } else {
                            g_AyaEquippedArmorSlot[0] = result;
                        }
                        return (unsigned int)result >> 31;
                    }
                    result = -1;
                } else {
                    result = -1;
                }
            }
        }
    }
    return (unsigned int)result >> 31;
}

void Menu_ResetInputState(void) {
    g_SavedMenuMode.value = Menu_GetActiveMode() & 0xFF;
    Window_SetBoundsByMode(0);
}

/* Open the player inventory using the same saved menu mode. */

void BattleCmd_SyncActiveAmmo(void);
void Menu_CreateInventoryTabView(void);

void Inventory_OpenAyaItemList(unsigned int arg) {
    g_MenuBattleEquipMode = arg;
    g_SavedMenuMode.value = Menu_GetActiveMode() & 0xFF;
    Window_SetBoundsByMode(0);
    BattleCmd_SyncActiveAmmo();
    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_AyaItemSelectionBits;
    g_InvSelectionBitWords = 2;
    Menu_CreateInventoryTabView();
    Menu_PlayConfirmSound();
}

/* Memory-card dialog gating from the inventory frontend. */
#include "pe1/menu_state.h"
#include "pe1/game_state.h"

void MemCard_CloseAll(void);

void MemCard_SetDialogActive(int active) {
    if (active == 0) {
        g_MemCardDialogState = 0;
        MemCard_CloseAll();
        g_GameState.flags &= 0xFFFF3FFF;
    } else if (g_MemCardDialogState == 0) {
        g_MemCardDialogState = 1;
        g_GameState.flags |= 0xC000;
    }
}
