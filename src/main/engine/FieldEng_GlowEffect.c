#include "common.h"
#include "pe1/field_engine_slot.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/field_anim_particle.h"
#include "pe1/field_sprite_state.h"
#include "pe1/gte_types.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/psyq_libc.h"

extern int D_800E0928;

int func_800C7BA0(char *object) {
    int half_a2;
    u8 fullBrightness;
    int shade_a0;
    register int value asm("$3");
    register void *slotData asm("$3");
    void **slot;

    slot = FieldEng_GetSlot(object);
    slotData = &D_800E0928;
    *slot = slotData;

    value = 0xBD;
    D_800E22D8.cell = value;
    value = 9;
    half_a2 = 0x80;
    fullBrightness = 0x80;
    asm volatile("" : "=r"(half_a2), "=r"(fullBrightness) : "0"(half_a2), "1"(fullBrightness));
    D_800E22D8.clut = value;
    value = 0xAE;
    D_800F3498.cell = value;
    value = 7;
    D_800F3498.clut = value;
    value = -0x32;
    shade_a0 = 0x50;
    asm volatile("" : "=r"(shade_a0) : "0"(shade_a0));
    D_800F3498.offset = value;
    value = 0x68;
    D_800E2318.cell = value;
    value = -0x3C;
    D_800E2318.offset = value;
    value = 0x42;
    D_800F34D8.cell = value;
    value = 0x20;
    D_800F34D8.clut = value;
    value = 0x32;
    D_800E22D8.offset = 0;
    D_800E22D8.depth = half_a2;
    D_800E22D8.r = fullBrightness;
    D_800E22D8.g = fullBrightness;
    D_800E22D8.b = fullBrightness;
    D_800E22D8.flip = 0;
    D_800F3498.depth = half_a2;
    D_800F3498.r = shade_a0;
    D_800F3498.g = shade_a0;
    D_800F3498.b = shade_a0;
    D_800F3498.flip = 0;
    D_800E2318.clut = 0;
    D_800E2318.depth = half_a2;
    D_800E2318.r = shade_a0;
    D_800E2318.g = shade_a0;
    D_800E2318.b = shade_a0;
    D_800E2318.flip = 0;
    D_800F34D8.offset = value;
    D_800F34D8.depth = half_a2;
    D_800F34D8.r = fullBrightness;
    D_800F34D8.g = fullBrightness;
    D_800F34D8.b = fullBrightness;
    D_800F34D8.flip = 0;

    return 0;
}

extern void FieldEng_Spawn6(int a, int b, int c, int d, int e, int f);

int func_800C7D00(int a, int b, int c, int d, int e, int f) {
    FieldEng_Spawn6(a, b, c, d, e, f);
    return 0;
}
extern char D_800E0824[];
extern void FieldEng_Register(void *o, void *table);

int func_800C7D2C(void *o) {
    FieldEng_Register(o, D_800E0824);
    return 0;
}
extern char D_800E0840[];
extern char D_800E0808[];
extern char D_800E085C[];
extern int func_800C251C(void *obj, void *table);
extern int func_800C2758(void *obj, void *table, void *extra);
extern int func_800C7DC4(char *obj);

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

extern char *D_800E279C;

int func_800C7DE4(char *obj) {
    char *data = *(char **)(obj + 8);

    D_800E2348.x = *(int *)(*(char **)(data + 0x238) + 0x274);
    D_800E2348.y = *(int *)(*(char **)(data + 0x238) + 0x278);
    D_800E2348.z = *(int *)(*(char **)(data + 0x238) + 0x27C);
    D_800E279C = data;
    func_800CEDA8(0);
}

int rand(void);

int func_800C7E50(void *arg0, void *arg1, FieldAnimMovingParticle *anim) {
    GteShortVector vec;
    int (*model)[];

    vec.x = -(rand() % 3 + 9);
    vec.y = -(rand() % 3 + 9);
    vec.z = rand() % 5 - 2;

    model = *(int (**)[])(D_800E279C + 0x238);
    ApplyMatrixSV(model, &vec, &anim->velocity);

    anim->position.x = D_800E2348.x;
    anim->position.y = D_800E2348.y;
    anim->position.z = D_800E2348.z;
    anim->lifetime = 0x14;
    anim->age = 0;
}

extern char *D_8009D254;
extern char D_800E08A8[];

int func_800C7F60(void *arg0, void *arg1, FieldAnimMatrixGlow *anim) {
    FieldAnimMatrixGlow *anim_s0 = anim;
    GteShortVector out;
    char *entry;
    char *data;
    char *model;
    int index;

    index = *(u16 *)(*(char **)(*(char **)D_8009D254 + 0x68) + 6) - 1;
    index <<= 16;
    index >>= 13;
    entry = D_800E08A8 + index;
    ApplyMatrixSV(*(char **)(D_800E279C + 0x238) + 0x260, entry, &out);

    anim_s0->position.x = D_800E2348.x + out.x;
    anim_s0->position.y = D_800E2348.y + out.y;
    data = D_800E279C;
    anim_s0->position.z = D_800E2348.z + out.z;

    model = *(char **)(data + 0x238);
    anim_s0->transform = *(GteMatrixStorage *)(model + 0x260);
    anim_s0->brightness = 0x7F;
}

extern GteShortVector D_800C214C;

int func_800C8064(void *arg0, void *arg1, FieldAnimGlowPoint *anim) {
    GteShortVector in;
    GteShortVector out;
    char *model;

    in = D_800C214C;

    model = *(char **)(D_800E279C + 0x238);
    ApplyMatrixSV(model + 0x260, &in, &out);

    anim->position.x = D_800E2348.x + out.x;
    anim->position.y = D_800E2348.y + out.y;
    anim->position.z = D_800E2348.z + out.z;
    anim->brightness = 0x7F;
    anim->scale = 0;
}

int func_800C811C(void *arg0, void *arg1, FieldAnimGlowPoint *anim) {
    char *entity_v1 = D_8009D254;
    int value;
    int z_v1;

    anim->position.x = D_800E2348.x;
    value = *(s16 *)(entity_v1 + 0x2E);
    anim->position.y = value;
    z_v1 = D_800E2348.z;
    anim->brightness = 0x7F;
    anim->scale = 0x224;
    anim->position.z = z_v1;
}

extern char D_800E08E8[];

int func_800C815C(void *arg0, void *arg1, FieldAnimMatrixGlow *anim) {
    FieldAnimMatrixGlow *anim_s0 = anim;
    GteShortVector out;
    char *data;
    char *model;
    char *entry;
    int index;

    index = *(u16 *)(*(char **)(*(char **)D_8009D254 + 0x68) + 6) - 1;
    index <<= 16;
    index >>= 13;
    entry = D_800E08E8 + index;
    ApplyMatrixSV(*(char **)(D_800E279C + 0x238) + 0x260, entry, &out);

    anim_s0->position.x = D_800E2348.x + out.x;
    anim_s0->position.y = D_800E2348.y + out.y;
    data = D_800E279C;
    anim_s0->position.z = D_800E2348.z + out.z;

    model = *(char **)(data + 0x238);
    anim_s0->transform = *(GteMatrixStorage *)(model + 0x260);
    anim_s0->brightness = 0x7F;
    anim_s0->scale = 0x224;
}

void func_800C8268(void) {
}

extern s16 D_800E0888[];

int func_800C8270(void *arg0, void *arg1, u8 *anim) {
    GteMatrix matrix;
    GteShortVector rot;
    GteVector scaleCopy;
    GteVector scale;
    s16 index;

    index = *(u16 *)(*(char **)(*(char **)D_8009D254 + 0x68) + 6) - 1;

    rot.x = 0;
    rot.y = 0;
    rot.z = (s8)anim[1] << 6;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x10, 0x10);
    func_800C3238(0);

    RotMatrix(&rot, &matrix);

    matrix.t[0] = *(s16 *)(anim + 0x8);
    matrix.t[1] = *(s16 *)(anim + 0xA);
    matrix.t[2] = *(s16 *)(anim + 0xC);

    memset(&scale, 0, sizeof(scale));
    scale.x = D_800E0888[index];
    scale.y = D_800E0888[index];
    scale.z = D_800E0888[index];
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    func_800C42A4(&D_800E22D8, &matrix, 1);
}

#define FIELD_GLOW_LAYERS_FUNC func_800C83A0
#define FIELD_GLOW_LAYERS_SPIN_A D_800C2154
#define FIELD_GLOW_LAYERS_SPIN_B D_800C215C
#define FIELD_GLOW_LAYERS_SCALE D_800C2164
#define FIELD_GLOW_LAYERS_SPRITE D_800F3498
#include "FieldEng_GlowLayers.inc"

extern u16 D_800E2322;

int func_800C8870(void *arg0, void *arg1, u8 *anim) {
    GteMatrix matrix;
    int scale;
    u16 field;
    int localScale[4];
    u16 *field_s3 = &D_800E2322;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    scale = *(s16 *)(anim + 0x6) + 0x170;
    field = *(u16 *)(anim + 0x4);

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
    matrix.t[0] = *(s16 *)(anim + 0x8);
    matrix.t[1] = *(s16 *)(anim + 0xA);
    matrix.t[2] = *(s16 *)(anim + 0xC);

    memset(localScale, 0, sizeof(localScale));
    localScale[0] = scale;
    localScale[1] = scale;
    localScale[2] = scale;

    ScaleMatrix(&matrix, (const GteVector *)localScale);
    func_800C42A4((FieldGlowSprite *)((u8 *)field_s3 - 10), &matrix, 1);
}

extern GteShortVector D_800C2174;
extern u16 D_800F34E2;

int func_800C8970(void *arg0, void *arg1, u8 *anim) {
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

    *field = *(u16 *)(anim + 0x4);
    RotMatrix(&rot, &matrix);

    matrix.t[0] = *(s16 *)(anim + 0x8);
    matrix.t[1] = *(s16 *)(anim + 0xA);
    matrix.t[2] = *(s16 *)(anim + 0xC);

    memset(&scale, 0, sizeof(scale));
    scale.x = *(s16 *)(anim + 0x6);
    scale.y = *(s16 *)(anim + 0x6);
    scale.z = *(s16 *)(anim + 0x6);
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    func_800C42A4((FieldGlowSprite *)((u8 *)field - 10), &matrix, 0);
}

int func_800C8A88(void *arg0, void *arg1, u8 *anim) {
    u16 *field = &D_800F34E2;
    GteMatrix matrix;
    GteVector scaleCopy;
    GteVector scale;

    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    *field = *(u16 *)(anim + 0x4);
    matrix = *(GteMatrix *)(anim + 0x10);

    matrix.t[0] = *(s16 *)(anim + 0x8);
    matrix.t[1] = *(s16 *)(anim + 0xA);
    matrix.t[2] = *(s16 *)(anim + 0xC);

    memset(&scale, 0, sizeof(scale));
    scale.x = *(s16 *)(anim + 0x6);
    scale.y = *(s16 *)(anim + 0x6);
    scale.z = *(s16 *)(anim + 0x6);
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    func_800C42A4((FieldGlowSprite *)((u8 *)field - 10), &matrix, 0);
}

int func_800C8BB4(void *arg0, u8 *state) {
    int ret = 2;

    state[1] = ret;
    return ret;
}
int func_800C8BC0(void *arg0, u8 *state, u8 *anim) {
    u8 *state_a3 = state;
    u8 *anim_a2 = anim;
    int temp_v0;
    int temp_v1;
    register int temp_a0 asm("$4");
    register int temp_a1 asm("$5");
    u8 count;

    temp_v0 = *(u16 *)(anim_a2 + 0x8);
    temp_v1 = *(u16 *)(anim_a2 + 0x10);
    temp_a0 = *(u16 *)(anim_a2 + 0x12);
    temp_a1 = *(u16 *)(anim_a2 + 0x14);

    temp_v0 += temp_v1;
    *(u16 *)(anim_a2 + 0x8) = temp_v0;
    temp_v0 = *(u16 *)(anim_a2 + 0xA);
    temp_v1 = *(u16 *)(anim_a2 + 0xC);
    temp_v0 += temp_a0;
    temp_v1 += temp_a1;
    *(u16 *)(anim_a2 + 0xA) = temp_v0;
    temp_v0 = *(u16 *)(anim_a2 + 0x12);
    temp_a0 = (unsigned int)anim_a2;
    *(u16 *)(anim_a2 + 0xC) = temp_v1;
    temp_v1 = anim_a2[1];
    temp_v0 += 3;
    *(u16 *)(anim_a2 + 0x12) = temp_v0;
    temp_v0 = *(s16 *)(anim_a2 + 0xA);
    temp_v1++;
    anim_a2[1] = temp_v1;
    if (temp_v0 > 0) {
        *(u16 *)(anim_a2 + 0x12) = -*(u16 *)(anim_a2 + 0x12);
    }

    count = ((u8 *)temp_a0)[2];
    ((u8 *)temp_a0)[2] = count - 1;
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
