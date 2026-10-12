#include "scene_e08.h"
typedef struct {
    char pad_00[1];
    u8 status;
    s16 target;
} Ovl178FadeInput;

void func_8018F8B0(void *arg0, Ovl178FadeInput *input, Ovl178FadeState *state) {
    unsigned int i;
    s16 value;

    for (i = 0; i < 4; i++) {
        state->glowScale[i] = (u16)state->glowScale[i] + 0x3C;
        state->glowAngle[i] = (u16)state->glowAngle[i] + 0x80;
    }

    for (i = 0; i < 2; i++) {
        if (state->ringScale[i] < 0) {
            state->ringScale[i] = 0;
        }
        state->ringScale[i] = (u16)state->ringScale[i] - 0x1E;
    }

    value = state->glowDepth;
    if (value >= 3) {
        state->glowDepth = value - 2;
    }

    if (input->target == 0x28) {
        input->status = 2;
    }
}
