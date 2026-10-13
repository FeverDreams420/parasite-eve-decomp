#include "pe1/room_m349_effects.h"
#include "room_m349.h"

/* Spark that jitters its velocity (states 0 and 1) or its position
 * (state 2) by a small random amount each frame and bounces off the floor
 * height; draws as a spinning flare, the last state with a glow ring. */
int func_8018F010(int mode, RoomM349FlareSpark *spark) {
    GteRotation rotation = D_8018EFF4;
    RenderColor color = D_8018EFFC;
    int angle;
    int size;
    int fade;
    int frame;
    int fall;
    int bounce;

    switch (mode) {
    case 1:
        switch (spark->state) {
        case 0:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 63 / 64;
            spark->vz = spark->vz * 63 / 64;
            fall = (u16)spark->vy - 1;
            spark->vy = fall;
            if (spark->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            spark->vx += (rand() & 0xF) - 7;
            spark->vy += (rand() & 0xF) - 7;
            spark->vz += (rand() & 0xF) - 7;
            if ((s16)spark->timer < 48) break;
            return 1;
        case 1:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 63 / 64;
            spark->vz = spark->vz * 63 / 64;
            fall = (u16)spark->vy - 2;
            spark->vy = fall;
            if (spark->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            spark->vx += (rand() & 7) - 3;
            spark->vy += (rand() & 7) - 3;
            spark->vz += (rand() & 7) - 3;
            if ((s16)spark->timer < 32) break;
            return 1;
        case 2:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 63 / 64;
            spark->vz = spark->vz * 63 / 64;
            fall = (u16)spark->vy;
            spark->vy = fall;
            if (spark->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            spark->x += (rand() & 7) - 3;
            spark->y += (rand() & 7) - 3;
            spark->z += (rand() & 7) - 3;
            spark->angle += 8;
            if ((s16)spark->timer < 8) break;
            return 1;
        }
        break;
    case 2:
        switch (spark->state) {
        case 0: {
            u16 clut;
            int kind;
            int palette;
            angle = ((s16)spark->timer << 10) / 48;
            fade = rsin(angle * 2) / 32;
            size = rcos(angle) / 2 + 0x2000;
            rotation.z = spark->angle - (s16)spark->timer * 8;
            frame = ((s16)spark->timer / 2) & 7;
            if (frame >= 4) frame += 12;
            D_800F3368.depth = 0x226;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = GetClut(0, palette);
            func_800CEE20((GteShortVector *)spark, &rotation, size, size,
                          (s16)D_800F3368.parameter02 * frame, clut, 2,
                          fade / 3, 0);
            break;
        }
        case 1: {
            u16 clut;
            int kind;
            int palette;
            angle = (s16)spark->timer << 5;
            fade = rsin(angle * 2) / 32;
            size = rcos(angle) / 4 + 0x2000;
            rotation.z = spark->angle + (s16)spark->timer * 12;
            frame = ((s16)spark->timer / 2) & 7;
            if (frame >= 4) frame += 12;
            D_800F3368.depth = 0x80;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = GetClut(0, palette);
            func_800CEE20((GteShortVector *)spark, &rotation, size, size,
                          (s16)D_800F3368.parameter02 * frame, clut, 3, fade, 0);
            break;
        }
        case 2: {
            u16 clut;
            int kind;
            int palette;
            fade = rcos((s16)spark->timer << 5) / 40;
            rotation.z = spark->angle;
            size = spark->size;
            D_800F3368.depth = 0x80;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = GetClut(0x10, palette);
            func_800CEE20((GteShortVector *)spark, &rotation, size, size * 2,
                          (s16)D_800F3368.parameter02 * ((s16)spark->timer / 2) + 0x80,
                          clut, 1, 0x50, 0);
            func_800D004C((GteShortVector *)spark, 500, 500, 8, 0, size, size,
                          &color, 0, fade, 3);
            break;
        }
        }
        break;
    }
    return 0;
}

/* Spawns flare sparks on a ring around the target every `period` frames
 * (and a second kind half a period later) until `lastFrame`, scatters
 * glints around a model joint on random even frames, stops the looping
 * sound 16 frames after the ring ends and draws two joint glows. */
int func_8018F7C4(int mode, RoomM349FlareTarget *target,
                  RoomM349FlareParams *params) {
    GteShortVector joint;
    GteShortVector anchor;
    GteRotation rotation = D_8018F000;
    GteRotation rotation2 = D_8018F008;
    RenderColor color = D_8018EFFC;
    RoomM349FlareSpark *child;
    int radius;
    int period;
    int reach;
    int size;
    /* Ring angle while spawning, glow fade while drawing: one variable,
     * as retail keeps both in the same saved register. */
    int phase;

    switch (mode) {
    case 0:
        target->position.x = params->x;
        target->position.y = params->y;
        target->position.z = params->z;
        target->position.y = g_RoomFloorY->y;
        return func_800CE560(ANIM_POOL_AS(RoomSparkChannel)->pool, 20, 70,
                             (FieldFxCallbackListCallback)func_8018F010);
    case 1:
        func_800CE870(ANIM_OWNER_AS(RoomSparkChannel)->pool, 0, (s16 *)&joint);
        radius = params->radius;
        period = params->period;
        if (D_800E27EC <= params->lastFrame) {
            if (D_800E27EC % period == 0) {
                child = func_800CE610(ANIM_POOL_AS(RoomSparkChannel)->pool);
                if (child) {
                    child->x = target->position.x;
                    child->y = target->position.y;
                    child->z = target->position.z;
                    phase = rand();
                    reach = radius * (rand() & 0xFF) / 256;
                    child->x += rcos(phase) * reach / 4096;
                    child->y -= rand() & 0x1F;
                    child->z += rsin(phase) * reach / 4096;
                    child->angle = rand();
                    child->vx = rand() % 32 - 16;
                    child->vy = -(rand() % 32);
                    child->vz = rand() % 32 - 16;
                    child->state = 0;
                    child->timer = 0;
                }
            }
            if (D_800E27EC % period == period / 2) {
                child = func_800CE610(ANIM_POOL_AS(RoomSparkChannel)->pool);
                if (child) {
                    child->x = target->position.x;
                    child->y = target->position.y;
                    child->z = target->position.z;
                    phase = rand();
                    reach = radius * (rand() & 0xFF) / 256;
                    child->x += rcos(phase) * reach / 4096;
                    child->y -= rand() & 0x1F;
                    child->z += rsin(phase) * reach / 4096;
                    child->angle = rand();
                    child->vx = rand() % 24 - 12;
                    child->vy = -(rand() % 24);
                    child->vz = rand() % 24 - 12;
                    child->state = 1;
                    child->timer = 0;
                }
            }
        }
        if (!(D_800E27EC & 1) && !(rand() & 1)) {
            child = func_800CE610(ANIM_POOL_AS(RoomSparkChannel)->pool);
            if (child) {
                child->x = joint.x;
                child->y = joint.y;
                child->z = joint.z;
                child->x += (rand() & 0x1FF) - 0x100;
                child->y += rand() % 800 - 400;
                child->z += (rand() & 0x1FF) - 0x100;
                child->size = (rand() & 0x1FF) + 0x2AA;
                child->angle = rand();
                child->vx = rand() % 24 - 12;
                child->vy = -(rand() % 24);
                child->vz = rand() % 24 - 12;
                child->state = 2;
                child->timer = 0;
            }
        }
        if (D_800E27EC == params->lastFrame + 16 && target->sound != -1) {
            Akao_Cmd_21(target->sound, 0);
        }
        if (D_800E27EC < 16) break;
        return 2;
    case 2:
        D_800F3368.parameter00 = 16;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 16;
        D_800F3368.extent_y = 16;
        {
            int tpage = D_800E2850[D_800E11E8];
            D_800F3368.palette = 2;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 4;
            D_800F3368.tpage = tpage;
        }
        size = 0x2AA;
        phase = (rand() & 0x1F) + 0x20;
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomSparkChannel)->pool, 13, &rotation, &anchor);
        {
            u16 clut;
            int kind;
            int palette;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = GetClut(0x20, palette);
            func_800CEE20(&anchor, 0, size, size,
                          (s16)D_800F3368.parameter02 + 0xD8, clut, 1, phase,
                          &color);
        }
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomSparkChannel)->pool, 13, &rotation2, &anchor);
        {
            u16 clut;
            int kind;
            int palette;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = GetClut(0x20, palette);
            func_800CEE20(&anchor, 0, size, size,
                          (s16)D_800F3368.parameter02 + 0xD8, clut, 1, phase,
                          &color);
        }
        D_800F3368.parameter00 = 0x40;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 0x40;
        D_800F3368.extent_y = 0x40;
        {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}

short *func_801900D0(int arg0, int arg1, int arg2, int arg3) {
    if (arg0 == 1) {
        D_80190194 = arg1;
        D_80190196 = arg2;
        D_80190198 = arg3;
    }
    if (arg0 == 2) {
        D_8019019C = arg1;
        D_801901A4 = arg2;
    } else {
        if (arg1 < 2) {
            arg1 = 2;
        }
        D_801901A0 = arg1;
    }
    return &D_80190194;
}
