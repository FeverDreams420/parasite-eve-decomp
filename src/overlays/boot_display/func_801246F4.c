#include "pe1/boot_display.h"

void func_801246F4(void) {
    func_80074DC0(0);
    func_80073A44(2);
    func_80074A44(1);
    PutDispEnv(&D_800BCE80[g_ActiveDrawSlot]);
    func_800754E4(g_BootDisplayOrderingTable + 0x3C,
                  &D_800BCDC8[g_ActiveDrawSlot]);
    g_ActiveDrawSlot ^= 1;
}
