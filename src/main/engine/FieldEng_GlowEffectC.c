#include "common.h"
#include "pe1/field_anim_pool.h"
#include "pe1/battle.h"
#include "pe1/field_engine_slot.h"
#include "pe1/field_anim_particle.h"
#include "pe1/field_sprite_state.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/gte_types.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/psyq_libc.h"

extern int D_800E0B38;

int func_800C9A70(char *object) {
    int half;
    register int value asm("$3");
    register void *slotData asm("$3");
    void **slot;

    slot = FieldEng_GetSlot(object);
    slotData = &D_800E0B38;
    *slot = slotData;

    value = 0xBD;
    D_800E22F8.cell = value;
    value = 9;
    half = 0x80;
    asm volatile("" : "=r"(half) : "0"(half));
    D_800E22F8.clut = value;
    value = 0x80;
    D_800E22F8.r = value;
    D_800E22F8.g = value;
    D_800E22F8.b = value;
    value = 0xAC;
    D_800F34B8.cell = value;
    value = 6;
    D_800F34B8.clut = value;
    value = -0x32;
    D_800F34B8.offset = value;
    value = 0x50;
    D_800E22F8.offset = 0;
    D_800E22F8.depth = half;
    D_800E22F8.flip = 0;
    D_800F34B8.depth = half;
    D_800F34B8.r = value;
    D_800F34B8.g = value;
    D_800F34B8.b = value;
    D_800F34B8.flip = 0;

    return 0;
}
extern void FieldEng_Spawn6(int a, int b, int c, int d, int e, int f);

int func_800C9B3C(int a, int b, int c, int d, int e, int f) {
    FieldEng_Spawn6(a, b, c, d, e, f);
    return 0;
}
extern char D_800E0A94[];
extern void FieldEng_Register(void *o, void *table);

int func_800C9B68(void *o) {
    FieldEng_Register(o, D_800E0A94);
    return 0;
}
extern char D_800E0AA4[];
extern char D_800E0A84[];
extern char D_800E0AB4[];
extern int func_800C251C(void *obj, void *table);
extern int func_800C2758(void *obj, void *table, void *extra);
extern int func_800C9C00(char *obj);

int func_800C9B90(void *obj) {
    int first;
    int second;
    int status;

    first = func_800C251C(obj, D_800E0AA4);
    second = func_800C2758(obj, D_800E0A84, D_800E0AB4);
    status = first | second;
    if (status == -1) {
        func_800C9C00(obj);
    }
    return 0;
}
int func_800C9C00(char *arg0) {
    *arg0 = 4;
    return 0;
}
int func_800C9C10(void) {
    return 0;
}
int func_800C9C18(void) {
    return 0;
}

extern FieldActor *D_800E27A4;

int func_800C9C20(FieldAnimObjectPrefix *obj) {
    FieldActor *data = obj->actor;

    D_800E2358.x = data->render_object.matrices[19].translation[0];
    D_800E2358.y = data->render_object.matrices[19].translation[1];
    D_800E2358.z = data->render_object.matrices[19].translation[2];
    D_800E27A4 = data;
    func_800CEDA8(0);
}

int rand(void);

int func_800C9C8C(void *arg0, void *arg1, FieldAnimMovingParticle *anim) {
    GteShortVector vec;
    RenderMatrix *model;

    vec.x = -(rand() % 3 + 9);
    vec.y = -(rand() % 3 + 9);
    vec.z = rand() % 5 - 2;

    model = D_800E27A4->render_object.matrices;
    ApplyMatrixSV((GteMatrix *)model, &vec, &anim->velocity);

    anim->position.x = D_800E2358.x;
    anim->position.y = D_800E2358.y;
    anim->position.z = D_800E2358.z;
    anim->lifetime = 0x14;
    anim->age = 0;
}

extern struct BattleEntity *D_8009D254;
extern char D_800E0AF8[];

int func_800C9D9C(void *arg0, void *arg1, FieldAnimMatrixGlow *anim) {
    FieldAnimMatrixGlow *anim_s0 = anim;
    GteShortVector out;
    FieldActor *data;
    char *entry;
    RenderMatrix *model;

    entry = D_800E0AF8 + (((short)((u16)((Combatant *)D_8009D254->core)->action->actionCode.actionId - 1)) << 3);
    ApplyMatrixSV((GteMatrix *)&D_800E27A4->render_object.matrices[19], entry, &out);

    anim_s0->position.x = D_800E2358.x + out.x;
    anim_s0->position.y = D_800E2358.y + out.y;
    data = D_800E27A4;
    anim_s0->position.z = D_800E2358.z + out.z;

    model = data->render_object.matrices;
    anim_s0->transform = *(GteMatrixStorage *)&model[19];
    anim_s0->brightness = 0x7F;
}

void func_800C9EA0(void) {
}

extern s16 D_800E0AD8[];

int func_800C9EA8(void *arg0, void *arg1, FieldAnimMatrixGlow *anim) {
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
    scale.x = D_800E0AD8[index];
    scale.y = D_800E0AD8[index];
    scale.z = D_800E0AD8[index];
    scaleCopy = scale;

    ScaleMatrix(&matrix, &scaleCopy);
    func_800C42A4(&D_800E22F8, &matrix, 1);
}

#define GLOW_LAYERS_FUNC func_800C9FD8
#define GLOW_LAYERS_SPIN_A D_800C21A4
#define GLOW_LAYERS_SPIN_B D_800C21AC
#define GLOW_LAYERS_SCALE D_800C21B4
#define GLOW_LAYERS_SPRITE D_800F34B8
#include "FieldEng_GlowLayers.inc"

int func_800CA4A8(void *arg0, FieldEngSlot *state) {
    int ret = 2;

    state->flag = ret;
    return ret;
}

int func_800CA4B4(void *arg0, FieldEngSlot *state, FieldAnimMovingParticle *anim)
{
  u16 coordinate;
  FieldEngSlot *state_a3 = state;
  FieldAnimMovingParticle *anim_a2 = anim;
  int temp_v0;
  int temp_v1;
  u16 positionY;
  register int temp_a0 asm("$4");
  u16 *velocityY;
  u16 velocityZ;
  u8 count;
  temp_v0 = anim_a2->position.x;
  coordinate = anim_a2->position.y;
  positionY = coordinate;
  velocityY = (u16 *)&anim_a2->velocity.y;
  temp_v1 = *((u16 *) (&anim_a2->velocity.x));
  coordinate = anim_a2->position.z;
  temp_a0 = *velocityY;
  temp_v0 += temp_v1;
  anim_a2->position.x = temp_v0;
  velocityZ = *((u16 *) (&anim_a2->velocity.z));
  temp_v0 = positionY;
  temp_v1 = coordinate;
  temp_v0 += temp_a0;
  temp_v1 += velocityZ;
  anim_a2->position.y = temp_v0;
  temp_v0 = *velocityY;
  temp_a0 = (unsigned int) anim_a2;
  anim_a2->position.z = temp_v1;
  temp_v1 = anim_a2->age;
  temp_v0 += 3;
  *((u16 *) (&anim_a2->velocity.y)) = temp_v0;
  temp_v0 = (s16) anim_a2->position.y;
  temp_v1++;
  anim_a2->age = temp_v1;
  if (temp_v0 > 0)
  {
    *((u16 *) (&anim_a2->velocity.y)) = -(*velocityY);
  }
  count = ((FieldAnimMovingParticle *) temp_a0)->lifetime;
  ((FieldAnimMovingParticle *) temp_a0)->lifetime = count - 1;
  if (count == 0)
  {
    state_a3->flag = 2;
  }
}

int func_800CA540(void *arg0, FieldEngSlot *state, FieldAnimGlowPoint *anim) {
    s16 value = anim->brightness - 0x14;

    anim->brightness = value;
    if (value < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}
