/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "common.h"
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/menu_draw_list.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_record.h"
#include "pe1/menu_widget.h"
#include "pe1/text.h"

/* List renderers for the item, selectable equip-slot and context-action
 * lists, the action-option and equip-slot adapters, and the usable-item and
 * skill action-code lists. */

MenuWidgetNode *g_MenuActiveWidget;
int g_MenuBattleStatusOverlayActive;
int g_MenuActiveItemList;
int g_InvItemUsableFlag;

void Menu_DrawItemSlot(int index);
void Menu_DrawSelectableEquipSlotRow(int index);
void Sfx_ContextPick(int index);
void Menu_DrawActionOptionRow(int index);
void Menu_SoundTestSelect(int index);
void Menu_DrawActionCodeItem(int index);
void Draw_SetTextDimmed(int value);
int Battle_GetStateFlag1(void);
void Battle_DrawActiveStatus(void);

void Menu_DrawItemList(MenuWidgetNode *node) {
    int count;

    g_MenuActiveWidget = node;
    MenuWidget_DrawList(node, Menu_DrawItemSlot);
    Draw_SetTextDimmed(1);

    count = node->visible_rows;
    while (count != 0) {
        Draw_AllocSprite(0x68);
        Draw_OffsetCursor(0, 0x10);
        count--;
    }

    if (Battle_GetStateFlag1() == 0) {
        if (g_MenuBattleStatusOverlayActive != 0) {
            Battle_DrawActiveStatus();
        }
    }
}

void Menu_DrawSelectableEquipSlotList(MenuWidgetNode *node) {
    Inv_SelectActiveList(0);
    g_MenuActiveWidget = node;
    MenuWidget_DrawList(node, Menu_DrawSelectableEquipSlotRow);
}

void Menu_DrawContextActionList(MenuWidgetNode *node) {
    MenuWidgetNode *saved = node;

    Inv_SelectActiveList(g_MenuActiveItemList);
    MenuWidget_DrawList(saved, Sfx_ContextPick);
}

void Menu_DrawActionOptionList(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_DrawActionOptionRow);
}

void Menu_DrawSoundTestList(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_SoundTestSelect);
}

void Menu_DrawUsableItemActionList(MenuWidgetNode *node) {
    ItemDataRecord *ptr;

    ptr = Inv_LookupActiveListData(Inv_RestoreSelection(0));
    g_MenuActionItemData = ptr;
    g_InvItemUsableFlag = ptr->kind != 9;
    Draw_SetPrimCallback(node, ptr->tailCount);
    g_MenuActiveWidget = node;
    MenuWidget_DrawList(node, Menu_DrawActionCodeItem);
}

void Menu_SetupSkillSubmenu(MenuWidgetNode *list) {
    MenuWidgetNode *node;
    MenuWidgetNode *other;
    int index;
    ItemDataRecord *ptr;

    node = MenuWidget_FindByModeAndSelectedBase(1, 7);
    other = MenuWidget_FindByModeAndSelectedBase(2, 0x36);
    Inv_SelectActiveList(MenuWidget_GridCellIndex(other) == 1);

    if (node != 0 && node->cursor_y == 0) {
        index = MenuWidget_GridCellIndex(MenuWidget_GetChild(node, 0));
        if (index >= 0) {
            other = MenuWidget_FindByModeAndSelectedBase(2, 7);
            index = Inv_GetPackedListItem(MenuWidget_GridCellIndex(other));
            Inv_RememberSelection(1, index);
        }
    }

    ptr = Inv_LookupActiveListData(Inv_RestoreSelection(1));
    g_MenuActionItemData = ptr;
    Draw_SetPrimCallback(list, ptr->tailCount);
    g_MenuActiveWidget = list;
    MenuWidget_DrawList(list, Menu_DrawActionCodeItem);
}

void Menu_DrawUsableItemActionList2(MenuWidgetNode *node) {
    ItemDataRecord *ptr;

    ptr = Inv_LookupActiveListData(Inv_RestoreSelection(1));
    g_MenuActionItemData = ptr;
    g_MenuActiveWidget = node;
    MenuWidget_DrawList(node, Menu_DrawActionCodeItem);
}
