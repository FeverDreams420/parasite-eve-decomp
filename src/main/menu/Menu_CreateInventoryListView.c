/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_list_input.h"

void Menu_CreateInventoryListView(MenuWidgetNode *parent) {
    MenuWidgetNode *root;
    MenuWidgetNode *node;

    root = MenuWidget_CreateSimpleNode(6, parent, 0, 0);
    node = MenuWidget_CreateNode(6, root, root);
    root->update = (void (*)())Menu_InventoryPageInputHandler;
    root->disabled = 1;
    node->draw = Menu_DrawUsableItemActionList;
    node->layout_flags |= 0x80;
    if (g_MenuLayoutLocked != 0) {
        MenuWidget_ClearColumnLayout(node);
    } else if (g_InvItemUsableFlag == 0) {
        MenuWidget_SetColumnLayout(node, 0x14);
    }
    if (node->cursor_x >= 0) {
        MenuWidget_SetCurrentNode(node);
    }
}
