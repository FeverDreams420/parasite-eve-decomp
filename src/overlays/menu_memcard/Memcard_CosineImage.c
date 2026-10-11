#include "menu_memcard_pulse.h"

s32 Memcard_CosineImage(s32 mode, MemcardPulseState *state) {
    GteRotation rotation;
    s32 angle, scale, palette;
    register s32 intensity;
    register s32 cosine asm("$3");
    register s32 index;
    register s32 four;
    s16 timer;
    s32 twist;
    switch (mode) {
    case 1:
        if (state->phase != 0) return 0;
        timer = state->timer + 1;
        state->timer = timer;
        if (timer < 12) return 0;
        __asm__ volatile("" : : : "memory");
        return 1;
    case 2:
        if (state->phase != 0) return 0;
        angle = (state->timer << 10) / 12;
        scale = rcos(angle) + 2048;
        cosine = rcos(angle);
        __asm__("" : : "r"(cosine));
        intensity = cosine / 32;
        __asm__("" : : "r"(intensity));
        rotation.x = 0; rotation.y = 0;
        twist = state->timer;
        index = D_800F3368.palette;
        __asm__ volatile("" ::: "memory");
        four = 4;
        __asm__("" : : "r"(four));
        rotation.flags = 0;
        rotation.z = twist << 5;
        palette = D_800E1204[index];
        if (index == four && D_800F3428 != 0) palette += 7;
        else palette += 3;
        func_800CEE20(&state->position, &rotation, scale, scale, 66,
                     GetClut(0, palette), 1, intensity, &D_801F1F30);
        break;
    }
    return 0;
}
