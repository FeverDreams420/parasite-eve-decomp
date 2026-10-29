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

static inline ItemDataRecord *LookupArmorTrackedItem(int index) {
    int value, saved;
    ItemDataRecord *result;
    /* Match the shared armor pointer across reserve scanning and rollback. */
    register ItemDataRecord *output asm("$16") = 0;

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

static inline int FilteredSlot(int index) {
    int slot;
    if (index >= 0 && index < g_InvPackedListCount) slot = g_InvPackedList[index];
    else slot = 0;
    return slot;
}

int g_MenuBattleEquipMode;


/* Historical name: switch armor using a filtered-list index, returning status. */
int Inv_GetSlotItemData(int index) {
    ItemDataRecord *armor;
    int previousReserve = 0;
    int nextReserve, previousSlot, status, i;
    BattleCmdEntry *command;
    armor = LookupOldArmor(g_InventoryRuntime.tracked[2]);
    if (armor != 0) {
        for (i = 0; i < armor->tailCount; i++) {
            previousReserve = (armor->tailData[i] & 31) - 8;
            if ((unsigned)previousReserve < 3) break;
        }
        if (i < armor->tailCount) previousReserve = 1 << previousReserve;
        else previousReserve = 0;
    }
    armor = LookupTrackedItem(FilteredSlot(index));
    for (i = 0; i < armor->tailCount; i++) {
        nextReserve = (armor->tailData[i] & 31) - 8;
        if ((unsigned)nextReserve < 3) break;
    }
    if (i < armor->tailCount) nextReserve = 1 << nextReserve;
    else nextReserve = 0;
    previousSlot = g_InventoryRuntime.tracked[2];
    armor = LookupArmorTrackedItem(previousSlot);
    g_InventoryRuntime.tracked[2] = FilteredSlot(index);
    status = Inv_CheckFreeSlotCapacity(previousReserve - nextReserve);
    if (status == 1) {
        command = BattleCmd_AllocSlot();
        command->header.word = 3;
        command->payload.equip_restore.item_data = armor;
        Inv_SetActiveList(3, 0);
    } else {
        g_InventoryRuntime.tracked[2] = previousSlot;
    }
    return status;
}