/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/draw_level_bar.h"
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_list_input.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_widget.h"
#include "pe1/text.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_InvAmmoSpendActiveList;
int g_InvItemUsableFlag;
int g_InvSelectedItemIndex;
int g_InvSwapSourceList;
int g_InvSwapTargetIndex;
int g_MenuEquipMode;
int g_MenuItemUseMode;
int g_MenuSelectionLocked;
int g_MenuLayoutLocked;

#define NULL ((void *)0)

int Menu_AmmoSpendInputHandler(s32 arg0, s32 arg1);
void Menu_DrawAmmoSpendPanel(void);

int g_MenuSpendArrowDirection;
#undef NULL

/* Equip item selection view, its stat delta preview, grid scroll and slot
 * step, and the skill screen input handler it installs. */

#define NULL ((void *)0)




int Menu_InventoryPageInputHandler(MenuWidgetNode *root, u32 flags) {
    MenuWidgetNode *widget;
    MenuWidgetNode *node;
    int handled;

    handled = 0;
    widget = MenuWidget_GetChild(root, 0);
    node = MenuWidget_FindByModeAndSelectedBase(2, 0xB);

    if (node != 0) {
        handled = Menu_InventoryNavigate(widget, node, flags);
    }

    if (handled == 0 && (flags & 0x40)) {
        if (node != 0) {
            return 1;
        }

        MenuWidget_NavScrollTo(7);
        MenuWidget_DestroyNode(root);
        MenuWidget_NavScrollTo(5);
        MenuWidget_NavScrollTo(0x1C);
        Menu_CreateBonusPointAllocationView();
        Menu_PlayCancelSound();
        return 1;
    }

    if (flags & 0x1000) {
        widget->cursor_x = -1;
        if (node == 0) {
            widget = MenuWidget_FindByModeAndSelectedBase(2, 5);
        } else {
            widget = MenuWidget_FindByModeAndSelectedBase(2, 0x1B);
        }

        if (widget != 0) {
            widget->cursor_x = 0;
            widget->cursor_y = widget->y_limit - 1;
            MenuWidget_SetCurrentNode(widget);
        }
        Menu_PlayMoveSound();
    }

    return 1;
}