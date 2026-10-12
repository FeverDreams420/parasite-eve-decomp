/* CC1_FLAGS: -fno-schedule-insns */
#include "pe1/aya.h"

void Aya_UnlockParasiteSpell(int spell) {
    u32 *flags = &D_800C0E00.parasite_spell_flags;
    u32 mask = 1;
    u32 value = *flags;

    mask <<= spell;
    value |= mask;
    *flags = value;
}
