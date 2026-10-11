#include "common.h"
#include "pe1/inventory_category.h"
#include "pe1/save.h"
#include "pe1/game_timers.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

void Draw_BlendColor(int color);
int Menu_ClampRange(int value);
int Draw_GetBaseY(void);
void Draw_SetBaseOffsetPosition(int x, int y);
void Menu_SetActionSubmenuSelection(int value);
void MenuWidget_SetColumnLayoutMode(int mode);
void Menu_CopyPromptCodes(u8 *src);

void Save_RestoreHeader(void) {
    u16 *src;
    int i;
    int category;
    u8 *prompt;

    i = 0;
    src = g_SaveRuntimeState.categoryCounts;
    category = 0;
    while (i < 6) {
        g_InvCategoryItemTable[category].count = *src++;
        i++;
        category++;
    }

    Draw_BlendColor(g_SaveRuntimeState.drawBlendColor);
    Menu_ClampRange(g_SaveRuntimeState.menuClampValue);

    GAME_TIME_COUNTER(2) = (g_SaveRuntimeState.windows.header.playTimeMinutes * 15) * 4;
    GAME_TIME_COUNTER(0) = (g_SaveRuntimeState.windows.header.gameTimeMinutes * 15) * 4;
    Draw_SetBaseOffsetPosition(0, g_SaveRuntimeState.windows.header.drawBaseY - Draw_GetBaseY());
    Menu_SetActionSubmenuSelection(g_SaveRuntimeState.windows.header.menuLayout & 3);
    MenuWidget_SetColumnLayoutMode((g_SaveRuntimeState.windows.header.menuLayout >> 2) & 1);

    prompt = g_SaveRuntimeState.windows.metadata[0].text;
    if (g_SaveMetadataWindowIndex != 0) {
        prompt = g_SaveRuntimeState.windows.metadata[1].text;
    }
    Menu_CopyPromptCodes(prompt);
}
