/* CC1_FLAGS: -fno-schedule-insns */

#include "common.h"
#include "pe1/field_vector_math.h"

int FieldFx_TurnToward(int current, int target, int step) {
    int target_copy;
    register int diff asm("$5");
    int abs_diff;
    int step_s16;
    register int delta asm("$3");
    int result;
    current &= 0xFFF;
    result = target & 0xFFF;
    diff = result;
    target_copy = diff;
    diff = (s16)(diff - current);

    result = diff;
    if (diff < 0) {
        result = -result;
    }
    abs_diff = (s16)result;

    step_s16 = (s16)step;
    delta = step;
    if (step_s16 >= abs_diff) {
        result = target_copy;
    } else {
        if (diff < 0) {
            delta = -step;
        }
        if (abs_diff >= 0x801) {
            delta = -delta;
        }
        result = delta + current;
    }

    return result & 0xFFF;
}


s32 FieldFx_AngleDistance(s32 lhs, s32 rhs) {
    register s32 delta asm("$4");
    s32 abs_delta;

    delta = lhs - rhs;
    abs_delta = delta;
    if ((delta << 16) < 0) {
        delta = -delta;
        abs_delta = delta;
    }

    if ((short)abs_delta >= 0x801) {
        abs_delta = 0x1000 - abs_delta;
    }

    return (short)abs_delta;
}
