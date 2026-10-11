/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/menu_widget.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_dialog.h"

#define NULL ((void *)0)

s32 Menu_CreateSaveSlotListView(s32 arg0, s32 arg1) {
    s32 temp_s0;
    s32 temp_s0_2;
    void *temp_a0;
    MenuWidgetNode *temp_s2;
    MenuWidgetNode *temp_v0;

    temp_s0_2 = arg0 + 0x25;
    temp_v0 = MenuWidget_CreateSimpleNode(temp_s0_2, MenuWidget_FindByModeAndSelectedBase(2, 0x24), 0, 0);
    temp_s2 = MenuWidget_CreateNode(temp_s0_2, temp_v0, temp_v0);
    temp_a0 = temp_s2;
    temp_v0->update = (void (*)())Menu_StepSaveSelect;
    temp_s2->draw = (void (*)())Menu_DrawSaveSlotList;
    temp_s2->selectionAvailable = Menu_IsMemCardSlotCompatible;
    Draw_SetPrimCallback(temp_a0, arg1);
    temp_s0 = MenuWidget_GridCellIndex(temp_s2);
    if (MenuWidget_FindLastMode1WithCursorX() == 0) {
        temp_s2->cursor_x = 0;
        MenuWidget_SetCurrentNode(temp_s2);
    } else {
        temp_s2->cursor_x = -1;
    }
    MenuWidget_CreateSimpleNode(0x3F, temp_s2, 0, 0)->draw = (void (*)())Menu_DrawSaveSlotPortLabel;
    Queue_Init();
    g_SaveSelectedSlot = arg0;
    return temp_s0;
}

void Menu_CloseSaveSlotListView(s32 arg0) {
    MenuWidgetNode *temp_s0;

    temp_s0 = MenuWidget_FindByModeAndSelectedBase(2, 0x24);
    MenuWidget_NavScrollTo(0x28);
    if (g_MenuDeferredCallback != NULL) {
        g_MenuDeferredCallback();
        g_MenuDeferredCallback = NULL;
    }
    if (arg0 == g_SaveSelectedSlot) {
        MenuWidget_NavScrollTo(0x3F);
        MenuWidget_NavScrollTo(0x27);
        MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x29));
        MenuWidget_NavScrollTo(0x1F);
        MenuWidget_NavScrollTo(arg0 + 0x25);
        if (temp_s0 != NULL) {
            temp_s0->cursor_x = 0;
            if (MenuWidget_FindLastMode1WithCursorX() == 0) {
                MenuWidget_SetCurrentNode(temp_s0);
            }
        }
    }
}
