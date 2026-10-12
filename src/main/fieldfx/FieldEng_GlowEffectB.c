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
#include "pe1/field_effect_modules.h"

int GlowEffectB_PlantScript(char *object) {
    void **slot;

    slot = FieldEng_GetSlot(object);
    *slot = &g_GlowEffectBScript;

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

int GlowEffectB_Spawn6(int a, int b, int c, int d, int e, int f) {
    FieldEng_Spawn6(a, b, c, d, e, f);
    return 0;
}
int GlowEffectB_RegisterDrawList(void *o) {
    FieldEng_Register(o, g_GlowEffectBDrawList);
    return 0;
}
int GlowEffectB_RegisterPairedTables(void *obj) {
    int first;
    int second;
    int status;

    first = FieldEng_UpdateSlots(obj, g_GlowEffectBUpdateList);
    second = FieldEng_RunScript(obj, g_GlowEffectBInitList, g_GlowEffectBSlotSizes);
    status = first | second;
    if (status == -1) {
        GlowEffectB_CloseTarget(obj);
    }
    return 0;
}
int GlowEffectB_CloseTarget(char *arg0) {
    *arg0 = 4;
    return 0;
}
int GlowEffectB_ClassNop6(void) {
    return 0;
}
int GlowEffectB_ClassNop0(void) {
    return 0;
}

int func_800C8F28(FieldAnimObjectPrefix *obj) {
    FieldActor *data = obj->actor;

    D_800E2350.x = data->render_object.matrices[19].translation[0];
    D_800E2350.y = data->render_object.matrices[19].translation[1];
    D_800E2350.z = data->render_object.matrices[19].translation[2];
    g_GlowEffectBOwner = data;
    func_800CEDA8(0);
}

int func_800C8F94(void *arg0, void *arg1, FieldAnimMovingParticle *anim) {
    GteShortVector vec;

    vec.x = -((rand() % 3) + 9);
    vec.y = -((rand() % 3) + 9);
    vec.z = (rand() % 5) - 2;

    ApplyMatrixSV((GteMatrix *)g_GlowEffectBOwner->render_object.matrices, &vec, &anim->velocity);

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

int func_800C90A4(void *arg0, void *arg1, FieldAnimMatrixGlow *anim) {
    FieldAnimMatrixGlow *anim_s0 = anim;
    GteShortVector out;
    FieldActor *data;
    GteShortVector *entry;
    RenderMatrix *model;

    entry = &g_GlowEffectBFlareOffsets[(short)((u16)((Combatant *)D_8009D254->core)->action->actionCode.actionId - 1)];
    ApplyMatrixSV((GteMatrix *)&g_GlowEffectBOwner->render_object.matrices[19], entry, &out);

    anim_s0->position.x = D_800E2350.x + out.x;
    anim_s0->position.y = D_800E2350.y + out.y;
    data = g_GlowEffectBOwner;
    anim_s0->position.z = D_800E2350.z + out.z;

    model = data->render_object.matrices;
    anim_s0->transform = *(GteMatrixStorage *)&model[19];
    anim_s0->brightness = 0x7F;
}

int func_800C91A8(void *arg0, void *arg1, FieldAnimGlowPoint *anim) {
    GteShortVector in;
    GteShortVector out;

    in = g_GlowEffectBPointOffset;
    ApplyMatrixSV((GteMatrix *)&g_GlowEffectBOwner->render_object.matrices[19], &in, &out);

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
    scale.x = g_GlowEffectBScaleByAction[index];
    scale.y = g_GlowEffectBScaleByAction[index];
    scale.z = g_GlowEffectBScaleByAction[index];
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
    func_800C42A4(&D_800E2328, &matrix, 1);
}

int func_800C9968(void *arg0, u8 *state) {
    int ret = 2;

    state[1] = ret;
    return ret;
}
int func_800C9974(void *arg0, FieldEngSlot *state, FieldAnimMovingParticle *anim) {
    FieldAnimMovingParticle *particle = anim;
    u16 vy = anim->velocity.y;
    u16 vz = anim->velocity.z;
    int age;
    int lifetime;

    anim->position.x += anim->velocity.x;
    anim->position.y += vy;
    anim->position.z += vz;
    anim->velocity.y += 3;
    age = anim->age;
    anim->age = age + 1;
    if ((s16)anim->position.y > 0) {
        anim->velocity.y = -anim->velocity.y;
    }
    lifetime = particle->lifetime;
    particle->lifetime = lifetime - 1;
    if (lifetime == 0) {
        state->flag = 2;
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
