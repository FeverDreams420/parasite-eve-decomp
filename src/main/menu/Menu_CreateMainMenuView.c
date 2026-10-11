
#include "pe1/menu_widget.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_sound.h"
#include "pe1/inventory_slots.h"

int Menu_MainMenuInputHandler(MenuWidgetNode *arg0, u32 arg1);
void Menu_OpenCursorModeMenu(MenuWidgetNode *parent);
int Menu_ActionSubmenuInputHandler(MenuWidgetNode *arg0, u32 arg1);
int Menu_CursorModeInputHandler(MenuWidgetNode *arg0, u32 arg1);

void Menu_CreateMainMenuView(MenuWidgetNode *arg0) {
    MenuWidgetNode *parent;
    MenuWidgetNode *child;

    parent = MenuWidget_CreateSimpleNode(0x20, arg0, 0, 0);
    child = MenuWidget_CreateNode(0x20, parent, parent);
    parent->update = Menu_MainMenuInputHandler;
    child->draw = Menu_DrawMainMenuList;
    MenuWidget_SetCurrentNode(child);
    Draw_SetPrimCallback(child, 4);
}


int Menu_MainMenuInputHandler(MenuWidgetNode *arg0, u32 arg1) {
    MenuWidgetNode *child = MenuWidget_GetChild(arg0, 0);

    if (arg1 & 0x10000) {
        switch (MenuWidget_GridCellIndex(child)) {
        case 0:
            Menu_CreateActionSubmenu(child);
            break;
        case 1:
            Menu_OpenCursorModeMenu(child);
            break;
        case 2:
            Menu_OpenBlendColorScreen(child);
            break;
        case 3:
            Menu_OpenScreenAdjustView(child);
            break;
        case 4:
        case 5:
            Inv_InitMaxLevelInventory(MenuWidget_GridCellIndex(child) - 4);
            MenuWidget_DestroyNode(arg0);
            Menu_CreateBonusPointAllocationView();
            Menu_PlayConfirmSound();
            break;
        }
        Menu_PlayConfirmSound();
        return 1;
    }

    if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_CreateBonusPointAllocationView();
        Menu_PlayCancelSound();
    }
    return 1;
}


void Menu_CreateActionSubmenu(MenuWidgetNode *arg0) {
    MenuWidgetNode *created;
    MenuWidgetNode *node;

    created = MenuWidget_CreateSimpleNode(0x21, arg0, 0, 0);
    node = MenuWidget_CreateNode(0x21, created, created);
    created->update = Menu_ActionSubmenuInputHandler;
    node->draw = Menu_DrawActionSubmenuList;
    MenuWidget_SetCurrentNode(node);
}

int Menu_ActionSubmenuInputHandler(MenuWidgetNode *arg0, u32 arg1) {
    MenuWidgetNode *base;

    base = MenuWidget_GetChild(arg0, 0);
    if (arg1 & 0x10000) {
        Menu_PlayConfirmSound();
        Menu_SetActionSubmenuSelection(MenuWidget_GridCellIndex(base));
        MenuWidget_DestroyNode(arg0);
        return 1;
    }
    if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_PlayCancelSound();
    }
    return 1;
}

void Menu_OpenCursorModeMenu(MenuWidgetNode *arg0) {
    MenuWidgetNode *created;
    MenuWidgetNode *node;

    created = MenuWidget_CreateSimpleNode(0x23, arg0, 0, 0);
    node = MenuWidget_CreateNode(0x23, created, created);
    created->update = Menu_CursorModeInputHandler;
    node->draw = Menu_DrawCursorModeList;
    MenuWidget_SetCurrentNode(node);
}

int Menu_CursorModeInputHandler(MenuWidgetNode *arg0, u32 arg1) {
    MenuWidgetNode *base;

    base = MenuWidget_GetChild(arg0, 0);
    if (arg1 & 0x10000) {
        Menu_PlayConfirmSound();
        MenuWidget_SetColumnLayoutMode(MenuWidget_GridCellIndex(base));
        MenuWidget_DestroyNode(arg0);
        return 1;
    }
    if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_PlayCancelSound();
    }
    return 1;
}
