/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/akao.h"
#include "pe1/akao/commands.h"
#include "pe1/akao_counters.h"
#include "pe1/akao_send.h"
#include "pe1/aya.h"
#include "pe1/battle_modifiers.h"
#include "pe1/draw_state.h"
#include "pe1/game_state.h"
#include "pe1/game_timers.h"
#include "pe1/inventory.h"
#include "pe1/inventory_category.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/inventory_sort.h"
#include "pe1/inventory_state.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_state.h"
#include "pe1/random.h"
#include "pe1/save.h"
#include "pe1/script_queue.h"
#include "pe1/text.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
s32 g_ItemDropRngIndex;
s32 g_InvPackedListCount;
s16 *g_InvItemPtr;
s32 g_InvSlotLimit;
u32 *g_InvSelectionBits;
s32 g_InvSelectionBitWords;
s32 g_InvPackedListCursor;
s32 g_InvPendingCompactCount;
int g_SaveMetadataWindowIndex;
s16 *g_InvActiveListOverride;
s32 g_InvOverrideSlotLimit;
int g_AkaoPendingBgmVolumeFade;
int g_MemCardDialogState;
unsigned int g_MenuActionSubmenuSel;
unsigned int g_MenuErrorSoundPending;
SavedMenuMode g_SavedMenuMode;

/* Retail passes the window index although the reset ignores it. */
void Save_ResetMetadataWindow(int);
void Menu_ResetStateFlags(void);
void Inv_SetDefaultItemId(void);
void MenuWidget_InitPoolUnk(void);
void Menu_InitStateBuffer(void);
void MemCard_ClearDelayedCallback(void);
void BattleCmd_InitTableCursor(void);
void Inv_BuildItemGridFromCategory(void);
void Menu_SetEquipSlotIndex(int index);

int GameTime_GetZero(void)
{
    return 0;
}

unsigned int GameTime_GetCounterSeconds(int timer) {
    return (unsigned int)g_GameTimers[timer].current / 60U;
}

void GameTime_SetCounterSeconds(int timer, int seconds) {
    g_GameTimers[timer].current = seconds * 60;
}

/* Item-drop random table: seeding from the play clock and the per-draw
 * lagged-xor refresh of the 0x209-byte table. */

int g_ItemDropRngIndex;

#define SCRAMBLE_RANDOM_BYTES()                    \
    {                                           \
        int scramble_i_;                           \
        for (scramble_i_ = 0; scramble_i_ < 0x20; scramble_i_++) { \
            g_ItemDropRngTable[scramble_i_] ^= g_ItemDropRngXorKeyLow[scramble_i_]; \
        }                                          \
        for (scramble_i_ = 0x20; scramble_i_ < 0x209; scramble_i_++) { \
            g_ItemDropRngTable[scramble_i_] ^= g_ItemDropRngXorKeyHigh[scramble_i_]; \
        }                                          \
    }

void Inv_SetDefaultItemId(void) {
    u32 state[0x209];
    u32 seed;
    u32 bits;
    int bit;
    int index;

    seed = (unsigned int)g_GameTimers[0].current / 60;
    bits = 0;

    for (index = 0; index < 0x11; index++) {
        for (bit = 0x1F; bit >= 0; bit--) {
            seed = seed * 0x5D588B65 + 1;
            bits = (bits >> 1) | (seed & 0x80000000);
        }
        state[index] = bits;
    }

    state[0x10] = (state[0x10] << 23) ^ (state[0] >> 9) ^ state[0xF];

    for (index = 0x11; index < 0x209; index++) {
        state[index] = (state[index - 0x11] << 23) ^ (state[index - 0x10] >> 9) ^ state[index - 1];
    }

    for (index = 0; index < 0x209; index++) {
        g_ItemDropRngTable[index] = state[index];
    }

    SCRAMBLE_RANDOM_BYTES();
    SCRAMBLE_RANDOM_BYTES();
    SCRAMBLE_RANDOM_BYTES();

    g_ItemDropRngIndex = 0x208;
}

int Inv_ScrambleGrid(void)
{
    int i;

    g_ItemDropRngIndex++;
    if (g_ItemDropRngIndex >= 0x209) {
        for (i = 0; i < 0x20; i++) {
            g_ItemDropRngTable[i] = g_ItemDropRngTable[i] ^ g_ItemDropRngXorKeyLow[i];
        }
        for (i = 0x20; i < 0x209; i++) {
            g_ItemDropRngTable[i] = g_ItemDropRngTable[i] ^ g_ItemDropRngXorKeyHigh[i];
        }
        g_ItemDropRngIndex = 0;
    }

    return g_ItemDropRngTable[g_ItemDropRngIndex];
}

/* Text byte-string helpers, the item grid rebuild and the active inventory
 * list selection (Aya's list or a storage override) with its slot limit. */

s16 *g_InvItemPtr;
s16 *g_InvActiveListOverride;
int g_InvSlotLimit;
int g_InvOverrideSlotLimit;
u32 *g_InvSelectionBits;
int g_InvSelectionBitWords;

void Text_CopyFFTerminatedBytes(u8 *dst, u8 *src)
{
    u8 first;

    first = *src++;
    *dst++ = first;
    if (first != 0xFF) {
        int term;
        int next;

        term = 0xFF;
        do {
            next = *src++;
            *dst++ = next;
        } while (next != term);
    }
}

void Text_AppendFFTerminatedBytes(u8 *dst, u8 *src)
{
    int scan;
    u8 first;

    scan = *dst++;
    if (scan != 0xFF) {
        int term;
        int next;

        term = 0xFF;
        do {
            next = *dst++;
        } while (next != term);
    }

    dst--;

    first = *src++;
    *dst++ = first;
    if (first != 0xFF) {
        int term;
        int next;

        term = 0xFF;
        do {
            next = *src++;
            *dst++ = next;
        } while (next != term);
    }
}

void Inv_BuildItemGridFromCategory(void)
{
    int i;
    s16 *clear;
    ItemDataRecord *src;
    int category;
    int base;
    int lookup;

    Inv_SelectActiveList(0);

    i = 0x31;
    clear = &g_AyaInventoryItems[0x31];
    do {
        *clear = 0;
        i--;
        clear--;
    } while (i >= 0);

    i = 0;
    category = 0x13;
    do {
        lookup = i;
        i++;
        src = Item_LookupBaseData(lookup);
    } while (src != 0 && src->kind != category);

    g_InvCategoryBaseItemId = i;

    /* Nine grid cells cycle through the three records after the first
     * category match; each cell clears its third base stat and sets the
     * third bonus to the 999 placeholder. */
    i = 0;
    do {
        base = g_InvCategoryBaseItemId + i % 3;
        src = Item_LookupBaseData(base - 1);
        g_InvSpecialItemRecords[i] = *src;
        g_InvSpecialItemRecords[i].baseStats[2] = 0;
        g_InvSpecialItemRecords[i].bonusStats[2] = 999;
    } while (++i < 9);

    i = 0x63;
    clear = &g_WayneStorageItems[0x63];
    do {
        *clear = 0;
        i--;
        clear--;
    } while (i >= 0);

    i = 0x51;
    clear = (s16 *)&g_InvItemArray[0x51];
    do {
        *clear = 0;
        i--;
        clear--;
    } while (i >= 0);

    g_InvActiveListOverride = 0;
}

void Inv_SelectActiveList(int useOverride) {
    if (useOverride != 0 && g_InvActiveListOverride != 0) {
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

void Inv_SetActiveListOverride(s16 *items, int count) {
    g_InvActiveListOverride = items;
    g_InvOverrideSlotLimit = count;
}

void Inv_ResetActiveList(void) {
    g_InvActiveListOverride = 0;
    g_InvOverrideSlotLimit = 0;
    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
}

int Inv_IsActiveListOverrideSelected(void) {
    return g_InvItemPtr != g_AyaInventoryItems;
}

void Inv_SetAyaSlotCount(int count) {
    if (count >= 0x33) {
        count = 0x32;
    }

    g_AyaInventorySlotCount[0] = count;
    if (g_InvItemPtr == (s16 *)(g_AyaInventorySlotCount + 0x3C)) {
        g_InvSlotLimit = Inv_GetAyaSlotLimit();
    }
}

int Inv_GetAyaSlotLimit(void) {
    if (g_AyaInventorySlotCount[0] + Inv_GetBonusSlotCount() >= 0x33) {
        return 0x32;
    }

    return g_AyaInventorySlotCount[0] + Inv_GetBonusSlotCount();
}

void Inv_ClearEquipFlagForKind(InvItemSlot *obj) {
    u8 *base;
    u8 *entry;
    u8 *end;
    int kind;
    int is_kind_9;
    int kind_9;
    int one;

    if (obj != 0) {
        kind = obj->kind;
        base = (u8 *)g_InvItemSlotArray;
        kind ^= 9;
        is_kind_9 = (unsigned int)kind < 1;
        kind = (int)(base + 0x1000);
        if (base < (u8 *)kind) {
            kind_9 = 9;
            one = 1;
            entry = base + 5;
            end = base + 0x1005;

            do {
                if (entry[1] != kind_9) {
                    if (is_kind_9 == one) {
                        entry += 0x20;
                        continue;
                    }
                } else if (is_kind_9 == 0) {
                    entry += 0x20;
                    continue;
                }

                entry[0] &= 0xEF;
                entry += 0x20;
            } while (entry < end);
        }

        obj->flags |= 0x10;
    }
}

#define NULL ((void *)0)

void *Inv_LookupActiveListDisplayData(s32 index);

void *Inv_LookupActiveListDisplayData(s32 index) {
    s16 itemId;
    u32 lookupIndex;
    ItemDataRecord *entry;
    void *displayData;

    itemId = g_InvItemPtr[index];
    displayData = NULL;
    if ((itemId - 0x100) < 0x80U) {
        entry = &((ItemDataRecord *)g_EquipItemDataTable)[itemId];
        if (entry->flags & 0x10) {
            displayData = g_EquipItemDataTable + 0x31F8;
            if (entry->kind == 9) {
                displayData = g_EquipItemDataTable + 0x3208;
            }
        } else {
            lookupIndex = entry->itemId - 1;
            displayData = Text_LookupTable8(lookupIndex);
        }
    } else {
        lookupIndex = itemId - 1;
        if (lookupIndex >= 0xFFU) {
            if ((itemId - 0x200) < 9U) {
                lookupIndex = (g_InvCategoryBaseItemId + itemId) - 0x201;
                displayData = Text_LookupTable8(lookupIndex);
            }
        } else {
            displayData = Text_LookupTable8(lookupIndex);
        }
    }
    return displayData;
}
#undef NULL


int g_InvSlotLimit;
s16 *g_InvItemPtr;

#define NULL ((void *)0)

#undef NULL


/* Insert an item into a free active slot. Non-zero means the insert failed. */
#define NULL ((void *)0)


int g_InvCategoryBaseItemId;
s16 *g_InvItemPtr;
int g_InvSlotLimit;

/* Contiguous inventory counts, removal-by-ID, active-item kind lookup, base
 * item kinds, the equipped-slot reservation test and the packed-list
 * accessors. */
s16 *g_InvItemPtr;
int g_InvSlotLimit;
u32 *g_InvSelectionBits;
int g_InvSelectionBitWords;

/* The equipped-slot bytes live at 0x800C0E20/0x800C0E22, outside the gp
 * window; incomplete array types keep -G8 from making them small data. */

int g_InvPackedListCount;

int g_InvPackedListCursor;