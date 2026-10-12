#include "scene_e08.h"
#include "pe1/random.h"

void func_80193480(void *unused0, void *unused1, Ovl178RandomizedEffect *effect) {
    unsigned i;
    s32 threshold;

    for (i = 0; i < 10; i++) {
        D_80199658[i] = Engine_Random() % 4096;
        D_80199590[i] = Engine_Random() % 4096;
        effect->tail.flags[i] = 0;
        effect->offsets[i] = 0;
    }
    threshold = *FieldEng_VarPtr(2);
    if (Engine_Random() % 256 < threshold) {
        *FieldEng_VarPtr(1) = 1;
    } else {
        *FieldEng_VarPtr(1) = 0;
    }
}
