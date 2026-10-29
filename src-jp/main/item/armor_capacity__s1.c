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

int Inv_CompareItemsForSort(const void *leftElement, const void *rightElement) {
    const short *leftIndex = leftElement;
    const short *rightIndex = rightElement;
    ItemDataRecord *left;
    ItemDataRecord *right;
    int leftKind;
    int rightKind;
    int leftSubtype;
    int rightSubtype;
    int leftName;
    int rightName;
    int leftItem;
    int rightItem;
    ItemDataRecord *(*defaultLookup)(int);
    ItemDataRecord *(*lookup)(int);
    defaultLookup = Inv_LookupActiveListData;
    asm volatile("" ::: "memory");
    leftItem = *leftIndex;
    rightItem = *rightIndex;
    lookup = g_InvSortLookup;
    if (!(lookup != defaultLookup && leftItem == 0)) {
    if (lookup != defaultLookup) {
        if (rightItem == 0) {
            return -1;
        }
    }

    left = lookup(leftItem);
    right = g_InvSortLookup(rightItem);
    leftKind = g_InvSortRankTable[left->kind];
    rightKind = g_InvSortRankTable[right->kind];
    leftSubtype = g_InvSortTypeRankTable[((unsigned char *)left)[0xE] & 0xF];
    rightSubtype = g_InvSortTypeRankTable[((unsigned char *)right)[0xE] & 0xF];

    if (left != 0) {
        if (right == 0) {
            return -1;
        }
        asm volatile("" ::: "memory");

    } else {
    if (right != 0) {
        return 1;
    }
    }
    if (rightKind < leftKind) {
        return 1;
    }
    if (leftKind < rightKind) {
        return -1;
    }
    if (rightSubtype < leftSubtype) {
        return 1;
    }
    if (leftSubtype < rightSubtype) {
        return -1;
    }
    leftName = left->itemId;
    rightName = right->itemId;
    return (unsigned int)rightName < (unsigned int)leftName
               ? 1
               : -((unsigned int)leftName < (unsigned int)rightName);

    }
    return rightItem != 0;
}

int Inv_CompareItemsByStatForSort(const void *leftElement, const void *rightElement)
{
    s16 *leftIndex = (s16 *)leftElement;
    s16 *rightIndex = (s16 *)rightElement;
    ItemDataRecord *left;
    ItemDataRecord *right;
    int leftValue;
    int rightValue;
    int leftName;
    int rightName;

    leftValue = 0;
    rightValue = 0;
    left = g_InvSortLookup(*leftIndex);
    right = g_InvSortLookup(*rightIndex);

    switch (g_InvSortStatIndex) {
    case 0:
        leftValue = left->baseStats[0] + left->bonusStats[0];
        rightValue = right->baseStats[0] + right->bonusStats[0];
        break;
    case 1:
        leftValue = left->baseStats[1] + left->bonusStats[1];
        rightValue = right->baseStats[1] + right->bonusStats[1];
        break;
    case 2:
        leftValue = left->baseStats[2] + left->bonusStats[2];
        rightValue = right->baseStats[2] + right->bonusStats[2];
        break;
    }

    if (rightValue < leftValue) {
        return -1;
    }
    if (leftValue < rightValue) {
        return 1;
    }

    leftName = left->itemId;
    rightName = right->itemId;
    return (unsigned int)rightName < (unsigned int)leftName
               ? 1
               : -((unsigned int)leftName < (unsigned int)rightName);
}

void Inv_SortWeaponSubrange(void) {
    u16 *var_s0;
    u16 *var_s1;
    s32 mask;
    s32 var_a0;
    u16 tmpu;

    var_s0 = g_InvSortListBase;
    g_InvSortStatIndex = g_InvWeaponSortStatIndex;
    mask = 0x1FE;
    if (var_s0 < g_InvSortListBase + g_InvSortListCount) {
        while (*(s16 *) var_s0 != 0) {
            if ((mask >> g_InvSortLookup((s16)*var_s0)->kind) & 1) {
                break;
            }
            var_s0 += 1;
            if (!(var_s0 < g_InvSortListBase + g_InvSortListCount)) {
                return;
            }
        }
        if (var_s0 < g_InvSortListBase + g_InvSortListCount) {
            tmpu = *var_s0;
            var_s1 = var_s0;
            if (*(s16 *) var_s0 != 0) {
                do {
                    var_a0 = tmpu << 0x10;
                    if (!((mask >> g_InvSortLookup((s16)(var_a0 >> 0x10))->kind) & 1)) {
                        break;
                    }
                    var_s0 += 1;
                    if (!(var_s0 < g_InvSortListBase + g_InvSortListCount)) {
                        break;
                    }
                    tmpu = *var_s0;
                } while (*(s16 *) var_s0 != 0);
                qsort(var_s1, (var_s0 - var_s1), 2, &Inv_CompareItemsByStatForSort);
            }
        }
    }
}

void Inv_SortAmmoSubrange(void) {
    u16 *var_s0;
    u16 *var_s1;
    s32 mask;
    s32 var_a0;
    u16 tmpu;

    var_s0 = g_InvSortListBase;
    g_InvSortStatIndex = g_InvAmmoSortStatIndex;
    mask = 0x200;
    if (var_s0 < g_InvSortListBase + g_InvSortListCount) {
        while (*(s16 *) var_s0 != 0) {
            if ((mask >> g_InvSortLookup((s16)*var_s0)->kind) & 1) {
                break;
            }
            var_s0 += 1;
            if (!(var_s0 < g_InvSortListBase + g_InvSortListCount)) {
                return;
            }
        }
        if (var_s0 < g_InvSortListBase + g_InvSortListCount) {
            tmpu = *var_s0;
            var_s1 = var_s0;
            if (*(s16 *) var_s0 != 0) {
                do {
                    var_a0 = tmpu << 0x10;
                    if (!((mask >> g_InvSortLookup((s16)(var_a0 >> 0x10))->kind) & 1)) {
                        break;
                    }
                    var_s0 += 1;
                    if (!(var_s0 < g_InvSortListBase + g_InvSortListCount)) {
                        break;
                    }
                    tmpu = *var_s0;
                } while (*(s16 *) var_s0 != 0);
                qsort(var_s1, (var_s0 - var_s1), 2, &Inv_CompareItemsByStatForSort);
            }
        }
    }
}

void Inv_SortInventoryByMode(s32 arg0, s32 arg1) {
    s16 *var_a0;
    s16 *var_a0_2;
    s32 temp_s1;
    s32 var_s0;
    s32 var_v1;
    s32 var_v1_2;

    switch (arg0) {
    case 0:
        g_InvSortRankTable = g_InvSortRankTableC;
        g_InvAmmoSortStatIndex = arg1;
        break;
    case 1:
        g_InvSortRankTable = g_InvSortRankTableA;
        g_InvWeaponSortStatIndex = arg1;
        break;
    case 2:
        g_InvSortRankTable = g_InvSortRankTableD;
        if (arg1 != 0) {
            g_InvSortTypeRankTable = g_InvTypeRankTableA;
        } else {
            g_InvSortTypeRankTable = g_InvTypeRankTableB;
        }
        break;
    }
    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
    temp_s1 = g_InvItemPtr[g_AyaEquippedWeaponSlot[0]];
    if (g_AyaEquippedArmorSlot[0] >= 0) {
        var_s0 = g_InvItemPtr[g_AyaEquippedArmorSlot[0]];
    } else {
        var_s0 = -1;
    }
    g_InvSortLookup = &Inv_LookupData;
    g_InvSortListBase = g_InvItemPtr;
    g_InvSortListCount = g_InvSlotLimit;
    qsort(g_InvItemPtr, g_InvSlotLimit, 2, &Inv_CompareItemsForSort);
    Inv_SortWeaponSubrange();
    Inv_SortAmmoSubrange();
    for (var_v1 = 0; var_v1 < g_InvSlotLimit; var_v1++) {
        if (g_InvItemPtr[var_v1] == temp_s1) {
            break;
        }
    }
    if (var_v1 < g_InvSlotLimit) {
        g_AyaEquippedWeaponSlot[0] = var_v1;
    }
    if (var_s0 >= 0) {
        for (var_v1_2 = 0; var_v1_2 < g_InvSlotLimit; var_v1_2++) {
            if (g_InvItemPtr[var_v1_2] == var_s0) {
                break;
            }
        }
        if (var_v1_2 < g_InvSlotLimit) {
            g_AyaEquippedArmorSlot[0] = var_v1_2;
        }
    }
    Inv_RebuildSelectableMask();
}

void Inv_SortContainerItems(int arg0) {
    if (arg0 != 0) {
        g_InvSortRankTable = g_InvSortRankTableA;
        g_InvSortTypeRankTable = g_InvTypeRankTableA;
    } else {
        g_InvSortRankTable = g_InvSortRankTableB;
        g_InvSortTypeRankTable = g_InvTypeRankTableB;
    }

    g_InvSortListBase = g_WayneStorageItems;
    g_InvSortListCount = 0x64;
    g_InvSortLookup = Inv_LookupData;
    qsort(g_WayneStorageItems, 0x64, 2, Inv_CompareItemsForSort);
    Inv_SortWeaponSubrange();
    Inv_SortAmmoSubrange();
    Inv_SortInventoryByMode(2, arg0);
    Inv_TransferItemAlt2(0xF400);
}

void Inv_SortInventoryItems(int arg0, int arg1) {
    if (arg0 != 0) {
        g_InvWeaponSortStatIndex = arg1;
        g_InvSortRankTable = g_InvSortRankTableA;
    } else {
        g_InvAmmoSortStatIndex = arg1;
        g_InvSortRankTable = g_InvSortRankTableB;
    }

    g_InvSortListBase = g_InvItemArray;
    g_InvSortTypeRankTable = g_InvTypeRankTableA;
    g_InvSortListCount = 0x52;
    g_InvSortLookup = Inv_LookupData;
    qsort(g_InvItemArray, 0x52, 2, Inv_CompareItemsForSort);
    Inv_SortWeaponSubrange();
    Inv_SortAmmoSubrange();
    Inv_SortInventoryByMode(arg0, arg1);
    Inv_TransferItemAlt2(0x3803FE);
}

int g_MenuBattleEquipMode;

void Menu_SetBattleEquipMode(int value) {
    g_MenuBattleEquipMode = value;
}

int Menu_GetBattleEquipMode(void) {
    return g_MenuBattleEquipMode;
}