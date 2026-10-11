#ifndef PE1_MENU_MEMCARD_DISPLAY_H
#define PE1_MENU_MEMCARD_DISPLAY_H

#include "common.h"
#include "pe1/psyq_gpu.h"

/* Double-buffered display and draw environments used by the video player. */
extern DISPENV D_800BCE80[2];
extern DRAWENV D_800BCDC8[2];

/* Video display mode of each copied player (2 = 320 wide, 3 = 480 wide). */
extern u8 D_801D0DBE;
extern u8 D_801223F6;


#endif
