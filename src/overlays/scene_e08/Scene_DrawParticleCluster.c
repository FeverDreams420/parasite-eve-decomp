#include "pe1/gte_types.h"
#include "scene_particle_slots.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/field_script_context.h"
#include "pe1/field_sprite_state.h"
#include "pe1/psyq_libc.h"

typedef GteMatrix Matrix;
typedef GteVector Vec4;

typedef struct SceneParticleOwner {
    u8 reserved[0x24];
    u8 drawMode;
} SceneParticleOwner;

extern s16 D_801994B2;
void func_800C2FF0(int, int);
void func_800C3098(int);

void func_8018FBA4(void *unused0, void *unused1, SceneParticleCluster *argSlots) {
    SceneParticleCluster *slots = argSlots;
    Matrix matrix;
    Vec4 scratch, draw;
    SceneParticleOwner *owner;
    register unsigned i;
    s16 *slot = &D_801994B2;

    owner = func_800C2B50();
    i = 0;
    /* Hoist the draw state address before the cursor setup. */
    asm("" : : "r"(slot));
    func_800C2EAC(owner->drawMode);
    func_800C3098(0x10);
    func_800C3238(2);

    for (; i < 30; i++) {
        if (slots->active[i] == 1) {
            matrix.m[2][2] = 0x1000;
            matrix.m[1][1] = 0x1000;
            matrix.m[0][0] = 0x1000;
            matrix.t[2] = 0;
            matrix.t[1] = 0;
            matrix.t[0] = 0;
            matrix.m[2][1] = 0;
            matrix.m[2][0] = 0;
            matrix.m[1][2] = 0;
            matrix.m[1][0] = 0;
            matrix.m[0][2] = 0;
            matrix.m[0][1] = 0;

            memset(&draw, 0, 0x10);
            draw.x = slots->fieldF0[i];
            draw.y = slots->fieldF0[i];
            draw.z = slots->fieldF0[i];
            scratch = draw;
            ScaleMatrix(&matrix, &scratch);

            matrix.t[0] = slots->position[i].x;
            matrix.t[1] = slots->position[i].y;
            matrix.t[2] = slots->position[i].z;
            *slot = slots->brightness[i];
            if (slots->texture[i] == 0x44) {
                func_800C2FF0(0x20, 0x20);
                ((FieldGlowSprite *)((char *)slot - 10))->cell = slots->texture[i];
            }
            if (slots->texture[i] == 0x4C) {
                func_800C2FF0(0x10, 0x10);
                {
                    u8 color = slots->texture[i];
                    u8 counter = slots->counter[i];
                    color += 3;
                    ((FieldGlowSprite *)((char *)slot - 10))->cell = color - (counter >> 1);
                }
            }
            if (slots->texture[i] == 0x58) {
                func_800C2FF0(0x10, 0x10);
                {
                    u8 color = slots->texture[i];
                    u8 counter = slots->counter[i];
                    color += 7;
                    ((FieldGlowSprite *)((char *)slot - 10))->cell = color - (counter >> 1);
                }
            }
            ((FieldGlowSprite *)((char *)slot - 10))->clut = slots->mode[i];
            func_800C42A4((FieldGlowSprite *)((char *)slot - 10), &matrix, 1);
        }
    }
}
