#include "pe1/menu_inventory.h"
#include "common.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_widget.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --dont-expand-li */
/* ASPSX_VERSION: 2.21 */




void Menu_CreateInvSwapView(MenuWidgetNode *parent, int mode) {
    int mode_offset;
    MenuWidgetNode *node;
    MenuWidgetNode *current;

    mode_offset = mode;
    node = MenuWidget_CreateSimpleNode(mode_offset + 0x3A, parent, 0, 0);
    current = MenuWidget_CreateNode(mode_offset + 0x3A, node, node);
    node->update = (void (*)())Menu_EquipOptionsInputHandler;
    current->draw = Menu_DrawEquipOptionsList;
    MenuWidget_SetCurrentNode(current);
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x33));

    if (mode_offset != 0) {
        u32 value;

        value = (unsigned int)D_8009CF0C;
        g_MenuInventoryViewMode = value;
    } else {
        g_MenuInventoryViewMode = 0;
    }
}
