#include "common.h"
#include "pe1/akao_send.h"
#include "pe1/asset_find.h"
#include "pe1/field_actor.h"
#include "pe1/field_flash_burst.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/field_ring_geometry.h"
#include "pe1/field_script_context.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_sprite_params.h"
#include "pe1/field_sprite_state.h"
#include "pe1/fieldfx_calls.h"
#include "pe1/floor_polygon.h"
#include "pe1/gte_types.h"
#include "pe1/pe_image.h"
#include "pe1/psyq_gpu.h"
#include "pe1/psyq_libc.h"
#include "pe1/random.h"
#include "pe1/room_drop_field.h"
#include "pe1/room_floor.h"
#include "pe1/room_fx.h"
#include "pe1/room_rearm_timer.h"
#include "pe1/room_sound_slot.h"
#include "pe1/scene_archive.h"
#include "pe1/scene_e08_glow_sprites.h"
#include "pe1/scene_e08_layered_sprite.h"
#include "pe1/scene_e08_orbit_particles.h"
#include "pe1/scene_e08_ring_effect.h"
#include "pe1/scene_glow_model.h"
#include "pe1/scene_grab_head.h"
#include "pe1/scene_particle_slot_draw.h"
#include "pe1/scene_player_orb.h"
#include "pe1/scene_ring_sprites.h"

#include "../room_lib/room_lib.h"
#include "e08_externs.h"
#include "scene_e08.h"
#include "scene_e08_modules.h"
#include "scene_particle_slots.h"

typedef struct {
    s32 w0;
    s32 w1;
    s32 w2;
    s32 w3;
    s32 w4;
    s32 w5;
    s32 w6;
    s32 w7;
} SceneE08MatrixFields;

typedef struct {
    RoomLink *link;
    SceneE08MatrixFields matrix;
    void *asset;
} SceneE08LinkInitState;

typedef struct SceneShiftedPosition {
    char reserved[0x14];
    s32 x, y, z;
} SceneShiftedPosition;

typedef struct SceneFadeOwner {
    u8 reserved[4];
    SceneShiftedPosition position;
    u8 drawMode;
} SceneFadeOwner;

typedef union SceneFadeSeed {
    RoomFxSeed8 raw;
    struct {
        u8 initial[4];
        s16 angle;
        u8 tail[2];
    } fields;
} SceneFadeSeed;

typedef struct {
    char pad_00[1];
    u8 status;
    s16 target;
} Ovl178FadeInput;

typedef GteMatrix Matrix;

typedef GteVector Vec4;

typedef struct SceneParticleOwner {
    u8 reserved[0x24];
    u8 drawMode;
} SceneParticleOwner;

typedef struct SceneE08MatrixWords {
    int words[8];
} SceneE08MatrixWords;

typedef struct SceneE08State {
    RoomLink *link;
    SceneE08MatrixWords matrix;
    void *asset;
    u16 timer;
    u16 counter;
    u16 flag;
} SceneE08State;

typedef struct SceneActorPositionTail {
    u8 pad00[0x14];
    s32 x;
    s32 y;
    s32 z;
} SceneActorPositionTail;

typedef struct SceneFadeSlots {
    unsigned char pad00[0x28];
    s16 intensity[4];
    unsigned char pad30[8];
    u8 frame[4];
    u8 state[4];
    u8 delay[4];
    u8 activeCount;
} SceneFadeSlots;


typedef struct {
    RoomLink *link;
    SceneE08MatrixWords matrix;
    void *asset;
} SceneE08InitState;

typedef GteMatrix SceneE08Matrix;

typedef GteVector SceneE08Vector;

typedef struct SceneE08RandomEntries {
    u8 pad00[12];
    u16 counter[10];
    u8 pad20[4];
    u8 active[10];
} SceneE08RandomEntries;

typedef SceneE08EffectState SceneE08RandomState;

typedef struct {
    u8 pad_00;
    u8 mode;
    s16 timer;
} Ovl178EffectController;

void func_8018F3C4(RoomEnt *ent, void *arg1, SceneE08LinkInitState *state) {
    SceneE08MatrixFields *matrix;

    FieldFx_SetScriptContext(state);

    state->link = ent->link;
    matrix = state->link->p238;
    state->matrix = *matrix;
    state->asset = Asset_SearchByKeyType(0x2E);

    D_80199670.cell = 0x46;
    D_80199670.clut = 3;
    D_80199670.offset = -0x1E;
    D_80199670.depth = 0x80;
    D_80199670.r = 0xA0;
    D_80199670.g = 0x80;
    D_80199670.b = 0xF0;
    D_80199670.flip = 0;
    D_801994A8.cell = 0x46;
    D_801994A8.clut = 3;
    D_801994A8.offset = -0x190;
    D_801994A8.depth = 0x80;
    D_801994A8.r = 0xA0;
    D_801994A8.g = 0x80;
    D_801994A8.b = 0xF0;
    D_801994A8.flip = 0;
}

void func_8018F4FC(void) {
}

void func_8018F504(void *arg0, unsigned char *state) {
    if (FieldFx_IsLastSlotHalted() == 1) {
        state[1] = 2;
    }
}

void func_8018F53C(void *unused0, void *unused1, Ovl178FadeState *state)
{
    unsigned i;
    int offset;

    state->glowDepth = 0x80;
    i = 0;
    offset = 0;
    for (; i < 4; i++) {
        state->glowScale[i] = offset;
        offset += 0x12C;
        state->glowAngle[i] = i << 10;
    }

    for (i = 0; i < 2; i++) {
        RoomFxEmitterParams *item = &state->emitters[i];

        state->ringScale[i] = (i + 1) * 700;
        item->mode = 0x10;
        item->intensity = 0x80;
        item->color1[0] = 0xC8;
        item->color1[1] = 0xFF;
        item->extent0 = 0x514;
        item->extent1 = 0x320;
        item->offset = 0;
        item->color1[2] = 0;
        item->color0[0] = 0;
        item->color0[1] = 0;
        item->color0[2] = 0;
        item->source = state->sourceData[i];
        func_800C4E50(item);
    }
}

void func_800C2FF0(int, int);
void func_800C3098(int);

void func_8018F640(void *unused, s16 *args, Ovl178FadeState *state) {
    RoomSpriteMatrix matrix;
    SceneFadeSeed seed;
    RoomFxVec4 scale, sourceScale;
    SceneFadeOwner *owner;
    SceneShiftedPosition *ownerCursor;
    unsigned i;

    owner = FieldFx_GetScriptContext();
    seed.raw = D_8018EFF4;

    ownerCursor = &owner->position;
    func_800C2EAC(owner->drawMode);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);
    func_800C3134(D_801986F8, args[1], (u8 *)&D_80199670);
    for (i = 0; i < 4; ++i) {
        seed.fields.angle = state->glowAngle[i];
        RotMatrix(&seed.raw.vector, &matrix);
        memset(&sourceScale, 0, 0x10);
        sourceScale.x = state->glowScale[i];
        sourceScale.y = state->glowScale[i];
        sourceScale.z = state->glowScale[i];
        scale = sourceScale;
        ScaleMatrix(&matrix, &scale);
        matrix.t[0] = ownerCursor->x;
        matrix.t[1] = ownerCursor->y;
        matrix.t[2] = ownerCursor->z;
        D_80199670.depth = state->glowDepth;
        func_800C42A4(&D_80199670, &matrix, 1);
    }
    for (i = 0; i < 2; ++i) {
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
        memset(&sourceScale, 0, 0x10);
        sourceScale.x = state->ringScale[i];
        sourceScale.y = state->ringScale[i];
        sourceScale.z = state->ringScale[i];
        scale = sourceScale;
        ScaleMatrix(&matrix, &scale);
        matrix.t[0] = ownerCursor->x;
        matrix.t[1] = ownerCursor->y;
        matrix.t[2] = ownerCursor->z;
        func_800C3134(D_80198708, args[1], &state->emitters[i].color1[0]);
        func_800C4FC4(&state->emitters[i], &matrix, 1);
    }
}

void func_8018F8B0(void *arg0, Ovl178FadeInput *input, Ovl178FadeState *state) {
    unsigned int i;
    s16 value;

    for (i = 0; i < 4; i++) {
        state->glowScale[i] = (u16)state->glowScale[i] + 0x3C;
        state->glowAngle[i] = (u16)state->glowAngle[i] + 0x80;
    }

    for (i = 0; i < 2; i++) {
        if (state->ringScale[i] < 0) {
            state->ringScale[i] = 0;
        }
        state->ringScale[i] = (u16)state->ringScale[i] - 0x1E;
    }

    value = state->glowDepth;
    if (value >= 3) {
        state->glowDepth = value - 2;
    }

    if (input->target == 0x28) {
        input->status = 2;
    }
}

void func_8018F958(int unused0, int unused1, SceneParticleCluster *slots) {
    unsigned int i;

    for (i = 0; i < 30; ++i) {
        {
            int randomValue = rand();
            slots->position[i].y = 0;
            slots->position[i].x = randomValue % 800 - 200;
        }
        slots->position[i].z = rand() % 600 - 300;
        slots->angle[i] = rand() % 4096;
        slots->counter[i] = 0;
        slots->field1FE[i] = 0;
        slots->active[i] = 0;
        slots->timer[i] = rand() % 50 + 2;

        if (i >= 4) {
            if ((rand() & 1) == 0) {
                slots->brightness[i] = 0xFF;
                slots->texture[i] = 0x4C;
                slots->mode[i] = 5;
                slots->scale[i] = rand() % 256 + 0x200;
            } else {
                slots->brightness[i] = 0xFF;
                slots->texture[i] = 0x58;
                slots->mode[i] = 6;
                slots->scale[i] = rand() % 512 + 0x300;
            }
        } else {
            slots->brightness[i] = 0x80;
            slots->texture[i] = 0x44;
            slots->mode[i] = 2;
            slots->scale[i] = 0x7C;
        }
    }
    slots->count = 30;
    {
        void *volatile *sound = D_800B0E64;
        if (*sound) {
            Akao_SendTableCommand(*sound, 0x608, 0, 0x80, 0x7F);
        }
    }
}

void func_800C2FF0(int, int);
void func_800C3098(int);

void func_8018FBA4(void *unused0, void *unused1, SceneParticleCluster *argSlots) {
    SceneParticleCluster *slots = argSlots;
    Matrix matrix;
    Vec4 scratch, draw;
    SceneParticleOwner *owner;
    register unsigned i;
    s16 *slot = &D_801994B2;

    owner = FieldFx_GetScriptContext();
    i = 0;
    /* Hoist the draw state address before the cursor setup. */
    asm("" : : "r"(slot));
    func_800C2EAC(owner->drawMode);
    func_800C3098(0x10);
    func_800C3238(2);

    for (; i < 30; i++) {
        if (slots->active[i] == 1) {
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

            memset(&draw, 0, 0x10);
            draw.x = slots->scale[i];
            draw.y = slots->scale[i];
            draw.z = slots->scale[i];
            scratch = draw;
            ScaleMatrix(&matrix, &scratch);

            matrix.t[0] = slots->position[i].x;
            matrix.t[1] = slots->position[i].y;
            matrix.t[2] = slots->position[i].z;
            *slot = slots->brightness[i];
            if (slots->texture[i] == 0x44) {
                func_800C2FF0(0x20, 0x20);
                ((FieldGlowSprite *)((char *)slot - 10))->cell = slots->texture[i];
            }
            if (slots->texture[i] == 0x4C) {
                func_800C2FF0(0x10, 0x10);
                {
                    u8 color = slots->texture[i];
                    u8 counter = slots->counter[i];
                    color += 3;
                    ((FieldGlowSprite *)((char *)slot - 10))->cell = color - (counter >> 1);
                }
            }
            if (slots->texture[i] == 0x58) {
                func_800C2FF0(0x10, 0x10);
                {
                    u8 color = slots->texture[i];
                    u8 counter = slots->counter[i];
                    color += 7;
                    ((FieldGlowSprite *)((char *)slot - 10))->cell = color - (counter >> 1);
                }
            }
            ((FieldGlowSprite *)((char *)slot - 10))->clut = slots->mode[i];
            func_800C42A4((FieldGlowSprite *)((char *)slot - 10), &matrix, 1);
        }
    }
}

void func_8018FDA8(void *unused, RoomFxControl *signal, u8 *state) {
    SceneParticleCluster *slots = (SceneParticleCluster *)state;
    int eight;
    unsigned i;
    int one;

    i = 0;
    one = 1;
    eight = 8;
    for (; i < 30; i++) {
        u8 timer = slots->timer[i];
        int done;
        if (timer != 0) {
            timer--;
            slots->timer[i] = timer;
            if (timer == 0) {
                slots->active[i] = one;
            }
        }

        if (slots->active[i] != one) {
            continue;
        }

        slots->brightness[i] -= 0x10;
        slots->counter[i]++;
        done = 0;

        if (slots->texture[i] == 0x44) {
            slots->scale[i] += 0xC8;
            if (slots->texture[i] == 0x44 && slots->counter[i] == eight) {
                done = 1;
            }
        }
        if (slots->texture[i] == 0x4C && slots->counter[i] == eight) {
            done = 1;
        }
        if (slots->texture[i] == 0x58 && slots->counter[i] == 0x10) {
            done = 1;
        }
        if (done == one) {
            slots->active[i] = 0;
            slots->count--;
            if (slots->count == 0) {
                signal->state = 2;
            }
        }
    }
}

/*
 * The module class of scene_e08's second module (emitter and fade slots,
 * the player orb and its ring sprites): the seven methods its class table
 * lists in front of the module. The registration and start methods act
 * whatever the field engine's status. The script and the lists are named
 * in the symbol file (scene_e08_modules.h).
 */

ROOMLIB_RETURN_ZERO(SceneE08_OrbNop0)

ROOMLIB_PLANT_TABLE(SceneE08_OrbPlantScript, g_SceneE08OrbScript)

ROOMLIB_SPAWN6(SceneE08_OrbSpawn6)

ROOMLIB_REGISTER_TABLE_ANY(SceneE08_OrbRegister, g_SceneE08OrbDrawList)

ROOMLIB_START_ANY(SceneE08_OrbStart, g_SceneE08OrbUpdateList,
                  g_SceneE08OrbInitList, g_SceneE08OrbSpawnLayout,
                  SceneE08_OrbClose)

ROOMLIB_CLOSE_TARGET(SceneE08_OrbClose)

ROOMLIB_RETURN_ZERO(SceneE08_OrbNop6)

void func_80190060(RoomEnt *ent, void *unused, SceneE08State *state) {
    SceneE08MatrixWords *matrix;

    FieldFx_SetScriptContext(state);
    state->link = ent->link;
    matrix = state->link->p238;
    state->matrix = *matrix;
    state->asset = Asset_SearchByKeyType(0x2E);
    state->timer = 30;
    state->counter = 0;
    state->flag = 0;

    D_801994B8.cell = 0x46;
    D_801994B8.clut = 3;
    D_801994B8.offset = -400;
    D_801994B8.depth = 128;
    D_801994B8.r = 240;
    D_801994B8.g = 64;
    D_801994B8.b = 64;
    D_801994B8.flip = 0;

    D_801995A8.cell = 0xA4;
    D_801995A8.clut = 10;
    D_801995A8.offset = -30;
    D_801995A8.depth = 128;
    D_801995A8.r = 160;
    D_801995A8.g = 128;
    D_801995A8.b = 240;
    D_801995A8.flip = 0;

    D_801995B8.cell = 0x44;
    D_801995B8.clut = 2;
    D_801995B8.offset = -30;
    D_801995B8.depth = 128;
    D_801995B8.r = 160;
    D_801995B8.g = 128;
    D_801995B8.b = 240;
    D_801995B8.flip = 0;

    D_80199690.cell = 0x4C;
    D_80199690.clut = 5;
    D_80199690.offset = -30;
    D_80199690.depth = 128;
    D_80199690.r = 160;
    D_80199690.g = 128;
    D_80199690.b = 240;
    D_80199690.flip = 0;
}

void func_80190244(void) {
}

ROOM_REARM_TIMER_TICK(func_8019024C, 0x28)

void func_801902CC(int unused0, int unused1, SceneEmitterSlots *slots) {
    unsigned int i;
    SceneActorPositionTail *origin = (SceneActorPositionTail *)((s32 *)FieldFx_GetScriptContext() + 1);

    slots->activeSlots = 4;
    for (i = 0; i < 4; ++i) {
        slots->field3C[i] = 0;
        {
            int randomValue = rand();
            int center = origin->x - 100;
            slots->position[i].x = center + randomValue % 200;
        }
        slots->position[i].y = origin->y;
        {
            int randomValue = rand();
            int center = origin->z - 100;
            slots->position[i].z = center + randomValue % 200;
        }
        slots->field40[i] = rand() % 5 + 1;
        slots->field20[i] = rand() % 256 + 0x400;
        slots->field30[i] = 0x48;
        slots->field34[i] = 4;
        slots->field28[i] = 0x40;
        slots->field38[i] = 0;
    }
}

void func_800C2FF0(int, int);
void func_800C3098(int);

void func_80190460(void *unused0, void *unused1, char *effect) {
    Matrix matrix;
    Vec4 scratch, draw;
    FieldGlowSprite *sprite;
    Ovl178Position *owner;
    SceneE08GlowSprites *base = (SceneE08GlowSprites *)effect;
    register unsigned i;
    int diag = 0x1000;
    s16 *slot = &D_801994C2;
    owner = FieldFx_GetScriptContext();
    i = 0;
        asm volatile("" : : "r"(slot));
    func_800C2EAC(owner->spriteMode);
    func_800C3098(0x10);
    func_800C3238(2);

    for (; i < 4; i++) {
        if (base->active[i] == 1) {
            matrix.m[2][2] = diag;
            matrix.m[1][1] = diag;
            matrix.m[0][0] = diag;
            matrix.t[2] = 0;
            matrix.t[1] = 0;
            matrix.t[0] = 0;
            matrix.m[2][1] = 0;
            matrix.m[2][0] = 0;
            matrix.m[1][2] = 0;
            matrix.m[1][0] = 0;
            matrix.m[0][2] = 0;
            matrix.m[0][1] = 0;

            memset(&draw, 0, 0x10);
            draw.x = base->scale[i];
            draw.y = base->scale[i];
            draw.z = base->scale[i];
            scratch = draw;
            ScaleMatrix(&matrix, &scratch);

            matrix.t[0] = base->position[i].x;
            matrix.t[1] = base->position[i].y;
            matrix.t[2] = base->position[i].z;
            *slot = base->depth[i];
            func_800C2FF0(0x10, 0x10);
            sprite = (FieldGlowSprite *)((char *)slot - 10);
            sprite->cell = base->cell[i] + (base->frame[i] >> 1);
            sprite->clut = base->clut[i];
            func_800C42A4(sprite, &matrix, 1);
        }
    }
}

void SceneE08_UpdateFadeSlots(int unused, u8 *sceneState, SceneFadeSlots *slots) {
    unsigned int i;
    u8 frame;

    for (i = 0; i < 4; i++) {
        if (slots->delay[i] != 0) {
            slots->delay[i]--;
            if (slots->delay[i] == 0) {
                slots->state[i] = 1;
            }
        }

        if (slots->state[i] == 1) {
            if (slots->intensity[i] >= 0x11) {
                slots->intensity[i] -= 0x10;
            }
            frame = slots->frame[i];
            slots->frame[i] = frame + 1;
            if (frame == 8) {
                slots->state[i] = 0;
                slots->activeCount--;
                if (slots->activeCount == 0) {
                    sceneState[1] = 2;
                }
            }
        }
    }
}

/* Spawn the orb 150 units around the player, rotate that offset around the
 * scene anchor by the anchor's heading, and set the trail colours. */
void func_801906BC(void *object, void *timer, ScenePlayerOrb *orb)
{
    GteShortVector offset;
    GteShortVector rotated;
    GteRotation rotation;
    GteMatrix matrix;
    ScenePlayerOrbAnchor *anchor;

    anchor = FieldFx_GetScriptContext();
    rotation = D_8018EFFC;
    orb->count = 3;
    orb->anchor.x = anchor->x;
    orb->anchor.y = anchor->y;
    orb->anchor.z = anchor->z;
    orb->start.x = anchor->x;
    orb->start.y = anchor->y;
    orb->start.z = anchor->z;
    orb->position.x = g_PlayerEntity->pos_x >> 16;
    orb->position.y = g_PlayerEntity->pos_y >> 16;
    orb->position.z = g_PlayerEntity->pos_z >> 16;
    orb->position.x += rand() % 300 - 150;
    orb->position.z += rand() % 300 - 150;
    rotation.y = *FieldFx_RegPtr(2);
    RotMatrix(&rotation, &matrix);
    offset.x = orb->position.x - orb->anchor.x;
    offset.y = orb->position.y - orb->anchor.y;
    offset.z = orb->position.z - orb->anchor.z;
    ApplyMatrixSV(&matrix, &offset, &rotated);
    orb->position.x = orb->anchor.x + rotated.x;
    orb->position.y = orb->anchor.y + rotated.y;
    orb->position.z = orb->anchor.z + rotated.z;
    orb->head.r = 0x80;
    orb->head.g = 0x80;
    orb->head.b = 0xF0;
    orb->tail.r = 0x20;
    orb->tail.g = 0x20;
    orb->tail.b = 0xF0;
    orb->alpha = 0xFF;
    orb->active = 0;
    orb->fade = 0;
    orb->delay = *FieldFx_RegPtr(1);
    if (D_800B0E64[0] != 0) {
        Akao_SendTableCommand(D_800B0E64[0], 0x606, 0, 0x80, 0x7F);
    }
}

/* Draw the orb's two fading ribbons and the eight trail sprites. */
void func_80190914(void *object, void *timer, ScenePlayerOrb *orb)
{
    RenderColor color;
    GteMatrix matrix;
    GteVector scale;
    ScenePlayerOrbAnchor *anchor;
    u32 i;

    anchor = FieldFx_GetScriptContext();
    color = D_8018F004;
    func_800C2EAC(anchor->mode);
    func_800C3098(0x10);
    func_800C3238(2);
    func_800C2FF0(0x10, 0x10);
    if (orb->active != 0) {
        func_800C3134(D_801987E4, orb->fade, (unsigned char *)&orb->head);
        func_800C3134(D_801987E4, orb->fade, (unsigned char *)&orb->tail);
        func_800D3114(orb->trailA, orb->count - 1, 0x3C, 0x20, 0x90, 0x51, 0x10,
                      GetTPage(1, 0, 0x340, 0x100), GetClut(0, 0x1D9),
                      orb->alpha, &orb->head, &orb->tail, 1);
        func_800D3114(orb->trailB, orb->count - 1, 0x3C, 0x20, 0x90, 0x51, 0x10,
                      GetTPage(1, 0, 0x340, 0x100), GetClut(0, 0x1D9),
                      orb->alpha, &color, &color, 2);
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
        scale = D_8018F008;
        ScaleMatrix(&matrix, &scale);
        for (i = 0; i < 8; i++) {
            matrix.t[0] = orb->points[i].x;
            matrix.t[1] = orb->points[i].y;
            matrix.t[2] = orb->points[i].z;
            func_800C42A4(&D_80199690, &matrix, 1);
        }
    }
}

/* After the start delay, slide the orb's ribbon from the anchor toward its
 * target in sixteenth steps, burst it at step 12, and scatter the trail
 * sprites around the ribbon head. */
void func_80190B98(void *object, ScenePlayerOrbTimer *timer, ScenePlayerOrb *orb)
{
    GteShortVector step;
    ScenePlayerOrbAnchor *anchor;
    GteShortVector *spawn;
    u32 i;

    anchor = FieldFx_GetScriptContext();
    if (orb->delay != 0) {
        if (--orb->delay == 0) {
            orb->active = 1;
            FieldFx_StartSlot(object, 1, (unsigned short *)g_SceneE08OrbSpawnLayout,
                          (int (**)(void *, void *, void *))g_SceneE08OrbInitList);
        }
    }
    if (orb->active != 0) {
        orb->fade++;
        if (++orb->active == 12) {
            spawn = FieldFx_StartSlot(object, 3, (unsigned short *)g_SceneE08OrbSpawnLayout,
                                 (int (**)(void *, void *, void *))g_SceneE08OrbInitList);
            if (spawn != 0) {
                spawn->x = orb->position.x;
                spawn->y = orb->position.y;
                spawn->z = orb->position.z;
                if (func_800C6B90(spawn, 200) != 0) {
                    anchor->orbHit = 1;
                }
            }
            timer->state = 2;
        }
    }
    step.x = (orb->position.x - orb->anchor.x) >> 4;
    step.y = (orb->position.y - orb->anchor.y) >> 4;
    step.z = (orb->position.z - orb->anchor.z) >> 4;
    orb->trailA[2].x = orb->anchor.x + step.x * (orb->active - 2);
    orb->trailA[2].y = orb->anchor.y + step.y * (orb->active - 2);
    orb->trailA[2].z = orb->anchor.z + step.z * (orb->active - 2);
    orb->trailA[0].x = orb->trailA[2].x + step.x * (orb->active - 1);
    orb->trailA[0].y = orb->trailA[2].y + step.y * (orb->active - 1);
    orb->trailA[0].z = orb->trailA[2].z + step.z * (orb->active - 1);
    orb->trailA[1].x = (orb->trailA[0].x + orb->trailA[2].x) >> 1;
    orb->trailA[1].y = (orb->trailA[0].y + orb->trailA[2].y) >> 1;
    orb->trailA[1].z = (orb->trailA[0].z + orb->trailA[2].z) >> 1;
    orb->trailB[0].x = orb->trailA[0].x;
    orb->trailB[0].y = g_RoomFloorY->y;
    orb->trailB[0].z = orb->trailA[0].z;
    orb->trailB[1].x = orb->trailA[1].x;
    orb->trailB[1].y = g_RoomFloorY->y;
    orb->trailB[1].z = orb->trailA[1].z;
    orb->trailB[2].x = orb->trailA[2].x;
    orb->trailB[2].y = g_RoomFloorY->y;
    orb->trailB[2].z = orb->trailA[2].z;
    for (i = 0; i < 8; i++) {
        orb->points[i].x = orb->trailA[2].x - (step.x << 2) + rand() % 100 - 50;
        orb->points[i].y = orb->trailA[2].y;
        orb->points[i].z = orb->trailA[2].z - (step.z << 2) + rand() % 100 - 50;
    }
}

void func_80190F0C(void *unused0, void *unused1, SceneRingSpritesState *effect) {
    u8 *mid = D_801995D4;
    u8 *source;
    int width;
    FieldRingGeometry *base;
    int first;
    asm("" : "=r"(mid) : "0"(mid));
    base = (FieldRingGeometry *)(mid - 12);
    width = 128;
    first = 300;
    asm volatile("" : : "r"(width));
    effect->size = first;
    effect->reserved0D = 156;
    effect->ticks = 0;
    effect->depth = width;

    base[0].mode = 8;
    D_801995DA = 0;
    D_801995DC = width;
    D_801995D0 = 120;
    D_801995D1 = 100;
    D_801995D2 = 30;
    D_801995CC = 0;
    D_801995CD = 0;
    D_801995CE = 0;
    D_801995D6 = 1800;
    D_801995D8 = 1200;
    source = D_80198B90;
    base[0].source = source;
    func_800C4E50(base);

    base[1].mode = 8;
    base[1].offset = 0;
    base[1].intensity = width;
    base[1].color1[0] = 20;
    base[1].color1[1] = 20;
    base[1].color1[2] = 0;
    base[1].color0[0] = 120;
    base[1].color0[1] = 100;
    base[1].color0[2] = 30;
    base[1].extent0 = 1200;
    base[1].extent1 = 800;
    base[1].source = source + 128;
    func_800C4E50(&base[1]);
}

/* Draw the flash sprite while the state is younger than 12 ticks, then the
 * glow sprite and the two shaded rings, all at the state's position. The
 * untyped data argument is read through two typed views and then reused for
 * the matrix pointer of the second half, as retail's register use shows
 * (the incoming copy serves the flash block, a second copy the rest). */
void func_8019104C(void *object, void *timer, void *data)
{
    GteRotation rotation;
    GteMatrix matrix;
    GteVector scale;
    GteVector ringScale;
    GteVector source;
    SceneRingSpritesOwner *owner;
    SceneRingSpritesState *ring;
    SceneRingSpritesState *state;

    owner = FieldFx_GetScriptContext();
    rotation = D_8018F018;
    ring = data;
    state = data;
    func_800C2EAC(owner->mode);
    func_800C3098(0x10);
    func_800C3238(2);
    func_800C2FF0(0x20, 0x20);
    if (ring->ticks < 12) {
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
        scale = D_8018F020;
        ScaleMatrix(&matrix, &scale);
        matrix.t[0] = ring->x;
        matrix.t[1] = ring->y;
        matrix.t[2] = ring->z;
        D_801995A8.depth = ring->depth;
        D_801995A8.cell = (ring->ticks % 12 & 0xFE) - 0x5C;
        func_800C42A4(&D_801995A8, &matrix, 1);
    }
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
    scale = D_8018F030;
    data = &matrix;
    ScaleMatrix(data, &scale);
    matrix.t[0] = state->x;
    matrix.t[1] = state->y;
    matrix.t[2] = state->z;
    D_801995B8.depth = state->depth >> 2;
    func_800C42A4(&D_801995B8, data, 1);
    RotMatrix(&rotation, data);
    memset(&source, 0, 0x10);
    source.x = state->size;
    source.y = state->size;
    source.z = state->size;
    ringScale = source;
    ScaleMatrix(data, &ringScale);
    matrix.t[0] = state->x;
    matrix.t[1] = state->y;
    matrix.t[2] = state->z;
    D_801995C8[0].depth = state->depth;
    func_800C4FC4(&D_801995C8[0], data, 0);
    D_801995C8[1].depth = state->depth;
    func_800C4FC4(&D_801995C8[1], data, 0);
}

void func_80191348(s32 arg0, RoomFxControl *rec, SceneRingSpritesState *p) {
    SceneRingSpritesState *q;
    s16 zero;
    s32 step;

    q = p;
    step = rcos(rec->frame << 5) >> 6;
    p->size += step;
    p->ticks++;
    p->depth -= 4;
    zero = 0;
    if (p->depth < 0) {
        p->depth = zero;
    }
    if (q->depth == zero) {
        rec->state = 2;
    }
}

/*
 * The module class of scene_e08's third module (moving and particle slots,
 * the glow model and the limb beams): the seven methods its class table
 * lists in front of the module. The registration and start methods act
 * whatever the field engine's status. The script and the lists are named
 * in the symbol file (scene_e08_modules.h).
 */

ROOMLIB_RETURN_ZERO(SceneE08_SlotsNop0)

ROOMLIB_PLANT_TABLE(SceneE08_SlotsPlantScript, g_SceneE08SlotsScript)

ROOMLIB_SPAWN6(SceneE08_SlotsSpawn6)

ROOMLIB_REGISTER_TABLE_ANY(SceneE08_SlotsRegister, g_SceneE08SlotsDrawList)

ROOMLIB_START_ANY(SceneE08_SlotsStart, g_SceneE08SlotsUpdateList,
                  g_SceneE08SlotsInitList, g_SceneE08SlotsSpawnLayout,
                  SceneE08_SlotsClose)

ROOMLIB_CLOSE_TARGET(SceneE08_SlotsClose)

ROOMLIB_RETURN_ZERO(SceneE08_SlotsNop6)

void func_8019155C(RoomEnt *ent, void *unused, SceneE08InitState *state) {
    SceneE08MatrixWords *matrix;
    void *asset;
    s16 half;
    FieldActor *player;
    register int intensity asm("$3");
    register int mode asm("$2");
    register int offset asm("$5");
    void *volatile *sound = D_800B0E64;
    void *handle;
    int playerX;

    FieldFx_SetScriptContext(state);
    D_80199528 = Asset_FindTable08ByU32Key(*sound, 0x10D8704);
    state->link = ent->link;
    matrix = state->link->p238;
    state->matrix = *matrix;
    asset = Asset_SearchByKeyType(0x2E);
    intensity = 0x80;
    state->asset = asset;
    mode = 8;
    offset = -400;

    D_80199688 = offset;
    D_801996A8 = offset;
    /* Empty compiler barriers retain the retail register lifetimes. */
    player = g_PlayerEntity;
    asm volatile ("" : : "r"(player));
    half = 0x80;
    asm volatile ("" : : "r"(half));
    D_80199685 = mode;
    D_801996A4 = 0x44;
    D_801996A5 = 2;
    D_801996B0.cell = 0x60;
    D_801996B0.clut = 0x40;
    D_80199684 = intensity;
    D_8019968A = half;
    D_80199680 = intensity;
    D_80199681 = intensity;
    D_80199682 = intensity;
    D_80199686 = 0;
    D_801996AA = half;
    D_801996A0[0] = intensity;
    D_801996A1 = intensity;
    D_801996A2 = intensity;
    D_801996A6 = 0;
    D_801996B0.offset = -500;
    D_801996B0.depth = half;
    D_801996B0.r = intensity;
    D_801996B0.g = intensity;
    D_801996B0.b = intensity;
    D_801996B0.flip = 0;

    handle = *sound;
    /* Keep the handle load ahead of the two player-coordinate loads. */
    asm volatile("" : : "r"(handle));
    intensity = Pe1Fixed_Integer(&player->pos_z);
    playerX = Pe1Fixed_Integer(&player->pos_x);
    D_8019956C = playerX;
    D_8019957C = intensity;
    if (handle != 0) {
        Akao_SendTableCommand(*sound, 0x603, 0, 0x80, 0x7F);
    }
}

void func_80191740(void) {
}

void Battle_HaltOnPositiveX(void);
void Battle_InitEntityColors(void);
int FieldFx_GetStatus(void *arg0);

void func_80191748(SceneGrabHead *arg0, RoomFxControl *arg1) {
    int *status;
    int *flags;
    int *objectFlags;

    status = FieldFx_VarPtr(1);
    if (*status == 1) {
        if (arg1->frame < 0xF0) {
            Battle_HaltOnPositiveX();
        }

        if (arg1->frame == 0xF0) {
            if (FieldFx_GetStatus(arg0) == 3) {
                flags = *(int **)D_8009D254;
                flags[0x13] |= 0x4000;

                objectFlags = &arg0->actor->state->core_flags;
                *objectFlags |= 0x80000000;
                Battle_InitEntityColors();
            }
        }
    }

    if (FieldFx_IsLastSlotHalted() == 1) {
        arg1->state = 2;
    }
}

void func_80191830(void *arg0, void *arg1, SceneE08CentreGlow *glow) {
    glow->x = 0;
    glow->y = 0;
    glow->z = 0;
    glow->scale = 0x400;
    glow->unused0A = 0;
    glow->unused0C = 0;
}

void func_800C2FF0(s32 arg0, s32 arg1);
void func_800C3098(s32 arg0);

void func_80191850(void *arg0, void *arg1, SceneE08CentreGlow *glow) {
    SceneE08Matrix matrix;
    SceneE08Vector scratch_scale;
    SceneE08Vector draw_scale;
    Ovl178Position *owner;

    owner = FieldFx_GetScriptContext();
    func_800C2EAC(owner->spriteMode);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

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

    memset(&draw_scale, 0, 0x10);
    draw_scale.x = glow->scale;
    draw_scale.y = glow->scale;
    draw_scale.z = 0x1000;
    scratch_scale = draw_scale;
    ScaleMatrix(&matrix, &scratch_scale);

    matrix.t[0] = glow->x + D_8019956C;
    matrix.t[1] = glow->y;
    matrix.t[2] = glow->z + D_8019957C;
    func_800C42A4((FieldGlowSprite *)D_801996A0, &matrix, 1);
}

void func_80191970(void *arg0, RoomFxControl *control, void *arg2) {
    if (control->frame >= 0x79) {
        control->state = 2;
    }
}

void func_80191990(void *arg0, void *arg1, SceneMovingSlots *slots) {
    unsigned int i;

    for (i = 0; i < 16; i++) {
        slots->active[i] = 0;
    }
}

void func_801919B0(s32 arg0, s32 arg1, SceneMovingSlots *base) {
    Ovl178Position *st;
    u32 i;
    u8 buf38[8];
    u8 buf40[8];

    st = FieldFx_GetScriptContext();
    func_800C2EAC(st->spriteMode);
    func_800C3098(0x10);
    func_800C3238(2);

    for (i = 0; i < 0x10; i++) {
        s16 type = base->active[i];
        if (type == 1) {
            s32 v64;
            s32 vAA;
            func_800C3134(D_8019880C, base->life[i], buf38);
            func_800C3134(D_8019880C, base->life[i], buf40);
            buf40[0] = 0;
            buf40[1] = 0;
            buf40[2] = 0;
            D_800F3368.depth = 0x1F4;
            v64 = GetTPage(0, 0, 0x340, 0x100) & 0xFFFF;
            vAA = GetClut(0, 0x1D7) & 0xFFFF;
            func_800D3114((struct GteShortVector *)&base->vector[i], 2, base->range[i], 0x40,
                          0x80, 0x9C, 0x10, v64, vAA, base->initialSize[i],
                          (struct RenderColor *)buf38, (struct RenderColor *)buf40, type);
        }
    }
}

void func_80191B14(int unused, SceneMovingState *state, SceneMovingSlots *slots) {
    unsigned int i;

    for (i = 0; i < 16; ++i) {
        if (slots->active[i] == 0 && rand() % 3 == 0) {
            slots->angle[i] = rand() % 4096;
            slots->initialSize[i] = 0x80;
            slots->active[i] = 1;
            slots->timer[i] = 90;
            slots->life[i] = 0;
            {
                int randomValue = rand();
                slots->radius[i] = 0;
                slots->range[i] = randomValue % 200 + 100;
            }
            {
                int randomValue = rand();
                int x = D_8019956C;
                int z = D_8019957C;
                slots->radius[i] = randomValue % 1500 + 300;
                slots->vector[i].y = 0;
                slots->vector[i].x = x;
                slots->vector[i].z = z;
            }
        }

        if (slots->active[i] == 1) {
            slots->life[i]++;
            if (--slots->timer[i] == 0) {
                slots->active[i] = 0;
            }
            slots->vector[i].dx = (rsin(slots->angle[i]) * slots->radius[i] >> 12) + D_8019956C;
            slots->vector[i].dy = rcos(slots->angle[i]) * slots->radius[i] >> 12;
            slots->vector[i].dz = (rcos(slots->angle[i]) * slots->radius[i] >> 12) + D_8019957C;
            slots->vector[i].combinedX = (u16)slots->vector[i].x + (u16)slots->vector[i].dx;
            slots->vector[i].combinedY = (u16)slots->vector[i].y + (u16)slots->vector[i].dy;
            slots->vector[i].combinedZ = (u16)slots->vector[i].z + (u16)slots->vector[i].dz;
        }
    }
    if (state->elapsedFrames >= 181) {
        state->status = 2;
    }
}

void SceneE08_InitParticleSlots(int unused0, int unused1, SceneParticleSlots *slots) {
    unsigned int i;

    slots->motionRamp = 0;
    slots->elapsedFrames = 0;
    for (i = 0; i < 4; i++) {
        slots->phase[i] = i << 11;
        slots->verticalOffset[i] = 0;
        slots->offset[i].x = 0;
        slots->offset[i].y = 0;
        slots->offset[i].z = 0;
        slots->timer[i] = rand() % 10 + 10;
    }
}

/* Draw each slot's sprite at its offset from the scene anchor (doubled in
 * depth), then each slot's shadow sprite using the raw offset. */
void func_80191E78(void *object, void *timer, SceneParticleSlots *slots)
{
    GteShortVector position;
    GteShortVector offset;
    RenderColor color;
    int scale;
    int kind;
    int palette;
    u32 i;

    memset(&offset, 0, 8);
    offset.x = D_8019956C;
    offset.y = 0;
    offset.z = D_8019957C;
    position = offset;
    offset = D_8018F040;
    color = D_8018F048;
    D_800F3368.tpage = D_800E2850[D_800E11EA];
    D_800F3368.palette = 3;
    D_800F3368.parameter06 = 0;
    D_800F3368.parameter00 = 0x40;
    D_800F3368.parameter02 = 4;
    D_800F3368.extent_x = 0x40;
    D_800F3368.extent_y = 0x40;
    D_800F3368.depth = 1000;
    D_800F3368.parameter0A = 0;
    for (i = 0; i < 4; i++) {
        offset = slots->offset[i];
        color.b = 0xFF;
        color.g = 0xFF;
        color.r = 0xFF;
        offset.z <<= 1;
        scale = slots->phase[i] * slots->motionRamp >> 12;
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428 != 0) {
            palette += 4;
        }
        func_800CEE20(&position, (GteRotation *)&offset, scale, scale, 0,
                      GetClut(0, palette), 1, slots->verticalOffset[i], 0);
    }
    D_800F3368.parameter00 = 0x40;
    D_800F3368.parameter02 = 4;
    D_800F3368.extent_x = 0x40;
    D_800F3368.extent_y = 0x40;
    D_800F3368.depth = 0;
    D_800F3368.parameter0A = 5;
    for (i = 0; i < 4; i++) {
        u16 *palettes = D_800E1204;
        int special = 4;

        scale = slots->phase[i] * slots->motionRamp >> 12;
        kind = D_800F3368.palette;
        palette = palettes[kind];
        if (kind == special && D_800F3428 != 0) {
            palette += 4;
        }
        func_800CEE20(&position, (GteRotation *)&slots->offset[i], scale, scale, 0x80,
                      GetClut(0x80, palette), 2, slots->verticalOffset[i], 0);
    }
}

void func_801921C8(int unused, u8 *state, SceneParticleSlots *slots)
{
    unsigned int i;
    u16 oldPhase;
    ++slots->elapsedFrames;
    if (slots->motionRamp < 0x1000) {
        slots->motionRamp += 200;
    }
    for (i = 0; i < 4; ++i) {
        slots->cycle[i] = (s16)((u16)slots->cycle[i] + 1) % 40;
        slots->phase[i] -= 200;
        slots->offset[i].x = rsin(((s16 *)state)[1] << 3) >> 3;
        slots->offset[i].y = rcos(((s16 *)state)[1] << 4) >> 3;
        oldPhase = (u16)slots->offset[i].z;
        slots->offset[i].x = 0;
        slots->offset[i].y = 0;
        slots->offset[i].z = oldPhase + (u16)slots->timer[i];
        if (slots->verticalOffset[i] < 0x80) {
            slots->verticalOffset[i] += 8;
        }
        if (slots->phase[i] < 0) {
            slots->cycle[i] = rand() % 10;
            slots->phase[i] = 0x2000;
            slots->verticalOffset[i] = 0;
            slots->offset[i].z = rand() % 4096;
            slots->timer[i] = rand() % 10 + 20;
        }
    }
    if (slots->elapsedFrames >= 181) {
        state[1] = 2;
    }
}

void func_800C4E50(void *);

void func_801923D8(void *unused0, void *unused1, SceneE08RingEffectState *effect) {
    u8 *mid = (u8 *)&D_80199604;
    u8 *source;
    FieldRingGeometry *ring;
    FieldRingGeometry *record = (FieldRingGeometry *)((u8 *)&D_80199604 - PE1_OFFSETOF(FieldRingGeometry, mode));
    int intensity = 0x80;
    register int span asm("$19");
    asm("" : "=r"(mid) : "0"(mid));

    ring = (FieldRingGeometry *)(mid - PE1_OFFSETOF(FieldRingGeometry, mode));
    asm ("" : : "r"(ring));
    effect->rotation[0] = 0;
    effect->rotation[1] = 0;
    effect->rotation[2] = 0;
    effect->secondScale = 0;

    ring->mode = 0x20;
    record->offset = 0x1F4;
    asm volatile("" : : "r"(intensity));
    record->color1[0] = 0xF0;
    record->color1[1] = 0xC8;
    span = 0x960;
    record->intensity = intensity;
    record->color1[2] = 0x78;
    asm volatile("" : "=r"(source) : "0"(D_80198C90));
    record->color0[0] = 0;
    record->color0[1] = 0;
    record->color0[2] = 0;
    record->extent0 = span;
    record->extent1 = 0;
    ring->source = source;
    func_800C4E50(ring);

    ++ring;

    ring->mode = 0x20;
    ring->offset = 0x5DC;
    ring->intensity = intensity;
    ring->color1[0] = 0;
    ring->color1[1] = 0;
    ring->color1[2] = 0;
    ring->color0[0] = 0xFF;
    ring->color0[1] = 0xFF;
    ring->color0[2] = 0xFF;
    ring->extent0 = span;
    ring->extent1 = 0;
    ring->source = source + 0x200;
    func_800C4E50(ring);

    record = ring + 1;
    asm ("" : : "r"(record));
    record->mode = 0x20;
    record->offset = 0x5DC;
    record->intensity = intensity;
    record->color1[0] = 0;
    record->color1[1] = 0;
    record->color1[2] = 0;
    record->color0[0] = 0xFF;
    record->color0[1] = 0xFF;
    record->color0[2] = 0xFF;
    record->extent0 = span;
    record->extent1 = 0;
    source += 0x400;
    record->source = source;
    func_800C4E50(record);
}

void func_80192544(void *unused, RoomFxControl *time, SceneE08RingEffectState *record) {
    Matrix matrix;
    Vec4 scratchScale, firstScale, secondScale;

    func_800C3238(3);
    record->rotation[0] = 0;
    record->rotation[1] = 0;
    record->rotation[2] = 0;
    RotMatrixYXZ(record->rotation, &matrix);
    memset(&firstScale, 0, 0x10);
    firstScale.x = record->firstScale;
    firstScale.y = record->firstScale;
    firstScale.z = record->firstScale;
    scratchScale = firstScale;
    ScaleMatrix(&matrix, &scratchScale);
    matrix.t[0] = D_8019956C;
    matrix.t[1] = 0;
    matrix.t[2] = D_8019957C;
    func_800C4FC4(D_801995F8, &matrix, 1);
    if (time->frame >= 0x3D) {
        func_800C3238(2);
        record->rotation[0] = 0x400;
        record->rotation[1] = 0x400;
        record->rotation[2] = 0;
        RotMatrixYXZ(record->rotation, &matrix);
        memset(&secondScale, 0, 0x10);
        secondScale.x = record->secondScale;
        secondScale.y = record->secondScale;
        secondScale.z = record->secondScale;
        firstScale = secondScale;
        ScaleMatrix(&matrix, &firstScale);
        matrix.t[0] = D_8019956C;
        matrix.t[1] = 0;
        matrix.t[2] = D_8019957C;
        func_800C4FC4(D_801995F8 + 0x18, &matrix, 0);
    }
}

void func_80192704(s32 arg0, s16 *rec, SceneE08RingEffectState *p) {
    u16 *q;
    s16 pad;

    q = (u16 *)p;
    if (p->firstScale < 0x960) {
        p->firstScale += 0x28;
    }
    if (rec[1] >= 0x3D) {
        q[9] += 0x190;
        if ((s16)q[9] >= 0x1449) {
            q[9] = 0x1448;
        }
    }
    if (rec[1] >= 0x6A) {
        ((s8 *)rec)[1] = 2;
    }
}

void func_80192788(void *arg0, void *arg1, SceneMovingSlots *slots) {
    unsigned int i;

    for (i = 0; i < 16; i++) {
        slots->active[i] = 0;
    }
}

void func_800C3098(int);

void func_801927A8(void *unused0, void *unused1, SceneMovingSlots *effect) {
    unsigned int i;
    SceneMovingVector *dst;
    SceneMovingWordLane *step;
    SceneMovingHalfLane *frame;
    Ovl178Position *owner;
    u8 scratchA[8];
    u8 scratchB[8];
    u8 *scratchPtr;
    u8 *table;
    int retA, retB;

    owner = FieldFx_GetScriptContext();
    i = 0;
    table = D_80198820;
    scratchPtr = scratchB;
    func_800C2EAC(owner->spriteMode);
    func_800C3098(0x10);
    func_800C3238(2);

    dst = effect->vector;
    asm("" : : "r"(dst));
    frame = (SceneMovingHalfLane *)dst;
    step = (SceneMovingWordLane *)frame;
    while (i < 16) {
        asm("" : "=r"(step), "=r"(frame) : "0"(step), "1"(frame));
        if (frame->active) {
            func_800C3134(table, step->life, scratchA);
            func_800C3134(table, step->life, scratchPtr);
            scratchB[0] = 0;
            scratchB[1] = 0;
            scratchB[2] = 0;
            D_800F3368.depth = 0;
            retA = GetTPage(1, 0, 0x340, 0x100) & 0xffff;
            retB = GetClut(0, 0x1D9) & 0xffff;
            func_800D3114((struct GteShortVector *)dst, 2, frame->range, 0x20, 0x90,
                           0x51, 0x10, retA, retB, frame->initialSize,
                           (struct RenderColor *)scratchA, (struct RenderColor *)scratchPtr, 1);
        }
        dst++;
        step = (SceneMovingWordLane *)((s32 *)step + 1);
        ++i;
        frame = (SceneMovingHalfLane *)((s16 *)frame + 1);
    }
}

void func_80192904(int unused, SceneMovingState *state, SceneMovingSlots *slots) {
    unsigned int i;

    for (i = 0; i < 16; ++i) {
        if (slots->active[i] == 0) {
            if (rand() % 15 == 0 && state->elapsedFrames < 120) {
                slots->angle[i] = rand() % 4096;
                slots->initialSize[i] = 0x80;
                slots->active[i] = 1;
                {
                    int randomValue = rand();
                    slots->radius[i] = 2000;
                    slots->extraRadius[i] = 2400;
                    slots->range[i] = randomValue % 600 + 100;
                }
                slots->life[i] = 0;
            }
        }
        if (slots->active[i] != 0) {
            slots->angle[i] = (u16)slots->angle[i] + (slots->range[i] >> 4);
            if (slots->active[i] == 1) {
                if (slots->extraRadius[i] < 20) {
                    slots->active[i] = 2;
                }
                slots->extraRadius[i] = (u16)slots->extraRadius[i] - 100;
                slots->radius[i] = (u16)slots->radius[i] - 20;
            }
            if (slots->active[i] == 2) {
                if (slots->radius[i] < 0) {
                    slots->active[i] = 0;
                }
                slots->extraRadius[i] = 0;
                slots->radius[i] = (u16)slots->radius[i] - 120;
            }
            slots->life[i]++;
            slots->vector[i].dx = (rsin(slots->angle[i]) *
                                   (slots->extraRadius[i] + slots->radius[i]) >> 12) + D_8019956C;
            slots->vector[i].dy = rcos(slots->angle[i]) *
                                  (slots->extraRadius[i] + slots->radius[i]) >> 12;
            slots->vector[i].dz = (rcos(slots->angle[i]) *
                                   (slots->extraRadius[i] + slots->radius[i]) >> 12) + D_8019957C;
            slots->vector[i].x = (rsin(slots->angle[i]) * slots->extraRadius[i] >> 12) + D_8019956C;
            slots->vector[i].y = rcos(slots->angle[i]) * slots->extraRadius[i] >> 12;
            slots->vector[i].z = (rcos(slots->angle[i]) * slots->extraRadius[i] >> 12) + D_8019957C;
            slots->vector[i].combinedX = (slots->vector[i].x + slots->vector[i].dx) >> 1;
            slots->vector[i].combinedY = (slots->vector[i].y + slots->vector[i].dy) >> 1;
            slots->vector[i].combinedZ = (slots->vector[i].z + slots->vector[i].dz) >> 1;
        }
    }
    if (state->elapsedFrames >= 181) {
        state->status = 2;
    }
}

void func_80192C78(void *arg0, void *arg1, SceneE08ScreenFade *fade) {
    fade->reserved = 0;
    fade->level = 0;
}

void func_80192C84(void *arg0, void *arg1, SceneE08ScreenFade *fade) {
    SceneE08FourBytes local;

    local = D_8018F04C;
    func_800D1AE0((unsigned char *)&local, fade->level, 1, 8);
}

void func_80192CCC(void *arg0, RoomFxControl *control, SceneE08ScreenFade *fade) {
    if (control->frame < 0x3D) {
        fade->phase = 1;
    } else {
        fade->phase = 2;
    }

    if (fade->phase == 1) {
        fade->level += 8;
        if (fade->level >= 0x80) {
            fade->level = 0x7F;
        }
    }

    if (fade->phase == 2) {
        fade->level -= 8;
        if (fade->level < 0) {
            fade->level = 0;
        }
    }

    if (control->frame >= 0x79) {
        control->state = 2;
    }
}

void func_80192D70(void *arg0, void *arg1, char *arg2) {
    char *end = arg2 + 0x20;

    do {
        *arg2++ = 0;
    } while (arg2 < end);
}

void func_800C2FF0(int, int);
void func_800C3098(int);
void func_80192D90(void *unused0, void *unused1, SceneE08OrbitParticles *effect) {
    SceneE08Matrix matrix;
    s16 temp[3];
    SceneE08Vector transformed, v;
    /* Keep the matrix scale live before the first rendering setup call. */
    register s16 scale asm("$19") = 0x1000;
    char *table = D_801996A0;
    Ovl178Position *owner;
    unsigned i;

    owner = FieldFx_GetScriptContext();
    i = 0;
    asm volatile("" : : "r"(scale));
    func_800C2EAC(owner->spriteMode);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);
    for (; i < 32; i++) {
        if (effect->active[i] == 1) {
            temp[0] = (rsin(effect->angle[i]) * effect->radius[i]) >> 12;
            temp[1] = 0;
            temp[2] = (rcos(effect->angle[i]) * effect->radius[i]) >> 12;
            matrix.m[2][2] = scale;
            matrix.m[1][1] = scale;
            matrix.m[0][0] = scale;
            matrix.t[2] = 0;
            matrix.t[1] = 0;
            matrix.t[0] = 0;
            matrix.m[2][1] = 0;
            matrix.m[2][0] = 0;
            matrix.m[1][2] = 0;
            matrix.m[1][0] = 0;
            matrix.m[0][2] = 0;
            matrix.m[0][1] = 0;
            memset(&v, 0, 0x10);
            v.x = effect->size[i];
            v.y = effect->size[i];
            v.z = scale;
            transformed = v;
            ScaleMatrix(&matrix, &transformed);
            matrix.t[0] = temp[0] + D_8019956C;
            matrix.t[1] = temp[1];
            matrix.t[2] = temp[2] + D_8019957C;
            func_800C3134(D_80198848, effect->frame[i], table);
            func_800C42A4((FieldGlowSprite *)table, &matrix, 1);
        }
    }
}

void func_80192F50(void *unused, RoomFxControl *condition_arg, SceneE08OrbitParticles *effect) {
    /* The byte cursor and loop-limit base remain pinned for matching. */
    RoomFxControl *condition = condition_arg;
    register char *base asm("$18") = (char *)effect;
    register char *state asm("$16") = base;
    int random;
    unsigned i = 0;

    do {
        if (((SceneE08OrbitParticles *)state)->active[0] == 0) {
            if (rand() % 30 == 0 && condition->frame < 50) {
                ((SceneE08OrbitParticles *)state)->active[0] = 1;
                ((SceneE08OrbitParticles *)state)->frame[0] = 0;
                effect->angle[i] = rand() % 4096;
                effect->size[i] = 0x7e8;
                effect->radius[i] = 0x7e8;
                random = rand();
                ((SceneE08OrbitParticles *)state)->radialSpeed[0] = 0;
                ((SceneE08OrbitParticles *)state)->angularSpeed[0] = random % 20 + 40;
            }
        } else {
            ((SceneE08OrbitParticles *)state)->frame[0] += 1;
            effect->angle[i] += ((SceneE08OrbitParticles *)state)->angularSpeed[0];
            effect->radius[i] -= ((SceneE08OrbitParticles *)state)->radialSpeed[0];
            ((SceneE08OrbitParticles *)state)->radialSpeed[0] += 2;
            effect->size[i] -= 40;
            if (effect->size[i] < 0) {
                effect->size[i] = 0;
            }
            if (effect->radius[i] < 0) {
                ((SceneE08OrbitParticles *)state)->active[0] = 0;
            }
        }
        i++;
        state++;
    } while (state < base + 32);

    if (condition->frame >= 121) {
        condition->state = 2;
    }
}

void func_80193114(void *arg0, void *arg1, short *state) {
    state[1] = -0x1000;
    state[4] = 0;
    state[5] = 0;
    state[6] = 0;
    state[0] = 0;
    state[2] = 0;
    state[8] = 0x270;
    state[9] = 0x1000;
    state[10] = 0x270;
    state[12] = 1;
}

/* Draw the scene model with the owner's texture page, then a glow sprite
 * whose size grows with the timer after tick 10 (capped at 0x7D00). */
void func_80193150(void *object, SceneGlowModelTimer *timer, SceneGlowModelState *state)
{
    RoomSpriteMatrix matrix;
    SceneGlowModelColor color;
    RoomFxVec4 scale;
    RoomFxVec4 source;
    SceneGlowModelOwner *owner;
    SceneGlowModelState *model;
    int size;

    model = state;
    owner = FieldFx_GetScriptContext();
    RotMatrix(&model->seed.vector, &matrix);
    memset(&source, 0, 0x10);
    source.x = model->scaleX;
    source.y = model->scaleY;
    source.z = model->scaleZ;
    scale = source;
    ScaleMatrix(&matrix, &scale);
    matrix.t[0] = D_8019956C;
    matrix.t[1] = g_RoomFloorY->y + model->height;
    matrix.t[2] = D_8019957C;
    FieldFx_ModelScrollUv((union FieldTexturedModel *)D_80199528, 0, 0);
    if (owner->mode == 0) {
        int tpage = GetTPage(0, 1, 0x340, 0x100);
        FieldFx_ModelSetTexture(tpage, GetClut(0, 0x1D7));
    }
    if (owner->mode == 1) {
        int tpage = GetTPage(0, 1, 0x340, 0x160);
        FieldFx_ModelSetTexture(tpage, GetClut(0, 0x1DB));
    }
    func_800C3134(D_80198860, timer->ticks, (unsigned char *)&color);
    FieldFx_ModelSetSemiTrans(1);
    FieldFx_ModelSaveColors(D_80199528);
    FieldFx_ModelScaleColors(D_80199528, model->depth);
    FieldFx_ModelDraw((union FieldTexturedModel *)D_80199528, &matrix);
    FieldFx_ModelRestoreColors(D_80199528);
    if (timer->ticks > 10) {
        size = (timer->ticks - 10) << 9;
        if (size > 0x7D00)
            size = 0x7D00;
        func_800C2EAC(3);
        func_800C2FF0(0x20, 0x20);
        func_800C3098(0x100);
        func_800C3238(2);
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
        memset(&source, 0, 0x10);
        source.x = size;
        source.y = size;
        source.z = size;
        ScaleMatrix(&matrix, &source);
        matrix.t[0] = D_8019956C;
        matrix.t[1] = 0;
        matrix.t[2] = D_8019957C;
        func_800C42A4(&D_801996B0, &matrix, 1);
    }
}

void func_80193400(void *arg0, RoomFxControl *signal, SceneGlowModelState *work) {
    work->seed.vector.y += 0xC8;
    if (signal->frame < 0x10) {
        u16 depth = work->depth;
        u16 height = work->height;

        depth += 8;
        height += 0x100;
        work->depth = depth;
        work->height = height;
    } else {
        u16 depth = work->depth;

        depth -= 4;
        work->depth = depth;
        work->scaleX += 0x50;
        work->scaleZ += 0x50;
    }
    if ((s16)work->depth <= 0) {
        signal->state = 2;
    }
}

void func_80193480(void *unused0, void *unused1, Ovl178RandomizedEffect *effect) {
    unsigned i;
    s32 threshold;

    for (i = 0; i < 10; i++) {
        D_80199658[i] = rand() % 4096;
        D_80199590[i] = rand() % 4096;
        effect->tail.flags[i] = 0;
        effect->offsets[i] = 0;
    }
    threshold = *FieldFx_VarPtr(2);
    if (rand() % 256 < threshold) {
        *FieldFx_VarPtr(1) = 1;
    } else {
        *FieldFx_VarPtr(1) = 0;
    }
}

/* Draw the ten limb beams that are switched on (mode 1). */
void func_801935A8(void *object, void *timer, SceneLimbBeams *beams)
{
    GteShortVector points[10];
    RenderColor head;
    RenderColor tail;
    ScenePlayerOrbAnchor *anchor;
    RenderMatrix *matrices;
    RenderMatrix *target;
    int mode;
    u32 i;
    int tpage;

    anchor = FieldFx_GetScriptContext();
    target = &g_PlayerEntity->render_object.matrices[2];
    func_800C2EAC(anchor->mode);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);
    D_800F3368.depth = 0x1F4;
    for (i = 0; i < 10; i++) {
        mode = beams->mode[i];
        if (mode == 1) {
            matrices = g_PlayerEntity->render_object.matrices;
            if (i == 0) {
                target = &matrices[18];
            }
            if (i == 1) {
                target = &matrices[24];
            }
            if (i == 2) {
                target = &matrices[28];
            }
            if (i == 3) {
                target = &matrices[24];
            }
            if (i == 4) {
                target = &matrices[28];
            }
            points[0].x = matrices[2].translation[0] + (rsin(D_80199658[i] + D_801994D8[i].swing) >> 5);
            points[0].y = matrices[2].translation[1];
            points[0].z = matrices[2].translation[2] + (rcos(D_80199658[i] + D_801994D8[i].swing) >> 5);
            points[1].x = D_801994D8[i].rise + matrices[2].translation[0];
            points[1].y = D_801994D8[i].rise + matrices[2].translation[1];
            points[1].z = D_801994D8[i].rise + matrices[2].translation[2];
            points[2].x = D_801994D8[i].bend + ((matrices[2].translation[0] + target->translation[0]) >> 1);
            points[2].y = D_801994D8[i].bend + ((matrices[2].translation[1] + target->translation[1]) >> 1);
            points[2].z = D_801994D8[i].bend + ((matrices[2].translation[2] + target->translation[2]) >> 1);
            points[3].x = D_801994D8[i].reach + target->translation[0];
            points[3].y = D_801994D8[i].reach + target->translation[1];
            points[3].z = D_801994D8[i].reach + target->translation[2];
            points[4].x = target->translation[0];
            points[4].y = target->translation[1];
            points[4].z = target->translation[2];
            func_800C3134(D_801989BC, beams->fade[i], (unsigned char *)&head);
            func_800C3134(D_801989D0, beams->fade[i], (unsigned char *)&tail);
            tpage = GetTPage(0, 0, 0x340, 0x100);
            func_800D3114(points, 4, 100, 0x40, 0x80, 0x9C, 0x10, tpage,
                          GetClut(0, 0x1D7), 0x80, &tail, &tail, mode);
        }
    }
}

void func_801938FC(int unused, SceneE08RandomState *state, SceneE08RandomEntries *entries) {
    unsigned int i;
    for (i = 0; i < 10; ++i) {
        if (entries->active[i] == 1) {
            ++entries->counter[i];
            D_801994D8[i].rise = rand() % 160 - 80;
            D_801994D8[i].bend = rand() % 160 - 80;
            D_801994D8[i].reach = rand() % 160 - 80;
            D_801994D8[i].swing = rand() % 512 - 256;
        } else if (rand() % 20 == 0) {
            entries->active[i] = 1;
        }
    }
    if (state->elapsedFrames >= 61) {
        state->status = 2;
    }
}

/*
 * The module class of scene_e08's fourth module (effect palettes, the
 * layered sprite and the arm glow): the seven methods its class table
 * lists in front of the module. The registration and start methods act
 * whatever the field engine's status. The script and the lists are named
 * in the symbol file (scene_e08_modules.h).
 */

ROOMLIB_RETURN_ZERO(SceneE08_LayeredSpriteNop0)

ROOMLIB_PLANT_TABLE(SceneE08_LayeredSpritePlantScript, g_SceneE08LayeredSpriteScript)

ROOMLIB_SPAWN6(SceneE08_LayeredSpriteSpawn6)

ROOMLIB_REGISTER_TABLE_ANY(SceneE08_LayeredSpriteRegister, g_SceneE08LayeredSpriteDrawList)

ROOMLIB_START_ANY(SceneE08_LayeredSpriteStart, g_SceneE08LayeredSpriteUpdateList,
                  g_SceneE08LayeredSpriteInitList, g_SceneE08LayeredSpriteSpawnLayout,
                  SceneE08_LayeredSpriteClose)

ROOMLIB_CLOSE_TARGET(SceneE08_LayeredSpriteClose)

ROOMLIB_RETURN_ZERO(SceneE08_LayeredSpriteNop6)

/* The 0x80199560 glow sprite, field by field as Scene_DrawLayeredSprite
 * stores its colour. */
void func_80193C68(SceneGrabHead *actor, void *unused, SceneE08Context *state) {
    FieldActor *source;
    RenderMatrix *block;
    void *sound;
    RoomSoundSlot *soundAddress;
    int sixtyFour;
    int one;
    u16 half128;
    u8 byte128;
    int thirtyTwo;
    register int two asm("$8");
    int minus300;
    int scratch; /* -50 offset, later the sound pan */
    int scale;
    FieldFx_SetScriptContext(state);
    source = actor->actor;
    state->source = source;
    block = source->render_object.matrices;
    state->rootMatrix = *block;
    state->spriteAsset = Asset_SearchByKeyType(0x2E);
    scale = 30;
        sixtyFour = 64;
    one = 1;
    state->hitReload = scale;
    asm volatile("" : : "m"(state->hitReload));
    scratch = -50;
    half128 = 128;
    byte128 = 128;
    thirtyTwo = 32;
    two = 2;
    asm volatile("" : : "r"(two));
    /* hitTimer and hitRequest: as field stores GCC would sink them past
     * the global stores below. */
    *(s16 *)((char *)state + 0x2A) = 0;
    *(s16 *)((char *)state + 0x2C) = 0;
    D_80199558 = scratch;
    D_80199580.offset = -100;
    D_80199580.g = 16;
    D_80199580.b = 5;
    D_80199564 = 70;
    D_80199565 = 3;
    D_80199568 = 1;
    D_80199540.cell = 96;
    D_80199555 = one;
    D_80199580.clut = one;
    soundAddress = (RoomSoundSlot *)D_800B0E64;
    D_80199554 = sixtyFour;
    D_8019955A = half128;
    D_80199550 = byte128;
    D_80199551 = sixtyFour;
    D_80199552 = thirtyTwo;
    D_80199556 = two;
    D_80199580.cell = sixtyFour;
    D_80199580.depth = half128;
    D_80199580.r = thirtyTwo;
    D_80199580.flip = two;
    D_8019956A = half128;
    D_80199560 = byte128;
    D_80199561 = byte128;
    D_80199562 = byte128;
    D_80199566 = 0;
    D_80199540.clut = 7;
    D_80199540.offset = 0;
    D_80199540.depth = half128;
    D_80199540.r = byte128;
    D_80199540.g = byte128;
    D_80199540.b = byte128;
    D_80199540.flip = 0;
    D_80199570.cell = 0;
    D_80199570.clut = 0;
    minus300 = -300;
    D_80199570.offset = minus300;
    D_80199570.depth = half128;
    D_80199570.r = byte128;
    D_80199570.g = byte128;
    D_80199570.b = byte128;
    D_80199570.flip = 0;
    sound = *(void *volatile *)&soundAddress->channel;
    D_801994C8.cell = 68;
    D_801994C8.clut = two;
    D_801994C8.offset = minus300;
    D_801994C8.depth = half128;
    D_801994C8.r = byte128;
    D_801994C8.g = sixtyFour;
    D_801994C8.b = thirtyTwo;
    D_801994C8.flip = 0;
    D_801994A0 = minus300;
    D_801994A2 = half128;
    D_80199490 = 0;
    D_80199491 = 0;
    D_80199492 = 0;
    D_80199494 = 0;
    D_80199495 = 0;
    D_80199496 = 0;
    D_80199498 = byte128;
    D_80199499 = sixtyFour;
    D_8019949A = thirtyTwo;
    D_8019949C = byte128;
    D_8019949D = sixtyFour;
    D_8019949E = thirtyTwo;
    if (sound) {
        scratch = 0x7F;
        Akao_SendTableCommand(soundAddress->channel, 0x604, 0, 0x80, scratch);
    }
}

void func_80193F70(void) {
}

ROOM_REARM_TIMER_TICK(func_80193F78, 0x28)

void func_80193FF8(s32 arg0, s32 arg1, SceneE08TrailGlow *p) {
    Ovl178Position *view;
    s32 z;

    view = FieldFx_GetScriptContext();
    FieldFx_RegPtr(1);
    p->acceleration = 0x14;
    p->unused88 = 0x80;
    p->speed = 0;
    p->rotation[0].x = 0;
    p->rotation[0].y = 0x400;
    p->rotation[0].z = 0;
    p->trail[0].x = view->x - 0x15E;
    p->trail[0].y = view->y + 0x78;
    z = view->z;
    p->scale[0].x = 0;
    p->scale[0].y = 0x800;
    p->scale[0].z = 0;
    p->trail[0].z = z;
}

void func_800C2FF0(int, int);
void func_800C3098(int);
void func_80194090(void *arg0, void *arg1, char *effects) {
    GteMatrix matrix;
    GteVector scratch, vec;
    Ovl178Position *owner;
    s32 third;
    register char *base asm("$20") = effects;
    register SceneE08PositionSample *state asm("$17");
    register SceneE08TrailLane *phase asm("$16");
    s16 *alpha;
    unsigned i;
    unsigned angleOffset;
    owner = FieldFx_GetScriptContext();
    i = 0;
    alpha = &D_8019955A;
    /* This single-pass block preserves the original setup scheduling. */
    { asm volatile("" : : : "memory"); 
        state = (SceneE08PositionSample *)base;
        phase = (SceneE08TrailLane *)base;
        angleOffset = 0x30;
        func_800C2EAC(owner->spriteMode);
        func_800C2FF0(0x40, 0x20);
        func_800C3098(0x10);
        func_800C3238(2);
     }

    for (; i < 3; ++i, angleOffset += 8) {
        RotMatrix((GteShortVector *)(base + angleOffset), &matrix);
        memset(&vec, 0, 0x10);
        vec.x = phase->scale.x;
        vec.y = phase->scale.y;
        vec.z = phase->scale.z;
        scratch = vec;
        ScaleMatrix(&matrix, &scratch);
        matrix.t[0] = state->x;
        matrix.t[1] = state->y;
        third = state->z;
        *alpha = 0x80 - (i << 5);
        matrix.t[2] = third;
        if (i == 0) {
            *alpha = 0xC0;
        }
        func_800C42A4((FieldGlowSprite *)((char *)alpha - 10), &matrix, 0);
        state++;
        phase = (SceneE08TrailLane *)(&phase->step + 1);
    }
    matrix.t[0] = ((SceneE08TrailGlow *)base)->trail[0].x;
    matrix.t[1] = g_RoomFloorY->y;
    matrix.t[2] = ((SceneE08TrailGlow *)base)->trail[0].z;
    func_800C42A4(&D_80199580, &matrix, 0);
}

void func_80194214(void *unused, RoomFxControl *phase, SceneE08TrailGlow *effect) {
    Ovl178Position *owner;
    s16 point[3];
    int i;
    int angle;
    u16 preserved;
    s32 x, z;

    owner = FieldFx_GetScriptContext();
    for (i = 2; i != 0; i--) {
        __builtin_memcpy(&effect->rotation[i], &effect->rotation[i - 1], 8);
        effect->trail[i] = effect->trail[i - 1];
        __builtin_memcpy(&effect->scale[i], &effect->scale[i - 1], 8);
    }

    if (phase->frame >= 0x33) {
        angle = effect->speed + effect->acceleration;
        preserved = effect->rotation[0].z;
        effect->speed = angle;
        effect->rotation[0].z = preserved;
        effect->trail[0].x = effect->trail[0].x - (s16)angle;
    }
    if ((u16)(phase->frame - 0x22) < 9) {
        effect->scale[0].x += 0x12C;
    }
    if (phase->frame >= 0x4C) {
        effect->scale[0].y += 0x96;
    }
    if (phase->frame == 0x78) {
        phase->state = 2;
    }

    x = effect->trail[0].x;
    point[0] = x;
    point[1] = effect->trail[0].y;
    z = effect->trail[0].z;
    point[2] = z;
    if (Geo_PointInPoly(x << 16, z << 16, D_8009D248, D_8009D1CC) == 0) {
        phase->state = 2;
    }
    if (func_800C6B90(point, 0x320) != 0) {
        owner->playerHit = 1;
    }
}

void func_801943CC(void *arg0, void *arg1, SceneLayeredSpriteState *out)
{
    Ovl178Position *position = FieldFx_GetScriptContext();

    out->center[0].fixed = position->x << 16;
    out->center[1].fixed = position->y << 16;
    out->center[2].fixed = position->z << 16;
    out->initialSeed.vector.x = 0;
    out->initialSeed.vector.y = 0;
    out->initialSeed.vector.z = 0;
    out->phase = *FieldFx_RegPtr(2);
    out->direction = *FieldFx_RegPtr(3);
    out->drift = *FieldFx_VarPtr(1);
    if (out->direction == 0)
        out->velocity[2] = 0x124F80;
    else
        out->velocity[2] = 0xFFEDB080;
    out->velocity[0] = 0x7A120;
    out->velocity[1] = 0xFFF551A0;
}

void func_800C2FF0(int, int);
void func_800C3098(int);

void func_80194498(void *unused, u16 *phase, SceneLayeredSpriteState *state) {
    RoomSpriteMatrix matrix;
    RoomFxVec4 firstScale, layerScale;
    char *owner;
    s16 *alpha;
    unsigned i;
    s16 *alphaBase;
    u8 *red;

    owner = FieldFx_GetScriptContext();
    func_800C2EAC(((Ovl178Position *)owner)->spriteMode);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

    RotMatrix(&state->initialSeed.vector, &matrix);
    firstScale = D_8018F050;
    ScaleMatrix(&matrix, &firstScale);
    if (phase[1] & 1) {
        D_80199560 = 0xFF;
        D_80199561 = 0xA0;
    } else {
        D_80199560 = 0xA0;
        D_80199561 = 0xFF;
    }
    D_80199562 = 0x60;
    asm("" : : "m"(D_80199562));

    alphaBase = &D_8019956A;
    alpha = alphaBase;
    matrix.t[0] = state->center[0].parts.integer;
    red = (u8 *)((char *)alpha - 10);
    matrix.t[1] = state->center[1].parts.integer;
    matrix.t[2] = state->center[2].parts.integer;
    *alpha = 0x80;
    func_800C42A4((FieldGlowSprite *)((char *)alphaBase - 10), &matrix, 0);

    matrix.t[0] = state->center[0].parts.integer;
    matrix.t[1] = g_RoomFloorY->y;
    matrix.t[2] = state->center[2].parts.integer;
    *alpha = 0x20;
    func_800C42A4((FieldGlowSprite *)((char *)alpha - 10), &matrix, 0);

    *red = 0x40;
    D_80199561 = 0x20;
    D_80199562 = 0;
    for (i = 0; i < 5; ++i) {
        RotMatrix(&state->layerSeeds[i].vector, &matrix);
        layerScale = D_8018F050;
        ScaleMatrix(&matrix, &layerScale);
        owner = (char *)alpha;
        matrix.t[0] = state->layers[i].x;
        matrix.t[1] = state->layers[i].y;
        matrix.t[2] = state->layers[i].z;
        *alpha = (5 - i) * 31;
        func_800C42A4((FieldGlowSprite *)(owner - 10), &matrix, 0);
    }
}

void func_801946D4(void *unused, RoomFxControl *control, char *state)
{
    Ovl178Position *owner = FieldFx_GetScriptContext();
    /* Keeps the later history-copy loop based at its original offset. */
    register char *cursor asm("$6");
    s16 point[3];
    u32 count;
    int random;
    int x;
    int y;
    int z;
    int nextX;
    s32 xVelocity;
    s32 zBase;
    s32 zVelocity;
    s32 yBase;
    int nextY;
    register s32 yVelocity asm("$7");
    int nextZ;
    int zArg;
    /* The Y velocity and later screen limit share the original argument register. */
    register u16 screenLimit asm("$7");
    int pointValue;
    s16 *ground;
    count = 4;
    for (; count != 0; --count)
    {
        ((SceneLayeredSpriteState *)state)->layers[count] = ((SceneLayeredSpriteState *)state)->layers[count - 1];
        ((SceneLayeredSpriteState *)state)->layerSeeds[count] = ((SceneLayeredSpriteState *)state)->layerSeeds[count - 1];
    }

    count = 7;
    x = ((SceneLayeredSpriteState *)state)->center[0].parts.integer;
    y = ((SceneLayeredSpriteState *)state)->center[1].parts.integer;
    z = ((SceneLayeredSpriteState *)state)->center[2].parts.integer;
    cursor = (char *)&((SceneLayeredSpriteState *)state)->layers[1];
    ((SceneLayeredSpriteState *)state)->layers[0].x = x;
    ((SceneLayeredSpriteState *)state)->layers[0].y = y;
    ((SceneLayeredSpriteState *)state)->layers[0].z = z;
    ((SceneLayeredSpriteState *)state)->layerSeeds[0] = ((SceneLayeredSpriteState *)state)->initialSeed;
    for (; count != 0; --count, cursor -= 8)
    {
        ((SceneLayeredHistoryLane *)cursor)->olderEntry = ((SceneLayeredHistoryLane *)cursor)->entry;
    }

    ((SceneLayeredSpriteState *)state)->history[0] = *(RoomFxSeed8 *)&((SceneLayeredSpriteState *)state)->layers[4];
    /* The volatile reads preserve the retail load order. */
    zBase = ((SceneLayeredSpriteState *)state)->center[2].fixed;
    zVelocity = ((SceneLayeredSpriteState *)state)->velocity[2];
    xVelocity = *((volatile s32 *)&((SceneLayeredSpriteState *)state)->velocity[0]);
    yBase = *((volatile s32 *)&((SceneLayeredSpriteState *)state)->center[1].fixed);
    yVelocity = *((volatile s32 *)&((SceneLayeredSpriteState *)state)->velocity[1]);
    nextZ = zBase + zVelocity;
    nextY = yBase + yVelocity;
    nextX = ((SceneLayeredSpriteState *)state)->center[0].fixed + xVelocity;
    screenLimit = D_8009D1CC;
    ((SceneLayeredSpriteState *)state)->center[0].fixed = nextX;
    *((s32 *) (state + 4)) = nextY;
    *((s32 *) (state + 8)) = nextZ;
    zArg = (unsigned)(nextZ >> 16) << 16;
    if (Geo_PointInPoly(((unsigned)((SceneLayeredSpriteState *)state)->center[0].parts.integer) << 16, zArg, D_8009D248, screenLimit) == 0)
    {
        control->state = 2;
    }
    if (control->frame < 50)
    {
        random = rand();
        ((SceneLayeredSpriteState *)state)->initialSeed.vector.y = (random % 1024) - 0x200;
        ground = &g_RoomFloorY->y;
        ((SceneLayeredSpriteState *)state)->initialSeed.vector.x = 0x400;
        ((SceneLayeredSpriteState *)state)->initialSeed.vector.z += 0x270;
        if ((((SceneLayeredSpriteState *)state)->center[1].fixed) < (*ground))
        {
            int gravity;
            gravity = 0xC350;
            ((SceneLayeredSpriteState *)state)->velocity[1] += gravity;
        }
        else
        {
            /* Preserve the second ground-height read after clearing velocity. */
            PE1_COMPILER_MEMORY_BARRIER();
            ((SceneLayeredSpriteState *)state)->velocity[0] = 0;
            ((SceneLayeredSpriteState *)state)->velocity[1] = 0;
            ((SceneLayeredSpriteState *)state)->velocity[2] = 0;
            ((SceneLayeredSpriteState *)state)->center[1].fixed = *ground;
        }
    }
    else
    {
        ((SceneLayeredSpriteState *)state)->initialSeed.vector.x = 0x400;
        ((SceneLayeredSpriteState *)state)->initialSeed.vector.y = 0;
        ++(((SceneLayeredSpriteState *)state)->phase);
        ((SceneLayeredSpriteState *)state)->initialSeed.vector.z += 0x270;
        ((SceneLayeredSpriteState *)state)->velocity[0] -= (((SceneLayeredSpriteState *)state)->drift) * 8;
        ((SceneLayeredSpriteState *)state)->velocity[2] = rsin((((SceneLayeredSpriteState *)state)->phase) << 6) * 1500;
    }
    pointValue = ((SceneLayeredSpriteState *)state)->center[0].parts.integer;
    point[0] = pointValue;
    pointValue = ((SceneLayeredSpriteState *)state)->center[1].parts.integer;
    point[1] = pointValue;
    pointValue = ((SceneLayeredSpriteState *)state)->center[2].parts.integer;
    point[2] = pointValue;
    if (func_800C6B90(point, 0xC8))
    {
        owner->playerHit = 1;
    }
}

void func_80194988(void *arg0, void *arg1, Ovl178MotionState *state) {
    Ovl178MotionState *state_reg;
    Ovl178Position *position;
    int z;

    state_reg = state;
    position = FieldFx_GetScriptContext();

    state_reg->x = position->x - 0xC8;
    state_reg->y = position->y;
    z = position->z;
    state_reg->rotation.x = 0;
    state_reg->rotation.y = 0;
    state_reg->rotation.z = 0;
    state_reg->scale = 0xC04;
    state_reg->frame = 0;
    state_reg->shrink = 0;
    state_reg->depth = 0;
    state_reg->z = z;
}

void func_800C2FF0(s32, s32);
void func_800C3098(s32);

void func_801949F4(s32 arg0, RoomFxControl *arg1, Ovl178MotionState *rec) {
    Ovl178Position *ctx;
    u32 idx;
    Ovl178MotionState *r;
    GteMatrix buf;
    GteVector vdst;
    GteVector vsrc;

    r = rec;
    ctx = FieldFx_GetScriptContext();
    idx = ((rec->frame >> 4) << 1) << 1;
    if (idx >= 0xD) {
        idx = 0xC;
    }
    func_800C2EAC(ctx->spriteMode);
    func_800C2FF0(0x40, 0x40);
    func_800C3098(0x10);
    func_800C3238(2);
    RotMatrix(&r->rotation, &buf);
    memset(&vsrc, 0, 0x10);
    vsrc.x = r->scale;
    vsrc.y = r->scale;
    vsrc.z = 0x1000;
    vdst = vsrc;
    ScaleMatrix(&buf, &vdst);
    func_800C3134(&D_80198AD4, arg1->frame, &D_80199570);
    buf.t[0] = r->x;
    buf.t[1] = r->y;
    buf.t[2] = r->z;
    D_80199570.cell = idx;
    D_80199570.depth = r->depth;
    func_800C42A4(&D_80199570, &buf, 1);
}

void func_80194B44(s32 arg0, s16 *rec, u16 *p)
{
  u8 *b = (u8 *) p;
  u16 *q;
  b[0x22] += 0x10;
  p[14] -= p[15];
  p[15] += 0xB;
  p[16] += 8;
  q = p;
  if (((s16) p[16]) >= 0x81)
  {
    p[16] = 0x80;
  }
  if (((s16) p[14]) < 0)
  {
    p[14] = 0;
  }
  if (b[0x22] >= 0xC9)
  {
    b[0x22] = 0xC8;
  }
  {
    s32 lvl = ((u8 *) q)[0x22] >> 4;
    if (lvl >= 4)
    {
      q[10] += 0xC8;
    }
  }
  if (rec[1] == 0x78)
  {
    ((s8 *) rec)[1] = 2;
  }
}

void func_80194C04(void *arg0, void *arg1, SceneE08ExpandingGlow *glow) {
    Ovl178Position *position = FieldFx_GetScriptContext();

    glow->x = position->x - 0x15E;
    glow->y = position->y;
    glow->z = position->z;
    glow->scale = 0;
    glow->growth = 0;
    glow->depth = 0xFF;
}

void func_80194C60(void *arg0, void *arg1, SceneE08ExpandingGlow *glow) {
    SceneE08Matrix matrix;
    SceneE08Vector scratch_scale;
    SceneE08Vector draw_scale;
    Ovl178Position *owner;
    s16 *draw_slot;

    owner = FieldFx_GetScriptContext();
    func_800C2EAC(owner->spriteMode);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

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

    memset(&draw_scale, 0, 0x10);
    draw_scale.x = glow->scale;
    draw_scale.y = glow->scale;
    draw_scale.z = 0x1000;
    scratch_scale = draw_scale;
    ScaleMatrix(&matrix, &scratch_scale);

    matrix.t[0] = glow->x;
    matrix.t[1] = glow->y;
    matrix.t[2] = glow->z;

    draw_slot = &D_801994D2;
    *draw_slot = glow->depth;
    func_800C42A4((FieldGlowSprite *)((char *)draw_slot - 0xA), &matrix, 1);
}

void func_80194D74(s32 arg0, RoomFxControl *rec, SceneE08ExpandingGlow *p) {
    s16 v;

    p->scale += p->growth;
    p->growth += 10;
    v = p->depth;
    if (v >= 9) {
        p->depth = v - 8;
    }
    if (rec->frame == 0x3C) {
        rec->state = 2;
    }
}

void func_80194DD0(void) {
}

/* Draw a glow sprite on the player's hand and elbow bones, then a ribbon
 * bent between them through the five script offsets. */
void func_80194DD8(void *object, void *timer, SceneArmGlow *glow)
{
    GteMatrix matrix;
    GteShortVector points[7];
    RenderColor head;
    RenderColor tail;
    GteVector scale;
    GteVector source;
    GteVector second;
    ScenePlayerOrbAnchor *anchor;
    RenderMatrix *matrices;
    int tpage;

    anchor = FieldFx_GetScriptContext();
    matrices = anchor->actor->render_object.matrices;
    head = D_8018F060;
    tail = D_8018F064;
    func_800C2EAC(anchor->mode);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);
    D_800F3368.depth = 0x1F4;
    D_801994C8.depth = glow->frame;
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
    memset(&source, 0, 0x10);
    source.x = glow->scale;
    source.y = glow->scale;
    source.z = 0x1000;
    scale = source;
    ScaleMatrix(&matrix, &scale);
    matrix.t[0] = matrices[32].translation[0];
    matrix.t[1] = matrices[32].translation[1];
    matrix.t[2] = matrices[32].translation[2];
    func_800C42A4(&D_801994C8, &matrix, 1);
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
    memset(&second, 0, 0x10);
    second.x = glow->scale;
    second.y = glow->scale;
    second.z = 0x1000;
    source = second;
    ScaleMatrix(&matrix, &source);
    matrix.t[0] = matrices[22].translation[0];
    matrix.t[1] = matrices[22].translation[1];
    matrix.t[2] = matrices[22].translation[2];
    func_800C42A4(&D_801994C8, &matrix, 1);
    points[0].x = matrices[32].translation[0];
    points[0].y = matrices[32].translation[1];
    points[0].z = matrices[32].translation[2];
    points[1].x = glow->offsets[0][0] + matrices[32].translation[0];
    points[1].y = glow->offsets[0][1] + matrices[32].translation[1];
    points[1].z = glow->offsets[0][2] + matrices[32].translation[2];
    points[2].x = glow->offsets[1][0] + matrices[32].translation[0];
    points[2].y = glow->offsets[1][1] + matrices[32].translation[1];
    points[2].z = glow->offsets[1][2] + matrices[32].translation[2];
    points[3].x = glow->offsets[4][0] + ((matrices[32].translation[0] + matrices[22].translation[0]) >> 1);
    points[3].y = glow->offsets[4][1] + ((matrices[32].translation[1] + matrices[22].translation[1]) >> 1);
    points[3].z = glow->offsets[4][2] + ((matrices[32].translation[2] + matrices[22].translation[2]) >> 1);
    points[4].x = glow->offsets[2][0] + matrices[22].translation[0];
    points[4].y = glow->offsets[2][1] + matrices[22].translation[1];
    points[4].z = glow->offsets[2][2] + matrices[22].translation[2];
    points[5].x = glow->offsets[3][0] + matrices[22].translation[0];
    points[5].y = glow->offsets[3][1] + matrices[22].translation[1];
    points[5].z = glow->offsets[3][2] + matrices[22].translation[2];
    points[6].x = matrices[22].translation[0];
    points[6].y = matrices[22].translation[1];
    points[6].z = matrices[22].translation[2];
    tpage = GetTPage(0, 0, 0x340, 0x100);
    func_800D3114(points, 7, 300, 0x40, 0x80, 0x9C, 0x10, tpage,
                  GetClut(0, 0x1D7), 0x80, &head, &tail, 1);
}

void func_80195210(void *unused, Ovl178EffectController *state,
                   Ovl178RandomizedEffect *effect) {
    unsigned i;

    if (effect->extent < 3000) {
        effect->extent += 50;
    }
    if (effect->intensity < 128) {
        effect->intensity += 8;
    }

    for (i = 0; i < 12; i++) {
        effect->offsets[i] = rand() % 400 - 200;
    }
    for (i = 0; i < 3; i++) {
        effect->tail.variation[i] = rand() % 300 - 150;
    }
    if (state->timer >= 36) {
        state->mode = 2;
    }
}
