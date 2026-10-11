/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/inventory.h"
#include "pe1/inventory_slots.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory_root.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_widget.h"
#include "pe1/draw_state.h"

void Menu_HandleInput(int arg0) {
    register int value;
    int flag;
    int blocked;

    if (arg0 >= Inv_GetPackedListCount()) {
        return;
    }

    if (g_MenuItemRenameMode != 0 || g_MenuItemDiscardMode != 0) {
        if (Inv_TestSelectionBit(arg0) == 0) {
            Draw_SetTextDimmed(1);
        }
    }

    value = Inv_GetPackedListItem(arg0);
    blocked = Inv_IsAyaInventorySlotReserved(value);

    if (blocked != 0) {
        if (g_MenuActiveWidget->selected_base != 0xD) {
            if (g_MenuLayoutLocked == 0 && g_MenuItemUseMode == 0) {
                Draw_SetTextDimmed(1);
            }
        }
    }

    if (g_MenuLayoutLocked != 0) {
    flag = 0;
    if (g_StatBaseTable[0] + g_StatBaseTable[1] + g_StatBaseTable[2] + g_StatBaseTable[3] != 0) {
        if ((Inv_LookupActiveListData(value)->flags & 0x40) == 0) {
            int mask;

            if (Inv_GetActiveListItemType(value) >= 6) {
                mask = 1 << Inv_GetActiveListItemType(value);
            } else {
                mask = 0x3E;
            }

            if (Inv_SetupSlotDisplay(mask) != 0) {
                flag = 1;
            }
        }
    }

    if (flag == 0) {
        Draw_SetTextDimmed(1);
    }
    }

    Sfx_DrawActiveListSlot(value);
    if (blocked != 0) {
        Draw_DropShadow();
    }
}
