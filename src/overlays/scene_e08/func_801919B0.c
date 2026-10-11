#include "scene_e08.h"
#include "scene_particle_slots.h"
#include "pe1/field_sprite_params.h"
#include "pe1/field_script_context.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_sprite_state.h"
#include "pe1/psyq_gpu.h"

#include "func_801919B0_externs.h"

void func_800C3098(s32 arg0);

void func_801919B0(s32 arg0, s32 arg1, SceneMovingSlots *base) {
    Ovl178Position *st;
    u32 i;
    u8 buf38[8];
    u8 buf40[8];

    st = func_800C2B50();
    func_800C2EAC(st->spriteMode);
    func_800C3098(0x10);
    func_800C3238(2);

    for (i = 0; i < 0x10; i++) {
        s16 type = base->active[i];
        if (type == 1) {
            s32 v64;
            s32 vAA;
            func_800C3134(D_8019880C, base->life[i], buf38);
            func_800C3134(D_8019880C, base->life[i], buf40);
            buf40[0] = 0;
            buf40[1] = 0;
            buf40[2] = 0;
            D_800F3368.depth = 0x1F4;
            v64 = GetTPage(0, 0, 0x340, 0x100) & 0xFFFF;
            vAA = GetClut(0, 0x1D7) & 0xFFFF;
            func_800D3114((struct GteShortVector *)&base->vector[i], 2, base->range[i], 0x40,
                          0x80, 0x9C, 0x10, v64, vAA, base->initialSize[i],
                          (struct RenderColor *)buf38, (struct RenderColor *)buf40, type);
        }
    }
}
