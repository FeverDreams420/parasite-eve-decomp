#include "common.h"
#include "menu_memcard_video.h"

extern s8 D_800B0DBB;

void func_801223EC(s8 enabled) {
    if ((enabled && !D_800B0DBB) || (!enabled && D_800B0DBB)) {
        D_801223F8 = 1;
    }
}
