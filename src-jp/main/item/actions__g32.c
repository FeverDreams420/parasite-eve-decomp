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
int Inv_CheckFreeSlotCapacity(int requested_slots);



s32 Inv_RemoveActiveListItem(s32 arg0) {
    s32 sp10;
    s16 *slot;
    s32 removed;
    s32 offset;
    s32 activeList;

    if ((g_InvItemPtr == g_AyaInventoryItems) && (g_EquippedArmorSlot == arg0)) {
        Inv_GetActiveSlotCount(&sp10);
        Inv_CheckFreeSlotCapacity(sp10);
        g_EquippedArmorSlot = -1;
        Inv_SetActiveList(3, 0);
    }

    activeList = g_InvItemPtr;
    offset = arg0 << 1;
    slot = (s16 *)(offset + activeList);
    removed = *slot;
    *slot = 0;

    if (removed >= 0x100) {
        g_InvItemSlotArray[removed - 0x100].iconId = 0;
    }

    return removed;
}