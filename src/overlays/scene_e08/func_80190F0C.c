#include "pe1/scene_ring_sprites.h"

#include "func_80190F0C_externs.h"
void func_800C4E50(void *item);

void func_80190F0C(void *unused0, void *unused1, SceneRingSpritesState *effect) {
    u8 *mid = D_801995D4;
    u8 *source;
    int width;
    FieldRingGeometry *base;
    int first;
    asm("" : "=r"(mid) : "0"(mid));
    base = (FieldRingGeometry *)(mid - 12);
    width = 128;
    first = 300;
    asm volatile("" : : "r"(width));
    effect->size = first;
    effect->reserved0D = 156;
    effect->ticks = 0;
    effect->depth = width;

    base[0].mode = 8;
    D_801995DA = 0;
    D_801995DC = width;
    D_801995D0 = 120;
    D_801995D1 = 100;
    D_801995D2 = 30;
    D_801995CC = 0;
    D_801995CD = 0;
    D_801995CE = 0;
    D_801995D6 = 1800;
    D_801995D8 = 1200;
    source = D_80198B90;
    base[0].source = source;
    func_800C4E50(base);

    base[1].mode = 8;
    base[1].offset = 0;
    base[1].intensity = width;
    base[1].color1[0] = 20;
    base[1].color1[1] = 20;
    base[1].color1[2] = 0;
    base[1].color0[0] = 120;
    base[1].color0[1] = 100;
    base[1].color0[2] = 30;
    base[1].extent0 = 1200;
    base[1].extent1 = 800;
    base[1].source = source + 128;
    func_800C4E50(&base[1]);
}
