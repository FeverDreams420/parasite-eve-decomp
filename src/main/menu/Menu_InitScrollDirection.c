/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/menu_state.h"

void Menu_InitScrollDirection(void) {
    g_MenuScrollStep = D_8009CDAC < 0x12C ? 4 : -4;
}
