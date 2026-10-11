#include "scene_particle_slots.h"
#include "pe1/scene_player_orb.h"

typedef struct SceneE08RandomEntries {
    u8 pad00[12];
    u16 counter[10];
    u8 pad20[4];
    u8 active[10];
} SceneE08RandomEntries;

typedef SceneE08EffectState SceneE08RandomState;

#include "func_801938FC_externs.h"

void func_801938FC(int unused, SceneE08RandomState *state, SceneE08RandomEntries *entries) {
    unsigned int i;
    for (i = 0; i < 10; ++i) {
        if (entries->active[i] == 1) {
            ++entries->counter[i];
            D_801994D8[i].rise = func_80071A54() % 160 - 80;
            D_801994D8[i].bend = func_80071A54() % 160 - 80;
            D_801994D8[i].reach = func_80071A54() % 160 - 80;
            D_801994D8[i].swing = func_80071A54() % 512 - 256;
        } else if (func_80071A54() % 20 == 0) {
            entries->active[i] = 1;
        }
    }
    if (state->elapsedFrames >= 61) {
        state->status = 2;
    }
}
