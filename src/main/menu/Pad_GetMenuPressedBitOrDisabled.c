/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/menu_flags.h"
#include "pe1/battle.h"

#include "Pad_GetMenuPressedBitOrDisabled_externs.h"
unsigned char g_BattleActionQueueCount;

signed char Pad_GetMenuPressedBitOrDisabled(void) {
    if (g_ActiveActor->stateFlags & 0x10000) {
        return -1;
    }
    return (signed char)g_BattleActionQueueCount;
}
