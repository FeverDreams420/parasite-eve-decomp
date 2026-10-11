#include "common.h"
#include "pe1/field_anim_pool.h"
#include "pe1/battle.h"
#include "pe1/field_glow_layers.h"
#include "pe1/field_engine_slot.h"
#include "pe1/field_sprite_state.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/gte_types.h"
#include "pe1/field_anim_particle.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/psyq_libc.h"

#include "FieldEng_GlowEffectB_externs.h"



int func_800C8D34(char *object) {
    void **slot;

    slot = FieldEng_GetSlot(object);
    *slot = &D_800E0A50;

    D_800E22E8.cell = 0xBD;
    D_800E22E8.clut = 9;
    D_800E22E8.offset = 0;
    D_800E22E8.depth = 0x80;
    D_800E22E8.r = 0x80;
    D_800E22E8.g = 0x80;
    D_800E22E8.b = 0x80;
    D_800E22E8.flip = 0;

    D_800F34A8.cell = 0xAE;
    D_800F34A8.clut = 7;
    D_800F34A8.offset = -0x32;
    D_800F34A8.depth = 0x80;
    D_800F34A8.r = 0x50;
    D_800F34A8.g = 0x50;
    D_800F34A8.b = 0x50;
    D_800F34A8.flip = 0;

    D_800E2328.cell = 0x68;
    D_800E2328.clut = 0;
    D_800E2328.offset = 0;
    D_800E2328.depth = 0x80;
    D_800E2328.r = 0x50;
    D_800E2328.g = 0x50;
    D_800E2328.b = 0x50;
    D_800E2328.flip = 0;

    return 0;
}

#include "FieldEng_GlowEffectB_externs_1.h"

int func_800C8E44(int a, int b, int c, int d, int e, int f) {
    FieldEng_Spawn6(a, b, c, d, e, f);
    return 0;
}
#include "FieldEng_GlowEffectB_externs_2.h"

int func_800C8E70(void *o) {
    FieldEng_Register(o, D_800E09A0);
    return 0;
}
#include "FieldEng_GlowEffectB_externs_3.h"

int func_800C8E98(void *obj) {
    int first;
    int second;
    int status;

    first = func_800C251C(obj, D_800E09B4);
    second = func_800C2758(obj, D_800E098C, D_800E09C8);
    status = first | second;
    if (status == -1) {
        func_800C8F08(obj);
    }
    return 0;
}
int func_800C8F08(char *arg0) {
    *arg0 = 4;
    return 0;
}
int func_800C8F18(void) {
    return 0;
}
int func_800C8F20(void) {
    return 0;
}

#include "FieldEng_GlowEffectB_externs_4.h"

int func_800C8F28(FieldAnimObjectPrefix *obj) {
    FieldActor *data = obj->actor;

    D_800E2350.x = data->render_object.matrices[19].translation[0];
    D_800E2350.y = data->render_object.matrices[19].translation[1];
    D_800E2350.z = data->render_object.matrices[19].translation[2];
    D_800E27A0 = data;
    func_800CEDA8(0);
}

int rand(void);

int func_800C8F94(void *arg0, void *arg1, FieldAnimMovingParticle *anim) {
    GteShortVector vec;

    vec.x = -((rand() % 3) + 9);
    vec.y = -((rand() % 3) + 9);
    vec.z = (rand() % 5) - 2;

    ApplyMatrixSV((GteMatrix *)D_800E27A0->render_object.matrices, &vec, &anim->velocity);

    {
        u16 z;

        anim->position.x = D_800E2350.x;
        anim->position.y = D_800E2350.y;
        z = D_800E2350.z;
        anim->lifetime = 0x14;
        anim->age = 0;
        anim->position.z = z;
    }
}

#include "FieldEng_GlowEffectB_externs_5.h"

int func_800C90A4(void *arg0, void *arg1, FieldAnimMatrixGlow *anim) {
    FieldAnimMatrixGlow *anim_s0 = anim;
    GteShortVector out;
    FieldActor *data;
    char *entry;
    RenderMatrix *model;

    entry = D_800E0A10 + (((short)((u16)((Combatant *)D_8009D254->core)->action->actionCode.actionId - 1)) << 3);
    ApplyMatrixSV((GteMatrix *)&D_800E27A0->render_object.matrices[19], entry, &out);

    anim_s0->position.x = D_800E2350.x + out.x;
    anim_s0->position.y = D_800E2350.y + out.y;
    data = D_800E27A0;
    anim_s0->position.z = D_800E2350.z + out.z;

    model = data->render_object.matrices;
    anim_s0->transform = *(GteMatrixStorage *)&model[19];
    anim_s0->brightness = 0x7F;
}

#include "FieldEng_GlowEffectB_externs_6.h"

int func_800C91A8(void *arg0, void *arg1, FieldAnimGlowPoint *anim) {
    GteShortVector in;
    GteShortVector out;

    in = D_800C217C;
    ApplyMatrixSV((GteMatrix *)&D_800E27A0->render_object.matrices[19], &in, &out);

    {
        u16 y;
        u16 z;

        anim->position.x = D_800E2350.x + out.x;
        y = D_800E2350.y + out.y;
        anim->position.y = y;
        z = D_800E2350.z + out.z;
        anim->brightness = 0x7F;
        anim->scale = 0;
        anim->position.z = z;
    }
}

void func_800C9260(void) {
}

#include "FieldEng_GlowEffectB_externs_7.h"

int func_800C9268(void *arg0, void *arg1, FieldAnimMatrixGlow *anim) {
    GteMatrix matrix;
    GteShortVector rot;
    GteVector scaleCopy;
    GteVector scale;
    s16 index;

    index = (u16)((Combatant *)D_8009D254->core)->action->actionCode.actionId - 1;

    rot.x = 0;
    rot.y = 0;
    rot.z = (s8)anim->age << 6;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x10, 0x10);
    func_800C3238(0);

    RotMatrix(&rot, &matrix);

    matrix.t[0] = (s16)anim->position.x;
    matrix.t[1] = (s16)anim->position.y;
    matrix.t[2] = (s16)anim->position.z;

    memset(&scale, 0, sizeof(scale));
    scale.x = D_800E09F0[index];
    scale.y = D_800E09F0[index];
    scale.z = D_800E09F0[index];
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    func_800C42A4(&D_800E22E8, &matrix, 1);
}

#define GLOW_LAYERS_FUNC func_800C9398
#define GLOW_LAYERS_SPIN_A D_800C2184
#define GLOW_LAYERS_SPIN_B D_800C218C
#define GLOW_LAYERS_SCALE D_800C2194
#define GLOW_LAYERS_SPRITE D_800F34A8
#include "FieldEng_GlowLayers.inc"

int func_800C9868(void *arg0, void *arg1, FieldAnimGlowPoint *anim) {
    u16 *field_s3 = &D_800E2328.depth;
    GteMatrix matrix;
    int scale;
    u16 field;
    int localScale[4];

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    scale = anim->scale + 0x170;
    field = anim->brightness;

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

    *field_s3 = field;
    matrix.t[0] = (s16)anim->position.x;
    matrix.t[1] = (s16)anim->position.y;
    matrix.t[2] = (s16)anim->position.z;

    memset(localScale, 0, sizeof(localScale));
    localScale[0] = scale;
    localScale[1] = scale;
    localScale[2] = scale;

    ScaleMatrix(&matrix, (const GteVector *)localScale);
    func_800C42A4((FieldGlowSprite *)((u8 *)field_s3 - 10), &matrix, 1);
}

int func_800C9968(void *arg0, u8 *state) {
    int ret = 2;

    state[1] = ret;
    return ret;
}
int func_800C9974(void *arg0, FieldEngSlot *state, FieldAnimMovingParticle *anim) {
    FieldEngSlot *state_a3 = state;
    FieldAnimMovingParticle *anim_a2 = anim;
    int temp_v0;
    int temp_v1;
    register int temp_a0 asm("$4");
    register int temp_a1 asm("$5");
    u8 count;

    temp_v0 = anim_a2->position.x;
    temp_v1 = (u16)anim_a2->velocity.x;
    temp_a0 = (u16)anim_a2->velocity.y;
    temp_a1 = (u16)anim_a2->velocity.z;

    temp_v0 += temp_v1;
    anim_a2->position.x = temp_v0;
    temp_v0 = anim_a2->position.y;
    temp_v1 = anim_a2->position.z;
    temp_v0 += temp_a0;
    temp_v1 += temp_a1;
    anim_a2->position.y = temp_v0;
    temp_v0 = (u16)anim_a2->velocity.y;
    temp_a0 = (unsigned int)anim_a2;
    anim_a2->position.z = temp_v1;
    temp_v1 = anim_a2->age;
    temp_v0 += 3;
    anim_a2->velocity.y = temp_v0;
    temp_v0 = (s16)anim_a2->position.y;
    temp_v1++;
    anim_a2->age = temp_v1;
    if (temp_v0 > 0) {
        anim_a2->velocity.y = -(u16)anim_a2->velocity.y;
    }
    count = ((FieldAnimMovingParticle *)temp_a0)->lifetime;
    ((FieldAnimMovingParticle *)temp_a0)->lifetime = count - 1;
    if (count == 0) {
        state_a3->flag = 2;
    }
}
int func_800C9A00(void *arg0, FieldEngSlot *state, FieldAnimGlowPoint *anim) {
    s16 value = anim->brightness - 0x14;

    anim->brightness = value;
    if (value < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}
int func_800C9A34(void *arg0, FieldEngSlot *state, FieldAnimGlowPoint *anim) {
    anim->brightness = anim->brightness - 8;
    anim->scale = (u16)anim->scale + 0x28;
    if ((s16)anim->brightness < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}
