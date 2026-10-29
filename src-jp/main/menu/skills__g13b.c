/* CC1_FLAGS: -G8 -fno-expensive-optimizations */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/inventory_sort.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_draw_list.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_list_input.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_state.h"
#include "pe1/menu_widget.h"
#include "pe1/save_tail_state.h"
#include "pe1/text.h"
#include "m2c_macros.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_MenuItemPrimListHandles[2];
int g_InvSelectedItemIndex;
int g_InvSwapTargetIndex;
int g_MenuActionSubmenuOpen;
int g_MenuActionTextBase;
int g_MenuActiveItemList;
MenuConfirmCallback g_MenuConfirmCallback;
int g_MenuEquipMode;
int g_MenuEquipSwapSource;
int g_MenuSelectionLocked;
int g_MenuLayoutLocked;
int g_MenuSelectedSpell;

int g_MenuInputPausePending;

#define NULL ((void *)0)
void Inv_BuildFilteredPackedListExcluding(int mask, int excluded);
int Menu_StepEquipConfirm(MenuWidgetNode *node, int input);
void Menu_DrawEquipSelectionList(MenuWidgetNode *node);
#undef NULL


void Menu_StepItemDetailPanel2(void);

int g_MenuSelectedSpell;

#define NULL ((void *)0)

void Menu_DrawEquipSortToggleList(int node);
int Menu_EquipGridHandler(void *node, int flags);
void Menu_CreateEquipInfoPanel(int parent, unsigned int option);
int Menu_EquipSelectInput(void *node, unsigned int flags);
void Menu_DrawEquipInfoOptionList(int node);
void Menu_DrawWeaponModSlots(s32 index);

int g_MenuInventoryViewMode;
int g_InvItemUsableFlag;
/* Confirmation input handling; the equip callback (Menu_OnEquipConfirm) lives in another unit in JP. */

int Menu_StepEquipConfirm(MenuWidgetNode *node, int input) {
    MenuWidgetNode *child;
    MenuWidgetNode *parent;
    MenuWidgetNode *dialog;
    MenuWidgetNode *option_node;
    MenuWidgetNode *next;
    int item;
    int handled;
    int allowed;
    int width;
    u8 *dialog_text;
    MenuConfirmCallback callback;

    handled = 0;
    child = MenuWidget_GetChild(node, 0);
    parent = child;

    if ((input & 0x10000) != 0) {
        item = Inv_GetPackedListItem(MenuWidget_GridCellIndex(parent));
        if (g_MenuLayoutLocked != 0) {
            Inv_SelectActiveList(MenuWidget_GridCellIndex(
                MenuWidget_FindByModeAndSelectedBase(2, 0x36)) == 1);

            allowed = 0;
            if ((g_StatBaseTable[0] + g_StatBaseTable[1] + g_StatBaseTable[2] + g_StatBaseTable[3]) != 0) {
                unsigned mask = Inv_LookupActiveListData(item)->flags & 0x40;
                allowed = mask < 1U;
            }

            if (allowed == 0) goto error;

            Inv_RememberSelection(1, item);
            MenuWidget_NavScrollTo(0x36);
            Menu_ConfigureScreen();
        } else {
            goto reserved;
        }
confirm:
        handled = 1;
        Menu_PlayConfirmSound();
        goto done;
reserved:
        if (Inv_IsAyaInventorySlotReserved(item) == 0) {
        if (Menu_GetBattleEquipMode() != 0) {

        dialog_text = Inv_LookupActiveListDisplayData(item);
        dialog = MenuWidget_CreateSimpleNode(0x29, parent, 0, 1);
        option_node = MenuWidget_CreateNode(0x29, dialog, dialog);
        dialog->draw = Menu_DrawItemLabel;
        dialog->update = Menu_ConfirmDialogHandler;
        option_node->draw = Menu_DrawActionOptionList;
        g_MenuActionTextBase = 5;
        MenuWidget_SetCurrentNode(option_node);
        callback = Menu_OnEquipConfirm;
        Inv_SelectActiveList(g_MenuActiveItemList);

        if (dialog_text) Text_CopyFFTerminatedBytes(g_MenuItemLabelBuffer, dialog_text);
        else g_MenuItemLabelBuffer[0] = 0xFF;
        Text_AppendFFTerminatedBytes(g_MenuItemLabelBuffer, Text_LookupTable4(0x71));
        g_MenuItemPrimListHandles[0] = 0;
        width = Draw_MeasureTextWidth(g_MenuItemLabelBuffer) < 120 ? 120 : Draw_MeasureTextWidth(g_MenuItemLabelBuffer);
        dialog->grid_width = width + 20;
        dialog->visible_rows = 50;
        dialog->x = (300 - width) >> 1;
        option_node->x = (dialog->grid_width - 128) >> 1;
        option_node->y = dialog->visible_rows - 20;
        g_MenuConfirmCallback = callback;
        goto confirm;
        }
simple_confirm:
        /* JP runs the confirm callback body in place, without rebuilding the packed list. */
        parent = MenuWidget_FindByModeAndSelectedBase(2, 7);
        if (g_InvItemUsableFlag) {
            Menu_OpenItemUsePanelAtIndex(MenuWidget_GridCellIndex(parent));
            parent->cursor_x = -1;
            child = MenuWidget_FindByModeAndSelectedBase(2, 5);
            MenuWidget_SetCurrentNode(child->cursor_x < 0 ?
                MenuWidget_FindByModeAndSelectedBase(2, 6) : child);
            MenuWidget_NavScrollTo(0x35);
        } else if (!Inv_GetSlotItemData(MenuWidget_GridCellIndex(parent))) {
            Menu_CreateNotificationDialog(0x1D, 0);
        } else {
            parent->cursor_x = -1;
            child = MenuWidget_FindByModeAndSelectedBase(2, 5);
            MenuWidget_SetCurrentNode(child->cursor_x < 0 ?
                MenuWidget_FindByModeAndSelectedBase(2, 6) : child);
            MenuWidget_NavScrollTo(0x35);
        }
        parent = MenuWidget_FindByModeAndSelectedBase(2, 6);
        if (parent == MenuWidget_GetCurrentNode()) {
            parent->cursor_y = 0;
            parent->cursor_x = 0;
        }
        if (g_InvItemUsableFlag) {
            MenuInput_SetPollingPaused(1);
            g_MenuInputPausePending = 1;
        }
        goto confirm;
        }
error:
        handled = 1;
        Menu_PlayErrorSound();
        goto done;
    } else if ((input & 0x1040) != 0) {
        if (g_MenuEquipMode != 0) {
            MenuWidget_NavScrollTo(0x35);
            parent->cursor_x = -1;
            parent->scroll_y = 0;
            MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0x36));
        } else {
            if ((input & 0x40) == 0) {
                handled = 1;
                goto done;
            }

            MenuWidget_NavScrollTo(0x35);
            if (g_MenuLayoutLocked != 0) {
                MenuWidget_DestroyNode(node);
                MenuWidget_NavScrollTo(5);
                MenuWidget_NavScrollTo(6);
                MenuWidget_NavScrollTo(0x1C);
                MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0));
                Menu_StepInventoryRoot(0x33E, -1, -1);
            } else {
                child->cursor_x = -1;
                parent = MenuWidget_FindByModeAndSelectedBase(2, 6);
                if (parent == MenuWidget_GetCurrentNode()) {
                    int hidden = ((unsigned)parent->has_scroll < 1U);
                    next = parent;
                    next->cursor_x = hidden;
                } else {
                    next = MenuWidget_FindByModeAndSelectedBase(2, 5);
                }
                MenuWidget_SetCurrentNode(next);
            }
        }

        handled = 1;
        Menu_PlayCancelSound();
    }
done:
    return handled;
}