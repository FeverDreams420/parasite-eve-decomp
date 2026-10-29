/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/aya.h"
#include "pe1/battle_cmd.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/inventory_sort.h"
#include "pe1/inventory_state.h"
#include "pe1/menu_state.h"
#include "pe1/psyq_libc.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
s32 g_InvPackedListCount;
s16 *g_InvItemPtr;
s32 g_InvSlotLimit;
u32 *g_InvSelectionBits;
s32 g_InvSelectionBitWords;
u8 *g_InvScratchBufferPtrWriteOnly;
s32 g_InvScratchWordAWriteOnly;
s32 g_InvScratchWordBWriteOnly;
s32 g_InvSavedSelectionIndex0;
s32 g_InvSavedSelectionIndex1;
s32 g_InvSavedSelectionFromStorage0;
s32 g_InvSavedSelectionFromStorage1;
s16 *g_InvActiveListOverride;
s32 g_InvOverrideSlotLimit;
s32 g_InvWeaponSortStatIndex;
s32 g_InvAmmoSortStatIndex;
unsigned char *g_InvSortRankTable;
unsigned char *g_InvSortTypeRankTable;
u16 *g_InvSortListBase;
s32 g_InvSortListCount;
ItemDataRecord *(*g_InvSortLookup)(int index);
int g_InvSortStatIndex;

static inline ItemDataRecord *LookupOldArmor(int index) {
    int value, saved;
    ItemDataRecord *result;

    if (index >= 0 && index < g_InvSlotLimit) {
        value = g_InvItemPtr[index];
        saved = value;
        if (value - 0x100U < 0x80U) {
            result = &g_InventoryRuntime.equipment[value - 0x100];
        } else {
            if (value - 1U < 0xFFU) {
                return Item_LookupBaseData(value - 1);
            }
            if (saved - 0x200U < 9U) {
                result = &((ItemDataRecord *)g_KeyItemDataTable)[saved];
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

static inline ItemDataRecord *LookupTrackedItem(int index) {
    int value, saved;
    ItemDataRecord *result;
    ItemDataRecord *output = 0;

    if (index >= 0 && index < g_InvSlotLimit) {
        value = g_InvItemPtr[index];
        saved = value;
        if (value - 0x100U < 0x80U) {
            result = &g_InventoryRuntime.equipment[value - 0x100];
        } else {
            if (value - 1U < 0xFFU) {
                result = Item_LookupBaseData(value - 1);
            } else if (saved - 0x200U < 9U) {
                result = &((ItemDataRecord *)g_KeyItemDataTable)[saved];
            } else {
                result = 0;
            }
        }
        output = result;
    }
    return output;
}

/* Historical name: checks free capacity against the armor-derived reserve.
 * Also selects Aya's inventory and optionally returns the required reserve. */
int Inv_GetActiveSlotCount(int *out) {
    ItemDataRecord *armor;
    int reserve = 0;
    int used;
    int capacity, i, enough, occupied;
    int capacityBeforeScan, bonusSlots;
    s16 *slot, *end;

    armor = LookupOldArmor(g_InventoryRuntime.tracked[2]);
    if (armor != 0) {
        for (i = 0; i < armor->tailCount; i++) {
            reserve = (armor->tailData[i] & 0x1F) - 8;
            if ((unsigned)reserve < 3) break;
        }
        if (i < armor->tailCount) reserve = 1 << reserve;
        else reserve = 0;
    }
    bonusSlots = Inv_GetBonusSlotCount();
    if (g_InvBaseCapacity + bonusSlots < 51) {
        used = 0;
        bonusSlots = Inv_GetBonusSlotCount();
        capacityBeforeScan = g_InvBaseCapacity + bonusSlots;
    } else {
        used = 0;
        capacityBeforeScan = 50;
    }
    g_InvItemPtr = g_AyaInventoryItems;
    capacity = capacityBeforeScan;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
    slot = g_InvItemPtr;
    end = slot + g_InvSlotLimit;
    while (slot < end) {
        occupied = *slot++;
        occupied = occupied != 0;
        used += occupied;
    }
    enough = capacity - used >= reserve;
    if (out != 0) *out = reserve;
    return enough;
}

/* Historical names retained: snapshot, then restore the two selected records.
 * Both selected records must resolve, as in the retail copy sequences. */
void Inv_InitWayneStorage(void) {
    g_InvScratchBufferPtrWriteOnly = g_InvScratchBufferUnused;
    g_InvScratchWordBWriteOnly = 0;
    g_InvScratchWordAWriteOnly = 0;
    g_InvSavedSelectionRecord0 = *LookupTrackedItem(Inv_RestoreSelection(0));
    g_InvSavedSelectionRecord1 = *LookupTrackedItem(Inv_RestoreSelection(1));
}

void Inv_RememberSelection(unsigned int index, int value) {
    if (index < 2) {
        g_InvSavedSelectionIndex[index] = value;
        g_InvSavedSelectionFromStorage[index] = g_InvItemPtr != g_AyaInventoryItems;
    }
}

static inline s32 RestoreSelection(u32 index) {
    if (index < 2U) {
        if ((g_InvSavedSelectionFromStorage[index] != 0) && (g_InvActiveListOverride != 0)) {
            g_InvItemPtr = g_InvActiveListOverride;
            g_InvSelectionBits = g_InvStorageSelectionBits;
            g_InvSelectionBitWords = 4;
            g_InvSlotLimit = g_InvOverrideSlotLimit;
        } else {
            g_InvItemPtr = g_AyaInventoryItems;
            g_InvSlotLimit = Inv_GetAyaSlotLimit();
            g_InvSelectionBits = g_InvSelectionBitStorage;
            g_InvSelectionBitWords = 2;
        }
        return g_InvSavedSelectionIndex[index];
    }
    return -1;
}

s32 Inv_RestoreSelection(u32 index) {
    return RestoreSelection(index);
}

static inline void RestoreList(int storage) {
    if (storage && g_InvActiveListOverride != 0) {
        g_InvItemPtr = g_InvActiveListOverride;
        g_InvSelectionBits = g_InvStorageSelectionBits;
        g_InvSelectionBitWords = 4;
        g_InvSlotLimit = g_InvOverrideSlotLimit;
    } else {
        g_InvItemPtr = g_AyaInventoryItems;
        g_InvSlotLimit = Inv_GetAyaSlotLimit();
        g_InvSelectionBits = g_InvSelectionBitStorage;
        g_InvSelectionBitWords = 2;
    }
}
void Inv_BuildStorageDisplay(void) {
    RestoreList(g_InvSavedSelectionFromStorage0);
    *LookupTrackedItem(g_InvSavedSelectionIndex0) = g_InvSavedSelectionRecord0;
    RestoreList(g_InvSavedSelectionFromStorage1);
    *LookupTrackedItem(g_InvSavedSelectionIndex1) = g_InvSavedSelectionRecord1;
}

int g_MenuBattleEquipMode;