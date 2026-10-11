#include "common.h"
#include "pe1/field_engine_slot.h"
#include "pe1/field_anim_particle.h"
#include "pe1/field_sprite_state.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/gte_types.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_glow_layers.h"
#include "pe1/gte.h"
#include "pe1/field_anim.h"
#include "pe1/field_engine_state.h"
#include "pe1/battle.h"
#include "pe1/psyq_libc.h"

extern int D_800E0C88;

int func_800CA574(char *object) {
    int half_a2;
    u8 fullBrightness;
    int shade_a0;
    register int value asm("$3");
    register void *slotData asm("$3");
    void **slot;

    slot = FieldEng_GetSlot(object);
    slotData = &D_800E0C88;
    *slot = slotData;

    value = 0xBD;
    D_800E2308.cell = value;
    value = 9;
    half_a2 = 0x80;
    fullBrightness = 0x80;
    asm volatile("" : "=r"(half_a2), "=r"(fullBrightness) : "0"(half_a2), "1"(fullBrightness));
    D_800E2308.clut = value;
    value = 0xAE;
    D_800F34C8.cell = value;
    value = 7;
    D_800F34C8.clut = value;
    value = -0x32;
    shade_a0 = 0x50;
    asm volatile("" : "=r"(shade_a0) : "0"(shade_a0));
    D_800F34C8.offset = value;
    value = 0x68;
    D_800E2338.cell = value;
    value = -0x3C;
    D_800E2338.offset = value;
    value = 0x42;
    D_800F34E8.cell = value;
    value = 0x20;
    D_800F34E8.clut = value;
    value = 0x32;
    D_800E2308.offset = 0;
    D_800E2308.depth = half_a2;
    D_800E2308.r = fullBrightness;
    D_800E2308.g = fullBrightness;
    D_800E2308.b = fullBrightness;
    D_800E2308.flip = 0;
    D_800F34C8.depth = half_a2;
    D_800F34C8.r = shade_a0;
    D_800F34C8.g = shade_a0;
    D_800F34C8.b = shade_a0;
    D_800F34C8.flip = 0;
    D_800E2338.clut = 0;
    D_800E2338.depth = half_a2;
    D_800E2338.r = shade_a0;
    D_800E2338.g = shade_a0;
    D_800E2338.b = shade_a0;
    D_800E2338.flip = 0;
    D_800F34E8.offset = value;
    D_800F34E8.depth = half_a2;
    D_800F34E8.r = fullBrightness;
    D_800F34E8.g = fullBrightness;
    D_800F34E8.b = fullBrightness;
    D_800F34E8.flip = 0;

    return 0;
}

extern void FieldEng_Spawn6(int a, int b, int c, int d, int e, int f);

int func_800CA6D4(int a, int b, int c, int d, int e, int f) {
    FieldEng_Spawn6(a, b, c, d, e, f);
    return 0;
}

extern char D_800E0B84[];
extern void FieldEng_Register(void *o, void *table);

int func_800CA700(void *o) {
    FieldEng_Register(o, D_800E0B84);
    return 0;
}

extern char D_800E0BA0[];
extern char D_800E0B68[];
extern char D_800E0BBC[];
extern int func_800C251C(void *obj, void *table);
extern int func_800C2758(void *obj, void *table, void *extra);
extern int func_800CA798(char *obj);

int func_800CA728(void *obj) {
    int first;
    int second;
    int status;

    first = func_800C251C(obj, D_800E0BA0);
    second = func_800C2758(obj, D_800E0B68, D_800E0BBC);
    status = first | second;
    if (status == -1) {
        func_800CA798(obj);
    }
    return 0;
}

int func_800CA798(char *arg0) {
    *arg0 = 4;
    return 0;
}

int func_800CA7A8(void) {
    return 0;
}

int func_800CA7B0(void) {
    return 0;
}

extern FieldActor *D_800E27A8;

int func_800CA7B8(FieldAnimObjectPrefix *obj) {
    FieldActor *data = obj->actor;

    D_800E2360.x = data->render_object.matrices[19].translation[0];
    D_800E2360.y = data->render_object.matrices[19].translation[1];
    D_800E2360.z = data->render_object.matrices[19].translation[2];
    D_800E27A8 = data;
    func_800CEDA8(0);
}

int rand(void);

int func_800CA824(void *arg0, void *arg1, FieldAnimMovingParticle *anim) {
    GteShortVector vec;

    vec.x = -((rand() % 3) + 9);
    vec.y = -((rand() % 3) + 9);
    vec.z = (rand() % 5) - 2;

    ApplyMatrixSV((GteMatrix *)D_800E27A8->render_object.matrices, &vec, &anim->velocity);

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

extern struct BattleEntity *D_8009D254;
extern char D_800E0C08[];

int func_800CA934(void *arg0, void *arg1, FieldAnimMatrixGlow *anim) {
    FieldAnimMatrixGlow *anim_s0 = anim;
    GteShortVector out;
    FieldActor *data;
    char *entry;
    RenderMatrix *model;

    entry = D_800E0C08 + (((short)((u16)((Combatant *)D_8009D254->core)->action->actionCode.actionId - 1)) << 3);
    ApplyMatrixSV((GteMatrix *)&D_800E27A8->render_object.matrices[19], entry, &out);

    anim_s0->position.x = D_800E2360.x + out.x;
    anim_s0->position.y = D_800E2360.y + out.y;
    data = D_800E27A8;
    anim_s0->position.z = D_800E2360.z + out.z;

    model = data->render_object.matrices;
    anim_s0->transform = *(GteMatrixStorage *)&model[19];
    anim_s0->brightness = 0x7F;
}

extern GteShortVector D_800C21C4;
extern GteShortVector D_800C21CC;

int func_800CAA38(void *arg0, void *arg1, FieldAnimTwinGlow *anim) {
    FieldAnimTwinGlow *anim_s0;
    u16 *field_s1;
    GteShortVector *out_s2;
    u16 z_base_v1;
    u16 z_out_a0;
    GteShortVector in0;
    GteShortVector in1;
    GteShortVector out;
    FieldActor *data;

    anim_s0 = anim;

    in0 = D_800C21C4;
    in1 = D_800C21CC;
    out_s2 = &out;

    ApplyMatrixSV((GteMatrix *)&D_800E27A8->render_object.matrices[19], &in0, out_s2);
    field_s1 = &D_800E2360.x;
    anim_s0->position[0].x = field_s1[0] + out.x;
    anim_s0->position[0].y = D_800E2360.y + out.y;
    data = D_800E27A8;
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

int func_800CAB88(void *arg0, void *arg1, FieldAnimGlowPoint *anim) {
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

extern char D_800E0C48[];

int func_800CABC8(void *arg0, void *arg1, FieldAnimMatrixGlow *anim) {
    FieldAnimMatrixGlow *anim_s0 = anim;
    GteShortVector out;
    FieldActor *data;
    char *entry;
    RenderMatrix *model;

    entry = D_800E0C48 + (((short)((u16)((Combatant *)D_8009D254->core)->action->actionCode.actionId - 1)) << 3);
    ApplyMatrixSV((GteMatrix *)&D_800E27A8->render_object.matrices[19], entry, &out);

    anim_s0->position.x = D_800E2360.x + out.x;
    anim_s0->position.y = D_800E2360.y + out.y;
    data = D_800E27A8;
    anim_s0->position.z = D_800E2360.z + out.z;

    model = data->render_object.matrices;
    anim_s0->transform = *(GteMatrixStorage *)&model[19];
    anim_s0->brightness = 0x7F;
    anim_s0->scale = 0x224;
}

void func_800CACD4(void) {
}

extern s16 D_800E0BE8[];

int func_800CACDC(void *arg0, void *arg1, FieldAnimMatrixGlow *anim) {
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
    scale.x = D_800E0BE8[index];
    scale.y = D_800E0BE8[index];
    scale.z = D_800E0BE8[index];
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

int func_800CB750(void *arg0, void *arg1, FieldAnimTwinGlow *anim) {
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
    func_800C42A4((FieldGlowSprite *)((u8 *)field_s4 - 10), &matrix, 1);

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
    func_800C42A4((FieldGlowSprite *)((u8 *)field_s4 - 10), &matrix, 1);
}

extern GteShortVector D_800C2204;

int func_800CB8E0(void *arg0, void *arg1, FieldAnimMovingParticle *anim) {
    s16 *position;
    u16 *field = &D_800F34E8.depth;
    GteMatrix matrix;
    GteShortVector rot;
    GteVector scaleCopy;
    GteVector scale;

    rot = D_800C2204;

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
    func_800C42A4((FieldGlowSprite *)((u8 *)field - 10), &matrix, 0);
}

int func_800CB9F8(void *arg0, void *arg1, FieldAnimMatrixGlow *anim) {
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
    func_800C42A4((FieldGlowSprite *)((u8 *)field - 10), &matrix, 0);
}

int func_800CBB24(void *arg0, FieldEngSlot *state) {
    int ret = 2;

    state->flag = ret;
    return ret;
}

int func_800CBB30(void *arg0, FieldEngSlot *state, FieldAnimMovingParticle *anim) {
    FieldEngSlot *state_a3 = state;
    FieldAnimMovingParticle *anim_a2 = anim;
    int temp_v0;
    int temp_v1;
    register int temp_a0 asm("$4");
    register int temp_a1 asm("$5");
    u8 count;

    temp_v0 = (u16)anim_a2->position.x;
    temp_v1 = (u16)anim_a2->velocity.x;
    temp_a0 = (u16)anim_a2->velocity.y;
    temp_a1 = (u16)anim_a2->velocity.z;

    temp_v0 += temp_v1;
    anim_a2->position.x = temp_v0;
    temp_v0 = (u16)anim_a2->position.y;
    temp_v1 = (u16)anim_a2->position.z;
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

int func_800CBBBC(void *arg0, FieldEngSlot *state, FieldAnimGlowPoint *anim) {
    s16 value = anim->brightness - 0x14;

    anim->brightness = value;
    if (value < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}

int func_800CBBF0(void *arg0, FieldEngSlot *state, FieldAnimGlowPoint *anim) {
    anim->brightness = anim->brightness - 8;
    anim->scale = (u16)anim->scale + 0x3C;
    if ((s16)anim->brightness < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}

int func_800CBC2C(void *arg0, FieldEngSlot *state, FieldAnimGlowPoint *anim) {
    anim->brightness = anim->brightness - 8;
    anim->scale = (u16)anim->scale + 0x78;
    if ((s16)anim->brightness < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}

int func_800CBC68(void *arg0, FieldEngSlot *state, FieldAnimGlowPoint *anim) {
    anim->brightness = anim->brightness - 8;
    anim->scale = (u16)anim->scale + 0xA;
    if ((s16)anim->brightness < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}
