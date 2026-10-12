#include "common.h"
#include "pe1/fieldfx_slot.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/fieldfx_particle.h"
#include "pe1/field_sprite_state.h"
#include "pe1/gte_types.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/psyq_libc.h"
#include "pe1/battle.h"
#include "pe1/fieldfx_task_pool.h"
#include "pe1/field_effect_modules.h"
#include "pe1/field_glow_layers.h"
#include "pe1/gte.h"
#include "pe1/fieldfx_task.h"
#include "pe1/fieldfx_state.h"

int GlowEffect_PlantScript(char *object) {
    void **slot;

    slot = FieldFx_GetSlot(object);
    *slot = &g_GlowEffectScript;

    D_800E22D8.cell = 0xBD;
    D_800E22D8.clut = 9;
    D_800E22D8.offset = 0;
    D_800E22D8.depth = 0x80;
    D_800E22D8.r = 0x80;
    D_800E22D8.g = 0x80;
    D_800E22D8.b = 0x80;
    D_800E22D8.flip = 0;

    D_800F3498.cell = 0xAE;
    D_800F3498.clut = 7;
    D_800F3498.offset = -0x32;
    D_800F3498.depth = 0x80;
    D_800F3498.r = 0x50;
    D_800F3498.g = 0x50;
    D_800F3498.b = 0x50;
    D_800F3498.flip = 0;

    D_800E2318.cell = 0x68;
    D_800E2318.clut = 0;
    D_800E2318.offset = -0x3C;
    D_800E2318.depth = 0x80;
    D_800E2318.r = 0x50;
    D_800E2318.g = 0x50;
    D_800E2318.b = 0x50;
    D_800E2318.flip = 0;

    D_800F34D8.cell = 0x42;
    D_800F34D8.clut = 0x20;
    D_800F34D8.offset = 0x32;
    D_800F34D8.depth = 0x80;
    D_800F34D8.r = 0x80;
    D_800F34D8.g = 0x80;
    D_800F34D8.b = 0x80;
    D_800F34D8.flip = 0;

    return 0;
}

int GlowEffect_Spawn6(int a, int b, int c, int d, int e, int f) {
    FieldFx_Spawn6(a, b, c, d, e, f);
    return 0;
}
int GlowEffect_RegisterDrawList(void *o) {
    FieldFx_Register(o, g_GlowEffectDrawList);
    return 0;
}
int GlowEffect_RegisterPairedTables(void *obj) {
    int first;
    int second;
    int status;

    first = FieldFx_UpdateSlots(obj, g_GlowEffectUpdateList);
    second = FieldFx_RunScript(obj, g_GlowEffectInitList, g_GlowEffectSlotSizes);
    status = first | second;
    if (status == -1) {
        GlowEffect_CloseTarget(obj);
    }
    return 0;
}
int GlowEffect_CloseTarget(char *arg0) {
    *arg0 = 4;
    return 0;
}
int GlowEffect_ClassNop6(void) {
    return 0;
}
int GlowEffect_ClassNop0(void) {
    return 0;
}

int func_800C7DE4(FieldFxObjectPrefix *obj) {
    FieldActor *data = obj->actor;

    D_800E2348.x = data->render_object.matrices[19].translation[0];
    D_800E2348.y = data->render_object.matrices[19].translation[1];
    D_800E2348.z = data->render_object.matrices[19].translation[2];
    g_GlowEffectOwner = data;
    func_800CEDA8(0);
}

int func_800C7E50(void *arg0, void *arg1, FieldFxMovingParticle *anim) {
    GteShortVector vec;
    RenderMatrix *model;

    vec.x = -(rand() % 3 + 9);
    vec.y = -(rand() % 3 + 9);
    vec.z = rand() % 5 - 2;

    model = g_GlowEffectOwner->render_object.matrices;
    ApplyMatrixSV((GteMatrix *)model, &vec, &anim->velocity);

    anim->position.x = D_800E2348.x;
    anim->position.y = D_800E2348.y;
    anim->position.z = D_800E2348.z;
    anim->lifetime = 0x14;
    anim->age = 0;
}

int func_800C7F60(void *arg0, void *arg1, FieldFxMatrixGlow *anim) {
    FieldFxMatrixGlow *anim_s0 = anim;
    GteShortVector out;
    GteShortVector *entry;
    FieldActor *data;
    RenderMatrix *model;

    entry = &g_GlowEffectFlareOffsets[(short)((u16)((Combatant *)D_8009D254->core)->action->actionCode.actionId - 1)];
    ApplyMatrixSV((GteMatrix *)&g_GlowEffectOwner->render_object.matrices[19], entry, &out);

    anim_s0->position.x = D_800E2348.x + out.x;
    anim_s0->position.y = D_800E2348.y + out.y;
    data = g_GlowEffectOwner;
    anim_s0->position.z = D_800E2348.z + out.z;

    model = data->render_object.matrices;
    anim_s0->transform = *(GteMatrixStorage *)&model[19];
    anim_s0->brightness = 0x7F;
}

int func_800C8064(void *arg0, void *arg1, FieldFxGlowPoint *anim) {
    GteShortVector in;
    GteShortVector out;
    RenderMatrix *model;

    in = g_GlowEffectPointOffset;

    model = g_GlowEffectOwner->render_object.matrices;
    ApplyMatrixSV((GteMatrix *)&model[19], &in, &out);

    anim->position.x = D_800E2348.x + out.x;
    anim->position.y = D_800E2348.y + out.y;
    anim->position.z = D_800E2348.z + out.z;
    anim->brightness = 0x7F;
    anim->scale = 0;
}

int func_800C811C(void *arg0, void *arg1, FieldFxGlowPoint *anim) {
    BattleEntity *entity_v1 = D_8009D254;
    int value;
    int z_v1;

    anim->position.x = D_800E2348.x;
    value = entity_v1->posY.parts.integer;
    anim->position.y = value;
    z_v1 = D_800E2348.z;
    anim->brightness = 0x7F;
    anim->scale = 0x224;
    anim->position.z = z_v1;
}

int func_800C815C(void *arg0, void *arg1, FieldFxMatrixGlow *anim) {
    FieldFxMatrixGlow *anim_s0 = anim;
    GteShortVector out;
    FieldActor *data;
    RenderMatrix *model;
    GteShortVector *entry;

    entry = &g_GlowEffectHaloOffsets[(short)((u16)((Combatant *)D_8009D254->core)->action->actionCode.actionId - 1)];
    ApplyMatrixSV((GteMatrix *)&g_GlowEffectOwner->render_object.matrices[19], entry, &out);

    anim_s0->position.x = D_800E2348.x + out.x;
    anim_s0->position.y = D_800E2348.y + out.y;
    data = g_GlowEffectOwner;
    anim_s0->position.z = D_800E2348.z + out.z;

    model = data->render_object.matrices;
    anim_s0->transform = *(GteMatrixStorage *)&model[19];
    anim_s0->brightness = 0x7F;
    anim_s0->scale = 0x224;
}

void func_800C8268(void) {
}

int func_800C8270(void *arg0, void *arg1, FieldFxMatrixGlow *anim) {
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
    scale.x = g_GlowEffectScaleByAction[index];
    scale.y = g_GlowEffectScaleByAction[index];
    scale.z = g_GlowEffectScaleByAction[index];
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    func_800C42A4(&D_800E22D8, &matrix, 1);
}

#define GLOW_LAYERS_FUNC func_800C83A0
#define GLOW_LAYERS_SPIN_A D_800C2154
#define GLOW_LAYERS_SPIN_B D_800C215C
#define GLOW_LAYERS_SCALE D_800C2164
#define GLOW_LAYERS_SPRITE D_800F3498
#include "glow_layers.inc"

int func_800C8870(void *arg0, void *arg1, FieldFxGlowPoint *anim) {
    GteMatrix matrix;
    int scale;
    u16 field;
    int localScale[4];
    u16 *field_s3 = &D_800E2318.depth;

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
    func_800C42A4(&D_800E2318, &matrix, 1);
}

int func_800C8970(void *arg0, void *arg1, FieldFxGlowPoint *anim) {
    u16 *field = &D_800F34D8.depth;
    GteMatrix matrix;
    GteShortVector rot;
    GteVector scaleCopy;
    GteVector scale;

    rot = g_GlowEffectSpriteRotation;

    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    *field = anim->brightness;
    RotMatrix(&rot, &matrix);

    matrix.t[0] = (s16)anim->position.x;
    matrix.t[1] = (s16)anim->position.y;
    matrix.t[2] = (s16)anim->position.z;

    memset(&scale, 0, sizeof(scale));
    scale.x = anim->scale;
    scale.y = anim->scale;
    scale.z = anim->scale;
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    func_800C42A4(&D_800F34D8, &matrix, 0);
}

int func_800C8A88(void *arg0, void *arg1, FieldFxMatrixGlow *anim) {
    u16 *field = &D_800F34D8.depth;
    GteMatrix matrix;
    GteVector scaleCopy;
    GteVector scale;

    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    *field = anim->brightness;
    matrix = anim->transform.matrix;

    matrix.t[0] = (s16)anim->position.x;
    matrix.t[1] = (s16)anim->position.y;
    matrix.t[2] = (s16)anim->position.z;

    memset(&scale, 0, sizeof(scale));
    scale.x = anim->scale;
    scale.y = anim->scale;
    scale.z = anim->scale;
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    func_800C42A4(&D_800F34D8, &matrix, 0);
}

int func_800C8BB4(void *arg0, u8 *state) {
    int ret = 2;

    state[1] = ret;
    return ret;
}
int func_800C8BC0(void *arg0, FieldFxSlot *state, FieldFxMovingParticle *anim) {
    FieldFxMovingParticle *particle = anim;
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
void func_800C8C4C(char *arg0, char *arg1, short *arg2) {
    arg2[2] -= 0x14;
    if (arg2[2] < 0x14) {
        arg2[2] = 0;
        arg1[1] = 2;
    }
}
void func_800C8C80(char *arg0, char *arg1, short *arg2) {
    arg2[2] -= 8;
    arg2[3] += 0x3C;
    if (arg2[2] < 0x14) {
        arg2[2] = 0;
        arg1[1] = 2;
    }
}
void func_800C8CBC(char *arg0, char *arg1, short *arg2) {
    arg2[2] -= 8;
    arg2[3] += 0x78;
    if (arg2[2] < 0x14) {
        arg2[2] = 0;
        arg1[1] = 2;
    }
}
void func_800C8CF8(char *arg0, char *arg1, short *arg2) {
    arg2[2] -= 8;
    arg2[3] += 0xA;
    if (arg2[2] < 0x14) {
        arg2[2] = 0;
        arg1[1] = 2;
    }
}

int GlowEffectB_PlantScript(char *object) {
    void **slot;

    slot = FieldFx_GetSlot(object);
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
    FieldFx_Spawn6(a, b, c, d, e, f);
    return 0;
}
int GlowEffectB_RegisterDrawList(void *o) {
    FieldFx_Register(o, g_GlowEffectBDrawList);
    return 0;
}
int GlowEffectB_RegisterPairedTables(void *obj) {
    int first;
    int second;
    int status;

    first = FieldFx_UpdateSlots(obj, g_GlowEffectBUpdateList);
    second = FieldFx_RunScript(obj, g_GlowEffectBInitList, g_GlowEffectBSlotSizes);
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

int func_800C8F28(FieldFxObjectPrefix *obj) {
    FieldActor *data = obj->actor;

    D_800E2350.x = data->render_object.matrices[19].translation[0];
    D_800E2350.y = data->render_object.matrices[19].translation[1];
    D_800E2350.z = data->render_object.matrices[19].translation[2];
    g_GlowEffectBOwner = data;
    func_800CEDA8(0);
}

int func_800C8F94(void *arg0, void *arg1, FieldFxMovingParticle *anim) {
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

int func_800C90A4(void *arg0, void *arg1, FieldFxMatrixGlow *anim) {
    FieldFxMatrixGlow *anim_s0 = anim;
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

int func_800C91A8(void *arg0, void *arg1, FieldFxGlowPoint *anim) {
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

int func_800C9268(void *arg0, void *arg1, FieldFxMatrixGlow *anim) {
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
#include "glow_layers.inc"

int func_800C9868(void *arg0, void *arg1, FieldFxGlowPoint *anim) {
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
int func_800C9974(void *arg0, FieldFxSlot *state, FieldFxMovingParticle *anim) {
    FieldFxMovingParticle *particle = anim;
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
int func_800C9A00(void *arg0, FieldFxSlot *state, FieldFxGlowPoint *anim) {
    s16 value = anim->brightness - 0x14;

    anim->brightness = value;
    if (value < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}
int func_800C9A34(void *arg0, FieldFxSlot *state, FieldFxGlowPoint *anim) {
    anim->brightness = anim->brightness - 8;
    anim->scale = (u16)anim->scale + 0x28;
    if ((s16)anim->brightness < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}

int GlowEffectC_PlantScript(char *object) {
    void **slot;

    slot = FieldFx_GetSlot(object);
    *slot = &g_GlowEffectCScript;

    D_800E22F8.cell = 0xBD;
    D_800E22F8.clut = 9;
    D_800E22F8.offset = 0;
    D_800E22F8.depth = 0x80;
    D_800E22F8.r = 0x80;
    D_800E22F8.g = 0x80;
    D_800E22F8.b = 0x80;
    D_800E22F8.flip = 0;

    D_800F34B8.cell = 0xAC;
    D_800F34B8.clut = 6;
    D_800F34B8.offset = -0x32;
    D_800F34B8.depth = 0x80;
    D_800F34B8.r = 0x50;
    D_800F34B8.g = 0x50;
    D_800F34B8.b = 0x50;
    D_800F34B8.flip = 0;

    return 0;
}
int GlowEffectC_Spawn6(int a, int b, int c, int d, int e, int f) {
    FieldFx_Spawn6(a, b, c, d, e, f);
    return 0;
}
int GlowEffectC_RegisterDrawList(void *o) {
    FieldFx_Register(o, g_GlowEffectCDrawList);
    return 0;
}
int GlowEffectC_RegisterPairedTables(void *obj) {
    int first;
    int second;
    int status;

    first = FieldFx_UpdateSlots(obj, g_GlowEffectCUpdateList);
    second = FieldFx_RunScript(obj, g_GlowEffectCInitList, g_GlowEffectCSlotSizes);
    status = first | second;
    if (status == -1) {
        GlowEffectC_CloseTarget(obj);
    }
    return 0;
}
int GlowEffectC_CloseTarget(char *arg0) {
    *arg0 = 4;
    return 0;
}
int GlowEffectC_ClassNop6(void) {
    return 0;
}
int GlowEffectC_ClassNop0(void) {
    return 0;
}

int func_800C9C20(FieldFxObjectPrefix *obj) {
    FieldActor *data = obj->actor;

    D_800E2358.x = data->render_object.matrices[19].translation[0];
    D_800E2358.y = data->render_object.matrices[19].translation[1];
    D_800E2358.z = data->render_object.matrices[19].translation[2];
    g_GlowEffectCOwner = data;
    func_800CEDA8(0);
}

int func_800C9C8C(void *arg0, void *arg1, FieldFxMovingParticle *anim) {
    GteShortVector vec;
    RenderMatrix *model;

    vec.x = -(rand() % 3 + 9);
    vec.y = -(rand() % 3 + 9);
    vec.z = rand() % 5 - 2;

    model = g_GlowEffectCOwner->render_object.matrices;
    ApplyMatrixSV((GteMatrix *)model, &vec, &anim->velocity);

    anim->position.x = D_800E2358.x;
    anim->position.y = D_800E2358.y;
    anim->position.z = D_800E2358.z;
    anim->lifetime = 0x14;
    anim->age = 0;
}

int func_800C9D9C(void *arg0, void *arg1, FieldFxMatrixGlow *anim) {
    FieldFxMatrixGlow *anim_s0 = anim;
    GteShortVector out;
    FieldActor *data;
    GteShortVector *entry;
    RenderMatrix *model;

    entry = &g_GlowEffectCFlareOffsets[(short)((u16)((Combatant *)D_8009D254->core)->action->actionCode.actionId - 1)];
    ApplyMatrixSV((GteMatrix *)&g_GlowEffectCOwner->render_object.matrices[19], entry, &out);

    anim_s0->position.x = D_800E2358.x + out.x;
    anim_s0->position.y = D_800E2358.y + out.y;
    data = g_GlowEffectCOwner;
    anim_s0->position.z = D_800E2358.z + out.z;

    model = data->render_object.matrices;
    anim_s0->transform = *(GteMatrixStorage *)&model[19];
    anim_s0->brightness = 0x7F;
}

void func_800C9EA0(void) {
}

int func_800C9EA8(void *arg0, void *arg1, FieldFxMatrixGlow *anim) {
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
    scale.x = g_GlowEffectCScaleByAction[index];
    scale.y = g_GlowEffectCScaleByAction[index];
    scale.z = g_GlowEffectCScaleByAction[index];
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    func_800C42A4(&D_800E22F8, &matrix, 1);
}

#define GLOW_LAYERS_FUNC func_800C9FD8
#define GLOW_LAYERS_SPIN_A D_800C21A4
#define GLOW_LAYERS_SPIN_B D_800C21AC
#define GLOW_LAYERS_SCALE D_800C21B4
#define GLOW_LAYERS_SPRITE D_800F34B8
#include "glow_layers.inc"

int func_800CA4A8(void *arg0, FieldFxSlot *state) {
    int ret = 2;

    state->flag = ret;
    return ret;
}

int func_800CA4B4(void *arg0, FieldFxSlot *state, FieldFxMovingParticle *anim) {
    FieldFxMovingParticle *particle = anim;
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

int func_800CA540(void *arg0, FieldFxSlot *state, FieldFxGlowPoint *anim) {
    s16 value = anim->brightness - 0x14;

    anim->brightness = value;
    if (value < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}

int FourLayerGlow_PlantScript(char *object) {
    void **slot;

    slot = FieldFx_GetSlot(object);
    *slot = &g_FourLayerGlowScript;

    D_800E2308.cell = 0xBD;
    D_800E2308.clut = 9;
    D_800E2308.offset = 0;
    D_800E2308.depth = 0x80;
    D_800E2308.r = 0x80;
    D_800E2308.g = 0x80;
    D_800E2308.b = 0x80;
    D_800E2308.flip = 0;

    D_800F34C8.cell = 0xAE;
    D_800F34C8.clut = 7;
    D_800F34C8.offset = -0x32;
    D_800F34C8.depth = 0x80;
    D_800F34C8.r = 0x50;
    D_800F34C8.g = 0x50;
    D_800F34C8.b = 0x50;
    D_800F34C8.flip = 0;

    D_800E2338.cell = 0x68;
    D_800E2338.clut = 0;
    D_800E2338.offset = -0x3C;
    D_800E2338.depth = 0x80;
    D_800E2338.r = 0x50;
    D_800E2338.g = 0x50;
    D_800E2338.b = 0x50;
    D_800E2338.flip = 0;

    D_800F34E8.cell = 0x42;
    D_800F34E8.clut = 0x20;
    D_800F34E8.offset = 0x32;
    D_800F34E8.depth = 0x80;
    D_800F34E8.r = 0x80;
    D_800F34E8.g = 0x80;
    D_800F34E8.b = 0x80;
    D_800F34E8.flip = 0;

    return 0;
}

int FourLayerGlow_Spawn6(int a, int b, int c, int d, int e, int f) {
    FieldFx_Spawn6(a, b, c, d, e, f);
    return 0;
}

int FourLayerGlow_RegisterDrawList(void *o) {
    FieldFx_Register(o, g_FourLayerGlowDrawList);
    return 0;
}

int FourLayerGlow_RegisterPairedTables(void *obj) {
    int first;
    int second;
    int status;

    first = FieldFx_UpdateSlots(obj, g_FourLayerGlowUpdateList);
    second = FieldFx_RunScript(obj, g_FourLayerGlowInitList, g_FourLayerGlowSlotSizes);
    status = first | second;
    if (status == -1) {
        FourLayerGlow_CloseTarget(obj);
    }
    return 0;
}

int FourLayerGlow_CloseTarget(char *arg0) {
    *arg0 = 4;
    return 0;
}

int FourLayerGlow_ClassNop6(void) {
    return 0;
}

int FourLayerGlow_ClassNop0(void) {
    return 0;
}

int func_800CA7B8(FieldFxObjectPrefix *obj) {
    FieldActor *data = obj->actor;

    D_800E2360.x = data->render_object.matrices[19].translation[0];
    D_800E2360.y = data->render_object.matrices[19].translation[1];
    D_800E2360.z = data->render_object.matrices[19].translation[2];
    g_FourLayerGlowOwner = data;
    func_800CEDA8(0);
}

int func_800CA824(void *arg0, void *arg1, FieldFxMovingParticle *anim) {
    GteShortVector vec;

    vec.x = -((rand() % 3) + 9);
    vec.y = -((rand() % 3) + 9);
    vec.z = (rand() % 5) - 2;

    ApplyMatrixSV((GteMatrix *)g_FourLayerGlowOwner->render_object.matrices, &vec, &anim->velocity);

    {
        u16 z;

        anim->position.x = D_800E2360.x;
        anim->position.y = D_800E2360.y;
        z = D_800E2360.z;
        anim->lifetime = 0x14;
        anim->age = 0;
        anim->position.z = z;
    }
}

int func_800CA934(void *arg0, void *arg1, FieldFxMatrixGlow *anim) {
    FieldFxMatrixGlow *anim_s0 = anim;
    GteShortVector out;
    FieldActor *data;
    GteShortVector *entry;
    RenderMatrix *model;

    entry = &g_FourLayerGlowFlareOffsets[(short)((u16)((Combatant *)D_8009D254->core)->action->actionCode.actionId - 1)];
    ApplyMatrixSV((GteMatrix *)&g_FourLayerGlowOwner->render_object.matrices[19], entry, &out);

    anim_s0->position.x = D_800E2360.x + out.x;
    anim_s0->position.y = D_800E2360.y + out.y;
    data = g_FourLayerGlowOwner;
    anim_s0->position.z = D_800E2360.z + out.z;

    model = data->render_object.matrices;
    anim_s0->transform = *(GteMatrixStorage *)&model[19];
    anim_s0->brightness = 0x7F;
}

int func_800CAA38(void *arg0, void *arg1, FieldFxTwinGlow *anim) {
    FieldFxTwinGlow *anim_s0;
    u16 *field_s1;
    GteShortVector *out_s2;
    u16 z_base_v1;
    u16 z_out_a0;
    GteShortVector in0;
    GteShortVector in1;
    GteShortVector out;
    FieldActor *data;

    anim_s0 = anim;

    in0 = g_FourLayerGlowPointOffsetA;
    in1 = g_FourLayerGlowPointOffsetB;
    out_s2 = &out;

    ApplyMatrixSV((GteMatrix *)&g_FourLayerGlowOwner->render_object.matrices[19], &in0, out_s2);
    field_s1 = &D_800E2360.x;
    anim_s0->position[0].x = field_s1[0] + out.x;
    anim_s0->position[0].y = D_800E2360.y + out.y;
    data = g_FourLayerGlowOwner;
    anim_s0->position[0].z = D_800E2360.z + out.z;

    ApplyMatrixSV((GteMatrix *)&data->render_object.matrices[19], &in1, out_s2);
    anim_s0->position[1].x = field_s1[0] + out.x;
    anim_s0->position[1].y = D_800E2360.y + (u16)out.y;
    z_base_v1 = D_800E2360.z;
    z_out_a0 = out.z;
    anim_s0->brightness = 0x7F;
    anim_s0->scale = 0;
    anim_s0->position[1].z = z_base_v1 + z_out_a0;
}

int func_800CAB88(void *arg0, void *arg1, FieldFxGlowPoint *anim) {
    u16 x = D_800E2360.x;
    BattleEntity *data = D_8009D254;
    int y;
    u16 z;

    anim->position.x = x;
    y = data->posY.parts.integer;
    anim->position.y = y;
    z = D_800E2360.z;
    anim->brightness = 0x7F;
    anim->scale = 0x224;
    anim->position.z = z;
}

int func_800CABC8(void *arg0, void *arg1, FieldFxMatrixGlow *anim) {
    FieldFxMatrixGlow *anim_s0 = anim;
    GteShortVector out;
    FieldActor *data;
    GteShortVector *entry;
    RenderMatrix *model;

    entry = &g_FourLayerGlowHaloOffsets[(short)((u16)((Combatant *)D_8009D254->core)->action->actionCode.actionId - 1)];
    ApplyMatrixSV((GteMatrix *)&g_FourLayerGlowOwner->render_object.matrices[19], entry, &out);

    anim_s0->position.x = D_800E2360.x + out.x;
    anim_s0->position.y = D_800E2360.y + out.y;
    data = g_FourLayerGlowOwner;
    anim_s0->position.z = D_800E2360.z + out.z;

    model = data->render_object.matrices;
    anim_s0->transform = *(GteMatrixStorage *)&model[19];
    anim_s0->brightness = 0x7F;
    anim_s0->scale = 0x224;
}

void func_800CACD4(void) {
}

int func_800CACDC(void *arg0, void *arg1, FieldFxMatrixGlow *anim) {
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
    scale.x = g_FourLayerGlowScaleByAction[index];
    scale.y = g_FourLayerGlowScaleByAction[index];
    scale.z = g_FourLayerGlowScaleByAction[index];
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    func_800C42A4(&D_800E2308, &matrix, 1);
}

/* Four-layer variant of the field glow: two pairs of fixed spins, the
 * second pair pushed further out and drawn at the full depth.
 * Matching debt: 6 register pins and the remaining empty constraints.
 * Explicit spin address aliases retain the retail spill order. Rotation,
 * column, and translation windows use the stock GTE macros. */
void func_800CAE0C(void *object, void *slot, FieldGlowLayers *glow)
{
    int halfBrightness;
    u16 brightness;
    GteMatrix *workMatrix;
    register GteMatrix *stepMatrix asm("$18");

    GteShortVector *spinCAddress;
    GteShortVector *spinDAddress;

    register const volatile u16 *column1 asm("$19");
    register const volatile u16 *column2 asm("$22");
    register u16 *outColumn1 asm("$20");
    u16 *outColumn2;
    const s32 *translation;
    s32 *outTranslation;
    register GteShortVector *nextSpin asm("$23");
    GteShortVector *thirdSpin;
    GteShortVector *fourthSpin;
    GteMatrix matrix;
    GteMatrix rotation;
    GteShortVector spinA;
    GteShortVector spinB;
    GteShortVector spinC;
    GteShortVector spinD;
    GteVector scaleA;
    GteVector scaleB;
    GteVector scaleC;
    GteVector scaleD;

    spinA = D_800C21D4;
    spinB = D_800C21DC;
    spinCAddress = &spinC;
    spinDAddress = &spinD;
    spinC = D_800C21E4;
    spinD = D_800C21EC;
    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x10);
    func_800C3238(2);
    halfBrightness = (s16)glow->depth >> 1;
    D_800F34C8.offset = -10;
    D_800F34C8.depth = halfBrightness;

    RotMatrixYXZ(&spinA, &rotation);
    stepMatrix = &rotation;
        matrix = glow->matrix;
    matrix.t[0] = glow->x;
    matrix.t[1] = glow->y;
    matrix.t[2] = glow->z;
    rotation.t[0] = 0;
    rotation.t[1] = 0;
    rotation.t[2] = -20;
    nextSpin = &spinB;
    thirdSpin = spinCAddress;
    fourthSpin = spinDAddress;
    workMatrix = &matrix;

    gte_ldrotmatrix((const GteMatrixWords *)workMatrix);
    gte_ldclmv((const u16 *)stepMatrix);
    gte_rtir();

    gte_stclmv((u16 *)workMatrix);
    column1 = (const volatile u16 *)&rotation + 1;
        gte_ldclmv(column1);
    gte_rtir();
    outColumn1 = (u16 *)&matrix + 1;
        gte_stclmv(outColumn1);
    column2 = (const volatile u16 *)&rotation + 2;
        gte_ldclmv(column2);
    gte_rtir();
    outColumn2 = (u16 *)&matrix + 2;
    asm("" : "=r"(outColumn2) : "0"(outColumn2));
    gte_stclmv(outColumn2);
    /* Compose the translation using the current matrix. */

    gte_ldtransmatrix((const GteMatrixWords *)workMatrix);
    translation = rotation.t;
            gte_ldlv0(translation);
    gte_rt();
    outTranslation = matrix.t;
    gte_swc2_9_0(outTranslation);
    gte_swc2_10_4(outTranslation);
    gte_swc2_11_8(outTranslation);

    scaleA = D_800C21F4;
    ScaleMatrix(workMatrix, &scaleA);
    func_800C42A4(&D_800F34C8, workMatrix, 0);

    RotMatrixYXZ(nextSpin, stepMatrix);
    matrix = glow->matrix;
    matrix.t[0] = glow->x;
    matrix.t[1] = glow->y;
    matrix.t[2] = glow->z;

    gte_ldrotmatrix((const GteMatrixWords *)workMatrix);
    gte_ldclmv((const u16 *)stepMatrix);
    gte_rtir();

    gte_stclmv((u16 *)workMatrix);
    gte_ldclmv(column1);
    gte_rtir();
    gte_stclmv(outColumn1);
    gte_ldclmv(column2);
    gte_rtir();
    gte_stclmv(outColumn2);
    /* Compose the translation using the current matrix. */

    gte_ldtransmatrix((const GteMatrixWords *)workMatrix);
    gte_ldlv0(translation);
    gte_rt();
    gte_swc2_9_0(outTranslation);
    gte_swc2_10_4(outTranslation);
    gte_swc2_11_8(outTranslation);

    scaleB = D_800C21F4;
    ScaleMatrix(workMatrix, &scaleB);
    func_800C42A4(&D_800F34C8, workMatrix, 0);

    brightness = glow->depth;
    D_800F34C8.offset = -50;
    D_800F34C8.depth = brightness;

    RotMatrixYXZ(thirdSpin, stepMatrix);
    matrix = glow->matrix;
    matrix.t[0] = glow->x;
    matrix.t[1] = glow->y;
    matrix.t[2] = glow->z;
    rotation.t[0] = 0;
    rotation.t[1] = -10;
    rotation.t[2] = -400;

    gte_ldrotmatrix((const GteMatrixWords *)workMatrix);
    gte_ldclmv((const u16 *)stepMatrix);
    gte_rtir();

    gte_stclmv((u16 *)workMatrix);
    gte_ldclmv(column1);
    gte_rtir();
    gte_stclmv(outColumn1);
    gte_ldclmv(column2);
    gte_rtir();
    gte_stclmv(outColumn2);
    /* Compose the translation using the current matrix. */

    gte_ldtransmatrix((const GteMatrixWords *)workMatrix);
    gte_ldlv0(translation);
    gte_rt();
    gte_swc2_9_0(outTranslation);
    gte_swc2_10_4(outTranslation);
    gte_swc2_11_8(outTranslation);

    scaleC = D_800C21F4;
    ScaleMatrix(workMatrix, &scaleC);
    func_800C42A4(&D_800F34C8, workMatrix, 0);

    RotMatrixYXZ(fourthSpin, stepMatrix);
    matrix = glow->matrix;
    matrix.t[0] = glow->x;
    matrix.t[1] = glow->y;
    matrix.t[2] = glow->z;

    gte_ldrotmatrix((const GteMatrixWords *)workMatrix);
    gte_ldclmv((const u16 *)stepMatrix);
    gte_rtir();

    gte_stclmv((u16 *)workMatrix);
    gte_ldclmv(column1);
    gte_rtir();
    gte_stclmv(outColumn1);
    gte_ldclmv(column2);
    gte_rtir();
    gte_stclmv(outColumn2);
    /* Compose the translation using the current matrix. */

    gte_ldtransmatrix((const GteMatrixWords *)workMatrix);
    gte_ldlv0(translation);
    gte_rt();
    gte_swc2_9_0(outTranslation);
    gte_swc2_10_4(outTranslation);
    gte_swc2_11_8(outTranslation);

            scaleD = D_800C21F4;
    ScaleMatrix(workMatrix, &scaleD);
    func_800C42A4(&D_800F34C8, workMatrix, 0);
}

int func_800CB750(void *arg0, void *arg1, FieldFxTwinGlow *anim) {
    u16 *field_s4 = &D_800E2338.depth;
    GteMatrix matrix;
    int scale;
    u16 field;
    int localScale[4];
    int localScale2[4];

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    scale = anim->scale + 0x42C;
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

    *field_s4 = field;
    matrix.t[0] = (s16)anim->position[0].x;
    matrix.t[1] = (s16)anim->position[0].y;
    matrix.t[2] = (s16)anim->position[0].z;

    memset(localScale, 0, sizeof(localScale));
    localScale[0] = scale;
    localScale[1] = scale;
    localScale[2] = scale;

    ScaleMatrix(&matrix, (const GteVector *)localScale);
    func_800C42A4(&D_800E2338, &matrix, 1);

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

    matrix.t[0] = (s16)anim->position[1].x;
    matrix.t[1] = (s16)anim->position[1].y;
    matrix.t[2] = (s16)anim->position[1].z;

    memset(localScale2, 0, sizeof(localScale2));
    localScale2[0] = scale;
    localScale2[1] = scale;
    localScale2[2] = scale;

    ScaleMatrix(&matrix, (const GteVector *)localScale2);
    func_800C42A4(&D_800E2338, &matrix, 1);
}

int func_800CB8E0(void *arg0, void *arg1, FieldFxMovingParticle *anim) {
    s16 *position;
    u16 *field = &D_800F34E8.depth;
    GteMatrix matrix;
    GteShortVector rot;
    GteVector scaleCopy;
    GteVector scale;

    rot = g_FourLayerGlowSpriteRotation;

    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    *field = anim->brightness;
    RotMatrix(&rot, &matrix);

    position = (s16 *)&anim->position.x;
    matrix.t[0] = position[0];
    matrix.t[1] = position[1];
    matrix.t[2] = position[2];

    memset(&scale, 0, sizeof(scale));
    scale.x = anim->scale;
    scale.y = anim->scale;
    scale.z = anim->scale;
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    func_800C42A4(&D_800F34E8, &matrix, 0);
}

int func_800CB9F8(void *arg0, void *arg1, FieldFxMatrixGlow *anim) {
    u16 *field = &D_800F34E8.depth;
    GteMatrix matrix;
    GteVector scaleCopy;
    GteVector scale;

    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    *field = anim->brightness;
    matrix = anim->transform.matrix;

    matrix.t[0] = (s16)anim->position.x;
    matrix.t[1] = (s16)anim->position.y;
    matrix.t[2] = (s16)anim->position.z;

    memset(&scale, 0, sizeof(scale));
    scale.x = anim->scale;
    scale.y = anim->scale;
    scale.z = anim->scale;
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    func_800C42A4(&D_800F34E8, &matrix, 0);
}

int func_800CBB24(void *arg0, FieldFxSlot *state) {
    int ret = 2;

    state->flag = ret;
    return ret;
}

int func_800CBB30(void *arg0, FieldFxSlot *state, FieldFxMovingParticle *anim) {
    FieldFxMovingParticle *particle = anim;
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

int func_800CBBBC(void *arg0, FieldFxSlot *state, FieldFxGlowPoint *anim) {
    s16 value = anim->brightness - 0x14;

    anim->brightness = value;
    if (value < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}

int func_800CBBF0(void *arg0, FieldFxSlot *state, FieldFxGlowPoint *anim) {
    anim->brightness = anim->brightness - 8;
    anim->scale = (u16)anim->scale + 0x3C;
    if ((s16)anim->brightness < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}

int func_800CBC2C(void *arg0, FieldFxSlot *state, FieldFxGlowPoint *anim) {
    anim->brightness = anim->brightness - 8;
    anim->scale = (u16)anim->scale + 0x78;
    if ((s16)anim->brightness < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}

int func_800CBC68(void *arg0, FieldFxSlot *state, FieldFxGlowPoint *anim) {
    anim->brightness = anim->brightness - 8;
    anim->scale = (u16)anim->scale + 0xA;
    if ((s16)anim->brightness < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}
