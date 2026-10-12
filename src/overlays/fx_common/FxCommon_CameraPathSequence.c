#include "fx_common_vectors.h"
#include "pe1/gte_types.h"
#include "pe1/asset_tim.h"
#include "fx_common.h"
#include "fx_common_setup.h"


void func_80195D3C(void)
{
    FxCommonVec3 point;
    GteShortVector scratch;
    void *lookup;

    if (D_8019C040 > 1) {
        D_8019C040--;
        D_8019C041++;

        lookup = Gpu_GetTimTableEntry(&D_801D0260, 2);
        func_8018F55C(D_8019C041 << 8, D_8019C042 + 1,
                       lookup, &point, &scratch);
        D_8019C810 = point.x;
        D_8019C814 = point.y;
        D_8019C818 = point.z;

        lookup = Gpu_GetTimTableEntry(&D_801D0260, 2);
        func_8018F55C(D_8019C041 << 8, D_8019C042,
                       lookup, &point, &scratch);
        D_8019C330 = point.x;
        D_8019C334 = point.y;
        D_8019C338 = point.z;
    }
}

void func_80195E4C(int index, int unused1, int unused2, int source)
{
    FxCommonVec3 point;
    GteShortVector scratch;
    short savedIndex = index;
    void *lookup;
    int result;
    int shortIndex;

    lookup = Gpu_GetTimTableEntry(&D_801D0260, 2);
    shortIndex = savedIndex;
    func_8018F55C(source, shortIndex + 1, lookup, &point, &scratch);
    D_8019C054 = 0;
    D_8019C810 = point.x;
    D_8019C814 = point.y;
    D_8019C818 = point.z;

    lookup = Gpu_GetTimTableEntry(&D_801D0260, 2);
    result = func_8018F55C(source, shortIndex, lookup, &point, &scratch);
    D_8019C040 = result - 2;
    D_8019C050 = 0;
    D_8019C041 = 0;
    D_8019C042 = savedIndex;
    D_8019C02C = 0;
    D_8019C330 = point.x;
    D_8019C334 = point.y;
    D_8019C338 = point.z;
}
