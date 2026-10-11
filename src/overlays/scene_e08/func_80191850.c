#include "scene_e08.h"
#include "pe1/gte_types.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/field_script_context.h"
#include "pe1/field_sprite_state.h"
#include "pe1/psyq_libc.h"

typedef GteMatrix Overlay178Matrix;
typedef GteVector Overlay178Vec4;

#include "func_80191850_externs.h"

void func_800C2FF0(s32 arg0, s32 arg1);
void func_800C3098(s32 arg0);

void func_80191850(void *arg0, void *arg1, SceneE08CentreGlow *glow) {
    Overlay178Matrix matrix;
    Overlay178Vec4 scratch_scale;
    Overlay178Vec4 draw_scale;
    Ovl178Position *owner;

    owner = func_800C2B50();
    func_800C2EAC(owner->spriteMode);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

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

    memset(&draw_scale, 0, 0x10);
    draw_scale.x = glow->scale;
    draw_scale.y = glow->scale;
    draw_scale.z = 0x1000;
    scratch_scale = draw_scale;
    ScaleMatrix(&matrix, &scratch_scale);

    matrix.t[0] = glow->x + D_8019956C;
    matrix.t[1] = glow->y;
    matrix.t[2] = glow->z + D_8019957C;
    func_800C42A4((FieldGlowSprite *)D_801996A0, &matrix, 1);
}
