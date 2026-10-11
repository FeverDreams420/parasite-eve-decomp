#include "pe1/boot_display.h"
#include "pe1/psyq_gpu.h"
void Boot_SetImageDisplay(u16 index) {
    s32 displayY, drawY;
    u16 savedWidth;
    DRAWENV *drawArg;
    s32 zeroArg;
    s32 yArg;
    s32 widthArg;
    if (index != 0) {
        displayY = 0;
        drawY = 224;
    } else {
        displayY = 224;
        drawY = 0;
    }
    SetDefDispEnv(&D_800BCE80[index], 0, displayY, 320, 224);
    drawArg = &D_800BCDC8[index];
    zeroArg = 0;
    yArg = drawY;
    widthArg = 480;
    savedWidth = D_800BCE80[index].disp.w;
    D_800BCE80[index].isrgb24 = 1;
    D_800BCE80[index].screen.y = 8;
    D_800BCE80[index].screen.h = 224;
    D_800BCE80[index].disp.w = savedWidth;
    SetDefDrawEnv(drawArg, zeroArg, yArg, widthArg, 224);
    D_800BCDC8[index].dtd = 0;
    D_800BCDC8[index].isbg = 0;
    D_800BCDC8[index].clip.w = D_800BCDC8[index].clip.w * 2 / 3;
}
