/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/inventory_slots.h"

int Inv_CanAddActiveListItemToAya(int index) {
    int slot_index;
    ItemDataRecord *data;
    int saved_item_id;
    int item_id;
    int result;
    slot_index = index;
    if (slot_index >= 0 && slot_index < g_InvActiveListCount) {
        item_id = g_InvActiveListItems[slot_index];
        saved_item_id = item_id;
        if (item_id - 0x100U < 0x80U) {
            int shifted;
            u8 *base;
            shifted = item_id << 5;
            base = g_EquipItemDataTable;
            data = (ItemDataRecord *)(shifted + base);
        } else if (item_id - 1U < 0xFFU) {
            data = Item_LookupBaseData(item_id - 1);
        } else if (saved_item_id - 0x200U < 9U) {
            int shifted;
            u8 *base;
            shifted = saved_item_id << 5;
            base = g_KeyItemDataTable;
            data = (ItemDataRecord *)(shifted + base);
        } else {
            data = 0;
        }

        g_InvItemPtr = g_AyaInventoryItems;
        g_InvSlotLimit = Inv_GetAyaSlotLimit();
        g_InvSelectionBits = g_InvSelectionBitStorage;
        g_InvSelectionBitWords = 2;

        result = Inv_WriteSlotById(data);
        if (result == 0) {
            g_InvActiveListItems[slot_index] = 0;
        }
    } else {
        result = 1;
    }
    return result;
}


static inline int FindFreeActiveSlot(void) {
    s16 *p = g_InvItemPtr, *end = p + g_InvSlotLimit;

    if (p < end) {
        do {
            if (!*p)
                break;
            p++;
        } while (p < end);
        if (p < g_InvItemPtr + g_InvSlotLimit)
            return p - g_InvItemPtr;
    }
    return -1;
}


/* Transfer pending IDs, then rebuild the selectable-item mask. */
void Inv_TransferItemAlt(void) {
    int i = 0;
    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
    for (; i < g_InvActiveListCount; i++) {
        int id = g_InvActiveListItems[i];
        if (Inv_LookupItemRecord(id)->kind >= 16 && Inv_LookupItemRecord(id)->kind < 19) {
            if (!Inv_CanAddActiveListItemToAya(i))
                g_InvActiveListItems[i] = 0;
        } else if (id < 512) {
            int slot = FindFreeActiveSlot();
            if (slot >= 0) {
                g_InvItemPtr[slot] = id;
                g_InvActiveListItems[i] = 0;
            }
        }
    }
    Inv_RebuildSelectableMask();
}

static inline int ActiveItemKind(int index) {
    ItemDataRecord *item;
    if (index >= 0 && index < g_InvSlotLimit)
        item = Inv_LookupItemRecord(g_InvItemPtr[index]);
    else
        item = 0;
    return item ? item->kind : 0;
}
static inline int FindFreeEquipmentRecord(void) {
    int result;
    ItemDataRecord *p = D_800C0E20.equipment;
    ItemDataRecord *end;
    /* Preserve retail's independent base reload beside the cached bound. */
    asm("" : "=r"(p) : "0"(p));
    end = p + 128;

    if (p < D_800C0E20.equipment + 128) {
        do {
            if (!p->iconId)
                break;
            p++;
        } while (p < end);
        if (p < D_800C0E20.equipment + 128) {
            int index = p - D_800C0E20.equipment;
            result = index;
        } else {
            result = -1;
        }
    } else {
        result = -1;
    }
    return result;
}

/* Replace base equipment IDs with private, mutable equipment records. */
void Inv_ClearDisplaySlots(void) {
    int i = 0;
    g_InvActiveListOverride = 0;
    g_InvOverrideSlotLimit = 0;
    g_InvItemPtr = D_800C0E20.slots;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
    for (; i < g_InvSlotLimit; i++) {
        if ((unsigned)(ActiveItemKind(i) - 1) < 9 &&
            (unsigned)((u16)g_InvItemPtr[i] - 256) >= 128) {
            int record = FindFreeEquipmentRecord();
            if (record >= 0) {
                ItemDataRecord *item = &D_800C0E20.equipment[record];
                *item = *Item_LookupBaseData(g_InvItemPtr[i] - 1);
                g_InvItemPtr[i] = record + 256;
            }
        }
    }
}

/* Select a storage list, adding the special item ID when needed. */
int Inv_StepScrollDisplay2(int special) {
    if (special) {
        int i, found;
        g_InvActiveItemList = D_800C1F80;
        for (i = 0; i < 82; i++)
            if (g_InvActiveItemList[i] == 516)
                break;
        found = i < 82;
        if (!found) {
            for (i = 0; i < g_InvSlotLimit; i++) {
                ItemDataRecord *item;
                if (i >= 0 && i < g_InvSlotLimit)
                    item = Inv_LookupItemRecord(g_InvItemPtr[i]);
                else
                    item = 0;
                if (item && item->kind == 6)
                    break;
            }
            if (i < g_InvSlotLimit) {
                /* If slots 0..80 are occupied, retail writes slot 81 unchecked. */
                for (i = 0; i < 81; i++)
                    if (!g_InvActiveItemList[i])
                        break;
                if (i < 82) {
                    g_InvActiveItemList[i] = 516;
                    found = 1;
                }
            }
        }
        D_8009D080 = 80 + found;
    } else {
        g_InvActiveItemList = g_WayneStorageItems;
        D_8009D080 = 100;
    }
    g_InvActiveListOverride = g_InvActiveItemList;
    return g_InvOverrideSlotLimit = D_8009D080;
}
