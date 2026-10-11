#include "common.h"
#include "pe1/field_engine_slot.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/field_anim_particle.h"
#include "pe1/field_sprite_state.h"
#include "pe1/gte_types.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/psyq_libc.h"
#include "pe1/battle.h"
#include "pe1/field_anim_pool.h"

#include "FieldEng_GlowEffect_externs.h"

int func_800C7BA0(char *object) {
    void **slot;

    slot = FieldEng_GetSlot(object);
    *slot = &D_800E0928;

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

#include "FieldEng_GlowEffect_externs_1.h"

int func_800C7D00(int a, int b, int c, int d, int e, int f) {
    FieldEng_Spawn6(a, b, c, d, e, f);
    return 0;
}
#include "FieldEng_GlowEffect_externs_2.h"

int func_800C7D2C(void *o) {
    FieldEng_Register(o, D_800E0824);
    return 0;
}
#include "FieldEng_GlowEffect_externs_3.h"

int func_800C7D54(void *obj) {
    int first;
    int second;
    int status;

    first = func_800C251C(obj, D_800E0840);
    second = func_800C2758(obj, D_800E0808, D_800E085C);
    status = first | second;
    if (status == -1) {
        func_800C7DC4(obj);
    }
    return 0;
}
int func_800C7DC4(char *arg0) {
    *arg0 = 4;
    return 0;
}
int func_800C7DD4(void) {
    return 0;
}
int func_800C7DDC(void) {
    return 0;
}

#include "FieldEng_GlowEffect_externs_4.h"

int func_800C7DE4(FieldAnimObjectPrefix *obj) {
    FieldActor *data = obj->actor;

    D_800E2348.x = data->render_object.matrices[19].translation[0];
    D_800E2348.y = data->render_object.matrices[19].translation[1];
    D_800E2348.z = data->render_object.matrices[19].translation[2];
    D_800E279C = data;
    func_800CEDA8(0);
}

int rand(void);

int func_800C7E50(void *arg0, void *arg1, FieldAnimMovingParticle *anim) {
    GteShortVector vec;
    RenderMatrix *model;

    vec.x = -(rand() % 3 + 9);
    vec.y = -(rand() % 3 + 9);
    vec.z = rand() % 5 - 2;

    model = D_800E279C->render_object.matrices;
    ApplyMatrixSV((GteMatrix *)model, &vec, &anim->velocity);

    anim->position.x = D_800E2348.x;
    anim->position.y = D_800E2348.y;
    anim->position.z = D_800E2348.z;
    anim->lifetime = 0x14;
    anim->age = 0;
}

#include "FieldEng_GlowEffect_externs_5.h"

int func_800C7F60(void *arg0, void *arg1, FieldAnimMatrixGlow *anim) {
    FieldAnimMatrixGlow *anim_s0 = anim;
    GteShortVector out;
    char *entry;
    FieldActor *data;
    RenderMatrix *model;
    int index;

    index = (u16)((Combatant *)D_8009D254->core)->action->actionCode.actionId - 1;
    index <<= 16;
    index >>= 13;
    entry = D_800E08A8 + index;
    ApplyMatrixSV((GteMatrix *)&D_800E279C->render_object.matrices[19], entry, &out);

    anim_s0->position.x = D_800E2348.x + out.x;
    anim_s0->position.y = D_800E2348.y + out.y;
    data = D_800E279C;
    anim_s0->position.z = D_800E2348.z + out.z;

    model = data->render_object.matrices;
    anim_s0->transform = *(GteMatrixStorage *)&model[19];
    anim_s0->brightness = 0x7F;
}

#include "FieldEng_GlowEffect_externs_6.h"

int func_800C8064(void *arg0, void *arg1, FieldAnimGlowPoint *anim) {
    GteShortVector in;
    GteShortVector out;
    RenderMatrix *model;

    in = D_800C214C;

    model = D_800E279C->render_object.matrices;
    ApplyMatrixSV((GteMatrix *)&model[19], &in, &out);

    anim->position.x = D_800E2348.x + out.x;
    anim->position.y = D_800E2348.y + out.y;
    anim->position.z = D_800E2348.z + out.z;
    anim->brightness = 0x7F;
    anim->scale = 0;
}

int func_800C811C(void *arg0, void *arg1, FieldAnimGlowPoint *anim) {
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

#include "FieldEng_GlowEffect_externs_7.h"

int func_800C815C(void *arg0, void *arg1, FieldAnimMatrixGlow *anim) {
    FieldAnimMatrixGlow *anim_s0 = anim;
    GteShortVector out;
    FieldActor *data;
    RenderMatrix *model;
    char *entry;
    int index;

    index = (u16)((Combatant *)D_8009D254->core)->action->actionCode.actionId - 1;
    index <<= 16;
    index >>= 13;
    entry = D_800E08E8 + index;
    ApplyMatrixSV((GteMatrix *)&D_800E279C->render_object.matrices[19], entry, &out);

    anim_s0->position.x = D_800E2348.x + out.x;
    anim_s0->position.y = D_800E2348.y + out.y;
    data = D_800E279C;
    anim_s0->position.z = D_800E2348.z + out.z;

    model = data->render_object.matrices;
    anim_s0->transform = *(GteMatrixStorage *)&model[19];
    anim_s0->brightness = 0x7F;
    anim_s0->scale = 0x224;
}

void func_800C8268(void) {
}

#include "FieldEng_GlowEffect_externs_8.h"

int func_800C8270(void *arg0, void *arg1, FieldAnimMatrixGlow *anim) {
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
    scale.x = D_800E0888[index];
    scale.y = D_800E0888[index];
    scale.z = D_800E0888[index];
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    func_800C42A4(&D_800E22D8, &matrix, 1);
}

#define GLOW_LAYERS_FUNC func_800C83A0
#define GLOW_LAYERS_SPIN_A D_800C2154
#define GLOW_LAYERS_SPIN_B D_800C215C
#define GLOW_LAYERS_SCALE D_800C2164
#define GLOW_LAYERS_SPRITE D_800F3498
#include "FieldEng_GlowLayers.inc"

#include "FieldEng_GlowEffect_externs_9.h"

int func_800C8870(void *arg0, void *arg1, FieldAnimGlowPoint *anim) {
    GteMatrix matrix;
    int scale;
    u16 field;
    int localScale[4];
    u16 *field_s3 = &D_800E2322;

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

#include "FieldEng_GlowEffect_externs_10.h"

int func_800C8970(void *arg0, void *arg1, FieldAnimGlowPoint *anim) {
    u16 *field = &D_800F34E2;
    GteMatrix matrix;
    GteShortVector rot;
    GteVector scaleCopy;
    GteVector scale;

    rot = D_800C2174;

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
    func_800C42A4((FieldGlowSprite *)((u8 *)field - 10), &matrix, 0);
}

int func_800C8A88(void *arg0, void *arg1, FieldAnimMatrixGlow *anim) {
    u16 *field = &D_800F34E2;
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
    func_800C42A4((FieldGlowSprite *)((u8 *)field - 10), &matrix, 0);
}

int func_800C8BB4(void *arg0, u8 *state) {
    int ret = 2;

    state[1] = ret;
    return ret;
}
int func_800C8BC0(void *arg0, u8 *state, FieldAnimMovingParticle *anim) {
    u8 *state_a3 = state;
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
        anim_a2->velocity.y = -anim_a2->velocity.y;
    }

    count = ((FieldAnimMovingParticle *)temp_a0)->lifetime;
    ((FieldAnimMovingParticle *)temp_a0)->lifetime = count - 1;
    if (count == 0) {
        state_a3[1] = 2;
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
