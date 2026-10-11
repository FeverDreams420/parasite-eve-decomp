/* CC1_FLAGS: -fno-schedule-insns */
#include "common.h"
#include "pe1/akao/commands.h"
#include "pe1/field_anim.h"
#include "pe1/game_audio_state.h"
#include "pe1/field_engine_state.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_sprite_state.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/gte_types.h"
#include "pe1/field_anim_particle.h"
#include "pe1/field_engine_slot.h"
#include "pe1/field_billboard.h"

int func_800CD404(void *arg0, void *arg1, FieldAnimBurstData *anim) {
    FieldAnimBurstData *anim_s2 = anim;
    int *field_s1;
    u8 *cell;
    int *position_v0;
    register FieldGlowSprite *call_a0 asm("$4");
    register int *call_a1 asm("$5");
    register int call_a2 asm("$6");
    int value_v0;
    register int value_v1 asm("$3");
    int entry_v0;
    int i;
    int shade;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    cell = &D_800E2250.cell;
        call_a0 = (FieldGlowSprite *)(cell - PE1_OFFSETOF(FieldGlowSprite, cell));
        position_v0 = &D_800F33C0.t[0];
    value_v1 = anim_s2->header.origin_x;
    call_a1 = position_v0 - 5;
    position_v0[0] = value_v1;
    value_v0 = anim_s2->header.origin_y;
    call_a2 = 1;
    D_800F33C0.t[1] = value_v0;
    value_v0 = anim_s2->header.origin_z;
    i = 0;
    D_800F33C0.t[2] = value_v0;
    shade = anim_s2->header.shade;
    asm volatile("" ::: "$17");
    field_s1 = &D_800F32B0.t[0];
    *cell = (shade * 2) - 0x80;
    func_800C42A4(call_a0, (GteMatrix *)call_a1, call_a2);

    func_800C3238(3);

    do {
        call_a0 = &D_800F3460;
                entry_v0 = (int)&anim_s2->bytes[(i & 0xFFFF) * 8];
        value_v1 = (s16)((FieldAnimBurstWindow *)entry_v0)->point.x;
        call_a1 = field_s1 - 5;
        field_s1[0] = value_v1;
        value_v1 = (s16)((FieldAnimBurstWindow *)entry_v0)->point.y;
        call_a2 = 1;
        field_s1[1] = value_v1;
        value_v0 = (s16)((FieldAnimBurstWindow *)entry_v0)->point.z;
        asm volatile("" : "=r"(value_v0) : "0"(value_v0));
        field_s1[2] = value_v0;
        i++;
        func_800C42A4(call_a0, (GteMatrix *)call_a1, call_a2);
    } while ((i & 0xFFFFU) < 2);
}
