#include "common.h"
#include "pe1/field_anim_particle.h"
#include "pe1/field_anim.h"
#include "pe1/field_engine_state.h"
#include "pe1/field_engine_slot.h"
#include "pe1/battle.h"
#include "pe1/field_sprite_state.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/gte_types.h"
#include "pe1/battle_runtime.h"
#include "pe1/random.h"
#include "pe1/akao/commands.h"
#include "pe1/game_audio_state.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_actor.h"
#include "pe1/fixed_point.h"
#include "pe1/field_effect_modules.h"
#include "pe1/field_billboard.h"
#include "pe1/psyq_libc.h"

int PointSprite_PlantScript(char *object) {
    FieldActor *actor;
    void **slot;

    slot = FieldFx_GetSlot(object);
    *slot = g_PointSpriteScript;
    actor = ((FieldAnimObjectPrefix *)object)->actor;

    D_800E2290.x = actor->pos_x >> 16;
    D_800E2290.y = actor->pos_y >> 16;
    D_800E2290.z = actor->pos_z >> 16;
    g_PointSpriteOwner = actor;

    D_800F3430.oriented.rotation.x = 0;
    D_800F3430.oriented.rotation.y = 0;
    D_800F3430.oriented.rotation.z = 0;
    D_800F3430.oriented.cell = 0x68;
    D_800F3430.oriented.clut = 0;
    D_800F3430.oriented.depth = -100;

    D_800E2260.oriented.rotation.x = 0x400;
    D_800E2260.oriented.rotation.y = 0;
    D_800E2260.oriented.rotation.z = 0;
    D_800E2260.oriented.cell = 0x68;
    D_800E2260.oriented.clut = 0;
    D_800E2260.oriented.depth = 0;

    D_800F32E0.oriented.rotation.x = 0x400;
    D_800F32E0.oriented.rotation.y = 0;
    D_800F32E0.oriented.rotation.z = 0;
    D_800F32E0.oriented.cell = 0x68;
    D_800F32E0.oriented.clut = 0;
    D_800F32E0.oriented.depth = -100;
    D_800F32E0.oriented.rgb[0] = 0xA0;
    D_800F32E0.oriented.rgb[1] = 0xA0;
    D_800F32E0.oriented.rgb[2] = 0xA0;

    D_800F3380.oriented.rotation.x = 0x400;
    D_800F3380.oriented.rotation.y = 0;
    D_800F3380.oriented.rotation.z = 0;
    D_800F3380.oriented.cell = 0x46;
    D_800F3380.oriented.clut = 0x30;
    D_800F3380.oriented.depth = -0x50;
    D_800F3380.oriented.rgb[0] = 0x80;
    D_800F3380.oriented.rgb[1] = 0x80;
    D_800F3380.oriented.rgb[2] = 0x80;

    D_800E2818.oriented.scale.x = 0x1A8;
    D_800E2818.oriented.scale.y = 0x1A8;
    D_800E2818.oriented.scale.z = 0x1A8;
    D_800E2818.oriented.rotation.x = 0;
    D_800E2818.oriented.rotation.y = 0;
    D_800E2818.oriented.rotation.z = 0;
    D_800E2818.oriented.cell = 0xBC;
    D_800E2818.oriented.clut = 8;
    D_800E2818.oriented.depth = 0;
    D_800E2818.oriented.rgb[0] = 0x80;
    D_800E2818.oriented.rgb[1] = 0x80;
    D_800E2818.oriented.rgb[2] = 0x80;

    D_800F3338.oriented.rotation.x = 0x400;
    D_800F3338.oriented.rotation.y = 0;
    D_800F3338.oriented.rotation.z = 0;
    D_800F3338.oriented.cell = 0x42;
    D_800F3338.oriented.clut = 0x20;
    D_800F3338.oriented.depth = 0x32;
    D_800F3338.oriented.rgb[0] = 0x80;
    D_800F3338.oriented.rgb[1] = 0x80;
    D_800F3338.oriented.rgb[2] = 0x80;

    return 0;
}

int PointSprite_Spawn6(int a, int b, int c, int d, int e, int f) {
    FieldFx_Spawn6(a, b, c, d, e, f);
    return 0;
}

int PointSprite_RegisterDrawList(void *o) {
    FieldFx_Register(o, g_PointSpriteDrawList);
    return 0;
}

int PointSprite_RegisterPairedTables(void *obj) {
    int first;
    int second;
    int status;

    first = FieldFx_UpdateSlots(obj, g_PointSpriteUpdateList);
    second = FieldFx_RunScript(obj, g_PointSpriteInitList, g_PointSpriteSlotSizes);
    status = first | second;
    if (status == -1) {
        PointSprite_CloseTarget(obj);
    }
    return 0;
}

int PointSprite_CloseTarget(char *arg0) {
    *arg0 = 4;
    return 0;
}

int PointSprite_ClassNop6(void) {
    return 0;
}

int PointSprite_ClassNop0(void) {
    return 0;
}

int func_800CBFC4(void *arg0, void *arg1, FieldAnimPointSprite *anim) {
    int x_base_a1;
    int z_base_a1;
    int x_rand;
    int z_rand;

    D_800F3430.oriented.rgb[0] = *FieldFx_RegPtr(0);
    D_800F3430.oriented.rgb[1] = *FieldFx_RegPtr(1);
    D_800F3430.oriented.rgb[2] = *FieldFx_RegPtr(2);

    x_rand = rand();
    x_base_a1 = D_800E2290.x - 0x64;
    anim->point.x = x_base_a1 + (x_rand % 201);
    anim->point.y = D_800E2290.y;

    z_rand = rand();
    z_base_a1 = D_800E2290.z - 0x64;
    anim->extent = 0x20C;
    anim->scale = 0x7F;
    anim->point.z = z_base_a1 + (z_rand % 201);
}

void FieldFx_InitRadialEntries(void *arg0, void *arg1, FieldAnimRadialParticles *anim) {
    int i;
    int angle;
    int lift;

    D_800E2260.oriented.rgb[0] = *FieldFx_RegPtr(3);
    D_800E2260.oriented.rgb[1] = *FieldFx_RegPtr(4);
    D_800E2260.oriented.rgb[2] = *FieldFx_RegPtr(5);

    anim->scale = 0x7F;
    if (((Combatant *)D_8009D254->core)->action->actionCode.actionId == 3) {
        anim->count = 8;
    } else {
        anim->count = 0x10;
    }

    for (i = 0; i < anim->count; i++) {
        anim->points[i].position.x = D_800E2290.x;
        anim->points[i].position.y = D_800E2290.y;
        anim->points[i].position.z = D_800E2290.z;

        if (anim->count == 0x10) {
            angle = i << 8;
        } else {
            angle = i << 9;
        }

        lift = -0x1400;
        anim->velocity[i].x = rsin(angle);
        anim->velocity[i].y = lift;
        anim->velocity[i].z = rcos(angle);
        anim->extent[i] = 0x258;
    }
}

int func_800CC244(void *arg0, void *arg1, FieldAnimPointSprite *anim) {
    u16 z;

    anim->point.x = D_800E2290.x;
    anim->point.y = D_800E2290.y;
    z = D_800E2290.z;
    anim->extent = 0xBE8;
    anim->scale = 0x7F;
    anim->point.z = z;
}

int func_800CC284(void *arg0, void *arg1, FieldAnimPointSprite *anim) {
    u16 z;

    anim->point.x = D_800E2290.x;
    anim->point.y = D_800E2290.y - 0x64;
    z = D_800E2290.z;
    anim->extent = 0;
    anim->scale = 0x7F;
    anim->point.z = z;
}

void func_800CC2C4(void *arg0, void *arg1, FieldAnimScatteredParticles *state)
{
    int i;

    state->points.scale = 127;
    if (((Combatant *)D_8009D254->core)->action->actionCode.actionId == 3)
        state->points.count = 4;
    else
        state->points.count = 16;

    for (i = 0; i < state->points.count; i++) {
        state->points.x[i] = D_800E2290.x;
        state->points.y[i] = D_800E2290.y;
        state->points.z[i] = D_800E2290.z;
        state->velocity_x[i] = (rand() % 11 - 5) * 256;
        state->velocity_y[i] = -(rand() % 16 + 20) * 256;
        state->velocity_z[i] = (rand() % 11 - 5) * 256;
    }
}

/* Point sprite setup, draw and update callbacks, including radial and
 * scattered particle arrays. Shared payloads are declared in field_anim.h. */

int func_800CC440(void *arg0, void *arg1, FieldAnimPointSprite *anim) {
    u16 z;

    anim->point.x = D_800E2290.x;
    anim->point.y = D_800E2290.y;
    z = D_800E2290.z;
    anim->extent = 0x224;
    anim->scale = 0x7F;
    anim->point.z = z;
}

int func_800CC480(void *arg0, FieldEngSlot *params, FieldAnimPointSprite *anim) {
    short *base_a0;
    FieldBillboard *output;
    FieldAnimPointSprite *pointData = anim;
    u32 value_v0;
    int value_v1;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    value_v1 = params->counter;
    value_v0 = 0x80;
    value_v1 <<= 1;
    value_v0 -= value_v1;
    value_v1 = value_v0;
    value_v0 <<= 16;
    if (value_v0 > 0x7FFFFFFFU) {
        value_v1 = 0;
    }

    base_a0 = (short *)&D_800F3430.oriented.brightness;
    base_a0[0] = value_v1;
    value_v0 = pointData->point.x;
    output = (FieldBillboard *)((u8 *)base_a0 -
        PE1_OFFSETOF(FieldBillboard, brightness));
    output->position.x = value_v0;
    D_800F3430.oriented.position.y = pointData->point.y;
    D_800F3430.oriented.position.z = pointData->point.z;
    D_800F3430.oriented.scale.x = pointData->extent;
    D_800F3430.oriented.scale.y = pointData->extent * 2;
    D_800F3430.oriented.scale.z = pointData->extent;
    func_800C3B04(output);
}

void FieldFx_ProcessInterleavedPoints(void *arg0, void *arg1, FieldAnimRadialParticles *anim) {
    unsigned int i;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    D_800E2260.oriented.scale.x = (s16)anim->extent[0];
    D_800E2260.oriented.scale.y = (s16)anim->extent[0];
    D_800E2260.oriented.scale.z = (s16)anim->extent[0];
    D_800E2260.billboard.brightness = anim->scale;

    for (i = 0; i < anim->count; i++) {
        D_800E2260.billboard.position.x = anim->points[i].position.x;
        D_800E2260.billboard.position.y = anim->points[i].position.y;
        D_800E2260.billboard.position.z = anim->points[i].position.z;
        func_800C3B04(&D_800E2260.billboard);
    }
}

int func_800CC644(void *arg0, void *arg1, FieldAnimPointSprite *anim) {
    u16 value_v0;
    FieldOrientedSprite *output;
    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(3);

    value_v0 = anim->point.x;
    output = &D_800F32E0.oriented;
    output->position.x = value_v0;
    D_800F32E0.oriented.position.y = anim->point.y;
    D_800F32E0.oriented.position.z = anim->point.z;
    D_800F32E0.oriented.scale.x = anim->extent;
    D_800F32E0.oriented.scale.y = anim->extent;
    D_800F32E0.oriented.scale.z = anim->extent;
    D_800F32E0.oriented.brightness = (signed char)anim->scale;
    func_800C3324(output);
}

int func_800CC6F8(void *arg0, void *arg1, FieldAnimPointSprite *anim) {
    u16 value_v0;
    FieldBillboard *output;
    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    value_v0 = anim->point.x;
    output = &D_800F3380.billboard;
    output->position.x = value_v0;
    D_800F3380.oriented.position.y = anim->point.y;
    D_800F3380.oriented.position.z = anim->point.z;
    D_800F3380.oriented.scale.x = anim->extent;
    D_800F3380.oriented.scale.y = anim->extent;
    D_800F3380.oriented.scale.z = anim->extent;
    D_800F3380.oriented.brightness = (signed char)anim->scale;
    func_800C3B04(output);
}

void FieldFx_ProcessPointTriples(void *arg0, void *arg1,
                                   FieldAnimPointData *points) {
    unsigned int i;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x10, 0x10);
    func_800C3238(0);

    D_800E2818.billboard.brightness = (points->scale << 24) >> 23;

    for (i = 0; i < points->count; i++) {
        D_800E2818.billboard.position.x = points->x[i];
        D_800E2818.billboard.position.y = points->y[i];
        D_800E2818.billboard.position.z = points->z[i];
        func_800C3B04(&D_800E2818.billboard);
    }
}

int func_800CC878(void *arg0, void *arg1, FieldAnimPointSprite *anim) {
    u16 value_v0;
    FieldOrientedSprite *output;
    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    value_v0 = anim->point.x;
    output = &D_800F3338.oriented;
    output->position.x = value_v0;
    D_800F3338.oriented.position.y = anim->point.y;
    D_800F3338.oriented.position.z = anim->point.z;
    D_800F3338.oriented.scale.x = anim->extent;
    D_800F3338.oriented.scale.y = anim->extent;
    D_800F3338.oriented.scale.z = anim->extent;
    D_800F3338.oriented.brightness = (signed char)anim->scale;
    func_800C3324(output);
}

void func_800CC92C(void *arg0, FieldEngSlot *state, FieldAnimPointSprite *anim) {
    anim->point.x += 0; /* no horizontal drift, but the record is rewritten */
    anim->point.y -= 10;
    anim->scale -= 2;
    anim->extent += 0x1E;
    if ((s8)anim->scale < 0x1E) {
        state->flag = 2;
    }
}

void func_800CC974(void *arg0, FieldEngSlot *state, FieldAnimRadialParticles *anim) {
    int i;

    for (i = 0; i < anim->count; i++) {
        anim->points[i].position.x += anim->velocity[i].x >> 9;
        anim->points[i].position.y += anim->velocity[i].y >> 9;
        anim->points[i].position.z += anim->velocity[i].z >> 9;
        anim->velocity[i].y += 0xB4;
        anim->extent[i] += 0x18;
    }

    anim->scale -= 2;
    if (anim->scale < 2) {
        state->flag = 2;
    }
}

int func_800CCA40(void *arg0, FieldEngSlot *state, FieldAnimPointSprite *anim) {
    anim->scale -= 2;
    anim->extent = (u16)anim->extent + 0x28;
    if ((s8)anim->scale < 0x1E) {
        state->flag = 2;
    }
}

int func_800CCA78(void *arg0, FieldEngSlot *state, FieldAnimPointSprite *anim) {
    anim->scale -= 6;
    anim->extent = (u16)anim->extent + 0xB4;
    if ((s8)anim->scale < 0x1E) {
        state->flag = 2;
    }
}

void func_800CCAB0(void *arg0, FieldEngSlot *state, FieldAnimScatteredParticles *anim) {
    int i;

    for (i = 0; i < anim->points.count; i++) {
        anim->points.x[i] += anim->velocity_x[i] >> 8;
        anim->points.y[i] += anim->velocity_y[i] >> 8;
        anim->points.z[i] += anim->velocity_z[i] >> 8;
        anim->velocity_y[i] += 0xB4;
    }

    anim->points.scale -= 2;
    if ((s8)anim->points.scale < 2) {
        state->flag = 2;
    }
}

int func_800CCB6C(void *arg0, FieldEngSlot *state, FieldAnimPointSprite *anim) {
    anim->scale -= 8;
    anim->extent = (u16)anim->extent + 0x1A4;
    if ((s8)anim->scale < 0x14) {
        anim->scale = 0;
        state->flag = 2;
    }
}

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

    *FieldFx_GetSlot(object) = D_800E0EB8;
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
    FieldFx_Spawn6(a, b, c, d, e, f);
    return 0;
}

int BurstEffect_RegisterDrawList(void *o) {
    FieldFx_Register(o, g_BurstEffectDrawList);
    return 0;
}

int BurstEffect_RegisterPairedTables(void *obj) {
    int first;
    int second;
    int status;

    first = FieldFx_UpdateSlots(obj, g_BurstEffectUpdateList);
    second = FieldFx_RunScript(obj, g_BurstEffectInitList, g_BurstEffectSlotSizes);
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

    index = *FieldFx_RegPtr(0xD);
    targets = (FieldActor **)*FieldFx_VarPtr(1);
    target = targets[index];
    index++;
    next = targets[index];
    g_BurstEffectTarget = target;

    if (next == 0) {
        *FieldFx_RegPtr(0xE) = 1;
    }

    *FieldFx_RegPtr(0xD) = index;

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

void FieldFx_InitRandomOffsetPairs(void *arg0, void *arg1,
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

int func_800CD404(void *arg0, void *arg1, FieldAnimBurstData *anim) {
    FieldAnimBurstWindow *window;
    u16 i;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    D_800F33C0.t[0] = anim->header.origin_x;
    D_800F33C0.t[1] = anim->header.origin_y;
    D_800F33C0.t[2] = anim->header.origin_z;
    D_800E2250.cell = (anim->header.shade * 2) - 0x80;
    func_800C42A4(&D_800E2250, &D_800F33C0, 1);

    func_800C3238(3);

    for (i = 0; i < 2; i++) {
        window = (FieldAnimBurstWindow *)&anim->bytes[i * 8];
        D_800F32B0.t[0] = (s16)window->point.x;
        D_800F32B0.t[1] = (s16)window->point.y;
        D_800F32B0.t[2] = (s16)window->point.z;
        func_800C42A4(&D_800F3460, &D_800F32B0, 1);
    }
}

/* Burst and spark effect setup, draw and lifecycle callbacks. */

int func_800CD50C(void *arg0, void *arg1, FieldAnimGlowPoint *anim) {
    int *base_a1;
    u16 *field_v1;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(3);

    base_a1 = &D_800F3478.t[0];
    field_v1 = &D_800E27E0.depth;
    base_a1[0] = (s16)anim->position.x;
    D_800F3478.t[1] = (s16)anim->position.y;
    D_800F3478.t[2] = (s16)anim->position.z;
    *field_v1 = anim->brightness;
    func_800C42A4(&D_800E27E0, &D_800F3478, 1);
}

void func_800CD59C(void) {
}

int func_800CD5A4(void *arg0, FieldEngSlot *state) {
    int ret = 2;
    state->flag = ret;
    return ret;
}

int func_800CD5B0(void *arg0, FieldEngSlot *state, FieldAnimGlowPoint *anim) {
    anim->brightness = anim->brightness - 0xA;
    anim->scale += 0x3C;
    if ((s16)anim->brightness < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}

int func_800CD5EC(void *arg0, FieldEngSlot *state, FieldAnimTwoPointMotion *anim) {
    unsigned int i = 0;

    do {
        unsigned slot = i & 0xFFFF;

        i++;
        anim->position[slot].x = anim->position[slot].x + anim->velocity[slot].x;
        anim->position[slot].y = anim->position[slot].y + anim->velocity[slot].y;
        anim->position[slot].z = anim->position[slot].z + anim->velocity[slot].z;
    } while ((i & 0xFFFF) < 2);

    anim->age++;
    if ((signed char)anim->age >= 8) {
        state->flag = 2;
    }
}

int func_800CD678(void *arg0, FieldEngSlot *state, FieldAnimGlowPoint *anim) {
    int value = rand() % 11;
    int jitter;
    FieldAnimGlowPoint *output;
    u16 positionY = anim->position.y;
    int brightness = anim->brightness;
    int positionX = anim->position.x;
    positionY -= 8;
    brightness -= 8;
    output = anim;
    positionX -= 5;
    output->position.y = positionY;
    output->brightness = brightness;
    jitter = value;
    value = 8;
    positionX += jitter;
    anim->position.x = positionX;
    if (value > (short)brightness) {
        state->flag = 2;
    }
}

int func_800CD71C(void *arg0, FieldEngSlot *state) {
    int ret = 2;
    state->flag = ret;
    return ret;
}

int BurstEffectB_PlantScript(char *object) {
    void **slot = FieldFx_GetSlot(object);

    *slot = &g_BurstEffectBScript;

    D_800F33E8.oriented.scale.x = 0x5F4;
    D_800F33E8.oriented.scale.y = 0x5F4;
    D_800F33E8.oriented.scale.z = 0x5F4;
    D_800F33E8.oriented.rotation.x = 0;
    D_800F33E8.oriented.rotation.y = 0;
    D_800F33E8.oriented.rotation.z = 0;
    D_800F33E8.oriented.cell = 0x40;
    D_800F33E8.oriented.clut = 0x20;
    D_800F33E8.oriented.depth = -0x64;
    D_800F33E8.oriented.rgb[0] = 0x15;
    D_800F33E8.oriented.rgb[1] = 0x20;
    D_800F33E8.oriented.rgb[2] = 0x20;

    D_800E27B0.oriented.rotation.x = 0;
    D_800E27B0.oriented.rotation.y = 0;
    D_800E27B0.oriented.rotation.z = 0;
    D_800E27B0.oriented.cell = 0x46;
    D_800E27B0.oriented.clut = 0x30;
    D_800E27B0.oriented.depth = -0x6E;
    D_800E27B0.oriented.rgb[0] = 0xFF;
    D_800E27B0.oriented.rgb[1] = 0xB0;
    D_800E27B0.oriented.rgb[2] = 0xB0;

    D_800E2770.oriented.scale.x = 0xA4;
    D_800E2770.oriented.scale.y = 0xA4;
    D_800E2770.oriented.scale.z = 0x108;
    D_800E2770.oriented.rotation.x = 0;
    D_800E2770.oriented.rotation.y = 0;
    D_800E2770.oriented.rotation.z = 0;
    D_800E2770.oriented.cell = 0x6E;
    D_800E2770.oriented.clut = 3;
    D_800E2770.oriented.depth = -0x96;
    D_800E2770.oriented.rgb[0] = 0x80;
    D_800E2770.oriented.rgb[1] = 0x80;
    D_800E2770.oriented.rgb[2] = 0x80;

    return 0;
}

int BurstEffectB_Spawn6(int a, int b, int c, int d, int e, int f) {
    FieldFx_Spawn6(a, b, c, d, e, f);
    return 0;
}

int BurstEffectB_RegisterDrawList(void *o) {
    FieldFx_Register(o, g_BurstEffectBDrawList);
    return 0;
}

int BurstEffectB_RegisterPairedTables(void *obj) {
    int first;
    int second;
    int status;

    first = FieldFx_UpdateSlots(obj, g_BurstEffectBUpdateList);
    second = FieldFx_RunScript(obj, g_BurstEffectBInitList, g_BurstEffectBSlotSizes);
    status = first | second;
    if (status == -1) {
        BurstEffectB_CloseTarget(obj);
    }
    return 0;
}

int BurstEffectB_CloseTarget(u8 *state) {
    state[0] = 4;
    return 0;
}

int BurstEffectB_ClassNop6(void) {
    return 0;
}

int BurstEffectB_ClassNop0(void) {
    return 0;
}

int func_800CD980(void) {
    int index;
    FieldActor **targets;
    FieldActor *target;
    FieldActor *next;
    FieldActor *data;

    index = *FieldFx_RegPtr(0xD);
    targets = (FieldActor **)*FieldFx_VarPtr(1);
    target = targets[index];
    index++;
    next = targets[index];
    g_BurstEffectBTarget = target;

    if (next == 0) {
        *FieldFx_RegPtr(0xE) = 1;
    }

    *FieldFx_RegPtr(0xD) = index;

    data = g_BurstEffectBTarget;
    D_800E27F0.x = data->render_object.target_x;
    D_800E27F0.y = data->render_object.target_y;
    D_800E27F0.z = data->render_object.target_z;

    if (D_800B0CE8.reset_pending != 0) {
        Akao_Cmd_24(g_FieldEffectSoundHeader, 0, 0x80, 0x7F);
    }

    func_800CEDA8(0);
}

int func_800CDA5C(void *arg0, void *arg1, FieldAnimSparkCloud *anim) {
    u16 *base = &D_800E27F0.x;
    u16 *base_s4;
    int i = 0;
    int value;

    anim->center.position.x = base[0];
    anim->center.position.y = D_800E27F0.y;
    anim->center.position.z = D_800E27F0.z;
    anim->center.displacement.x = (rand() % 201) - 0x64;
    anim->center.displacement.y = (rand() % 101) - 0x32;

    base_s4 = base;
    anim->center.brightness = 0x7F;
    anim->center.displacement.z = 0;
    anim->center.scaleStep = 0;

    do {
        unsigned int slot = i & 0xFFFF;
        anim->position[0][slot] = base_s4[0];
        anim->position[1][slot] = base_s4[1];
        anim->position[2][slot] = base_s4[2];
        anim->velocity[0][slot] = (rand() % 50) - 0x19;
        value = (rand() % 50) - 0x19;
        i++;
        anim->velocity[2][slot] = 0;
        anim->velocity[1][slot] = value;
    } while ((unsigned int)(i & 0xFFFF) < 8U);
}

int func_800CDC24(void *arg0, void *arg1, FieldAnimSparkPoint *anim) {
    int value;

    anim->position.x = D_800E27F0.x;
    anim->position.y = D_800E27F0.y;
    anim->position.z = D_800E27F0.z;
    anim->brightness = 0x7F;
    anim->displacement.x = (rand() % 201) - 0x64;
    value = rand();
    anim->displacement.z = 0;
    anim->displacement.y = (value % 101) - 0x32;
}

void func_800CDD04(void) {
}

int func_800CDD0C(void *arg0, void *arg1, FieldAnimSparkCloud *anim) {
    u16 lhs_v0;
    FieldBillboard *output;
    u16 rhs_v1;
    unsigned int i;

    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    lhs_v0 = anim->center.position.x;
    rhs_v1 = anim->center.displacement.x;
    output = &D_800E27B0.billboard;
    lhs_v0 += rhs_v1;
    output->position.x = lhs_v0;
    D_800E27B0.oriented.position.y = anim->center.position.y + anim->center.displacement.y;
    D_800E27B0.oriented.position.z = anim->center.position.z + anim->center.displacement.z;
    D_800E27B0.oriented.scale.x = ((s8)anim->center.scaleStep * 8) + 0x20C;
    D_800E27B0.oriented.scale.y = ((s8)anim->center.scaleStep * 8) + 0x20C;
    D_800E27B0.oriented.scale.z = ((s8)anim->center.scaleStep * 8) + 0x20C;
    D_800E27B0.oriented.brightness = (s8)anim->center.brightness;

    func_800C3B04(output);
    func_800C3098(0x10);

    i = 0;
    do {
        unsigned int slot = i & 0xFFFF;
        D_800E2770.billboard.position.x = anim->position[0][slot] + anim->center.displacement.x;
        D_800E2770.billboard.position.y = anim->position[1][slot] + anim->center.displacement.y;
        D_800E2770.billboard.position.z = anim->position[2][slot] + anim->center.displacement.z;
        i++;
        D_800E2770.billboard.brightness = (s8)anim->center.brightness;
        func_800C3B04(&D_800E2770.billboard);
    } while ((i & 0xFFFF) < 8);
}

int func_800CDE90(void *arg0, void *arg1, FieldAnimSparkPoint *anim) {
    u16 lhs_v0;
    FieldBillboard *output;
    u16 rhs_v1;

    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(3);

    lhs_v0 = anim->position.x;
    rhs_v1 = anim->displacement.x;
    output = &D_800F33E8.billboard;
    lhs_v0 += rhs_v1;
    output->position.x = lhs_v0;
    D_800F33E8.oriented.position.y = anim->position.y + anim->displacement.y;
    D_800F33E8.oriented.position.z = anim->position.z + anim->displacement.z;
    D_800F33E8.oriented.brightness = (s8)anim->brightness;
    func_800C3B04(output);
}

int func_800CDF40(void *arg0, FieldEngSlot *state) {
    int ret = 2;
    state->flag = ret;
    return ret;
}

int func_800CDF4C(void *arg0, FieldEngSlot *state, FieldAnimSparkCloud *anim) {
    unsigned int i = 0;

    anim->center.brightness -= 0x10;
    anim->center.scaleStep += 0xC;

    do {
        unsigned int slot = i & 0xFFFF;

        i++;
        anim->position[0][slot] = anim->position[0][slot] + anim->velocity[0][slot];
        anim->position[1][slot] = anim->position[1][slot] + anim->velocity[1][slot];
        anim->position[2][slot] = anim->position[2][slot] + anim->velocity[2][slot];
    } while ((i & 0xFFFF) < 8);

    if ((signed char)anim->center.brightness < 0x10) {
        state->flag = 2;
    }
}

int func_800CDFE0(void *arg0, FieldEngSlot *state, FieldAnimSparkPoint *anim) {
    int value = rand() % 11;
    int jitter;
    FieldAnimSparkPoint *output;
    u16 positionY = anim->position.y;
    int brightness = anim->brightness;
    int positionX = anim->position.x;
    positionY -= 4;
    brightness -= 3;
    output = anim;
    positionX -= 5;
    output->position.y = positionY;
    output->brightness = brightness;
    jitter = value;
    value = 3;
    positionX += jitter;
    anim->position.x = positionX;
    if (value > (signed char)brightness) {
        state->flag = 2;
    }
}

int BurstEffectC_PlantScript(char *object) {
    void **slot = FieldFx_GetSlot(object);

    *slot = &g_BurstEffectCScript;
    D_800E22A8.oriented.scale.x = 0x300;
    D_800E22A8.oriented.scale.y = 0x300;
    D_800E22A8.oriented.scale.z = 0x300;
    D_800E22A8.oriented.clut = 5;
    D_800E22A8.oriented.depth = -100;
    D_800E22A8.oriented.rotation.x = 0;
    D_800E22A8.oriented.rotation.y = 0;
    D_800E22A8.oriented.rotation.z = 0;
    D_800E22A8.oriented.rgb[0] = 0x80;
    D_800E22A8.oriented.rgb[1] = 0x80;
    D_800E22A8.oriented.rgb[2] = 0x80;
    return 0;
}

int BurstEffectC_Spawn6(int a, int b, int c, int d, int e, int f) {
    FieldFx_Spawn6(a, b, c, d, e, f);
    return 0;
}

int BurstEffectC_RegisterDrawList(void *o) {
    FieldFx_Register(o, g_BurstEffectCDrawList);
    return 0;
}

int BurstEffectC_RegisterPairedTables(void *obj) {
    int first;
    int second;
    int status;

    first = FieldFx_UpdateSlots(obj, g_BurstEffectCUpdateList);
    second = FieldFx_RunScript(obj, g_BurstEffectCInitList, g_BurstEffectCSlotSizes);
    status = first | second;
    if (status == -1) {
        BurstEffectC_CloseTarget(obj);
    }
    return 0;
}

int BurstEffectC_CloseTarget(char *arg0) {
    *arg0 = 4;
    return 0;
}

int BurstEffectC_ClassNop6(void) {
    return 0;
}

int BurstEffectC_ClassNop0(void) {
    return 0;
}

int func_800CE1FC(void) {
    int index;
    FieldActor **targets;
    FieldActor *target;
    FieldActor *next;
    FieldActor *data;

    index = *FieldFx_RegPtr(0xD);
    targets = (FieldActor **)*FieldFx_VarPtr(1);
    target = targets[index];
    index++;
    next = targets[index];
    g_BurstEffectCTarget = target;

    if (next == 0) {
        *FieldFx_RegPtr(0xE) = 1;
    }

    *FieldFx_RegPtr(0xD) = index;

    data = g_BurstEffectCTarget;
    D_800E2808.x = data->render_object.target_x;
    D_800E2808.y = data->render_object.target_y;
    D_800E2808.z = data->render_object.target_z;

    func_800CEDA8(0);
}

int func_800CE2B4(void *arg0, void *arg1, FieldAnimTimedSpark *anim) {
    int randomX;
    int randomY;
    u16 base;
    int randomZ;

    randomX = rand();
    base = D_800E2808.x - 0x28;
    anim->position.x = base + (randomX % 80);

    randomY = rand();
    base = D_800E2808.y - 0x28;
    anim->position.y = base + (randomY % 80);

    randomZ = rand();
    base = D_800E2808.z;

    anim->brightness = 0x7F;
    anim->reserved04 = 0;
    anim->age = 0;
    base -= 0x28;
    anim->position.z = base + (randomZ % 80);
}

void func_800CE3AC(void) {
}

int func_800CE3B4(void *arg0, void *arg1, FieldAnimTimedSpark *anim) {
    u16 x;
    FieldBillboard *output;
    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    x = anim->position.x;
    output = &D_800E22A8.billboard;
    output->position.x = x;
    D_800E22A8.billboard.position.y = anim->position.y;
    D_800E22A8.billboard.position.z = anim->position.z;
    D_800E22A8.billboard.brightness = (s8)anim->brightness;
    D_800E22A8.billboard.cell = (anim->age * 2) - 0x60;

    func_800C3B04(output);
    func_800C3098(0x10);
}

int func_800CE464(void *arg0, char *arg1) {
    int value;

    value = 2;
    arg1[1] = value;
    return value;
}

void func_800CE470(void *arg0, char *arg1, FieldAnimTimedSpark *anim) {
    anim->age++;
    if ((signed char)anim->age == 6) {
        arg1[1] = 2;
    }
}
