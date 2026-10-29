/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/save.h"
#include "pe1/save_slot_metadata.h"
#include "pe1/textbox.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
unsigned char g_MenuActiveMode;
unsigned char g_SaveOverlayTimer;
u8 g_BattleSaveOverlayActive;
u8 *g_CurItemEffectData;

void Menu_SaveOverlayDraw(void)
{
    short color;
    int y;
    int value;
    switch (g_BattleSaveOverlayActive) {
    case 1:
        color = -1;
        Draw_ResetTextboxes();
        value = Save_GetMetadataWindowIndex() ? 20 : 97;
        y = g_MenuActiveMode < 2 ? 15 : 195;
        Menu_SetTextCursorRect(value, y, 0, 0);
        Render_SetupColorTable(0, 2, &color);
        value = g_BattleSaveOverlayActive;
        g_TextboxEntries[0].state = 2;
        g_SaveOverlayTimer = 75;
        g_TextboxEntries[0].message = g_CurItemEffectData;
        value++;
        g_BattleSaveOverlayActive = value;
        g_TextboxEntries[0].control.flags |= 0x2000000;
        break;
    case 2:
        if (!g_SaveOverlayTimer) {
            Draw_ResetTextboxes();
            g_BattleSaveOverlayActive = 0;
        }
        break;
    }
    --g_SaveOverlayTimer;
    Draw_SetCursor(0, g_MenuActiveMode < 2 ? 11 : 191);
    Draw_AllocColorGradient(320, 20, 0, 0);
}