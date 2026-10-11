#include "Draw_BaseOffset_externs.h"

int Draw_SetBaseY(int arg0, unsigned int arg1);

void Draw_SetBaseOffsetPosition(int x, int y) {
    Draw_SetBaseY(D_800B0DB0[0] + x, g_DrawBaseY + y);
}

#include "Draw_BaseOffset_externs_1.h"

int Draw_GetBaseY(void) {
    return g_DrawBaseY;
}
