#include "common.h"
#include "pe1/scene_e08_ring_effect.h"
#include "pe1/field_ring_geometry.h"

#include "func_801923D8_externs.h"
void func_800C4E50(void *);

void func_801923D8(void *unused0, void *unused1, SceneE08RingEffectState *effect) {
    u8 *mid = (u8 *)&D_80199604;
    u8 *source;
    FieldRingGeometry *base;
    FieldRingGeometry *third;
    int intensity = 0x80;
    register int span asm("$19");
    asm("" : "=r"(mid) : "0"(mid));

    base = (FieldRingGeometry *)(mid - PE1_OFFSETOF(FieldRingGeometry, mode));
    asm ("" : : "r"(base));
    effect->rotation[0] = 0;
    effect->rotation[1] = 0;
    effect->rotation[2] = 0;
    effect->secondScale = 0;

    base->mode = 0x20;
    D_8019960A = 0x1F4;
    asm volatile("" : : "r"(intensity));
    D_80199600 = 0xF0;
    D_80199601 = 0xC8;
    span = 0x960;
    D_8019960C = intensity;
    D_80199602 = 0x78;
    asm volatile("" : "=r"(source) : "0"(D_80198C90));
    D_801995FC = 0;
    D_801995FD = 0;
    D_801995FE = 0;
    D_80199606 = span;
    D_80199608 = 0;
    base->source = source;
    func_800C4E50(base);

    ((FieldRingGeometry *)(mid + 12))->mode = 0x20;
    ((FieldRingGeometry *)(mid + 12))->offset = 0x5DC;
    ((FieldRingGeometry *)(mid + 12))->intensity = intensity;
    ((FieldRingGeometry *)(mid + 12))->color1[0] = 0;
    ((FieldRingGeometry *)(mid + 12))->color1[1] = 0;
    ((FieldRingGeometry *)(mid + 12))->color1[2] = 0;
    ((FieldRingGeometry *)(mid + 12))->color0[0] = 0xFF;
    ((FieldRingGeometry *)(mid + 12))->color0[1] = 0xFF;
    ((FieldRingGeometry *)(mid + 12))->color0[2] = 0xFF;
    ((FieldRingGeometry *)(mid + 12))->extent0 = span;
    ((FieldRingGeometry *)(mid + 12))->extent1 = 0;
    ((FieldRingGeometry *)(mid + 12))->source = source + 0x200;
    func_800C4E50(mid + 12);

    third = (FieldRingGeometry *)(mid + 36);
    asm ("" : : "r"(third));
    third->mode = 0x20;
    third->offset = 0x5DC;
    third->intensity = intensity;
    third->color1[0] = 0;
    third->color1[1] = 0;
    third->color1[2] = 0;
    third->color0[0] = 0xFF;
    third->color0[1] = 0xFF;
    third->color0[2] = 0xFF;
    third->extent0 = span;
    third->extent1 = 0;
    source += 0x400;
    third->source = source;
    func_800C4E50(third);
}
