/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "common.h"
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/inventory_sort.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_widget.h"
#include "pe1/inventory_queries.h"
#include "pe1/menu_sound.h"
#include "pe1/text.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_draw_list.h"

#define NULL ((void *)0)

void Menu_DrawEquipSortToggleList(int node);
int Menu_EquipGridHandler(void *node, int flags);
void Menu_CreateEquipInfoPanel(int parent, unsigned int option);
int Menu_EquipSelectInput(void *node, unsigned int flags);
void Menu_DrawEquipInfoOptionList(int node);
void Menu_DrawWeaponModSlots(s32 index);

int g_MenuInventoryViewMode;
int g_InvItemUsableFlag;

void Menu_OpenInventoryOrSwapView(s32 showList) {
    s32 mode;
    s32 source;
    MenuWidgetNode *current;
    MenuWidgetNode *panel;
    MenuWidgetNode *list;

    MenuWidget_NavScrollTo(0x3C);
    MenuWidget_NavScrollTo(0x3B);
    MenuWidget_NavScrollTo(0x3A);
    mode = g_MenuInventoryViewMode;
    switch (mode) {
    case 0:
        if (showList != 0) {
            current = MenuWidget_GetCurrentNode();
            current->cursor_y = 0;
            MenuWidget_CreateSimpleNode(0x1B, NULL, 0, 0)->draw = (void (*)())&Menu_DrawAmmoTypeHeader;
            panel = MenuWidget_CreateSimpleNode(1, current, 0, 0);
            list = MenuWidget_CreateNode(1, panel, panel);
            panel->update = (void (*)())Menu_InventoryInputHandler;
            list->draw = Menu_DrawSelectableEquipSlotList;
            MenuWidget_SetCurrentNode(list);
            list->itemAction = (MenuWidgetItemAction)Inv_SwapSlots;
            list->refreshItems = Menu_RebuildSelectableMask;
            Inv_RebuildSelectableMask();
            g_InvSwapTargetIndex = -1;
            g_InvSelectedItemIndex = -1;
            Draw_SetPrimCallback(list, Inv_GetAyaSlotLimit());
            source = mode = g_MenuEquipSwapSource;
            g_MenuActionSubmenuOpen = 0;
            if (source != 0) {
                source -= 1;
                list->cursor_x = source & 1;
                list->cursor_y = (source >> 1) & 0x7F;
                list->scroll_y = source >> 8;
            }
        } else {
            Menu_CreateBonusPointAllocationView();
        }
        break;
    case 1:
        MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x33));
        break;
    case 2:
        MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x33));
        break;
    }
}

void Menu_DrawWeaponModSlots(s32 arg0) {
    s32 temp_s0;
    u8 var_a0;

    temp_s0 = arg0 + D_800922B8[g_MenuInventoryViewMode];
    if (g_MenuInventoryViewMode == 1) {
        Draw_OffsetCursor(0, 2);
        Draw_AllocSprite(D_800922BC[temp_s0]);
        Draw_OffsetCursor(0x12, 2);
        Draw_AllocSprite(0x22U);
        Draw_OffsetCursor(0xC, -2);
        Draw_AllocSprite(D_800922C4[temp_s0]);
        if (g_MenuInventoryViewMode == 0) {
            Draw_OffsetCursor(0x12, 2);
            Draw_AllocSprite(0x22U);
            Draw_OffsetCursor(0xC, -2);
            var_a0 = D_800922CC[temp_s0];
            Draw_AllocSprite(var_a0);
        }
    } else {
        Draw_AllocSprite(D_800922BC[temp_s0]);
        Draw_AllocSprite(0x68U);
        Draw_OffsetCursor(0x12, 4);
        Draw_AllocSprite(0x22U);
        Draw_OffsetCursor(0xC, -4);
        Draw_AllocSprite(D_800922C4[temp_s0]);
        Draw_AllocSprite(0x68U);
        if (g_MenuInventoryViewMode == 0) {
            Draw_OffsetCursor(0x12, 4);
            Draw_AllocSprite(0x22U);
            Draw_OffsetCursor(0xC, -4);
            Draw_AllocSprite(D_800922CC[temp_s0]);
            var_a0 = 0x68;
            Draw_AllocSprite(var_a0);
        }
    }
}

void Menu_DrawEquipOptionsList(int arg0) {
    MenuWidget_DrawList((MenuWidgetNode *)arg0, Menu_DrawWeaponModSlots);
}

s32 Menu_EquipOptionsInputHandler(void *arg0, s32 arg1) {
    s32 temp_a1;
    MenuWidgetNode *temp_v0;
    MenuWidgetNode *temp_v0_2;
    MenuWidgetNode *temp_v0_3;

    if (arg1 & 0x10000) {
        temp_v0 = MenuWidget_GetChild(arg0, 0);
        temp_a1 = MenuWidget_GridCellIndex(temp_v0);
        switch (g_MenuInventoryViewMode) {                       /* irregular */
        case 0:
            if (temp_a1 == 2) {
                temp_v0_2 = MenuWidget_CreateSimpleNode(0x3B, temp_v0, 0, 0);
                temp_v0_3 = MenuWidget_CreateNode(0x3B, temp_v0_2, temp_v0_2);
                temp_v0_2->update = (void (*)())&Menu_EquipGridHandler;
                MenuWidget_OffsetPosition(temp_v0_2, 0x70 - temp_v0_2->x, 0x48 - temp_v0_2->y);
                temp_v0_3->draw = (void (*)())&Menu_DrawEquipSortToggleList;
                MenuWidget_SetCurrentNode(temp_v0_3);
            } else {
            case 2:
                Menu_CreateEquipInfoPanel(temp_v0, temp_a1);
            }
            break;
        case 1:
            Sort_ContainerItems(temp_a1);
            Menu_OpenInventoryOrSwapView(1);
            break;
        }
        Menu_PlayConfirmSound();
        return 1;
    }
    if (arg1 & 0x40) {
        Menu_OpenInventoryOrSwapView(0);
        Menu_PlayCancelSound();
    }
    return 1;
}

void Menu_DrawEquipSortToggleList(int arg0) {
    MenuWidget_DrawList((MenuWidgetNode *)arg0, Menu_DrawEquipSortToggleRow);
}

int Menu_EquipGridHandler(void *arg0, int arg1) {
    int temp;

    if (arg1 & 0x10000) {
        temp = MenuWidget_GridCellIndex(MenuWidget_GetChild(arg0, 0));
        if (g_MenuInventoryViewMode != 0) {
            Sort_ContainerItems(temp);
        } else {
            Inv_SortInventoryByMode(2, temp);
        }
        Menu_OpenInventoryOrSwapView(1);
        Menu_PlayConfirmSound();
    } else if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_PlayCancelSound();
    }

    return 1;
}

void Menu_CreateEquipInfoPanel(int arg0, unsigned int arg1) {
    MenuWidgetNode *node;
    MenuWidgetNode *child;

    node = MenuWidget_CreateSimpleNode(0x3C, arg0, 0, 0);
    child = MenuWidget_CreateNode(0x3C, node, node);
    node->update = Menu_EquipSelectInput;
    child->draw = Menu_DrawEquipInfoOptionList;
    MenuWidget_SetCurrentNode(child);

    if (g_MenuInventoryViewMode == 0) {
        MenuWidget_OffsetPosition(
            node,
            0x70 - node->x,
            0x20 - node->y
        );
    }

    g_InvItemUsableFlag = arg1 < 1;
    if (arg1 != 0) {
        MenuWidget_OffsetPosition(node, 0, 0x14);
    }
}

void Menu_DrawEquipInfoOptionList(int arg0) {
    MenuWidget_DrawList((MenuWidgetNode *)arg0, Menu_DrawEquipInfoOptionRow);
}

int Menu_EquipSelectInput(void *arg0, unsigned int arg1) {
    int temp;
    void (*handler)(int, int);

    if (arg1 & 0x10000) {
        temp = MenuWidget_GridCellIndex(MenuWidget_GetChild(arg0, 0));
        handler = g_MenuInventoryViewMode ? Sort_InventoryItems : Inv_SortInventoryByMode;
        handler(g_InvItemUsableFlag, temp);
        Menu_OpenInventoryOrSwapView(1);
        Menu_PlayConfirmSound();
    } else if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_PlayCancelSound();
    }

    return 1;
}
