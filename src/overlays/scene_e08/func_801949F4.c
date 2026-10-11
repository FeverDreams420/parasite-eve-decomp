#include "scene_e08.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/field_script_context.h"
#include "pe1/field_sprite_state.h"
#include "pe1/psyq_libc.h"

void func_800C2FF0(s32, s32);
void func_800C3098(s32);

extern u8 D_80198AD4;
extern FieldGlowSprite D_80199570;

void func_801949F4(s32 arg0, RoomFxControl *arg1, Ovl178MotionState *rec) {
    Ovl178Position *ctx;
    u32 idx;
    Ovl178MotionState *r;
    GteMatrix buf;
    GteVector vdst;
    GteVector vsrc;

    r = rec;
    ctx = func_800C2B50();
    idx = ((rec->frame >> 4) << 1) << 1;
    if (idx >= 0xD) {
        idx = 0xC;
    }
    func_800C2EAC(ctx->spriteMode);
    func_800C2FF0(0x40, 0x40);
    func_800C3098(0x10);
    func_800C3238(2);
    RotMatrix(&r->rotation, &buf);
    memset(&vsrc, 0, 0x10);
    vsrc.x = r->scale;
    vsrc.y = r->scale;
    vsrc.z = 0x1000;
    vdst = vsrc;
    ScaleMatrix(&buf, &vdst);
    func_800C3134(&D_80198AD4, arg1->frame, &D_80199570);
    buf.t[0] = r->x;
    buf.t[1] = r->y;
    buf.t[2] = r->z;
    D_80199570.cell = idx;
    D_80199570.depth = r->depth;
    func_800C42A4(&D_80199570, &buf, 1);
}
