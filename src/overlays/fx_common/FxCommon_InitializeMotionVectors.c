#include "fx_common.h"

void func_801938E8(s16 base)
{
    FxCommonMotionRecord sample;
    GteShortVector extra;
    unsigned int i;

    D_8019C058 = 0;
    for (i = 0; i < 10; i++) {
        func_8018F55C(i << 8, base + 0x40,
                      Gpu_GetTimTableEntry(&D_801D0260, 2), &sample.vector, &extra);
        D_8019CAA8[i].vector.x = sample.vector.x << 16;
        D_8019CAA8[i].vector.y = sample.vector.y << 16;
        D_8019CAA8[i].vector.z = sample.vector.z << 16;
    }
}
