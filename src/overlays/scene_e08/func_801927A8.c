#include "scene_e08.h"
#include "scene_particle_slots.h"
#include "pe1/field_sprite_params.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_sprite_state.h"
#include "pe1/psyq_gpu.h"

#include "func_801927A8_externs.h"

void func_800C3098(int);

void func_801927A8(void *unused0, void *unused1, SceneMovingSlots *effect) {
    unsigned int i;
    SceneMovingVector *dst;
    SceneMovingWordLane *step;
    SceneMovingHalfLane *frame;
    Ovl178Position *owner;
    u8 scratchA[8];
    u8 scratchB[8];
    u8 *scratchPtr;
    u8 *table;
    int retA, retB;

    owner = func_800C2B50();
    i = 0;
    table = D_80198820;
    scratchPtr = scratchB;
    func_800C2EAC(owner->spriteMode);
    func_800C3098(0x10);
    func_800C3238(2);

    dst = effect->vector;
    asm("" : : "r"(dst));
    frame = (SceneMovingHalfLane *)dst;
    step = (SceneMovingWordLane *)frame;
    while (i < 16) {
        asm("" : "=r"(step), "=r"(frame) : "0"(step), "1"(frame));
        if (frame->active) {
            func_800C3134(table, step->life, scratchA);
            func_800C3134(table, step->life, scratchPtr);
            scratchB[0] = 0;
            scratchB[1] = 0;
            scratchB[2] = 0;
            D_800F3368.depth = 0;
            retA = GetTPage(1, 0, 0x340, 0x100) & 0xffff;
            retB = GetClut(0, 0x1D9) & 0xffff;
            func_800D3114((struct GteShortVector *)dst, 2, frame->range, 0x20, 0x90,
                           0x51, 0x10, retA, retB, frame->initialSize,
                           (struct RenderColor *)scratchA, (struct RenderColor *)scratchPtr, 1);
        }
        dst++;
        step = (SceneMovingWordLane *)((s32 *)step + 1);
        ++i;
        frame = (SceneMovingHalfLane *)((s16 *)frame + 1);
    }
}
