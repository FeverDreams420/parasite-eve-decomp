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


/* Text byte-string helpers, the item grid rebuild and the active inventory
 * list selection (Aya's list or a storage override) with its slot limit. */

s16 *g_InvItemPtr;
s16 *g_InvActiveListOverride;
int g_InvSlotLimit;
int g_InvOverrideSlotLimit;
u32 *g_InvSelectionBits;
int g_InvSelectionBitWords;

#define NULL ((void *)0)

void *Inv_LookupActiveListDisplayData(s32 index);
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



/* First empty slot of the active list, or -1. */
static inline int FindFreeSlot(void) {
    s16 *p = g_InvItemPtr, *end = p + g_InvSlotLimit;
    int slot;

    if (p < end) {
        do {
            if (!*p) break;
            p++;
        } while (p < end);
        if (p < g_InvItemPtr + g_InvSlotLimit) {
            slot = p - g_InvItemPtr;
        } else {
            slot = -1;
        }
    } else {
        slot = -1;
    }
    return slot;
}

/* JP compacts the list right away instead of leaving the count in
 * g_InvPendingCompactCount for Inv_CompactActiveListSlots. */
int Inv_CheckFreeSlotCapacity(int requested_slots) {
    s16 *slot;
    s16 *end;
    int capacity;
    int capacity_before_scan;
    int bonus_slots;
    int used_slots;
    int occupied;
    int result;
    int index, limit, from;

    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;

    bonus_slots = Inv_GetBonusSlotCount();
    if (g_InvBaseCapacity + bonus_slots < 51) {
        used_slots = 0;
        capacity_before_scan =
            g_InvBaseCapacity + Inv_GetBonusSlotCount();
    } else {
        used_slots = 0;
        capacity_before_scan = 50;
    }

    g_InvItemPtr = g_AyaInventoryItems;
    capacity = capacity_before_scan;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;

    slot = g_InvItemPtr;
    end = slot + g_InvSlotLimit;
    while (slot < end) {
        occupied = *slot++;
        occupied = occupied != 0;
        used_slots += occupied;
    }

    result = requested_slots <= capacity - used_slots;
    if (result == 1) {
        while (requested_slots > 0) {
            index = FindFreeSlot();
            if (index >= 0) {
                if (g_InvBaseCapacity + Inv_GetBonusSlotCount() < 51) {
                    limit = g_InvBaseCapacity + Inv_GetBonusSlotCount();
                } else {
                    limit = 50;
                }
                from = limit - requested_slots;
                g_InvItemPtr[index] = g_InvItemPtr[from];
                g_InvItemPtr[from] = 0;
                if (g_InvTrackedSlots[0] == from) {
                    g_InvTrackedSlots[0] = index;
                } else if (g_InvTrackedSlots[2] == from) {
                    g_InvTrackedSlots[2] = index;
                }
            }
            requested_slots--;
        }
    }
    return result;
}

int Inv_CheckItemEquippable(unsigned int list_index, int modifier_index) {
    /* Match the retail 0x60-byte stack frame; remove with a cleaner TU model. */
    unsigned char stack_pad[32];
    int selected_a;
    int selected_b;
    ItemDataRecord *item_a;
    ItemDataRecord *item_b;
    int item_id;
    int count;
    int i;
    int result;
    int mask;
    int modifier;
    int existing;
    int different_slot;

    mask = 0;
    different_slot = 0;
    selected_a = Inv_RestoreSelection(list_index);
    item_a = 0;
    if (selected_a >= 0 && selected_a < g_InvSlotLimit) {
        ItemDataRecord *resolved;
        int saved_id;
        int scaled_id;
        item_id = g_InvItemPtr[selected_a];
        saved_id = (s16)item_id;
        if (item_id - 0x100U < 0x80U) {
            resolved = (ItemDataRecord *)(g_EquipItemDataTable + (item_id << 5));
        } else if (item_id - 1U < 0xFFU) {
            resolved = Item_LookupBaseData(item_id - 1);
        } else if (saved_id - 0x200U < 9U) {
            resolved = &((ItemDataRecord *)g_KeyItemDataTable)[saved_id];
        } else {
            resolved = 0;
        }
        item_a = resolved;
    }

    selected_b = Inv_RestoreSelection(list_index < 1);
    item_b = 0;
    if (selected_b >= 0 && selected_b < g_InvSlotLimit) {
        ItemDataRecord *resolved;
        int saved_id;
        int scaled_id;
        item_id = g_InvItemPtr[selected_b];
        saved_id = (s16)item_id;
        if (item_id - 0x100U < 0x80U) {
            scaled_id = item_id << 5;
            resolved = (ItemDataRecord *)(g_EquipItemDataTable + scaled_id);
        } else if (item_id - 1U < 0xFFU) {
            resolved = Item_LookupBaseData(item_id - 1);
        } else if (saved_id - 0x200U < 9U) {
            resolved = &((ItemDataRecord *)g_KeyItemDataTable)[saved_id];
        } else {
            resolved = 0;
        }
        item_b = resolved;
    }

    if (modifier_index >= 0) {
        count = item_b->tailCount;
        modifier = item_a->tailData[modifier_index];
        i = 0;
        if (count > 0) {
            while (1) {
                if (item_b->tailData[i] == modifier) {
                    break;
                }
                ++i;
                if (i >= count) {
                    break;
                }
            }
            if (i < item_b->tailCount) return 4;
        }

        for (i = 0; i < item_b->tailCount; i++) {
            if (item_b->tailData[i] == 0) {
                break;
            }
        }

        if (i >= item_b->tailCount) {
            modifier = item_a->tailData[modifier_index];
            modifier &= 0xE0;
            if (modifier == 0) {
                return 3;
            }

            {
                int third_count;
                int third_modifier;
                third_count = item_b->tailCount;
                if (third_count > 0) {
                    i = 0;
                    third_modifier = modifier;
                    /* The modifier kind is saved; reuse its scratch for the bound. */
                    modifier = third_count;
                    while (1) {
                        if ((item_b->tailData[i] & 0xE0) == third_modifier) {
                            break;
                        }
                        ++i;
                        if (i >= modifier) {
                            break;
                        }
                    }
                    if (i >= item_b->tailCount) return 3;
                } else {
                    return 3;
                }
            }
        }
    }

    Inv_RestoreSelection(list_index);
    if (g_InvItemPtr == g_AyaInventoryItems && selected_a == g_AyaEquippedArmorSlot[0]) {
        int raw_count;
        raw_count = item_a->tailCount;
        i = 0;
        if (raw_count > 0) {
            /* The modifier scratch is free again; it holds the bound. */
            modifier = raw_count;
            while (1) {
                raw_count = item_a->tailData[i];
                raw_count &= 0x1F;
                existing = raw_count - 8;
                if ((unsigned int)existing < 3) {
                    break;
                }
                ++i;
                if (i >= modifier) {
                    break;
                }
            }
            if (i < item_a->tailCount) {
                mask = 1 << existing;
                different_slot = i != modifier_index;
            }
        }
    } else {
        Inv_RestoreSelection(list_index < 1);
        if (g_InvItemPtr == g_AyaInventoryItems) {
            if (selected_b == g_AyaEquippedArmorSlot[0] && modifier_index >= 0) {
                int raw;
                int final_count;
                int final_existing;
                raw = item_a->tailData[modifier_index];
                raw &= 0x1F;
                /* The earlier scan is finished; reuse its count for the requested slot. */
                count = raw - 8;
                if ((unsigned int)count < 3) {
                    raw = item_b->tailCount;
                    i = 0;
                    if (raw > 0) {
                        final_count = raw;
                        while (1) {
                            raw = item_b->tailData[i];
                            raw &= 0x1F;
                            final_existing = raw - 8;
                            if ((unsigned int)final_existing < 3) {
                                break;
                            }
                            ++i;
                            if (i >= final_count) {
                                break;
                            }
                        }
                        if (i < item_b->tailCount) {
                            mask = (1 << count) - (1 << final_existing);
                        }
                    }
                }
            } else {
                return 1;
            }
        } else {
            return 1;
        }
    }

    result = Inv_CheckFreeSlotCapacity(mask);
    if (result != 1 || !different_slot) {
        return result;
    }
    return 2;
}