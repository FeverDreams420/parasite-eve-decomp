#ifndef MENUCARD_SET_VIDEO_DISPLAY_NAME
#define MENUCARD_SET_VIDEO_DISPLAY_NAME MenuCard_SetVideoDisplay
#define MENUCARD_VIDEO_DISPLAY_MODE D_801D0DBE
#endif

void MENUCARD_SET_VIDEO_DISPLAY_NAME(s8 index, s8 wide) {
    s16 y[2];

    if (index == 0) {
        y[0] = 240;
        y[1] = 0;
    } else {
        y[0] = 0;
        y[1] = 240;
    }
    if (wide) {
        MENUCARD_VIDEO_DISPLAY_MODE = 3;
        SetDefDispEnv(&D_800BCE80[index], 0, y[0], 480, 240);
        D_800BCE80[index].isrgb24 = 1;
        D_800BCE80[index].disp.w = D_800BCE80[index].disp.w * 2 / 3;
        SetDefDrawEnv(&D_800BCDC8[index], 0, y[1], 480, 240);
        D_800BCDC8[index].isbg = 1;
        D_800BCDC8[index].dtd = 1;
        D_800BCDC8[index].dfe = 0;
        D_800BCDC8[index].r0 = 0;
        D_800BCDC8[index].g0 = 0;
        D_800BCDC8[index].b0 = 0;
        D_800BCDC8[index].clip.w = D_800BCDC8[index].clip.w * 2 / 3;
    } else {
        MENUCARD_VIDEO_DISPLAY_MODE = 2;
        SetDefDispEnv(&D_800BCE80[index], 0, y[0], 320, 240);
        SetDefDrawEnv(&D_800BCDC8[index], 0, y[1], 320, 240);
        D_800BCDC8[index].isbg = 1;
        D_800BCDC8[index].dtd = 1;
        D_800BCDC8[index].dfe = 0;
        D_800BCDC8[index].r0 = 0;
        D_800BCDC8[index].g0 = 0;
        D_800BCDC8[index].b0 = 0;
    }
}

#undef MENUCARD_VIDEO_DISPLAY_MODE
#undef MENUCARD_SET_VIDEO_DISPLAY_NAME
