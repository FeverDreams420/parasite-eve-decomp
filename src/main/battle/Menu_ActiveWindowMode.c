/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "common.h"
#include "pe1/menu_state.h"

/* Active menu window mode and its on-screen bounds.
 * g_MenuActiveMode is private state of this pair. */

u8 g_MenuActiveMode;
/* Match debt: volatile keeps the bound loads between the stores, in
 * retail order; plain or multi-set forms swap v0/v1 (lev 5). */
volatile u16 D_8009CE84;
u16 g_MenuActiveWindowYOffset;

void Window_SetBoundsByMode(int mode) {
    SaveWindowModeBounds bounds;
    volatile u16 *entry;

    bounds = g_MenuWindowModeBounds;
    entry = &bounds.values[(u8)mode * 2];
    D_8009CE84 = entry[0];
    g_MenuActiveMode = mode;
    g_MenuActiveWindowYOffset = entry[1];
}

unsigned int Menu_GetActiveMode(void) {
    return g_MenuActiveMode;
}
