#include "pe1/draw_state.h"
#include "pe1/menu_draw_list.h"
/* MASPSX_FLAGS: -G8 --use-comm-section */

MenuWidgetNode *g_MenuActiveWidget;
#include "Menu_DrawSaveSlotList_externs.h"

void Menu_DrawSaveSlotEntry(void);
void Draw_BlendColor(int arg0);

void Menu_DrawSaveSlotList(int arg0) {
    g_MenuActiveWidget = (MenuWidgetNode *)arg0;
    MenuWidget_DrawList((MenuWidgetNode *)arg0, (void (*)(int))Menu_DrawSaveSlotEntry);
    Draw_BlendColor(g_SavedDrawBlendColor[0]);
    Menu_SetSaveSlotBlendBase(0);
}
