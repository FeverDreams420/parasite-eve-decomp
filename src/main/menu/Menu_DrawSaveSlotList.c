/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/draw_state.h"
#include "pe1/draw_wipe_rect.h"
#include "pe1/menu_draw_list.h"
#include "pe1/menu_item_rows.h"

MenuWidgetNode *g_MenuActiveWidget;

void Menu_DrawSaveSlotList(int arg0) {
    g_MenuActiveWidget = (MenuWidgetNode *)arg0;
    MenuWidget_DrawList((MenuWidgetNode *)arg0, Menu_DrawSaveSlotEntry);
    Draw_BlendColor(g_SavedDrawBlendColor[0]);
    Menu_SetSaveSlotBlendBase(0);
}
