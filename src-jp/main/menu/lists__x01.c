/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "pe1/aya.h"
#include "pe1/draw_state.h"
#include "pe1/draw_wipe_rect.h"
#include "pe1/inventory.h"
#include "pe1/inventory_slots.h"
#include "pe1/menu_bonus_stats.h"
#include "pe1/menu_confirm_callback.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_draw_list.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_record.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_memcard_display.h"
#include "pe1/menu_notification_dialog.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_state.h"
#include "pe1/menu_widget.h"
#include "pe1/text.h"

int g_InvItemUsableFlag;
int g_MenuInputPausePending;

void Menu_ActivateItemPickSlot(int unused, int doIt) {
    MenuWidgetNode *node;
    MenuWidgetNode *next;

    if (doIt != 0) {
        node = MenuWidget_FindByModeAndSelectedBase(2, 7);
        if (g_InvItemUsableFlag != 0) {
            Menu_OpenItemUsePanelAtIndex(MenuWidget_GridCellIndex(node));
            node->cursor_x = -1;
            next = MenuWidget_FindByModeAndSelectedBase(2, 5);
            MenuWidget_SetCurrentNode(next->cursor_x >= 0 ? next : MenuWidget_FindByModeAndSelectedBase(2, 6));
            MenuWidget_NavScrollTo(0x35);
        } else if (Inv_GetSlotItemData(MenuWidget_GridCellIndex(node)) == 0) {
            Menu_CreateNotificationDialog(0x1D, 0);
        } else {
            node->cursor_x = -1;
            next = MenuWidget_FindByModeAndSelectedBase(2, 5);
            MenuWidget_SetCurrentNode(next->cursor_x >= 0 ? next : MenuWidget_FindByModeAndSelectedBase(2, 6));
            MenuWidget_NavScrollTo(0x35);
        }
        node = MenuWidget_FindByModeAndSelectedBase(2, 6);
        if (node == MenuWidget_GetCurrentNode()) {
            node->cursor_y = 0;
            node->cursor_x = 0;
        }
        if (g_InvItemUsableFlag != 0) {
            MenuInput_SetPollingPaused(1);
            g_MenuInputPausePending = 1;
        }
    }
}
