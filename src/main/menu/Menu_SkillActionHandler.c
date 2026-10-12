/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "pe1/menu_state.h"
#include "common.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_list_input.h"
#include "pe1/text.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_widget.h"

void Menu_StepItemDetailPanel2(void);

int g_MenuBattleSelectedItem;

s32 Menu_SkillActionHandler(MenuWidgetNode *node, s32 flags) {
    MenuWidgetNode *child;
    s32 handled;

    handled = 0;
    child = MenuWidget_GetChild(node, 0);
    if (flags & 0x10000) {
        if (MenuWidget_GetCellIndex(child) >= 0) {
            handled = 1;
            Menu_StepItemDetailPanel2();
            Menu_PlayConfirmSound();
        } else {
            handled = 1;
            Menu_PlayErrorSound();
        }
    } else if (flags & 0x40) {
        MenuWidget_DestroyNode(node);
        MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x1D));
        Menu_CreateBonusPointAllocationView();
        handled = 1;
        Menu_PlayCancelSound();
    }
    return handled;
}

void Menu_StepItemDetailPanel2(void) {
    MenuWidgetNode *node;
    MenuWidgetNode *current;
    u8 *label;
    MenuWidgetNode *option_node;
    int width;
    MenuConfirmCallback callback;
    int height;

    g_MenuSelectedSpell = Inv_GetPackedListItem(MenuWidget_GridCellIndex(MenuWidget_FindByModeAndSelectedBase(2, 8)));
    current = MenuWidget_GetCurrentNode();
    label = Str_LookupTable8(g_MenuSelectedSpell + 0xEB);
    node = MenuWidget_CreateSimpleNode(0x29, current, 0, 1);
    option_node = MenuWidget_CreateNode(0x29, node, node);
    node->draw = Menu_DrawItemLabel;
    node->update = Menu_ConfirmDialogHandler;
    option_node->draw = Menu_DrawActionOptionList;
    g_MenuActionTextBase = 5;
    MenuWidget_SetCurrentNode(option_node);
    callback = Menu_OnBattleCommandConfirm;
    Inv_SelectActiveList(g_MenuActiveItemList);

    if (label != 0) {
        Util_CopyFFTerminatedBytes(g_MenuItemLabelBuffer, label);
    } else {
        g_MenuItemLabelBuffer[0] = 0xFF;
    }

    {
        u8 *suffix;
        register u8 *text_buf asm("$4");

        suffix = Str_LookupTable4(0x1A);
        text_buf = g_MenuItemLabelBuffer;
        Util_AppendFFTerminatedBytes(text_buf, suffix);
        D_8009CFA0 = 0;
        text_buf = g_MenuItemLabelBuffer;
        width = Draw_MeasureTextWidth(text_buf);
        if (width < 0x78) {
            width = 0x78;
        } else {
            text_buf = g_MenuItemLabelBuffer;
            width = Draw_MeasureTextWidth(text_buf);
        }
    }

    node->grid_width = width + 0x14;
    node->visible_rows = 0x32;
    node->x = (0x12C - width) >> 1;
    option_node->x = (node->grid_width - 0x80) >> 1;
    height = node->visible_rows;
    g_MenuConfirmCallback = callback;
    option_node->y = height - 0x14;
}

void Menu_OnBattleCommandConfirm(MenuWidgetNode *arg0, int arg1) {
    if (arg1 != 0) {
        if (Menu_GetBattleEquipMode() != 0) {
            MenuWidget_NavScrollTo(8);
        }
        Battle_UseItem(g_MenuBattleSelectedItem);
    }
}
