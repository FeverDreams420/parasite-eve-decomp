#include "scene_e08.h"
#include "pe1/gte_types.h"
#include "pe1/scene_e08_glow_sprites.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/field_script_context.h"
#include "pe1/field_sprite_state.h"
#include "pe1/psyq_libc.h"

typedef GteMatrix Matrix;
typedef GteVector Vec4;

#include "func_80190460_externs.h"

void func_800C2FF0(int, int);
void func_800C3098(int);

void func_80190460(void *unused0, void *unused1, char *effect) {
    Matrix matrix;
    Vec4 scratch, draw;
    FieldGlowSprite *sprite;
    Ovl178Position *owner;
    SceneE08GlowSprites *base = (SceneE08GlowSprites *)effect;
    register unsigned i;
    int diag = 0x1000;
    s16 *slot = &D_801994C2;
    owner = func_800C2B50();
    i = 0;
        asm volatile("" : : "r"(slot));
    func_800C2EAC(owner->spriteMode);
    func_800C3098(0x10);
    func_800C3238(2);

    for (; i < 4; i++) {
        if (base->active[i] == 1) {
            matrix.m[2][2] = diag;
            matrix.m[1][1] = diag;
            matrix.m[0][0] = diag;
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
            draw.x = base->scale[i];
            draw.y = base->scale[i];
            draw.z = base->scale[i];
            scratch = draw;
            ScaleMatrix(&matrix, &scratch);

            matrix.t[0] = base->position[i].x;
            matrix.t[1] = base->position[i].y;
            matrix.t[2] = base->position[i].z;
            *slot = base->depth[i];
            func_800C2FF0(0x10, 0x10);
            sprite = (FieldGlowSprite *)((char *)slot - 10);
            sprite->cell = base->cell[i] + (base->frame[i] >> 1);
            sprite->clut = base->clut[i];
            func_800C42A4(sprite, &matrix, 1);
        }
    }
}
