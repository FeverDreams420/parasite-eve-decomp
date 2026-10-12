#include "menu_memcard_glint.h"

/* Origin callbacks of the glint effect descriptors; every effect is
 * anchored at the shared glint origin. */
GteShortVector *MenuCard_GetGlintOriginA(void) {
    return &D_801F1F28;
}

GteShortVector *MenuCard_GetGlintOriginB(void) {
    return &D_801F1F28;
}

GteShortVector *MenuCard_GetGlintOriginC(void) {
    return &D_801F1F28;
}

GteShortVector *MenuCard_GetGlintOriginD(void) {
    return &D_801F1F28;
}

GteShortVector *MenuCard_GetGlintOriginE(void) {
    return &D_801F1F28;
}

GteShortVector *MenuCard_GetGlintOriginF(void) {
    return &D_801F1F28;
}

/* Update hook of the cross flash: keeps it alive while the flash raises the
 * flag each frame and clears the flag for the next frame. */
int MenuCard_ConsumeCrossFlashFlag(int mode) {
    if (mode == 1) {
        if (D_801F1F3A == 0) return 1;
        D_801F1F3A = 0;
    }
    return 0;
}
