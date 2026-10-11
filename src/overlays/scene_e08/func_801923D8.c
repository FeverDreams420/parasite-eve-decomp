#include "common.h"
#include "pe1/scene_e08_ring_effect.h"
#include "pe1/field_ring_geometry.h"

#include "func_801923D8_externs.h"
void func_800C4E50(void *);

void func_801923D8(void *unused0, void *unused1, SceneE08RingEffectState *effect) {
    u8 *mid = (u8 *)&D_80199604;
    u8 *source;
    u8 *base;
    u8 *third;
    int intensity = 0x80;
    register int span asm("$19");
    asm("" : "=r"(mid) : "0"(mid));

    base = mid - 12;
    asm ("" : : "r"(base));
    effect->rotation[0] = 0;
    effect->rotation[1] = 0;
    effect->rotation[2] = 0;
    effect->secondScale = 0;

    ((FieldRingGeometry *)base)->mode = 0x20;
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
    ((FieldRingGeometry *)base)->source = source;
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

    third = mid + 36;
    asm ("" : : "r"(third));
    ((FieldRingGeometry *)(mid + 36))->mode = 0x20;
    ((FieldRingGeometry *)(mid + 36))->offset = 0x5DC;
    ((FieldRingGeometry *)(mid + 36))->intensity = intensity;
    ((FieldRingGeometry *)(mid + 36))->color1[0] = 0;
    ((FieldRingGeometry *)(mid + 36))->color1[1] = 0;
    ((FieldRingGeometry *)(mid + 36))->color1[2] = 0;
    ((FieldRingGeometry *)(mid + 36))->color0[0] = 0xFF;
    ((FieldRingGeometry *)(mid + 36))->color0[1] = 0xFF;
    ((FieldRingGeometry *)(mid + 36))->color0[2] = 0xFF;
    ((FieldRingGeometry *)(mid + 36))->extent0 = span;
    ((FieldRingGeometry *)(mid + 36))->extent1 = 0;
    source += 0x400;
    ((FieldRingGeometry *)third)->source = source;
    func_800C4E50(third);
}
