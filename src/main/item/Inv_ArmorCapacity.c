/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
#include "pe1/battle_cmd.h"
#include "pe1/inventory.h"
#include "pe1/inventory_slots.h"

/* COMMON supplies small-data metadata to MASPSX; other words stay absolute. */
s32 g_InvPackedListCount;
s16 *g_InvItemPtr;
s32 g_InvSlotLimit;
u32 *g_InvSelectionBits;
s32 g_InvSelectionBitWords;
u8 *D_8009D084;
s32 D_8009D088;
s32 D_8009D08C;
s32 D_8009D090;
s32 D_8009D094;
s32 D_8009D098;
s32 D_8009D09C;
s16 *g_InvActiveListOverride;
s32 g_InvOverrideSlotLimit;

static inline ItemDataRecord *LookupOldArmor(int index) {
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
            result = &D_800C0E20.equipment[value - 0x100];
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
            result = &D_800C0E20.equipment[value - 0x100];
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
/* Historical name: switch armor using a filtered-list index, returning status. */
int Inv_GetSlotItemData(int index) {
    ItemDataRecord *armor;
    int previousReserve = 0;
    int nextReserve, previousSlot, status, i;
    BattleCmdEntry *command;
    armor = LookupOldArmor(D_800C0E20.tracked[2]);
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
    previousSlot = D_800C0E20.tracked[2];
    armor = LookupArmorTrackedItem(previousSlot);
    D_800C0E20.tracked[2] = FilteredSlot(index);
    status = Inv_CheckFreeSlotCapacity(previousReserve - nextReserve);
    Inv_CompactActiveListSlots();
    if (status == 1) {
        command = BattleCmd_AllocSlot();
        command->header.word = 3;
        command->payload.equip_restore.item_data = armor;
        Inv_SetActiveList(3, 0);
    } else {
        D_800C0E20.tracked[2] = previousSlot;
    }
    return status;
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

    armor = LookupOldArmor(D_800C0E20.tracked[2]);
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
    D_8009D084 = D_800A1FE8;
    D_8009D08C = 0;
    D_8009D088 = 0;
    D_800A204C = *LookupTrackedItem(Inv_RestoreSelection(0));
    D_800A206C = *LookupTrackedItem(Inv_RestoreSelection(1));
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
    RestoreList(D_8009D098);
    *LookupTrackedItem(D_8009D090) = D_800A204C;
    RestoreList(D_8009D09C);
    *LookupTrackedItem(D_8009D094) = D_800A206C;
}

static inline ItemDataRecord *LookupActiveItem(int index) {
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
 * table address is added, as retail does; a direct &D_800A1E64[kind - 5]
 * folds the -5 into the address (lev 16). */
static inline ItemDataRecord *AmmoPoolRecord(int index) {
    return &D_800A1E64[index];
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
    if (id >= 256) D_800C0E20.equipment[id - 256].iconId = 0;
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
    if (index < 4) D_800C0E20.tracked[index] = value;
}
static inline int GetTracked(unsigned index) {
    return index < 4 ? D_800C0E20.tracked[index] : -1;
}
static inline int IsList(s16 *list) {
    return g_InvItemPtr == list;
}
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
                    ammo = D_800A1E64;
                    if ((int)kind - 4 > 0) ammo = AmmoPoolRecord((int)kind - 5);
                } else if (kind >= 19) ammo = AmmoPoolRecord((int)kind - 19);
                else ammo = &D_800A1E44;
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
                if (g_InvItemPtr == g_AyaInventoryItems && GetTracked(2) == slot)
                    Inv_GetActiveSlotCount(&reserve);
                DeleteSlot(slot);
                if (g_InvItemPtr == g_AyaInventoryItems && GetTracked(2) == slot) {
                    SetTracked(2, -1);
                    Inv_CheckFreeSlotCapacity(reserve);
                    Inv_CompactActiveListSlots();
                    Inv_SetActiveList(3, 0);
                }
            }
        }
    }
    if (!(flags & 1)) {
        slot = RestoreSelection(selection);
        {
            unsigned kind = source->kind;
            if (kind != 0 && kind < 8) {
                ammo = D_800A1E64;
                if ((int)kind - 4 > 0) ammo = AmmoPoolRecord((int)kind - 5);
            } else if (kind >= 19) ammo = AmmoPoolRecord((int)kind - 19);
            else ammo = &D_800A1E44;
        }
        ammo->ammo += source->ammo;
        ClampAmmo(ammo);
        if (g_InvItemPtr == g_AyaInventoryItems && GetTracked(2) == slot)
            Inv_GetActiveSlotCount(&reserve);
        DeleteSlot(slot);
        if (IsList(g_AyaInventoryItems)) {
            if (GetTracked(2) == slot) {
                SetTracked(2, -1);
                Inv_CheckFreeSlotCapacity(reserve);
                Inv_CompactActiveListSlots();
                Inv_SetActiveList(3, 0);
            }
            if (IsList(g_AyaInventoryItems)) {
                if (D_800C0E20.tracked[0] == slot) {
                    D_800C0E20.tracked[0] = FindWeapon(8, -1);
                    Inv_SetActiveList(2, 0);
                } else if (GetTracked(2) == slot) {
                    for (slot = 0; slot < g_InvSlotLimit; ++slot) {
                        ItemDataRecord *item = LookupActiveItem(slot);
                        if (item && item->kind == 9) break;
                    }
                    if (slot >= g_InvSlotLimit) slot = -1;
                    D_800C0E20.tracked[2] = slot;
                    Inv_SetActiveList(3, 0);
                }
            }
        }
    }
}
