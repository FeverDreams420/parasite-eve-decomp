#include "common.h"
#include "pe1/scene_e08_ring_effect.h"
#include "pe1/field_ring_geometry.h"

#include "func_801923D8_externs.h"
void func_800C4E50(void *);

void func_801923D8(void *unused0, void *unused1, SceneE08RingEffectState *effect) {
    u8 *mid = (u8 *)&D_80199604;
    u8 *source;
    FieldRingGeometry *ring;
    FieldRingGeometry *record = (FieldRingGeometry *)((u8 *)&D_80199604 - PE1_OFFSETOF(FieldRingGeometry, mode));
    int intensity = 0x80;
    register int span asm("$19");
    asm("" : "=r"(mid) : "0"(mid));

    ring = (FieldRingGeometry *)(mid - PE1_OFFSETOF(FieldRingGeometry, mode));
    asm ("" : : "r"(ring));
    effect->rotation[0] = 0;
    effect->rotation[1] = 0;
    effect->rotation[2] = 0;
    effect->secondScale = 0;

    ring->mode = 0x20;
    record->offset = 0x1F4;
    asm volatile("" : : "r"(intensity));
    record->color1[0] = 0xF0;
    record->color1[1] = 0xC8;
    span = 0x960;
    record->intensity = intensity;
    record->color1[2] = 0x78;
    asm volatile("" : "=r"(source) : "0"(D_80198C90));
    record->color0[0] = 0;
    record->color0[1] = 0;
    record->color0[2] = 0;
    record->extent0 = span;
    record->extent1 = 0;
    ring->source = source;
    func_800C4E50(ring);

    ++ring;

    ring->mode = 0x20;
    ring->offset = 0x5DC;
    ring->intensity = intensity;
    ring->color1[0] = 0;
    ring->color1[1] = 0;
    ring->color1[2] = 0;
    ring->color0[0] = 0xFF;
    ring->color0[1] = 0xFF;
    ring->color0[2] = 0xFF;
    ring->extent0 = span;
    ring->extent1 = 0;
    ring->source = source + 0x200;
    func_800C4E50(ring);

    record = ring + 1;
    asm ("" : : "r"(record));
    record->mode = 0x20;
    record->offset = 0x5DC;
    record->intensity = intensity;
    record->color1[0] = 0;
    record->color1[1] = 0;
    record->color1[2] = 0;
    record->color0[0] = 0xFF;
    record->color0[1] = 0xFF;
    record->color0[2] = 0xFF;
    record->extent0 = span;
    record->extent1 = 0;
    source += 0x400;
    record->source = source;
    func_800C4E50(record);
}
