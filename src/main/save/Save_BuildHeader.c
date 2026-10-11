#include "common.h"
#include "pe1/inventory_category.h"
#include "pe1/game_timers.h"
#include "pe1/save.h"
#include "pe1/draw_wipe_rect.h"

int Draw_GetBlendColor(void);
int Menu_GetItemContextFlag(void);
int Draw_GetBaseY(void);
unsigned int Menu_GetActionSubmenuSelection(void);
int MenuWidget_GetColumnLayoutMode(void);
int Save_ClassifyPlaytime(int arg0);
int Save_GetCurrentMapNumber(void);

void Save_BuildHeader(void) {
    u16 *dst;
    int i;
    int category;
    unsigned int value;

    i = 0;
    dst = g_SaveRuntimeState.categoryCounts;
    category = 0;
    while (i < 6) {
        *dst = g_InvCategoryItemTable[category].count;
        category++;
        i++;
        dst++;
    }

    g_SaveRuntimeState.drawBlendColor = Draw_GetBlendColor();
    g_SaveRuntimeState.menuClampValue = Menu_GetItemContextFlag();
    g_SaveRuntimeState.windows.header.playTimeMinutes =
        (unsigned int)GAME_TIME_COUNTER(2) / 60U;
    g_SaveRuntimeState.windows.header.gameTimeMinutes =
        (unsigned int)GAME_TIME_COUNTER(0) / 60U;
    g_SaveRuntimeState.windows.header.drawBaseY = Draw_GetBaseY();
    value = Menu_GetActionSubmenuSelection();
    g_SaveRuntimeState.windows.header.menuLayout =
        value | (MenuWidget_GetColumnLayoutMode() << 2);
    g_SaveRuntimeState.windows.header.reserved1E = 0;
    g_SaveRuntimeState.playTimeClass = Save_ClassifyPlaytime(g_PlayTimeFrameCounter);
    g_SaveRuntimeState.currentMapNumber = Save_GetCurrentMapNumber();
}
