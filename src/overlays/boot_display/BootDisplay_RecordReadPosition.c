#include "common.h"

#include "BootDisplay_RecordReadPosition_externs.h"

void BootDisplay_RecordReadPosition(u8 mode, int value) {
    if (mode == 2) {
        D_80172CFC = func_80080C48(value + 5);
        if (D_80172CFC != 0) {
            D_80172CF0 = D_80172CFC;
        }
    }
}
