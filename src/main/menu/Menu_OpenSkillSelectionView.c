#include "pe1/menu_widget.h"
#include "common.h"
#include "pe1/menu_draw_list.h"
#include "pe1/menu_inventory.h"

#define NULL ((void *)0)
void Menu_DrawSkillSelectionList(int arg0);

void Menu_OpenSkillSelectionView(void) {
    MenuWidgetNode *temp_a0;
    MenuWidgetNode *temp_v0;

    temp_v0 = MenuWidget_CreateSimpleNode(0x40, MenuWidget_GetCurrentNode(), 0, 0);
    temp_a0 = MenuWidget_CreateNode(0x40, temp_v0, temp_v0);
    temp_v0->update = (void (*)())Menu_HandleAyaInventorySelectionInput;
    temp_a0->draw = Menu_DrawSkillSelectionList;
    MenuWidget_SetCurrentNode(temp_a0);
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0));
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 1));
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x1B));
}

void Menu_DrawSkillItem(int arg0);

void Menu_DrawSkillSelectionList(int arg0) {
    MenuWidget_DrawList((MenuWidgetNode *)arg0, Menu_DrawSkillItem);
}
