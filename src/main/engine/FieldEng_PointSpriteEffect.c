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

int PointSprite_PlantScript(char *object) {
    FieldActor *actor;
    void **slot;

    slot = FieldEng_GetSlot(object);
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
    FieldEng_Spawn6(a, b, c, d, e, f);
    return 0;
}

int PointSprite_RegisterDrawList(void *o) {
    FieldEng_Register(o, g_PointSpriteDrawList);
    return 0;
}

int PointSprite_RegisterPairedTables(void *obj) {
    int first;
    int second;
    int status;

    first = FieldEng_UpdateSlots(obj, g_PointSpriteUpdateList);
    second = FieldEng_RunScript(obj, g_PointSpriteInitList, g_PointSpriteSlotSizes);
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

    D_800F3430.oriented.rgb[0] = *FieldEng_RegPtr(0);
    D_800F3430.oriented.rgb[1] = *FieldEng_RegPtr(1);
    D_800F3430.oriented.rgb[2] = *FieldEng_RegPtr(2);

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

void FieldAnim_InitRadialEntries(void *arg0, void *arg1, FieldAnimRadialParticles *anim) {
    int i;
    int angle;
    int lift;

    D_800E2260.oriented.rgb[0] = *FieldEng_RegPtr(3);
    D_800E2260.oriented.rgb[1] = *FieldEng_RegPtr(4);
    D_800E2260.oriented.rgb[2] = *FieldEng_RegPtr(5);

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

void FieldAnim_ProcessInterleavedPoints(void *arg0, void *arg1, FieldAnimRadialParticles *anim) {
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

void FieldAnim_ProcessPointTriples(void *arg0, void *arg1,
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
