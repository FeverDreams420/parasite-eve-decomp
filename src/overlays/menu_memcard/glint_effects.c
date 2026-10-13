#include "menu_memcard_glint.h"

/* Spark that drifts with damped velocity for 44 frames; state 0 draws a
 * flickering coloured glow, state 1 a larger glow that fades with a sine. */
int MenuCard_DriftGlowParticle(int mode, RoomDampedSpark *spark) {
    GteRotation rotation = D_801ED7FC;
    RenderColor color;
    int angle;
    int size;
    int fade;

    switch (mode) {
    case 1:
        switch (spark->state) {
        case 0:
        case 1:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 63 / 64;
            spark->vy = spark->vy * 63 / 64;
            spark->vz = spark->vz * 63 / 64;
            if ((s16)spark->timer < 44) break;
            return 1;
        }
        break;
    case 2:
        switch (spark->state) {
        case 0: {
            int kind;
            int palette;
            angle = ((s16)spark->timer << 10) / 44;
            fade = 128 - ((D_800E27EC & 1) << 5);
            func_800CF3AC(D_801F1BB0, &color, (s16)spark->timer);
            rotation.z = -D_800E27EC * 16;
            size = rcos(angle) + 0x1000;
            D_800F336C = 1;
            D_800F3368.tpage = D_800E2850[D_800E11E6];
            func_800CEDA8(1);
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            kind = D_800F336C;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20((GteShortVector *)spark, &rotation, size, size, 128,
                          GetClut(32, palette), 1, fade, &color);
            break;
        }
        case 1: {
            int kind;
            int palette;
            angle = ((s16)spark->timer << 10) / 44;
            fade = rsin(angle * 2) / 32;
            rotation.z = -D_800E27EC * 16;
            size = rsin(angle) / 2 + 0x1000;
            D_800F336C = 1;
            D_800F3368.tpage = D_800E2850[D_800E11F6];
            func_800CEDA8(1);
            D_800F3368.parameter06 = 1;
            D_800F3368.parameter0A = 5;
            kind = D_800F336C;
            palette = D_800E1204[kind];
            func_800CEE20((GteShortVector *)spark, &rotation, size * 3 / 2, size * 3 / 2, 64,
                          GetClut(0, (kind == 4 && D_800F3428) ? palette + 7 : palette + 3),
                          1, fade / 2, 0);
            break;
        }
        }
        break;
    }
    return 0;
}

/* Ring burst: sprays drift glows on its first frames, then grows a stack
 * of glow rings and bands around its position. */
int MenuCard_RingBurstController(int mode, MemcardRingBurst *burst) {
    GteShortVector position;
    GteShortVector offset;
    GteRotation spin = D_801ED7FC;
    GteRotation tilt = D_801ED804;
    RenderColor glow = D_801ED80C;
    RenderColor ring = D_801ED810;
    RenderColor band = D_801ED814;
    RoomDampedSpark *child;
    int i;
    int angle;
    int fade;
    int scale;
    int amount;
    int dim;

    switch (mode) {
    case 0:
        func_800CE870((char *)D_8009D254, 0, &burst->x);
        Akao_SendPositionalCmdStereo(0x4B7, 0, burst->x, burst->y, burst->z);
        burst->state = 0;
        burst->timer = 0;
        return func_800CE560(ANIM_POOL_AS(RoomSparkChannel)->pool, 20, 50,
                             (FieldFxCallbackListCallback)MenuCard_DriftGlowParticle);
    case 1:
        switch (burst->state) {
        case 0:
            if (++burst->timer < 24) {
                return 0;
            }
            burst->state = 1;
            burst->timer = 0;
            for (i = 0; i < 50; i++) {
                child = func_800CE610(ANIM_POOL_AS(RoomSparkChannel)->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->vx = rand() % 140 - 70;
                    child->vy = rand() % 140 - 70;
                    child->vz = rand() % 140 - 70;
                    child->state = (i & 3) == 0;
                    child->timer = 0;
                }
            }
            return 0;
        case 1:
            if (++burst->timer >= 48) {
                return 1;
            }
            break;
        }
        break;
    case 2:
        position.x = burst->x;
        position.y = burst->y;
        position.z = burst->z;
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        {
            u16 tpage = D_800E2850[D_800E11E6];
            D_800F3368.palette = 1;
            D_800F3368.tpage = tpage;
        }
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 41;
        switch (burst->state) {
        case 0:
            angle = (burst->timer << 10) / 24;
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            fade = rsin(angle) / 32;
            if (D_800E27EC & 1) {
                fade = fade * 15 / 16;
            }
            spin.z = burst->timer * 15;
            func_800D004C(&position, 300, 500, 70, &spin, 0x1000, 0x1000,
                          &glow, 0, fade / 2, 1);
            fade = rsin(angle) / 32;
            func_800D004C(&position, 700, 700, 20, 0, 0x1000, 0x1000,
                          &ring, 0, fade, 1);
            break;
        case 1:
            angle = (burst->timer << 10) / 48;
            if (burst->timer < 5) {
                amount = 0x80 - burst->timer * 32;
                func_800D1AE0((unsigned char *)&ring, amount, 1, 8);
            }
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            fade = rcos(angle) / 32;
            if (D_800E27EC & 1) {
                fade = fade * 15 / 16;
            }
            spin.z = rsin(angle) / 2;
            func_800D004C(&position, 70, rcos(angle) / 8 + 700, 8,
                          &spin, 0x1000, 0x1000, &ring, 0, fade / 2, 1);
            func_800D004C(&position, 300, 300, 8, 0, 0x1000, 0x1000,
                          &glow, 0, fade, 1);
            fade = rcos(angle) / 32;
            func_800D004C(&position, 1000, 1000, 16, 0, 0x1000, 0x1000,
                          &ring, 0, fade, 1);
            scale = rsin(angle) / 4 + 0xC00;
            func_800D0728(&position, 1500, 1900, 24, 0, scale, scale,
                          0, &ring, fade, 1);
            scale = rsin(angle);
            func_800D0728(&position, 2000, 2600, 20, &tilt, scale, scale,
                          &band, 0, fade, 1);
            amount = rsin(angle) / 12 + 80;
            dim = fade * 2 / 3;
            offset.x = position.x;
            offset.y = position.y;
            offset.z = position.z;
            offset.y -= amount;
            func_800D0728(&offset, 1000, 1600, 16, &tilt, scale, scale,
                          &band, 0, dim, 1);
            offset.x = position.x;
            offset.y = position.y;
            offset.z = position.z;
            offset.y += amount;
            func_800D0728(&offset, 1000, 1600, 16, &tilt, scale, scale,
                          &band, 0, dim, 1);
            break;
        }
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        {
            u16 tpage = D_800E2850[D_800E11E6];
            D_800F3368.palette = 1;
            D_800F3368.tpage = tpage;
        }
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        break;
    }
    return 0;
}

/* Spark with three phases: state 0 falls and bounces with a spinning
 * coloured glint, state 1 rises with jitter as a pulsing flame frame,
 * state 2 lies on the floor as a flat glow that fades with a sine. */
int MenuCard_RisingEmberParticle(int mode, RoomDampedSpark *spark) {
    GteRotation rotation = D_801ED7FC;
    RenderColor color;
    GteShortVector position;
    GteRotation tilt;
    int fall;
    int bounce;
    int angle;
    int size;
    int fade;

    switch (mode) {
    case 1:
        switch (spark->state) {
        case 0:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 127 / 128;
            spark->vz = spark->vz * 127 / 128;
            fall = (u16)spark->vy - 1;
            spark->vy = fall;
            if (spark->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            if ((s16)spark->timer < 16) break;
            return 1;
        case 1:
            spark->timer++;
            spark->y -= (rand() & 7) + 5;
            spark->x += (rand() & 7) - 3;
            spark->z += (rand() & 7) - 3;
            if ((s16)spark->timer < 16) break;
            return 1;
        case 2:
            spark->timer++;
            if ((s16)spark->timer < 18) break;
            return 1;
        }
        break;
    case 2:
        switch (spark->state) {
        case 0: {
            int kind;
            int palette;
            angle = (s16)spark->timer;
            func_800CF3AC(D_801F1C28, &color, angle * 2);
            rotation.z = spark->x + D_800E27EC * 32;
            size = rsin(angle << 7);
            D_800F3368.parameter00 = 16;
            D_800F3368.parameter02 = 1;
            D_800F3368.extent_x = 16;
            D_800F3368.extent_y = 16;
            {
                u16 tpage = D_800E2850[D_800E11E6];
                D_800F3368.palette = 1;
                D_800F3368.tpage = tpage;
            }
            func_800CEDA8(1);
            D_800F3368.parameter06 = 0;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20((GteShortVector *)spark, &rotation, size, size * 3 / 2, 141,
                          GetClut(48, palette), 1, 128, &color);
            break;
        }
        case 1: {
            int kind;
            int palette;
            angle = (s16)spark->timer << 6;
            fade = rcos(angle) / 32;
            rotation.z = D_800E27EC << 5;
            size = rcos(angle);
            D_800F3368.parameter00 = 32;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 32;
            D_800F3368.extent_y = 32;
            {
                u16 tpage = D_800E2850[D_800E11E6];
                D_800F3368.palette = 1;
                D_800F3368.tpage = tpage;
            }
            func_800CEDA8(1);
            D_800F3368.parameter06 = 0;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20((GteShortVector *)spark, 0, size / 2, size / 2,
                          (s16)D_800F3368.parameter02 * (spark->timer & 3) + 104,
                          GetClut(16, palette), 3, fade, 0);
            break;
        }
        case 2: {
            int kind;
            int palette;
            angle = ((s16)spark->timer << 10) / 18;
            fade = rsin(angle * 2) / 32;
            size = rsin(angle);
            D_800F3368.parameter00 = 32;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 32;
            D_800F3368.extent_y = 32;
            {
                u16 tpage = D_800E2850[D_800E11F6];
                D_800F3368.palette = 1;
                D_800F3368.tpage = tpage;
            }
            func_800CEDA8(1);
            D_800F3368.parameter06 = 1;
            tilt.x = 0x400;
            tilt.y = 0;
            tilt.z = (s16)spark->timer * 48;
            tilt.flags = 1;
            position.x = spark->x;
            position.z = spark->z;
            position.y = g_RoomFloorY->y;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800CEE20(&position, &tilt, size * 3, size * 3, 68,
                          GetClut(0, (kind == 4 && D_800F3428) ? palette + 7 : palette + 3),
                          1, fade / 2, 0);
            break;
        }
        }
        break;
    }
    return 0;
}

/* Controller that draws two crossed beams between the actor's hands while
 * the flash flag is raised, sheds sparks from both hands and mirrors every
 * beam to the other side. */
int MenuCard_CrossFlashController(int mode, MemcardCrossFlash *flash) {
    GteShortVector from;
    GteShortVector to;
    MemcardJointOffset hand = D_801ED818;
    MemcardJointOffset tipA = D_801ED820;
    MemcardJointOffset tipB = D_801ED828;
    MemcardJointOffset center = D_801ED830;
    RenderColor inner = D_801ED838;
    RenderColor outer = D_801ED83C;
    RenderColor edge = D_801ED840;
    RenderMatrixSlot *matrixSlot;
    RoomDampedSpark *child;
    int index;
    int size;
    int alpha;

    switch (mode) {
    case 0:
        flash->attachA = 6;
        flash->attachB = 19;
        flash->reserved04 = 0;
        flash->timer = 0;
        D_801F1F38 = 0;
        tipA.x *= -1;
        tipB.x *= -1;
        FieldFx_TransformMatrixPoint(((RoomSparkBattleEntity *)D_8009D254), 8, &hand, &from);
        FieldFx_TransformMatrixPoint(((RoomSparkBattleEntity *)D_8009D254), 8, &tipA, &to);
        func_800D1384(&from, &to, 0x3F0, (unsigned char *)&inner, (unsigned char *)&outer, 128, (struct FieldTrailPair *)flash->trailA, 1);
        FieldFx_TransformMatrixPoint(((RoomSparkBattleEntity *)D_8009D254), 8, &tipB, &from);
        func_800D1384(&from, &to, 0x3F2, (unsigned char *)&edge, (unsigned char *)&outer, 128, (struct FieldTrailPair *)flash->trailC, 1);
        tipA.x *= -1;
        tipB.x *= -1;
        FieldFx_TransformMatrixPoint(((RoomSparkBattleEntity *)D_8009D254), 21, &hand, &from);
        FieldFx_TransformMatrixPoint(((RoomSparkBattleEntity *)D_8009D254), 21, &tipA, &to);
        func_800D1384(&from, &to, 0x3F0, (unsigned char *)&inner, (unsigned char *)&outer, 128, (struct FieldTrailPair *)flash->trailB, 1);
        FieldFx_TransformMatrixPoint(((RoomSparkBattleEntity *)D_8009D254), 21, &tipB, &from);
        func_800D1384(&from, &to, 0x3F2, (unsigned char *)&edge, (unsigned char *)&outer, 128, (struct FieldTrailPair *)flash->trailD, 1);
        return func_800CE560(ANIM_POOL_AS(RoomSparkChannel)->pool, 20, 36,
                             (FieldFxCallbackListCallback)MenuCard_RisingEmberParticle);
    case 1:
        flash->timer++;
        D_801F1F3A = 1;
        if (D_801F1F38 == 0) {
            int phase = (s16)flash->timer % 3;
            if (phase == 0) {
                FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomSparkChannel)->pool, flash->attachA, &center, &from);
                child = func_800CE610(ANIM_POOL_AS(RoomSparkChannel)->pool);
                if (child) {
                    child->x = from.x;
                    child->y = from.y;
                    child->z = from.z;
                    child->state = 1;
                    child->timer = 0;
                }
            } else if (phase == 2) {
                FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomSparkChannel)->pool, flash->attachB, &center, &from);
                child = func_800CE610(ANIM_POOL_AS(RoomSparkChannel)->pool);
                if (child) {
                    child->x = from.x;
                    child->y = from.y;
                    child->z = from.z;
                    child->state = 1;
                    child->timer = 0;
                }
            }
            if (flash->timer & 1) index = 8;
            else index = 21;
            hand.z = -(rand() & 0x1FF);
            FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomSparkChannel)->pool, index, &hand, &from);
            child = func_800CE610(ANIM_POOL_AS(RoomSparkChannel)->pool);
            if (child) {
                child->x = from.x;
                child->y = from.y;
                child->z = from.z;
                child->vx = rand() % 16 - 8;
                child->vy = rand() % 16 - 8;
                child->vz = rand() % 16 - 8;
                child->state = 0;
                child->timer = 0;
            }
            if ((flash->timer & 7) == 0) {
                func_800CE870((char *)D_8009D254, 0, (s16 *)&from);
                child = func_800CE610(ANIM_POOL_AS(RoomSparkChannel)->pool);
                if (child) {
                    child->x = from.x;
                    child->y = from.y;
                    child->z = from.z;
                    child->state = 2;
                    child->timer = 0;
                }
            }
            break;
        }
        return 2;
    case 2:
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        {
            u16 tpage = D_800E2850[D_800E11E6];
            D_800F3368.palette = 1;
            D_800F3368.tpage = tpage;
        }
        func_800CEDA8(1);
        alpha = 140;
        size = 0x1000;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 16;
        if (D_800E27EC & 1) {
            size = 0x2000;
            alpha = 100;
        }
        {
            int kind;
            int palette;
            FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomSparkChannel)->pool, flash->attachA, &center, &from);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20(&from, 0, size / 4, size / 4, 128,
                          GetClut(192, palette), 1, alpha / 2, 0);
        }
        {
            int kind;
            int palette;
            FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomSparkChannel)->pool, flash->attachB, &center, &from);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20(&from, 0, size / 2, size / 2, 128,
                          GetClut(192, palette), 1, alpha / 2, 0);
        }
        tipA.x *= -1;
        tipB.x *= -1;
        FieldFx_TransformMatrixPoint(((RoomSparkBattleEntity *)D_8009D254), 8, &hand, &from);
        FieldFx_TransformMatrixPoint(((RoomSparkBattleEntity *)D_8009D254), 8, &tipA, &to);
        func_800D1384(&from, &to, 8, (unsigned char *)&inner, (unsigned char *)&outer, 128, (struct FieldTrailPair *)flash->trailA, 1);
        FieldFx_TransformMatrixPoint(((RoomSparkBattleEntity *)D_8009D254), 8, &tipB, &from);
        func_800D1384(&from, &to, 10, (unsigned char *)&edge, (unsigned char *)&outer, 128, (struct FieldTrailPair *)flash->trailC, 1);
        tipA.x *= -1;
        tipB.x *= -1;
        FieldFx_TransformMatrixPoint(((RoomSparkBattleEntity *)D_8009D254), 21, &hand, &from);
        FieldFx_TransformMatrixPoint(((RoomSparkBattleEntity *)D_8009D254), 21, &tipA, &to);
        func_800D1384(&from, &to, 8, (unsigned char *)&inner, (unsigned char *)&outer, 128, (struct FieldTrailPair *)flash->trailB, 1);
        FieldFx_TransformMatrixPoint(((RoomSparkBattleEntity *)D_8009D254), 21, &tipB, &from);
        func_800D1384(&from, &to, 10, (unsigned char *)&edge, (unsigned char *)&outer, 128, (struct FieldTrailPair *)flash->trailD, 1);
        D_800F3368.depth = 16;
        break;
    }
    return 0;
}

/* Spark that drifts with damped velocity for 24 frames; state 0 draws a
 * glint whose size follows a sine scaled by the spark's angle field, state 1
 * also falls and bounces and draws a larger spinning glint. */
int MenuCard_FadingGlintParticle(int mode, RoomDampedSpark *spark) {
    GteRotation rotation;
    int fall;
    int bounce;
    int angle;
    int size;
    int fade;

    switch (mode) {
    case 1:
        switch (spark->state) {
        case 0:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 127 / 128;
            spark->vy = spark->vy * 127 / 128;
            spark->vz = spark->vz * 127 / 128;
            if ((s16)spark->timer < 24) break;
            return 1;
        case 1:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 31 / 32;
            spark->vz = spark->vz * 31 / 32;
            fall = (u16)spark->vy + 1;
            spark->vy = fall;
            if (spark->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            if ((s16)spark->timer < 24) break;
            return 1;
        }
        break;
    case 2:
        switch (spark->state) {
        case 0: {
            u16 clut;
            int kind;
            int palette;
            angle = ((s16)spark->timer << 10) / 24;
            size = rsin(angle) + 0x800;
            size = size * (s16)spark->angle / 4096;
            fade = rcos(angle) / 32 + 40;
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = (s16)spark->timer * 32;
            rotation.flags = 0;
            D_800F3368.parameter00 = 32;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 32;
            D_800F3368.extent_y = 32;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            clut = GetClut(0, (kind == 4 && D_800F3428) ? palette + 6 : palette + 2);
            func_800CEE20((GteShortVector *)spark, &rotation, size, size, 6,
                          clut, 1, fade, 0);
            break;
        }
        case 1: {
            u16 clut;
            int kind;
            int palette;
            angle = ((s16)spark->timer << 10) / 24;
            size = rsin(angle) + 0x1000;
            fade = rsin(angle * 2) / 32;
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = (s16)spark->timer * 14;
            rotation.flags = 0;
            D_800F3368.parameter00 = 64;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 64;
            D_800F3368.extent_y = 64;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            clut = GetClut(0, (kind == 4 && D_800F3428) ? palette + 5 : palette + 1);
            func_800CEE20((GteShortVector *)spark, &rotation, size * 2, size * 2, 0,
                          clut, 1, fade, 0);
            break;
        }
        }
        break;
    }
    return 0;
}

/* Controller that sits on the scene anchor, bursts fading glints on its
 * early frames and flashes a glint, ring and halo while it fades. */
int MenuCard_GlintBurstController(int mode, MemcardGlintBurst *burst) {
    RenderColor color;
    RenderColor flash = D_801ED844;
    RoomDampedSpark *child;
    RenderMatrixSlot *matrixSlot;
    int i;
    int angle;
    int fade;
    int scale;

    switch (mode) {
    case 0:
        burst->x = ((MemcardGlintAnchor *)ANIM_OWNER_AS(RoomSparkChannel)->pool)->x;
        burst->y = ((MemcardGlintAnchor *)ANIM_OWNER_AS(RoomSparkChannel)->pool)->y;
        burst->z = ((MemcardGlintAnchor *)ANIM_OWNER_AS(RoomSparkChannel)->pool)->z;
        return func_800CE560(ANIM_POOL_AS(RoomSparkChannel)->pool, 20, 22,
                             (FieldFxCallbackListCallback)MenuCard_FadingGlintParticle);
    case 1:
        if (D_800E27EC == 3) {
            for (i = 0; i < 6; i++) {
                child = func_800CE610(ANIM_POOL_AS(RoomSparkChannel)->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->vx = rand() % 50 - 25;
                    child->vy = rand() % 50 - 25;
                    child->vz = rand() % 50 - 25;
                    child->state = 1;
                    child->timer = 0;
                }
            }
        }
        if (D_800E27EC < 5) {
            for (i = 0; i < 4; i++) {
                child = func_800CE610(ANIM_POOL_AS(RoomSparkChannel)->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->angle = (rand() & 0x7FF) + 0x800;
                    child->x += (rand() & 0x7F) - 64;
                    child->y += (rand() & 0x7F) - 64;
                    child->z += (rand() & 0x7F) - 64;
                    child->vx = rand() % 190 - 95;
                    child->vy = rand() % 190 - 95;
                    child->vz = rand() % 190 - 95;
                    child->state = 0;
                    child->timer = 0;
                }
            }
        }
        if (D_800E27EC >= 8) {
            return 2;
        }
        break;
    case 2:
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        {
            u16 tpage = D_800E2850[D_800E11E6];
            D_800F3368.palette = 1;
            D_800F3368.tpage = tpage;
        }
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 32;
        if (D_800E27EC == 1) {
            func_800D1AE0((unsigned char *)&flash, 128, 2, 8);
        } else if (D_800E27EC == 2) {
            func_800D1AE0((unsigned char *)&flash, 128, 1, 8);
        }
        if (D_800E27EC < 33) {
            int kind;
            int palette;
            angle = D_800E27EC << 5;
            fade = rcos(angle) / 32;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20((GteShortVector *)burst, 0, 0x2000, 0x2000,
                          (s16)D_800F3368.parameter02 * (D_800E27EC / 4) + 192,
                          GetClut(192, palette), 1, fade, 0);
            func_800CF3AC(D_801F1CB0, &color, D_800E27EC);
            scale = rsin(angle) + 0x1000;
            if (D_800E27EC & 1) {
                fade = fade * 2 / 3;
            }
            func_800D004C((GteShortVector *)burst, 500, 500, 14, 0, scale, scale,
                          &color, 0, fade, 1);
            fade = rcos(angle) / 32;
            scale = rsin(angle) + 0x800;
            func_800D0728((GteShortVector *)burst, 1100, 1700, 18, 0, scale, scale,
                          0, &color, fade / 2, 1);
        }
        {
            u16 tpage = D_800E2850[D_800E11F6];
            D_800F3368.palette = 1;
            D_800F3368.tpage = tpage;
        }
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 16;
        break;
    }
    return 0;
}

/* Particle that spins: state 0 draws a halo, state 1 a tilted textured
 * ring around the shared origin, state 2 a scaled model that orbits it. */
int MenuCard_SpinRingParticle(int mode, MemcardSpinParticle *p) {
    GteShortVector offset;
    RenderColor color;
    RenderColor colorA = D_801ED848;
    RenderColor colorB = D_801ED84C;
    MemcardModelMatrix matrix;
    MemcardModelScale scale;
    RenderMatrixSlot *matrixSlot;
    int angle;
    int size;
    int width;
    int height;
    int fade;
    int u;
    int v;

    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            if ((s16)p->timer < 32) break;
            return 1;
        case 1:
            p->timer++;
            p->rotation.x += p->x;
            p->rotation.y += p->y;
            p->rotation.z += p->z;
            if ((s16)p->timer < 40) break;
            return 1;
        case 2:
            p->timer++;
            p->rotation.z += 44;
            if ((s16)p->timer < 32) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0:
            size = rsin((s16)p->timer << 5);
            func_800CF3AC(D_801F1CD8, &color, (s16)p->timer);
            func_800D0728((GteShortVector *)p, 1400, 2200, 18, &p->rotation, size, size,
                          0, &color, 128, 1);
            break;
        case 1: {
            int kind;
            int palette;
            angle = ((s16)p->timer << 10) / 40;
            fade = rcos(angle) / 32;
            if (p->timer & 1) {
                fade = fade * 2 / 3;
            }
            offset.x = D_801F1F28.x;
            offset.y = D_801F1F28.y;
            offset.z = D_801F1F28.z;
            width = 1900;
            if ((s16)p->timer < 33) {
                width = rsin((s16)p->timer << 5) * 1900 / 4096;
            }
            height = rcos(angle) / 32 + 64;
            matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            size = (s16)(p->timer & 3);
            u = (size & 1) << 7;
            v = (size / 2) * 16 + 160;
            kind = D_800F336C;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800D2370(&offset, &p->rotation, width, height, u, v, 128, 16,
                          GetClut(96, palette), &colorA, &colorB, (s16)fade, 1);
            break;
        }
        case 2: {
            int kind;
            int palette;
            int page;
            u16 *index;
            int spin;
            spin = (s16)p->timer << 5;
            width = rsin(spin) / 8 + 100;
            func_800CFB7C((GteShortVector *)&p->rotation, (s16)width, &offset);
            offset.x += D_801F1F28.x;
            offset.y += D_801F1F28.y;
            offset.z += D_801F1F28.z;
            size = rsin(spin) * 2 + 0x800;
            fade = rcos(spin) / 32;
            index = &D_800E11E6;
            D_800F336C = 1;
            D_800F3368.tpage = D_800E2850[*index];
            func_800CEDA8(1);
            D_800F3368.parameter06 = 0;
            page = (D_800E2850[*index] | GetTPage(0, 1, 0, 0)) & 0xFFFF;
            kind = D_800F336C;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            FieldFx_ModelSetTexture(page, GetClut(224, palette));
            FieldFx_ModelSetSemiTrans(1);
            RotMatrixYXZ(&p->rotation, &matrix);
            matrix.t[0] = offset.x;
            matrix.t[1] = offset.y;
            matrix.t[2] = offset.z;
            scale.x = size;
            scale.y = size;
            scale.z = size;
            ScaleMatrix(&matrix, &scale);
            FieldFx_ModelSaveColors(D_800E22D4);
            FieldFx_ModelScaleColors(D_800E22D4, (u16)fade);
            FieldFx_ModelDraw((union FieldTexturedModel *)D_800E22D4, (GteMatrix *)&matrix);
            FieldFx_ModelRestoreColors(D_800E22D4);
            break;
        }
        }
        break;
    }
    return 0;
}

/* Controller that sits on the scene anchor, plays its sound on frame 3 and
 * spawns spinning halos, rings and model shards on frames 2 and 8. */
int MenuCard_SpinBurstController(int mode, MemcardSpinBurst *burst) {
    MemcardSpinParticle *child;
    int i;

    switch (mode) {
    case 0:
        burst->x = ((MemcardGlintAnchor *)ANIM_OWNER_AS(RoomSparkChannel)->pool)->x;
        burst->y = ((MemcardGlintAnchor *)ANIM_OWNER_AS(RoomSparkChannel)->pool)->y;
        burst->z = ((MemcardGlintAnchor *)ANIM_OWNER_AS(RoomSparkChannel)->pool)->z;
        func_800CE9D4((struct RoomFxTransformOwner *)D_8009D254, 0, (GteShortVector *)&burst->rotation);
        burst->rotation.x = 0;
        FieldFx_ModelScrollUv((union FieldTexturedModel *)D_800E22D4, 0, 0);
        return func_800CE560(ANIM_POOL_AS(RoomSparkChannel)->pool, 20, 40,
                             (FieldFxCallbackListCallback)MenuCard_SpinRingParticle);
    case 1:
        if (D_800E27EC == 3) {
            Akao_SendPositionalCmdStereo(1201, 0, burst->x, burst->y, burst->z);
        }
        if (D_800E27EC == 2) {
            child = (MemcardSpinParticle *)func_800CE610(ANIM_POOL_AS(RoomSparkChannel)->pool);
            if (child) {
                child->x = burst->x;
                child->y = burst->y;
                child->z = burst->z;
                child->rotation.x = burst->rotation.x;
                child->rotation.y = burst->rotation.y;
                child->rotation.z = burst->rotation.z;
                child->rotation.flags = 1;
                child->state = 0;
                child->timer = 0;
            }
            for (i = 0; i < 6; i++) {
                child = (MemcardSpinParticle *)func_800CE610(ANIM_POOL_AS(RoomSparkChannel)->pool);
                if (child) {
                    child->x = (rand() & 0x7F) - 64;
                    child->y = 0;
                    child->z = (rand() & 0x1F) + 16;
                    child->rotation.x = burst->rotation.x;
                    child->rotation.y = burst->rotation.y;
                    child->rotation.z = burst->rotation.z;
                    child->rotation.y += 0x400;
                    child->rotation.x += rand();
                    child->rotation.flags = 0;
                    child->state = 1;
                    child->timer = 0;
                }
            }
        }
        if (D_800E27EC == 8) {
            child = (MemcardSpinParticle *)func_800CE610(ANIM_POOL_AS(RoomSparkChannel)->pool);
            if (child) {
                child->x = burst->x;
                child->y = burst->y;
                child->z = burst->z;
                child->rotation.x = burst->rotation.x;
                child->rotation.y = burst->rotation.y;
                child->rotation.z = burst->rotation.z;
                child->rotation.x += 0x400;
                child->rotation.flags = 1;
                child->state = 0;
                child->timer = 0;
            }
            for (i = 0; i < 6; i++) {
                child = (MemcardSpinParticle *)func_800CE610(ANIM_POOL_AS(RoomSparkChannel)->pool);
                if (child) {
                    child->x = 0;
                    child->y = (rand() & 0x7F) - 64;
                    child->z = (rand() & 0x1F) + 16;
                    child->rotation.x = burst->rotation.x;
                    child->rotation.y = burst->rotation.y;
                    child->rotation.z = burst->rotation.z;
                    child->rotation.y += rand();
                    child->rotation.flags = 0;
                    child->state = 1;
                    child->timer = 0;
                }
            }
        }
        if (D_800E27EC == 2) {
            child = (MemcardSpinParticle *)func_800CE610(ANIM_POOL_AS(RoomSparkChannel)->pool);
            if (child) {
                child->x = burst->x;
                child->y = burst->y;
                child->z = burst->z;
                child->rotation.x = burst->rotation.x;
                child->rotation.y = burst->rotation.y;
                child->rotation.z = burst->rotation.z;
                child->rotation.y += 0x800;
                child->state = 2;
                child->timer = 0;
            }
        }
        if (D_800E27EC >= 8) {
            return 2;
        }
        break;
    case 2: {
        RenderMatrixSlot *matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        D_801F1F28.x = burst->x;
        D_801F1F28.y = burst->y;
        D_801F1F28.z = burst->z;
        {
            u16 tpage = D_800E2850[D_800E11E6];
            D_800F3368.palette = 1;
            D_800F3368.tpage = tpage;
        }
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 32;
        break;
    }
    }
    return 0;
}
