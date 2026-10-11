/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "pe1/menu_flags.h"

unsigned char g_BattleActionQueueCount;
unsigned char g_BattleActionIndex;
unsigned char g_BattleRemainingActions;
signed char g_BattleActiveTurnSlot;
signed char g_BattleItemMenuState;

void Save_ResetGlobalFlags(void) {
    g_BattleActionQueueCount = 0;
    g_BattleActionIndex = 0;
    g_BattleRemainingActions = 0;
    g_BattleActiveTurnSlot = 0;
    g_BattleItemMenuState = 0;
}
