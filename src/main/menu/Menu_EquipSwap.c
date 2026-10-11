/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "common.h"
#include "pe1/inventory.h"
#include "pe1/inventory_slots.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/psyq_nop.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_widget.h"

#define NULL ((void *)0)

/* Equipment swap: picking the Aya inventory item to exchange records the
 * swap source; returning from the equip screen reopens the inventory. */

int g_MenuEquipSwapSource;
int g_MenuSwapReturnPending;

s32 Menu_HandleAyaInventorySelectionInput(s32 arg0, s32 arg1)
{
    s32 temp_v0_2;
    void *temp_v0;

    if (arg1 & 0x10000) {
        temp_v0 = MenuWidget_FindByModeAndSelectedBase(2, 1);
        {
            s32 item_id;

            item_id = g_AyaInventoryItems[MenuWidget_GridCellIndex(temp_v0)];
            /* Match debt: MASPSX does not fill the load delay after the
             * indexed halfword load ahead of a $gp store; ASPSX did. */
            PE1_NOP();
            g_MenuEquipSwapItem = item_id;
        }
        temp_v0_2 = MenuWidget_GridCellIndex(temp_v0) | (((MenuWidgetNode *)temp_v0)->scroll_y << 8);
        g_MenuEquipSwapSource = temp_v0_2 + 1;
        Inv_RemoveActiveListItem(temp_v0_2 & 0xFF);
        MenuWidget_DestroyNode(arg0);
        MenuWidget_NavScrollTo(0x1B);
        MenuWidget_NavScrollTo(1);
        MenuWidget_NavScrollTo(0);
        Menu_OpenEquipScreen(MenuWidget_GridCellIndex(MenuWidget_GetChild(arg0, 0)));
        Menu_PlayConfirmSound();
        return 1;
    }
    if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0));
        MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 1));
        MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x1B));
        Menu_PlayCancelSound();
    }
    return 1;
}

void Menu_SetSwapReturnFlag(void) {
    if (g_MenuEquipSwapSource != 0) {
        g_MenuSwapReturnPending = 1;
    }
}

void Menu_ProcessSwapReturnIfPending(void) {
    if (g_MenuSwapReturnPending != 0) {
        Menu_OpenInventoryScreen();
        g_MenuSwapReturnPending = 0;
    }
}
