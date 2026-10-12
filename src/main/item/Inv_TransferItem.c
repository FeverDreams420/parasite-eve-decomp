#include "common.h"
#include "pe1/aya.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_state.h"
#include "pe1/inventory_slots.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

/* COMMON supplies small-data metadata to MASPSX; other words stay absolute. */
s16 *g_InvItemPtr;
s32 g_InvSlotLimit;
u32 *g_InvSelectionBits;
s32 g_InvSelectionBitWords;

#define LIST_ACTIVE 13
#define LIST_AYA 14

static inline void SwapItems(u16 *a, u16 *b) {
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}

static inline int IsEquippedSlot(int slot) {
    int equipped = 0;
    if (g_InvItemPtr == g_AyaInventoryItems &&
        (g_EquippedWeaponSlot == slot || g_EquippedArmorSlot == slot)) {
        equipped = 1;
    }
    return equipped;
}

/* Swaps an item between Aya's inventory and the active (pending) list, or
 * inside one of them. Returns 0 when the slot cannot move. */
int Inv_TransferItem(int fromList, int fromSlot, int toList, int toSlot) {
    int result = 1;

    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
    if (fromList == LIST_ACTIVE && toList == fromList) {
        u16 *table = (u16 *)g_InvActiveListItems;
        SwapItems(&table[fromSlot], &table[toSlot]);
        Inv_RebuildSelectableMask();
    } else if (fromList == LIST_AYA && toList == fromList) {
        int slot;
        SwapItems((u16 *)&g_InvItemPtr[fromSlot], (u16 *)&g_InvItemPtr[toSlot]);
        slot = g_EquippedWeaponSlot;
        if (slot == fromSlot) {
            g_EquippedWeaponSlot = toSlot;
        } else if (slot == toSlot) {
            g_EquippedWeaponSlot = fromSlot;
        }
        slot = g_EquippedArmorSlot;
        if (slot == fromSlot) {
            g_EquippedArmorSlot = toSlot;
        } else if (slot == toSlot) {
            g_EquippedArmorSlot = fromSlot;
        }
        Inv_RebuildSelectableMask();
    } else if (fromList == LIST_ACTIVE) {
        s16 *item;
        if (Inv_IsSlotSelectable(toSlot) && !IsEquippedSlot(toSlot)) {
            {
                s16 *table = g_InvActiveListItems;
                item = &table[fromSlot];
            }
            if (Item_LookupBaseData(*item - 1)->kind >= 16 && Item_LookupBaseData(*item - 1)->kind < 19) {
                result = Inv_CanAddActiveListItemToAya(fromSlot) == 0;
            } else {
                u16 *table = (u16 *)g_InvActiveListItems;
                SwapItems(&table[fromSlot], (u16 *)&g_InvItemPtr[toSlot]);
            }
            Inv_RebuildSelectableMask();
        } else {
            result = 0;
        }
    } else {
        if (Inv_IsSlotSelectable(fromSlot) && !IsEquippedSlot(fromSlot)) {
            SwapItems((u16 *)&g_InvItemPtr[fromSlot], (u16 *)&g_InvActiveListItems[toSlot]);
            Inv_RebuildSelectableMask();
        } else {
            result = 0;
        }
    }
    return result;
}
