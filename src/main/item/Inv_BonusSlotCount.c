/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/stat_modifiers.h"

#include "Inv_BonusSlotCount_externs.h"

int *Battle_GetModifierTable(void) {
    return g_StatScaleBase;
}

int Inv_GetBonusSlotCount(void) {
    return g_InvBonusSlotCount;
}
