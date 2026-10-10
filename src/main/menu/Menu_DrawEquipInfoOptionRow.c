#include "pe1/draw_state.h"
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_InvItemUsableFlag;

void Menu_DrawEquipInfoOptionRow(int arg0);

void Menu_DrawEquipInfoOptionRow(int arg0) {
    if (g_InvItemUsableFlag != 0) {
        Draw_AllocSprite(arg0 + 0x7C);
    } else {
        Draw_AllocSprite(arg0 + 0x7F);
    }
}
