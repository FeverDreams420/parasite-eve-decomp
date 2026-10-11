#include "pe1/draw_state.h"
#include "pe1/draw_buffers.h"

int Draw_SetBaseY(int arg0, unsigned int arg1);

void Draw_SetBaseOffsetPosition(int x, int y) {
    Draw_SetBaseY(D_800B0DB0[0] + x, g_DrawBaseY + y);
}


int Draw_GetBaseY(void) {
    return g_DrawBaseY;
}
