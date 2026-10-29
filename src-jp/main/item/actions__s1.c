/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/aya.h"
#include "pe1/battle_cmd.h"
#include "pe1/battle_modifiers.h"
#include "pe1/inventory.h"
#include "pe1/inventory_slots.h"
#include "pe1/inventory_state.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_state.h"
#include "pe1/random.h"
#include "pe1/text.h"
#include "m2c_macros.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
s16 *g_InvItemPtr;
s32 g_InvSlotLimit;
u32 *g_InvSelectionBits;
s32 g_InvSelectionBitWords;
s32 g_InvPendingCompactCount;
int g_InvCategoryBaseItemId;
s32 g_ItemDropRngIndex;

int g_InvPackedListCursor;
int g_InvPackedListCount;

int g_InvPackedListCount;
short g_InvPackedList[];

unsigned int *g_InvSelectionBits;
int g_InvSelectionBitWords;

/* Item selection masks, ammunition transfer between compared weapons, the
 * weapon comparison panel and the per-slot item actions and active-list controller. Contiguous at
 * 0x80055760 and joined by the comparison records and list state. */

ItemDataRecord *g_InvCompareLeftItem, *g_InvCompareRightItem;
s16 *g_InvActiveListOverride;
s32 g_InvOverrideSlotLimit;
int g_InvActiveListCount;
int g_MenuBattleEquipMode;
void Menu_DrawSlotRow(ItemDataRecord *entry, u8 *text);

int Inv_LoadWayneItemsAsOverride(short *items) {
    int count = 0;

    if (items != 0) {
        int base = g_InvCategoryBaseItemId;
        u16 *out = g_InvActiveListItems;
        int end = base + 3;
        int id;
        while (count < 10 && (id = items[0]) != 0) {
            if ((base <= id) && (id < end)) {
                int temp = id + 6;

                id = temp - base;
                temp = id + 0x200;
                *out = temp;
                g_InvCategoryItemTable[id].count = (u16)items[1];
                out++;
            } else {
                *out = id;
                out++;
            }

            count++;
            items += 2;
        }

        g_InvActiveListOverride = g_InvActiveListItems;
        g_InvOverrideSlotLimit = count;
        Inv_RebuildSelectableMask();
    }

    g_InvActiveListCount = count;
    return count;
}