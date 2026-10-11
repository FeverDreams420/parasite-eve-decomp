
#include "pe1/menu_widget.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_item_list_input.h"
#include "pe1/menu_item_rows.h"
#include "pe1/inventory_slots.h"

void Menu_CreateSkillActionScreen(MenuWidgetNode *arg0) {
    MenuWidgetNode *parent;
    MenuWidgetNode *child;

    parent = MenuWidget_CreateSimpleNode(8, arg0, 0, 0);
    child = MenuWidget_CreateNode(8, parent, parent);
    parent->update = (void (*)())Menu_SkillActionHandler;
    child->draw = Menu_DrawEquipModList;
    child->selectionAvailable = Menu_GetItemSlotAvailability;
    MenuWidget_SetCurrentNode(child);
    Item_PackParasiteSpells();
    Draw_SetPrimCallback(child, Inv_GetPackedListCount());
}
