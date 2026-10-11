/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/menu_flags.h"

extern int *g_ActiveActor[];
unsigned char g_BattleActionQueueCount;

signed char Pad_GetMenuPressedBitOrDisabled(void) {
    if (g_ActiveActor[0][0x13] & 0x10000) {
        return -1;
    }
    return (signed char)g_BattleActionQueueCount;
}
