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

static inline ItemDataRecord *LookupActiveItem(int index) {
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
        return result;
    }
    return 0;
}

/* Ammunition pool record. An inline call keeps the index scaled before the
 * table address is added, as retail does; a direct &g_InvSpecialItemRecords[kind - 5]
 * folds the -5 into the address (lev 16). */
static inline ItemDataRecord *AmmoPoolRecord(int index) {
    return &g_InvSpecialItemRecords[index];
}
static inline void ClampAmmo(ItemDataRecord *item) {
    int current = item->ammo;
    if (item->baseStats[2] + item->bonusStats[2] < 1000 ?
        item->baseStats[2] + item->bonusStats[2] < current : 999 < current) item->ammo = (item->baseStats[2] + item->bonusStats[2] < 1000 ?
        item->baseStats[2] + (u16)item->bonusStats[2] : 999);
}
static inline void DeleteSlot(int slot) {
    int id = g_InvItemPtr[slot];
    g_InvItemPtr[slot] = 0;
    if (id >= 256) g_InventoryRuntime.equipment[id - 256].iconId = 0;
}
static inline int ItemKind(int slot) {
    ItemDataRecord *item = LookupActiveItem(slot);
    if (item) return item->kind;
    return 0;
}
static inline int FindWeapon(int kind, int excluded) {
    s16 *slot = g_InvItemPtr;
    s16 *skip = slot + excluded;
    while (slot < g_InvItemPtr + g_InvSlotLimit) {
        if (slot != skip && Inv_LookupItemRecordScaled(*slot)->kind == kind) break;
        ++slot;
    }
    if (slot < g_InvItemPtr + g_InvSlotLimit) return slot - g_InvItemPtr;
    return -1;
}
static inline void SetTracked(unsigned index, int value) {
    if (index < 4) g_InventoryRuntime.tracked[index] = value;
}
static inline int GetTracked(unsigned index) {
    return index < 4 ? g_InventoryRuntime.tracked[index] : -1;
}
static inline int IsList(s16 *list) {
    return g_InvItemPtr == list;
}

int g_MenuBattleEquipMode;
int Inv_GetActiveSlotCount(int *out);


/* Transfer bonuses or one modifier; flags govern tool and donor consumption.
 * Signed comparisons and halfword additions must remain distinct: the final
 * halfword store preserves modular arithmetic, including negative bonuses. */
void Inv_TransferItemBetweenLists(unsigned selection, int modifier, int flags, int cost) {
    ItemDataRecord *source, *dest, *ammo;
    int reserve;
    int slot, i, j, value, group, move;
    {
        int index = RestoreSelection(selection);
        source = 0;
        if (index >= 0 && index < g_InvSlotLimit) {
            source = Inv_LookupItemRecordScaled(g_InvItemPtr[index]);
        }
    }
    {
        int index = RestoreSelection(!selection);
        dest = 0;
        if (index >= 0 && index < g_InvSlotLimit) {
            dest = Inv_LookupItemRecordScaled(g_InvItemPtr[index]);
        }
    }
    if (modifier >= 0) {
        value = source->tailData[modifier];
        group = value & 0xE0;
        if (group) {
            unsigned key = group;
            for (i = 0; i < dest->tailCount; ++i)
                if (key == (dest->tailData[i] & 0xE0)) break;
            if (i < dest->tailCount) {
                dest->tailData[i] = value;
                source->tailData[modifier] = 0;
            } else {
                for (j = 0; j < dest->tailCount; ++j)
                    if (!(dest->tailData[j] & 31)) break;
                if (j < dest->tailCount) {
                    dest->tailData[j] = source->tailData[modifier];
                    source->tailData[modifier] = 0;
                } else Menu_CreateNotificationDialog(7, 0);
            }
        } else {
            for (j = 0; j < dest->tailCount; ++j)
                if (!(dest->tailData[j] & 31)) break;
            if (j < dest->tailCount) {
                dest->tailData[j] = source->tailData[modifier];
                source->tailData[modifier] = 0;
            } else Menu_CreateNotificationDialog(7, 0);
        }
    } else {
#define TRANSFER_STAT(n) dest->bonusStats[n] = source->bonusStats[n] + dest->bonusStats[n] < 1000 ? (u16)source->bonusStats[n] + (u16)dest->bonusStats[n] : 999
        TRANSFER_STAT(0);
        TRANSFER_STAT(1);
        TRANSFER_STAT(2);
#undef TRANSFER_STAT
        source->bonusStats[0] = 0;
        source->bonusStats[1] = 0;
        source->bonusStats[2] = 0;
        if (source->kind != 9 && source->ammo > source->baseStats[2]) {
            int excess = source->ammo - source->baseStats[2];
            int loaded;
            int rawCapacity = dest->baseStats[2] + dest->bonusStats[2];
            loaded = dest->ammo;
            if (rawCapacity < 1000 ? rawCapacity - loaded < excess : 999 - loaded < excess) {
                int loaded;
                int rawCapacity = dest->baseStats[2] + dest->bonusStats[2];
                loaded = dest->ammo;
                move = rawCapacity < 1000 ? rawCapacity - loaded : 999 - loaded;
            }
            else move = source->ammo - source->baseStats[2];
            dest->ammo += move;
            {
                unsigned kind = source->kind;
                if (kind != 0 && kind < 8) {
                    ammo = g_InvSpecialItemRecords;
                    if ((int)kind - 4 > 0) ammo = AmmoPoolRecord((int)kind - 5);
                } else if (kind >= 19) ammo = AmmoPoolRecord((int)kind - 19);
                else ammo = &g_InvFallbackAmmoRecord;
            }
            ammo->ammo += source->ammo - source->baseStats[2] - move;
            ClampAmmo(ammo);
            source->ammo = source->baseStats[2];
        }
    }
    if (cost < 999) {
        cost = (flags & 1) ? 13 : 12;
        if (flags & 2) {
            for (slot = 0; slot < 100; ++slot)
                if (g_WayneStorageItems[slot] && Item_LookupBaseData(g_WayneStorageItems[slot] - 1)->kind == cost) break;
            if (slot < 100) g_WayneStorageItems[slot] = 0;
        } else {
            g_InvItemPtr = g_AyaInventoryItems;
            g_InvSlotLimit = Inv_GetAyaSlotLimit();
            g_InvSelectionBits = g_InvSelectionBitStorage;
            g_InvSelectionBitWords = 2;
            for (slot = 0; slot < g_InvSlotLimit; ++slot)
                if (ItemKind(slot) == cost) break;
            if (slot < g_InvSlotLimit) {
                if (g_InvItemPtr == g_AyaInventoryItems && GetTracked(2) == slot) {
                    Inv_GetActiveSlotCount(&reserve);
                    Inv_CheckFreeSlotCapacity(reserve);
                    SetTracked(2, -1);
                    Inv_SetActiveList(3, 0);
                }
                DeleteSlot(slot);
            }
        }
    }
    if (!(flags & 1)) {
        slot = RestoreSelection(selection);
        {
            unsigned kind = source->kind;
            if (kind != 0 && kind < 8) {
                ammo = g_InvSpecialItemRecords;
                if ((int)kind - 4 > 0) ammo = AmmoPoolRecord((int)kind - 5);
            } else if (kind >= 19) ammo = AmmoPoolRecord((int)kind - 19);
            else ammo = &g_InvFallbackAmmoRecord;
        }
        ammo->ammo += source->ammo;
        ClampAmmo(ammo);
        if (g_InvItemPtr == g_AyaInventoryItems && GetTracked(2) == slot) {
            Inv_GetActiveSlotCount(&reserve);
            Inv_CheckFreeSlotCapacity(reserve);
            SetTracked(2, -1);
            Inv_SetActiveList(3, 0);
        }
        DeleteSlot(slot);
        if (IsList(g_AyaInventoryItems)) {
            if (g_InventoryRuntime.tracked[0] == slot) {
                g_InventoryRuntime.tracked[0] = FindWeapon(8, -1);
                Inv_SetActiveList(2, 0);
            } else if (GetTracked(2) == slot) {
                for (slot = 0; slot < g_InvSlotLimit; ++slot) {
                    ItemDataRecord *item = LookupActiveItem(slot);
                    if (item && item->kind == 9) break;
                }
                if (slot >= g_InvSlotLimit) slot = -1;
                g_InventoryRuntime.tracked[2] = slot;
                Inv_SetActiveList(3, 0);
            }
        }
    }
}