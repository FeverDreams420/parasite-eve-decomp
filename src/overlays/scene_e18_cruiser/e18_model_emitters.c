#include "pe1/scene_e18_effects.h"

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
