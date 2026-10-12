#include "fx_common_vectors.h"
#include "pe1/gte_types.h"
#include "pe1/asset_tim.h"
#include "fx_common.h"
#include "fx_common_setup.h"

#include "func_80195D3C_externs.h"

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
