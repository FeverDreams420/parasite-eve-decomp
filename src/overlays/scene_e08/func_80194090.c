#include "scene_e08.h"
#include "pe1/field_flash_burst.h"
#include "pe1/gte_types.h"
#include "pe1/room_floor.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/field_script_context.h"
#include "pe1/field_sprite_state.h"
#include "pe1/psyq_libc.h"

#include "func_80194090_externs.h"

void func_800C2FF0(int, int);
void func_800C3098(int);
void func_80194090(void *arg0, void *arg1, char *effects) {
    GteMatrix matrix;
    GteVector scratch, vec;
    Ovl178Position *owner;
    s32 third;
    register char *base asm("$20") = effects;
    register SceneE08PositionSample *state asm("$17");
    register SceneE08TrailLane *phase asm("$16");
    s16 *alpha;
    unsigned i;
    unsigned angleOffset;
    owner = func_800C2B50();
    i = 0;
    alpha = &D_8019955A;
    /* This single-pass block preserves the original setup scheduling. */
    do {
        state = (SceneE08PositionSample *)base;
        phase = (SceneE08TrailLane *)base;
        angleOffset = 0x30;
        func_800C2EAC(owner->spriteMode);
        func_800C2FF0(0x40, 0x20);
        func_800C3098(0x10);
        func_800C3238(2);
    } while (0);

    for (; i < 3; ++i, angleOffset += 8) {
        RotMatrix((GteShortVector *)(base + angleOffset), &matrix);
        memset(&vec, 0, 0x10);
        vec.x = phase->scale.x;
        vec.y = phase->scale.y;
        vec.z = phase->scale.z;
        scratch = vec;
        ScaleMatrix(&matrix, &scratch);
        matrix.t[0] = state->x;
        matrix.t[1] = state->y;
        third = state->z;
        *alpha = 0x80 - (i << 5);
        matrix.t[2] = third;
        if (i == 0) {
            *alpha = 0xC0;
        }
        func_800C42A4((FieldGlowSprite *)((char *)alpha - 10), &matrix, 0);
        state++;
        phase = (SceneE08TrailLane *)(&phase->step + 1);
    }
    matrix.t[0] = ((SceneE08TrailGlow *)base)->trail[0].x;
    matrix.t[1] = g_RoomFloorY->y;
    matrix.t[2] = ((SceneE08TrailGlow *)base)->trail[0].z;
    func_800C42A4(&D_80199580, &matrix, 0);
}
