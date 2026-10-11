#include "common.h"
#include "pe1/room_liberty_choice.h"
extern u8 D_8019A8C8[];

extern s32 func_80052B2C(void);

s32 func_80193114(s32 arg0, RoomLibertyChoiceInputs *inputs) {
    s32 variant;
    s32 distanceBand;
    s32 threshold;
    s32 i;
    s32 sum;
    u8 *weights;

    variant = (*inputs->variantBits >> 10) & 3;
    distanceBand = *inputs->distance;

    if (distanceBand < 0x280) {
        distanceBand = 0;
    } else if (distanceBand < 0x600) {
        distanceBand = 1;
    } else {
        distanceBand = 2;
    }

    weights = D_8019A8C8 + (arg0 * 84) + (distanceBand * 28) + (variant * 7);
    threshold = ((func_80052B2C() & 0xFF) * 100) / 0x100;

    i = 0;
    sum = 0;
    while (i < 6) {
        sum += weights[i];
        if (threshold < sum) {
            return i;
        }
        i++;
    }

    return i;
}
