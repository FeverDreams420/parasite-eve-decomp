#include "common.h"
#include "pe1/akao/commands.h"
#include "pe1/field_anim.h"
#include "pe1/game_audio_state.h"
#include "pe1/field_engine_state.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_sprite_state.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/gte_types.h"
#include "pe1/field_anim_particle.h"
#include "pe1/field_engine_slot.h"
#include "pe1/field_billboard.h"
#include "pe1/psyq_libc.h"
#include "pe1/battle.h"
#include "pe1/field_effect_modules.h"

static __inline__ void identity_matrix(GteMatrix *matrix)
{
    matrix->m[0][0] = matrix->m[1][1] = matrix->m[2][2] = 4096;
    matrix->m[0][1] = matrix->m[0][2] = matrix->m[1][0] =
        matrix->m[1][2] = matrix->m[2][0] = matrix->m[2][1] =
        matrix->t[0] = matrix->t[1] = matrix->t[2] = 0;
}

int func_800CCBA8(char *object)
{
    GteVector first, second, third;

    *FieldEng_GetSlot(object) = D_800E0EB8;
    first = D_800C220C;
    identity_matrix(&D_800F3478);
    ScaleMatrix(&D_800F3478, &first);
    second = D_800C221C;
    identity_matrix(&D_800F33C0);
    ScaleMatrix(&D_800F33C0, &second);
    third = D_800C222C;
    identity_matrix(&D_800F32B0);
    ScaleMatrix(&D_800F32B0, &third);

    D_800E2298.cell = 0x42;
    D_800E2298.clut = 0x20;
    D_800E2298.offset = 50;
    D_800E2298.depth = 127;
    D_800E2298.r = 128;
    D_800E2298.g = 128;
    D_800E2298.b = 128;

    D_800E2250.cell = 128;
    D_800E2250.clut = 4;
    D_800E2250.offset = -50;
    D_800E2250.depth = 127;
    D_800E2250.r = 128;
    D_800E2250.g = 128;
    D_800E2250.b = 128;

    D_800E27E0.cell = 0x68;
    D_800E27E0.clut = 0;
    D_800E27E0.offset = -99;
    D_800E27E0.depth = 127;
    D_800E27E0.r = 64;
    D_800E27E0.b = 64;
    D_800E27E0.g = 128;

    D_800F3460.cell = 0x6E;
    D_800F3460.clut = 3;
    D_800F3460.offset = -100;
    D_800F3460.depth = 127;
    D_800F3460.r = 128;
    D_800F3460.g = 128;
    D_800F3460.b = 128;
    return 0;
}

int BurstEffect_Spawn6(int a, int b, int c, int d, int e, int f) {
    FieldEng_Spawn6(a, b, c, d, e, f);
    return 0;
}

int BurstEffect_RegisterDrawList(void *o) {
    FieldEng_Register(o, g_BurstEffectDrawList);
    return 0;
}

int BurstEffect_RegisterPairedTables(void *obj) {
    int first;
    int second;
    int status;

    first = FieldEng_UpdateSlots(obj, g_BurstEffectUpdateList);
    second = FieldEng_RunScript(obj, g_BurstEffectInitList, g_BurstEffectSlotSizes);
    status = first | second;
    if (status == -1) {
        BurstEffect_CloseTarget(obj);
    }
    return 0;
}

int BurstEffect_CloseTarget(u8 *state) {
    state[0] = 4;
    return 0;
}

int BurstEffect_ClassNop6(void) {
    return 0;
}

int BurstEffect_ClassNop0(void) {
    return 0;
}

int func_800CCFA0(void) {
    int index;
    FieldActor **targets;
    FieldActor *target;
    FieldActor *next;
    FieldActor *data;

    index = *FieldEng_RegPtr(0xD);
    targets = (FieldActor **)*FieldEng_VarPtr(1);
    target = targets[index];
    index++;
    next = targets[index];
    g_BurstEffectTarget = target;

    if (next == 0) {
        *FieldEng_RegPtr(0xE) = 1;
    }

    *FieldEng_RegPtr(0xD) = index;

    data = g_BurstEffectTarget;
    D_800E27F8.x = data->render_object.target_x;
    D_800E27F8.y = data->render_object.target_y;
    D_800E27F8.z = data->render_object.target_z;

    if (D_800B0CE8.reset_pending != 0) {
        Akao_Cmd_24(g_FieldEffectSoundHeader, 0, 0x80, 0x7F);
    }

    func_800CEDA8(0);
}

int func_800CD07C(void *arg0, void *arg1, FieldAnimBurstData *anim) {
    u16 x = D_800E27F8.x;
    BattleEntity *entity = D_8009D254;
    int y;
    u16 z;

    anim->header.origin_x = x;
    y = entity->posY.parts.integer;
    anim->header.origin_y = y;
    z = D_800E27F8.z;
    anim->header.scale = 0x7F;
    anim->header.duration = 0x60C;
    anim->header.origin_z = z;
}

void FieldAnim_InitRandomOffsetPairs(void *arg0, void *arg1,
                                     FieldAnimBurstData *anim) {
    FieldAnimPointTriple *base;
    u16 *base_values;
    FieldAnimBurstWindow *window;
    u8 *entry;
    u16 z;
    int i;

    base = &D_800E27F8;
    i = 0;

    anim->header.origin_x = base->x;
    anim->header.origin_y = base->y;
    z = base->z;
    anim->header.scale = 0x7F;
    anim->header.duration = 0x3B4;
    anim->header.shade = 0;
    anim->header.origin_z = z;
    base_values = &base->x;

    do {
        entry = &anim->bytes[(i & 0xFFFF) * 8];
        window = (FieldAnimBurstWindow *)entry;
        window->point.x = base_values[0];
        window->point.y = base_values[1];
        window->point.z = base_values[2];
        window->offset.x = (rand() % 50) - 0x19;
        window->offset.y = (rand() % 50) - 0x19;
        i++;
        window->offset.z = 0;
    } while (((unsigned int)i & 0xFFFF) < 2);
}

int func_800CD1FC(void *arg0, void *arg1, FieldAnimBurstData *anim) {
    u16 z;

    anim->header.origin_x = (D_800E27F8.x + (rand() % 201)) - 0x64;
    anim->header.origin_y = (D_800E27F8.y + (rand() % 101)) - 0x32;
    z = D_800E27F8.z;
    anim->header.scale = 0x7F;
    anim->header.duration = 0x544;
    anim->header.origin_z = z;
}

/* Two empty field effect callbacks. */
void func_800CD2DC(void) {
}

void func_800CD2E4(void) {
}

int func_800CD2EC(void *arg0, void *arg1, FieldAnimGlowPoint *anim) {
    u16 *field = &D_800E2298.depth;
    GteMatrix matrix;
    GteShortVector rot;
    GteVector scaleCopy;
    GteVector scale;

    rot = g_BurstEffectSpriteRotation;

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
    func_800C42A4(&D_800E2298, &matrix, 0);
}
