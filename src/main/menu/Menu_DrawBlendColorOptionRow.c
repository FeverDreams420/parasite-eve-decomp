#include "pe1/text.h"
void MenuWidget_DrawCenteredTableText(int arg0);

void Menu_DrawBlendColorOptionRow(int arg0) {
    Draw_OffsetCursor(0, 1);
    MenuWidget_DrawCenteredTableText(arg0 + 0x33);
}
