#include "pe1/menu_state.h"
#include "common.h"
#include "pe1/inventory.h"
#include "pe1/menu_widget.h"
#include "pe1/inventory_queries.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_equipment.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
void Menu_CreateContextHelpPanel(void);
#include "Menu_OpenEquipSlotSwap_externs.h"

s32 Menu_OpenEquipSlotSwap(s32 arg0) {
    s32 temp_s2;
    s32 temp_v1;
    MenuWidgetNode *temp_a0;
    MenuWidgetNode *temp_s1;
    MenuWidgetNode *temp_v0;

    temp_s2 = MenuWidget_FindByModeAndSelectedBase(1, 1) != NULL;
    if (temp_s2 == 0) {
        Queue_Init();
        MenuWidget_CreateSimpleNode(0x1B, 0, 0, 0)->draw = (void (*)())Menu_DrawAmmoTypeHeader;
        temp_v0 = MenuWidget_CreateSimpleNode(1, 0, 0, 0);
        temp_s1 = MenuWidget_CreateNode(1, temp_v0, temp_v0);
        temp_a0 = temp_s1;
        temp_v0->update = (void (*)())Menu_InventoryInputHandler;
        temp_s1->draw = (void (*)())Menu_DrawSelectableEquipSlotList;
        MenuWidget_SetCurrentNode(temp_a0);
        temp_s1->itemAction = (MenuWidgetItemAction)Inv_SwapSlots;
        temp_s1->refreshItems = (void (*)(void))Menu_RebuildSelectableMask;
        Inv_RebuildSelectableMask();
        g_InvSwapTargetIndex = -1;
        g_InvSelectedItemIndex = -1;
        Draw_SetPrimCallback(temp_s1, Inv_GetAyaSlotLimit());
        g_MenuActionSubmenuOpen = 0;
        if (g_MenuEquipSwapSource != 0) {
            temp_v1 = g_MenuEquipSwapSource - 1;
            temp_s1->cursor_x = (temp_v1 & 1);
            temp_s1->cursor_y = ((temp_v1 >> 1) & 0x7F);
            temp_s1->scroll_y = (temp_v1 >> 8);
        }
        Menu_CreateContextHelpPanel();
        MenuWidget_FindByModeAndSelectedBase(2, 1)->itemAction = NULL;
        Inv_SortSlotsByPriority();
        g_MenuActionSubmenuOpen = 1;
        D_8009CF5C = arg0;
        g_MenuSelectionLocked = 0;
        Menu_SetBattleEquipMode(0);
    }
    return temp_s2;
}
