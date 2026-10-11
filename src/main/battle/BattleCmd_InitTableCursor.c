/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/battle_cmd.h"

void BattleCmd_InitTableCursor(void) {
    g_BattleCmdStackTop = g_BattleCmdStack;
}
