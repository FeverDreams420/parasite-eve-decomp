/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "Menu_InitScrollDirection_externs.h"

void Menu_InitScrollDirection(void) {
    D_8009D0D4 = D_8009CDAC < 0x12C ? 4 : -4;
}
