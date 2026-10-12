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
    FieldAnimBurstWindow *window;
    u16 i;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    D_800F33C0.t[0] = anim->header.origin_x;
    D_800F33C0.t[1] = anim->header.origin_y;
    D_800F33C0.t[2] = anim->header.origin_z;
    D_800E2250.cell = (anim->header.shade * 2) - 0x80;
    func_800C42A4(&D_800E2250, &D_800F33C0, 1);

    func_800C3238(3);

    for (i = 0; i < 2; i++) {
        window = (FieldAnimBurstWindow *)&anim->bytes[i * 8];
        D_800F32B0.t[0] = (s16)window->point.x;
        D_800F32B0.t[1] = (s16)window->point.y;
        D_800F32B0.t[2] = (s16)window->point.z;
        func_800C42A4(&D_800F3460, &D_800F32B0, 1);
    }
}
