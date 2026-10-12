/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "common.h"
#include "pe1/inventory.h"
#include "pe1/menu_item_rows.h"
#include "pe1/save.h"
#include "pe1/text.h"

int g_InvActiveListCount;

int g_InvActiveListCount;
short g_InvActiveListItems[];

int Menu_GetBattleCount(void) {
    return g_InvActiveListCount;
}

int Menu_GetBattleCountEntry(int arg0) {
    if (arg0 >= 0 && arg0 < g_InvActiveListCount) {
        return g_InvActiveListItems[arg0];
    }
    return 0;
}

void Inv_ShowItemDescriptionPanel(s32 index) {
    int value;
    ItemDataRecord *entry;
    u8 *text;

    value = g_InvActiveListItems[index];
    if (value < 0x200) {
        if ((unsigned int)(value - 0x100) < 0x80U) {
            entry = (ItemDataRecord *)(g_EquipItemDataTable + (value << 5));
        } else if ((unsigned int)(value - 1) < 0xFFU) {
            entry = Item_LookupBaseData(value - 1);
        } else if ((unsigned int)(value - 0x200) < 9U) {
            entry = (ItemDataRecord *)(g_KeyItemDataTable + (value << 5));
        } else {
            entry = 0;
        }
        if (entry != 0) {
            Sfx_DrawSlotRow(entry, Str_LookupTable8(entry->itemId - 1));
        }
    } else {
        entry = (ItemDataRecord *)(g_KeyItemDataTable + (value << 5));
        if (entry->flags & ITEM_DATA_FLAG_GENERIC_DESCRIPTION) {
            text = g_CursorRenderDataBlock;
            if (entry->kind == 9) {
                text = g_CursorRenderDataBlock + 0x10;
            }
        } else {
            text = Str_LookupTable8(entry->itemId - 1);
        }
        Sfx_DrawSlotRow(entry, text);
    }
}
