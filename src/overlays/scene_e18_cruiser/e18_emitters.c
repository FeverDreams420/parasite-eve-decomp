/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/scene_e18_effects.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_anim_pool.h"
#include "pe1/asset_find.h"
#include "pe1/field_actor.h"
#include "pe1/scene_e18_pulse.h"
#include "pe1/render_object.h"
#include "pe1/psyq_gpu.h"
#include "pe1/gte_sine_table.h"
#include "pe1/gte.h"
#include "pe1/field_sprite_params.h"
#include "pe1/field_texture_tables.h"
#include "pe1/gte_types.h"

typedef struct Effect {
    s16 field00;
    s16 field02;
    s16 field04;
    s16 field06;
    s16 frame;
    s16 speed;
    s16 size;
} Effect;

typedef struct SceneE18Object {
    char reserved[0x1FC];
    s32 x, y, z;
} SceneE18Object;

typedef struct SceneE18ObjectLink {
    char reserved[8];
    SceneE18Object *object;
} SceneE18ObjectLink;

typedef struct SceneE18PulseState {
    s16 stage;
    u16 ticks;
} SceneE18PulseState;

typedef struct {
    GteShortVector position;
    u16 base;
} SceneE18PaletteEffect;

typedef struct SceneE18WaveEffect {
    s16 x, y, z, field06;
    s16 field08, field0a, field0c, field0e;
    s16 size, wave, amplitude;
} SceneE18WaveEffect;

#include "e18_emitters_externs.h"

/* Spinning shell controller: loads the shell model, then spins it around
 * the room model while it widens and rises; after frame 8 the spin and the
 * widening slow down, it brightens until frame 16 and fades after frame
 * 18. It finishes (setting the model's stage byte to 4) once the model's
 * seventh animation reaches its last frame. */
int func_801924E8(int mode, SceneE18SpinShell *shell) {
    GteMatrix matrix;
    GteVector scaleArg;
    GteVector scale;
    SceneE18Instance *instance = ANIM_OWNER_AS(SceneE18Actor)->instance;
    int c;
    int s;

    switch (mode) {
    case 0:
        D_801941C0 = Asset_FindTable08ByU32Key(D_800B0E64[0], 0xC54C7704);
        FieldModel_ScrollUv(D_801941C0, 0, 0);
        shell->width = 0x400;
        shell->heightGrowth = 0x40;
        shell->height = 0;
        shell->widthGrowth = 0;
        shell->angle = 0;
        shell->turn = -0x200;
        shell->brightness = 0;
        break;
    case 1:
        if (*instance->owner->stage == 1)
            *instance->owner->stage = 2;
        if (instance->animationId == 7
            && instance->animation.part.frame >= instance->frameLimit - 1U) {
            *instance->owner->stage = 4;
            return 1;
        }
        shell->angle += shell->turn;
        if (D_800E27EC >= 9)
            shell->turn -= 8;
        shell->width += shell->widthGrowth;
        if (shell->width < 0x40)
            shell->width = 0x40;
        if (D_800E27EC >= 9)
            shell->widthGrowth -= 2;
        shell->height += shell->heightGrowth;
        shell->heightGrowth += 6;
        if (D_800E27EC < 0x10) {
            shell->brightness += 8;
            break;
        }
        if (D_800E27EC < 0x13)
            return 0;
        if (shell->brightness != 0) {
            shell->brightness -= 5;
            if (shell->brightness < 0)
                shell->brightness = 0;
        }
        return 0;
    case 2:
        if (shell->brightness == 0)
            return 0;
        c = rcos(shell->angle);
        s = rsin(shell->angle);
        matrix.m[0][2] = s;
        matrix.m[2][0] = -s;
        matrix.m[0][0] = c;
        matrix.m[2][2] = c;
        matrix.t[2] = 0;
        matrix.t[1] = 0;
        matrix.t[0] = 0;
        matrix.m[2][1] = 0;
        matrix.m[1][2] = 0;
        matrix.m[1][0] = 0;
        matrix.m[0][1] = 0;
        matrix.m[1][1] = 0x1000;
        memset(&scale, 0, 0x10);
        scale.x = shell->width;
        scale.y = shell->height;
        scale.z = shell->width;
        scaleArg = scale;
        ScaleMatrix(&matrix, &scaleArg);
        matrix.t[0] = instance->transform.t[0];
        matrix.t[1] = instance->transform.t[1];
        matrix.t[2] = instance->transform.t[2];
        {
            int kind;
            int palette;
            int page = (D_800E2850[D_800E11FA] | GetTPage(0, 1, 0, 0)) & 0xFFFF;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800C6EC0(page, GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 8
                                                                         : palette + 4));
        }
        FieldModel_SetSemiTrans(1);
        FieldModel_SaveColors(D_801941C0);
        FieldModel_ScaleColors(D_801941C0, shell->brightness);
        FieldModel_Draw(D_801941C0, &matrix);
        FieldModel_RestoreColors(D_801941C0);
        break;
    }
    return 0;
}

/* Flank model particle: swings around the room model while it widens and
 * fades; the leading particle releases the actor once the player comes
 * within a quarter of its width. Each particle that fades out counts
 * D_801941C8 down. */
int func_801928CC(int mode, SceneE18FlankParticle *particle) {
    GteMatrix matrix;
    GteVector scaleArg;
    GteVector scale;
    int c;
    int s;

    if (mode == 1) {
        particle->angle += particle->turn;
        particle->width += particle->widthGrowth;
        particle->height += particle->heightGrowth;
        particle->widthGrowth += 0x30;
        particle->heightGrowth += 0x20;
        if (particle->leader) {
            int distance;
            int dz;
            int range;
            distance = RoomMain_ActorPtr->render_object.model_matrix.translation[0] - particle->position[0];
            dz = RoomMain_ActorPtr->render_object.model_matrix.translation[2] - particle->position[2];
            distance = func_8005186C(distance * distance + dz * dz);
            range = particle->width / 4;
            if (distance < range) {
                RoomMain_ActorPtr->state->flags |= 0x4000;
                ANIM_OWNER_AS(SceneE18Actor)->instance->owner->flags |= 0x80000000;
                particle->leader = 0;
            }
        }
        particle->brightness -= 10;
        if (particle->brightness < 10) {
            D_801941C8--;
            return 1;
        }
    } else if (mode == 2) {
        c = rcos(particle->angle);
        s = rsin(particle->angle);
        matrix.m[0][2] = s;
        matrix.m[2][0] = -s;
        matrix.m[0][0] = c;
        matrix.m[2][2] = c;
        matrix.t[2] = 0;
        matrix.t[1] = 0;
        matrix.t[0] = 0;
        matrix.m[2][1] = 0;
        matrix.m[1][2] = 0;
        matrix.m[1][0] = 0;
        matrix.m[0][1] = 0;
        matrix.m[1][1] = 0x1000;
        memset(&scale, 0, 0x10);
        scale.x = particle->width;
        scale.y = particle->height;
        scale.z = particle->width;
        scaleArg = scale;
        ScaleMatrix(&matrix, &scaleArg);
        matrix.t[0] = particle->position[0];
        matrix.t[1] = particle->position[1];
        matrix.t[2] = particle->position[2];
        FieldModel_SaveColors(D_801941C4);
        FieldModel_ScaleColors(D_801941C4, particle->brightness > 0x80 ? 0x80 : particle->brightness);
        FieldModel_Draw(D_801941C4, &matrix);
        FieldModel_RestoreColors(D_801941C4);
    }
    return 0;
}

/* Flank model emitter: loads the flank model, then every sixth frame
 * releases one of four model particles on alternating sides of the room
 * model (the first one plays the two launch sounds at the model's root
 * joint) and finishes once the particles have counted D_801941C8 down. */
int func_80192B8C(int mode, SceneE18EmitterState *state) {
    SceneE18FlankParticle *particle;
    SceneE18Actor *actor;
    int side;

    switch (mode) {
    case 0:
        D_801941C8 = 4;
        D_801941C4 = Asset_FindTable08ByU32Key(D_800B0E64[0], 0xC58C7704);
        FieldModel_ScrollUv(D_801941C4, 0, 0);
        state->count = 0;
        state->delay = 0;
        return func_800CE560(ANIM_POOL_AS(SceneE18Emitter)->pool, 0x14, 4,
                             (FieldAnimCallbackListCallback)func_801928CC);
    case 1:
        if (state->count < 4) {
            state->delay++;
            if (state->delay < 6) break;
            particle = func_800CE610(ANIM_POOL_AS(SceneE18Emitter)->pool);
            if (particle == 0) break;
            actor = ANIM_OWNER_AS(SceneE18Actor);
            particle->position = actor->instance->transform.t;
            particle->angle = 0;
            if (state->count & 1)
                side = 0x80;
            else
                side = -0x80;
            particle->width = 0x200;
            particle->height = 0x200;
            particle->turn = side;
            particle->widthGrowth = 0;
            particle->heightGrowth = 0;
            particle->brightness = 0xC0;
            particle->leader = state->count == 0;
            state->delay = 0;
            state->count++;
            if (state->count == 1) {
                Asset_Find08w(0x5C4, actor->instance->owner->asset,
                              (s16)actor->instance->transforms->t[0],
                              (s16)actor->instance->transforms->t[1],
                              (s16)actor->instance->transforms->t[2]);
                Asset_Find08w(0x5C5, ANIM_OWNER_AS(SceneE18Actor)->instance->owner->asset,
                              (s16)ANIM_OWNER_AS(SceneE18Actor)->instance->transforms->t[0],
                              (s16)ANIM_OWNER_AS(SceneE18Actor)->instance->transforms->t[1],
                              (s16)ANIM_OWNER_AS(SceneE18Actor)->instance->transforms->t[2]);
            }
            break;
        }
        if (D_801941C8 != 0) break;
        return 1;
    case 2:
        {
            int kind;
            int palette;
            int page = (D_800E2850[D_800E11FA] | GetTPage(0, 1, 0, 0)) & 0xFFFF;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800C6EC0(page, GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 10
                                                                         : palette + 6));
        }
        FieldModel_SetSemiTrans(1);
        break;
    }
    return 0;
}

int SceneE18_ExpandingRingCallback(int mode, Effect *effect) {
    s16 position[4];
    s16 params[4];
    SceneE18Object *object;

    if (mode == 1) {
        effect->frame += effect->speed;
        effect->speed += 0x30;
        effect->size -= 0x20;
        if (effect->size < 0x20) {
            D_801941CC--;
            return 1;
        }
        return 0;
    }
    if (mode != 2) {
        return 0;
    }

    object = ANIM_OWNER_AS(SceneE18ObjectLink)->object;
    position[0] = object->x;
    position[1] = object->y;
    position[2] = object->z;
    params[0] = 0x400;
    params[1] = D_800E27EC << 7;
    params[2] = 0;
    params[3] = 1;
    func_800D0728((struct GteShortVector *)position, 0x300, 0x400, 0x10, (struct GteRotation *)params,
                   effect->frame, effect->frame, (struct RenderColor *)effect, (struct RenderColor *)&effect->field04,
                   effect->size > 0x80 ? 0x80 : effect->size, 1);
    func_800D0728((struct GteShortVector *)position, 0x400, 0x500, 0x10, (struct GteRotation *)params,
                   effect->frame, effect->frame, (struct RenderColor *)&effect->field04, (struct RenderColor *)effect,
                   effect->size > 0x80 ? 0x80 : effect->size, 1);
    return 0;
}

/* Pulse ring particle: a pair of light rings around the room model that
 * shrink away. While it lives it advances the model's stage byte; the last
 * ring releases the actor once the player is within 3/8 of its radius. */
int func_80193018(int mode, SceneE18PulseRing *ring) {
    GteShortVector position;
    GteRotation spin;
    SceneE18Instance *instance;
    int angle;
    int brightness;

    switch (mode) {
    case 1:
        instance = ANIM_OWNER_AS(SceneE18Actor)->instance;
        if (*instance->owner->stage == 1)
            *instance->owner->stage = 2;
        ring->size -= ring->shrink;
        if (ring->size < 8) {
            if (ring->active)
                *instance->owner->stage = 4;
            return 1;
        }
        angle = (D_800E27EC + 1) * 24;
        if (angle > 0x400)
            angle = 0x400;
        ring->radius = rsin(angle);
        if (ring->last) {
            int dx = (RoomMain_ActorPtr->pos_x - instance->x) >> 16;
            int dz = (RoomMain_ActorPtr->pos_z - instance->z) >> 16;
            if (func_8005186C(dx * dx + dz * dz) < (ring->radius * 3) >> 3) {
                ring->last = 0;
                RoomMain_ActorPtr->state->flags |= 0x4000;
                instance->owner->flags |= 0x80000000;
            }
            if (ring->size < 0x40)
                ring->last = 0;
        }
        break;
    case 2:
        instance = ANIM_OWNER_AS(SceneE18Actor)->instance;
        brightness = (s16)(ring->size > 0x80 ? 0x80 : ring->size);
        if (ring->size > 0) {
            position.x = instance->transform.t[0];
            position.y = instance->transform.t[1] - 0x140;
            position.z = instance->transform.t[2];
            spin.x = 0x400;
            spin.y = D_800E27EC << 7;
            spin.z = 0;
            spin.flags = 1;
            func_800D0728(&position, 0x500, 0x600, 0x10, &spin, ring->radius, ring->radius,
                          &D_8019411C[0], &D_8019411C[1], brightness, 1);
            func_800D0728(&position, 0x600, 0x680, 0x10, &spin, ring->radius, ring->radius,
                          &D_8019411C[1], &D_8019411C[0], brightness, 1);
        }
        break;
    }
    return 0;
}

int func_8019330C(int mode, SceneE18PulseState *state) {
    SceneE18PulseParticle *particle;
    int stage;

    if (mode != 1) {
        if (mode < 2) {
            if (mode != 0) {
                return 0;
            }
        } else {
        return 0;

        }
        state->stage = 0;
        state->ticks = 0;
        return func_800CE560(ANIM_POOL_AS(SceneE18Emitter)->pool, 8, 4,
                             (FieldAnimCallbackListCallback)func_80193018);

    }
update:
    if (state->stage >= 4) return 2;
    state->ticks++;
    if ((s16)state->ticks < 2) return 0;

    particle = func_800CE610(ANIM_POOL_AS(SceneE18Emitter)->pool);
    if (!particle) return 0;

    if (state->stage == 0) {
        FieldActor *actor = (FieldActor *)ANIM_OWNER_AS(SceneE18Actor)->instance;
        RenderMatrix *matrix = actor->render_object.matrices;
        FieldActorState *owner = actor->state;
        Asset_Find08w(0x5C3, owner->progress,
                       (s16)matrix->translation[0],
                       (s16)matrix->translation[1],
                       (s16)matrix->translation[2]);
    }

    state->ticks = 0;
    *(volatile s16 *)&state->stage = state->stage + 1;
    stage = state->stage;

    if (stage == 4) {
        particle->scale = 0x100;
        particle->duration = 8;
        particle->active = 1;
        particle->final_stage = 1;
    } else {
        int scaled = stage << 5;
        particle->scale = scaled + 64;
        particle->duration = 8;
        particle->active = 0;
        particle->final_stage = 0;
    }

done:
    return 0;
}

int func_80193488(int mode, SceneE18PaletteEffect *effect) {
    int size, kind, palette, texture, base;
    int firstFrame, secondFrame;
    int sample;
    u16 clut;

    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
        effect->position.y -= effect->position.pad;
        effect->position.pad++;
    } else if (mode == 2) {
        firstFrame = D_800E27EC - 1;
        sample = D_800966EC[(((unsigned int)firstFrame << 8) & 0x3F00) / 4].word;
        base = effect->base;

        kind = D_800F3368.palette;
        size = base + sample;
        palette = D_800E1204[kind];

        clut = GetClut(0, (kind == 4 && D_800F3428) ? palette + 6 : palette + 2);
        secondFrame = D_800E27EC - 1;
        texture = (s16)D_800F3368.parameter02 * ((secondFrame >> 1) & 7);
        func_800CEE20(&effect->position, 0, (s16)size, (s16)size,
                       texture, clut, 1,
                       (s16)D_800966EC[(((unsigned int)secondFrame << 9) & 0x3E00) / 4].word >> 5,
                       0);
    }
    return 0;
}

/* Every fourth frame spawns a rising particle at a random model joint,
 * pushed out by a random horizontal offset that grows with the spawn
 * count, until the script raises the stop flag. */
int func_801935F0(int mode, SceneE18EmitterState *state) {
    switch (mode) {
    case 0: {
        SceneE18Instance *instance = ANIM_OWNER_AS(SceneE18Actor)->instance;
        GteMatrix *matrix = instance->transforms;

        Asset_Find08w(0x546, instance->owner->asset, (s16)matrix->t[0],
                      (s16)matrix->t[1], (s16)matrix->t[2]);
        state->count = 0;
        state->delay = 60;
        D_801941D0 = 0;
        return func_800CE560(ANIM_POOL_AS(SceneE18Emitter)->pool, 12, 10,
                             (FieldAnimCallbackListCallback)func_80193488);
    }
    case 1: {
        SceneE18RisingParticle *particle;
        GteShortVector offset;
        GteMatrix rotation;
        int random;
        int halfSize;
        int cosine;
        int sine;
        int count;
        int divisor;

        if (D_801941D0) return 2;
        if (state->delay-- > 0) break;
        particle = func_800CE610(ANIM_POOL_AS(SceneE18Emitter)->pool);
        if (particle == 0) break;
        count = state->count;
        particle->size = (count << 5) + 0x800;
        particle->position.x = ANIM_OWNER_AS(SceneE18Actor)->instance->transforms[func_80052B2C() % 38].t[0];
        particle->position.y = ANIM_OWNER_AS(SceneE18Actor)->instance->transforms[func_80052B2C() % 38].t[1];
        particle->position.z = ANIM_OWNER_AS(SceneE18Actor)->instance->transforms[func_80052B2C() % 38].t[2];
        random = func_80052B2C();
        particle->position.y -= ((random & 0x7F) * particle->size) >> 13;
        halfSize = particle->size >> 1;
        offset.x = 0;
        random = func_80071A54();
        divisor = 96;
        offset.z = ((random % divisor + 64) * halfSize) >> 12;
        cosine = rcos(func_80071A54());
        sine = rsin(func_80071A54());
        rotation.m[0][2] = sine;
        rotation.m[2][0] = -sine;
        rotation.m[1][1] = 0x1000;
        rotation.m[0][0] = cosine;
        rotation.m[2][2] = cosine;
        rotation.t[2] = 0;
        rotation.t[1] = 0;
        rotation.t[0] = 0;
        rotation.m[2][1] = 0;
        rotation.m[1][2] = 0;
        rotation.m[1][0] = 0;
        rotation.m[0][1] = 0;
        gte_ldrotmatrix(&rotation);
        gte_ldtransmatrix(&rotation);
        gte_ldv0(&offset);
        gte_rtv0tr_mac();
        gte_stsv(&offset);
        particle->position.x += offset.x;
        particle->position.z += offset.z;
        particle->position.pad = 0;
        state->delay = 4;
        state->count++;
        Asset_Find08w(0x5FD, ANIM_OWNER_AS(SceneE18Actor)->instance->owner->asset,
                      particle->position.x, particle->position.y,
                      particle->position.z);
        break;
    }
    case 2:
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11E4[11]];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 32;
        break;
    }
    return 0;
}

int func_80193A58(int mode, SceneE18WaveEffect *effect) {
    s16 position[4];
    s16 params[4];
    int palette;
    register u16 index;
    if (mode == 1) {
        effect->wave = (rsin(D_800E27EC << 4) * effect->amplitude) >> 12;
        if (D_800E27EC < 8) return 0;
        effect->size -= 8;
        if (effect->size >= 16) goto ret0;
        return 1;
    }
    if (mode != 2) return 0;
    index = D_800F3368.palette;
    palette = D_800E1204[index];
    palette = GetClut(0, (index == 4 && D_800F3428 != 0) ? palette + 7 : palette + 3);
    func_800CEE20((struct GteShortVector *)effect, (struct GteRotation *)&effect->field08, effect->wave, effect->wave,
                  0x40, palette & 0xffff, 3, effect->size, 0);
    position[0] = effect->x;
    position[1] = ANIM_OWNER_AS(FieldAnimObjectPrefix)->actor->render_object.model_matrix.translation[1];
    position[2] = effect->z;
    params[0] = 0x400;
    params[1] = (D_800E27EC * 3) << 6;
    params[2] = 0;
    params[3] = 1;
    func_800D004C((struct GteShortVector *)position, 0x100, 0x100, 10, (struct GteRotation *)params,
                  effect->wave, effect->wave, (struct RenderColor *)D_80194180, (struct RenderColor *)D_80194184,
                  effect->size, 1);
ret0:
    return 0;
}

/* Emits up to 24 wave rings from random model joints, one every 12 to 19
 * frames, and freezes the model animation at frame 3 once it has played. */
int func_80193C24(int mode, SceneE18EmitterState *state) {
    switch (mode) {
    case 0:
        state->count = 0;
        state->delay = 0;
        return func_800CE560(ANIM_POOL_AS(SceneE18Emitter)->pool, 24, 8,
                             (FieldAnimCallbackListCallback)func_80193A58);
    case 1:
        if (state->count < 24) {
            SceneE18WaveParticle *particle;
            SceneE18Actor *actor;
            SceneE18Instance *instance;
            int random;
            int index;
            int count;
            int high;
            int tilt;

            if (--state->delay == -1) {
                particle = func_800CE610(ANIM_POOL_AS(SceneE18Emitter)->pool);
                if (particle != 0) {
                    random = func_80071A54();
                    index = (random >> 2) % 38;
                    actor = ANIM_OWNER_AS(SceneE18Actor);
                    particle->position.x = actor->instance->transforms[index].t[0];
                    particle->position.y = actor->instance->transforms[index].t[1];
                    particle->position.z = actor->instance->transforms[index].t[2];
                    count = state->count;
                    particle->brightness = 128;
                    particle->amplitude = count * 2048 + 2048;
                    state->delay = (random & 7) + 12;
                    state->count++;
                    random = func_80071A54();
                    particle->controls[0] = (random & 0xFF) + 0x200;
                    high = (random >> 8) & 0xFF;
                    if (random & 2) {
                        tilt = high + 0x300;
                    } else {
                        tilt = high - 0x400;
                    }
                    particle->controls[1] = tilt;
                    particle->controls[2] = 0;
                    particle->controls[3] = 0;
                }
            }
            instance = ANIM_OWNER_AS(SceneE18Actor)->instance;
            if (instance->animation.part.frame >= instance->frameLimit) {
                instance->animation.word = 0x30000;
            }
            break;
        }
        D_801941D0 = 1;
        return 2;
    case 2:
        D_800F3368.parameter00 = 64;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 64;
        D_800F3368.extent_y = 64;
        D_800F3368.tpage = D_800E2850[D_800E11E4[11]];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 5;
        D_800F3368.depth = 0;
        break;
    }
    return 0;
}
