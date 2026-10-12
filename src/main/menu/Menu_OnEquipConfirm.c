/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/menu_state.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_inventory.h"
#include "pe1/text.h"
#include "pe1/inventory.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_item_list_input.h"

void Menu_OnEquipConfirm(MenuWidgetNode *unused, int confirmed)
{
    MenuWidgetNode *node;
    MenuWidgetNode *next;
    if (confirmed) {
        node = MenuWidget_FindByModeAndSelectedBase(2, 7);
        if (g_InvItemUsableFlag) {
            Menu_OpenItemUsePanelAtIndex(MenuWidget_GridCellIndex(node));
            node->cursor_x = -1;
            next = MenuWidget_FindByModeAndSelectedBase(2, 5);
            MenuWidget_SetCurrentNode(next->cursor_x < 0 ?
                MenuWidget_FindByModeAndSelectedBase(2, 6) : next);
            MenuWidget_NavScrollTo(0x35);
        } else if (!Inv_GetSlotItemData(MenuWidget_GridCellIndex(node))) {
            Menu_CreateNotificationDialog(0x1D, 0);
        } else {
            node->cursor_x = -1;
            next = MenuWidget_FindByModeAndSelectedBase(2, 5);
            MenuWidget_SetCurrentNode(next->cursor_x < 0 ?
                MenuWidget_FindByModeAndSelectedBase(2, 6) : next);
            MenuWidget_NavScrollTo(0x35);
        }
        Inv_BuildFilteredPackedList(g_InvItemUsableFlag ? 0x1FE : 0x200);
        node = MenuWidget_FindByModeAndSelectedBase(2, 6);
        if (node == MenuWidget_GetCurrentNode()) {
            node->cursor_y = 0;
            node->cursor_x = 0;
        }
        if (g_InvItemUsableFlag) {
            MenuInput_SetPollingPaused(1);
            g_MenuInputPausePending = 1;
        }
    }
}

/* Confirmation input handling shares the callback's equip-selection flow. */

int Menu_StepEquipConfirm(MenuWidgetNode *node, int input) {
    register MenuWidgetNode *child asm("$16");
    MenuWidgetNode *parent;
    MenuWidgetNode *dialog;
    MenuWidgetNode *option_node;
    int item;
    register int handled asm("$18");
    int allowed;
    int first_width;
    int width;
    register MenuWidgetNode *second_parent asm("$19");
    register u8 *dialog_text asm("$17");
    MenuConfirmCallback callback;

    handled = 0;
    child = MenuWidget_GetChild(node, 0);

    if ((input & 0x10000) != 0) {
        item = Inv_GetPackedListItem(MenuWidget_GridCellIndex(child));
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

        dialog = MenuWidget_CreateSimpleNode(0x29, child, 0, 1);
        option_node = MenuWidget_CreateNode(0x29, dialog, dialog);
        dialog->draw = Menu_DrawItemLabel;
        dialog->update = Menu_ConfirmDialogHandler;
        option_node->draw = Menu_DrawActionOptionList;
        g_MenuActionTextBase = 5;
        MenuWidget_SetCurrentNode(option_node);
        Inv_SelectActiveList(g_MenuActiveItemList);

        dialog_text = g_MenuItemLabelBuffer;
        dialog_text[0] = 0xFF;
        Util_AppendFFTerminatedBytes(dialog_text, (void *)Str_LookupTable4(0));
        D_8009CFA0 = 0;
        callback = Menu_OnEquipConfirm;
        first_width = Draw_MeasureTextWidth(dialog_text);
        if (first_width < 0x78) {
            first_width = 0x78;
        } else {
            first_width = Draw_MeasureTextWidth(dialog_text);
        }

        dialog->grid_width = first_width + 0x14;
        dialog->visible_rows = 0x32;
        dialog->x = (0x12C - first_width) >> 1;
        option_node->x = (dialog->grid_width - 0x80) >> 1;
        {
            int height = dialog->visible_rows;
            g_MenuConfirmCallback = callback;
            option_node->y = height - 0x14;
        }

        Util_CopyFFTerminatedBytes(g_MenuItemLabelBuffer, (void *)Str_LookupTable4(0x71));
        Util_AppendFFTerminatedBytes(g_MenuItemLabelBuffer, Inv_LookupActiveListDisplayData(item));
        if (Draw_MeasureTextWidth(g_MenuItemLabelBuffer) >= 0x78) {
            width = Draw_MeasureTextWidth(g_MenuItemLabelBuffer);
        } else {
            width = 0x78;
        }

        second_parent = MenuWidget_FindByModeAndSelectedBase(1, 0x29);
        /* Copy the returned node before preparing the next lookup. */
        asm volatile("" : : "r"(second_parent));
        second_parent->grid_width = width + 0x14;
        second_parent->x = (0x12C - width) >> 1;
        option_node = MenuWidget_FindByModeAndSelectedBase(2, 0x29);
        option_node->x = (second_parent->grid_width - 0x80) >> 1;
        goto confirm;
        }
simple_confirm:
        Menu_OnEquipConfirm(node, 1);
        goto confirm;
        }
error:
        handled = 1;
        Menu_PlayErrorSound();
        goto done;
    } else if ((input & 0x1040) != 0) {
        if (g_MenuEquipMode != 0) {
            MenuWidget_NavScrollTo(0x35);
            child->cursor_x = -1;
            child->scroll_y = 0;
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
                MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0));
                Menu_StepInventoryRoot(0x33E, -1, -1);
            } else {
                child->cursor_x = -1;
                parent = MenuWidget_FindByModeAndSelectedBase(2, 6);
                option_node = MenuWidget_GetCurrentNode();
                if (parent == option_node) {
                    parent->cursor_x = ((unsigned)parent->has_scroll < 1U);
                    MenuWidget_SetCurrentNode(parent);
                } else {
                    MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 5));
                }
            }
        }

        handled = 1;
        Menu_PlayCancelSound();
    }
done:
    return handled;
}
