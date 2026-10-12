/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "common.h"
#include "pe1/inventory.h"
#include "pe1/inventory_slots.h"
#include "pe1/menu_inventory.h"

/* Wayne-list item lookup, kind filtering, filtered-slot lookup, equip
 * eligibility and inventory swaps. */

u8 *Inv_LookupItemData(int index) {
    int value = g_InvActiveItemList[index];
    int saved_value = value;
    u8 *entry;

    if (value - 0x100U < 0x80U) {
        entry = g_EquipItemDataTable + (value << 5);
    } else if (value - 1U < 0xFFU) {
        entry = Item_LookupBaseData(value - 1);
    } else if (saved_value - 0x200U < 9U) {
        entry = g_KeyItemDataTable + (saved_value << 5);
    } else {
        entry = 0;
    }

    return entry;
}

static inline int WayneSlot(int index) {
    int result;
    if (index >= 0 && index < g_InvTransferItemCount) result = g_InvTransferItemList[index];
    else result = 0;
    return result;
}
static inline void SwapSlots(s16 *a, s16 *b) {
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}
static inline void SwapAyaSlots(int firstSlot, int secondSlot) {
    int from = firstSlot;
    int to = secondSlot;
    SwapSlots(&g_InvItemPtr[from], &g_InvItemPtr[to]);
    if (D_800C0E20.tracked[0] == from) D_800C0E20.tracked[0] = to;
    else if (D_800C0E20.tracked[0] == to) D_800C0E20.tracked[0] = from;
    if (D_800C0E20.tracked[2] == from) D_800C0E20.tracked[2] = to;
    else if (D_800C0E20.tracked[2] == to) D_800C0E20.tracked[2] = from;
}
static inline ItemDataRecord *LookupTrackedItem(int index) {
    int value, saved;
    ItemDataRecord *result;

    if (index >= 0 && index < g_InvSlotLimit) {
        value = g_InvItemPtr[index];
        saved = value;
        if (value - 0x100U < 0x80U) {
            result = &D_800C0E20.equipment[value - 0x100];
        } else {
            if (value - 1U < 0xFFU) {
                return Item_LookupBaseData(value - 1);
            }
            if (saved - 0x200U < 9U) {
                int shifted = saved << 5;
                result = (ItemDataRecord *)(g_KeyItemDataTable + shifted);
            } else {
                result = 0;
            }
        }
        /* Keep the item-ID result merge distinct from an invalid list index. */
        asm("");
        return result;
    }
    return 0;
}


static inline ItemDataRecord *LookupActiveItem(int index) {
    if (index >= 0 && index < g_InvSlotLimit) return Inv_LookupItemRecordScaled(g_InvItemPtr[index]);
    return 0;
}

static inline int ItemKind(int slot) {
    ItemDataRecord *item = LookupActiveItem(slot);
    if (item) return item->kind;
    return 0;
}
/* Historical name: collect kind-filtered slots, rebuild selection, then append
 * empty slots. Return the final row count consumed by inventory menus.
 * The C variable shift requires item kinds below 32. */
int Inv_TransferItemAlt2(int mask) {
    s16 *out = g_InvTransferItemList;
    int i;
    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
    for (i = 0; i < g_InvSlotLimit; i++) {
        if ((mask >> ItemKind(i)) & 1) *out++ = i;
    }
    g_InvTransferItemCount = out - g_InvTransferItemList;
    Inv_RebuildSelectableMask();
    for (i = 0; i < g_InvSlotLimit; i++) {
        if (g_InvItemPtr[i] == 0) *out++ = i;
    }
    g_InvTransferItemCount = out - g_InvTransferItemList;
    return g_InvTransferItemCount;
}

int Inv_GetWayneListItemByIndex(int index) {
    if (index >= 0 && index < g_InvTransferItemCount) {
        return g_InvTransferItemList[index];
    }
    return 0;
}

/* Historical name: tests whether a slot may be acted on, excluding equipped
 * weapon/armor slots in Aya's list. An unresolved item remains allowed. */
int Inv_IsSlotEquipped(int index) {
    ItemDataRecord *data;
    int allowed = 1;
    int equipped, kind, count, i;
    data = LookupTrackedItem(index);
    if (data != 0) {
        allowed = 0;
        if (!(data->flags & 0x20)) {
            equipped = 0;
            if (g_InvItemPtr == g_AyaInventoryItems &&
                (D_800C0E20.tracked[0] == index || D_800C0E20.tracked[2] == index))
                equipped = 1;
            if (!equipped) allowed = 1;
        }
        if (data->kind == 8) {
            kind = 8;
            count = 0;
            for (i = 0; i < g_InvSlotLimit; i++) {
                if (Inv_GetActiveListItemType(i) == kind) count++;
            }
            allowed &= count >= 2;
        }
    }
    return allowed;
}

/* Historical name: swaps inventory/storage entries after eligibility checks. */
int Inv_RebuildWithSlotLimit(int fromMode, int fromIndex, int toMode, int toIndex) {
    int success = 1;
    int from;
    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
    if (fromMode == 0x34 && toMode == fromMode) {
        SwapSlots(&g_InvActiveItemList[fromIndex], &g_InvActiveItemList[toIndex]);
        Inv_RebuildSelectableMask();
    } else if (fromMode == 0x33 && toMode == fromMode) {
        int first, second;
        first = WayneSlot(fromIndex);
        second = WayneSlot(toIndex);
        SwapAyaSlots(first, second);
        Inv_RebuildSelectableMask();
    } else if (fromMode == 0x34) {
        from = WayneSlot(toIndex);
        if (Inv_IsSlotEquipped(from) &&
            (Inv_LookupItemRecordScaled(g_InvActiveItemList[fromIndex])->kind < 19 ||
             Inv_LookupItemRecordScaled(g_InvActiveItemList[fromIndex])->kind >= 22)) {
            SwapSlots(&g_InvActiveItemList[fromIndex], &g_InvItemPtr[from]);
            Menu_SetSwapReturnFlag();
            Inv_RebuildSelectableMask();
        } else success = 0;
    } else {
        from = WayneSlot(fromIndex);
        if (Inv_IsSlotEquipped(from) &&
            (Inv_LookupItemRecordScaled(g_InvActiveItemList[toIndex])->kind < 19 ||
             Inv_LookupItemRecordScaled(g_InvActiveItemList[toIndex])->kind >= 22)) {
            SwapSlots(&g_InvItemPtr[from], &g_InvActiveItemList[toIndex]);
            Menu_SetSwapReturnFlag();
            Inv_RebuildSelectableMask();
        } else success = 0;
    }
    return success;
}
