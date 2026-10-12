/* MASPSX_FLAGS: --expand-div */

#include "common.h"
#include "room_m273_boss.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"
#include "pe1/field_sprite_params.h"
#include "pe1/scene_assets.h"
#include "room_m273_sway.h"
#include "pe1/field_model_draw.h"
#include "pe1/scene_archive.h"
#include "pe1/field_texture_tables.h"
#include "room_m273.h"
#include "pe1/field_anim_callback_list.h"
#include "room_m273_effects.h"
#include "pe1/field_anim_pool.h"
#include "room_m273_boss_state.h"

typedef GteShortVector Vector;

typedef RoomM273PairPoolRecord Record;

/* Partial storage views; the fourth halfword carries the emitter flag. */
typedef RoomM273PairPoolRecord Entry;

typedef struct { Entry entries[2]; unsigned char unknown[14]; unsigned char stopped; } EmitterState;

typedef GteShortVector Position;

typedef struct {
    char pad_00[4];
    s32 *source;
    s16 scaleX;
    s16 scaleY;
    u16 value;
    u16 step;
} RoomM273PulseState;

typedef struct {
    u16 *source;
} RoomM273PhaseState;

#include "m273_boss_sway_externs.h"

/* Animation 13 controller: turns the boss toward the player on entry, loops
 * the sweep window while loops remain, publishes the sweep attack angle and
 * transforms the sweep point. */
int func_80197BBC(int mode) {
    RoomM273BossInstance *instance = ANIM_OWNER_AS(RoomM273BossActor)->instance;
    RoomM273SweepStep *step;
    GteShortVector offset;
    s16 frame;
    s16 angle;
    s16 value;

    if (mode == 0) {
        D_8019AF04.started = 0;
        D_8019AF04.done = 0;
        D_8019AF04.in_window = 0;
        D_8019AF04.in_contact = 0;
        D_8019AF04.step = 0;
        D_8019AF04.count = 0;
        D_8019AF04.hit.pad = 0;
        D_8019AF04.floor = g_RoomFloorY->raw - 0x180;
        instance->owner->flags |= 0x40000000;
    } else if (mode == 1) {
        if (*instance->owner->status == 1) *instance->owner->status = 2;
        if (instance->animation == 13) {
            frame = instance->time.bits.frame;
            step = &D_8019AD80[D_8019AF04.side];
            if (frame == 0) {
                instance->time.fixed = step->start << 16;
                angle = (ratan2(instance->x - g_PlayerEntity->pos_x,
                                       instance->z - g_PlayerEntity->pos_z)
                         - instance->yaw) & 0xFFF;
                if (D_8019AF04.side != 0) {
                    if (angle <= 0x800) D_8019AF04.turn = angle >> 6;
                    else D_8019AF04.turn = 0;
                } else {
                    angle = 0x1000 - angle;
                    if (angle <= 0x800) D_8019AF04.turn = -angle >> 6;
                    else D_8019AF04.turn = 0;
                }
            } else {
                if (frame >= step->end) {
                    D_8019AF04.done = 1;
                    if (instance->owner) *instance->owner->status = 4;
                    return 1;
                }
                if (frame >= step->loop_end && D_8019AF04.loops > 0) {
                    instance->time.fixed = step->loop_start << 16;
                    D_8019AF04.loops--;
                }
            }
            if (frame >= step->loop_start) D_8019AF04.started = 1;
            D_8019AF04.in_window = frame >= step->loop_start && frame <= step->loop_end;
            {
                RoomM273BossInstance *current = ANIM_OWNER_AS(RoomM273BossActor)->instance;
                s16 previous;
                frame = current->time.bits.frame;
                previous = current->previous.bits.frame;
                D_8019AF04.in_contact = frame >= D_8019AF04.contact && previous < D_8019AF04.contact;
            }
            if (frame >= step->loop_start && frame <= step->loop_end) {
                value = step->scale * D_8019AF04.step / D_8019AF04.divisor;
                instance->attack_kind = 0x15;
                instance->attack_power = 2;
                instance->attack_angle = value;
                D_8019AF04.target = step->offset + (value + instance->yaw);
                D_8019AF04.step++;
            } else {
                instance->attack_kind = 0;
                instance->attack_power = 0;
            }
            if (frame > step->loop_end) D_8019AF04.turn >>= 1;
            instance->yaw += D_8019AF04.turn;
        }
    }
    offset.x = 0;
    offset.y = 0;
    offset.z = -0x180;
    {
        GteMatrix *matrix = instance->transforms + 22;
        GteShortVector *out;
        gte_ldrotmatrix(matrix);
        gte_ldtransmatrix(matrix);
        gte_ldv0(&offset);
        gte_rtv0tr_mac();
        out = &D_8019AEFC;
        gte_stsv(out);
    }
    return 0;
}

int func_80198060(int mode, GteShortVector *position) {
    int frame, sample, size, kind, palette;
    u16 clut;

    if (mode == 1) {
        if (D_800E27EC >= 9) return 1;
    } else if (mode == 2) {
        frame = D_800E27EC;
        if (frame - 2 < 0) return 0;

        sample = D_800966EC[((frame - 1) << 7) & 0xF80].part.cos;
        size = sample * 2;
        kind = D_800F3368.palette;
        /* Keep the table read before the palette lookup. */
        asm volatile("" ::: "memory");
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) palette += 4;

        clut = GetClut(32, palette);
        func_800CEE20(position, 0, (s16)size, (s16)size,
                       (s16)D_800F3368.parameter02 + 216, clut, 1,
                       /* The upper half of each packed trig entry is signed. */
                       (s16)(D_800966EC[((D_800E27EC - 1) << 7) & 0xF80].word >> 16) >> 5,
                       (RenderColor *)D_8019AE00);
    }
    return 0;
}

/* Sway shard: flies until it hits the player or the floor (recording the
 * impact in the sway point table), leaving a trail; draws a glow, two
 * crossed flares and two floor rings. */
int func_801981A4(int mode, RoomM273SwayShard *shard) {
    GteShortVector position;
    GteRotation spin;

    if (mode == 1) {
        if (shard->state == 0 && D_8019AF74.cooldown == 0 &&
            shard->position.y > (s16)g_RoomFloorY->raw - 0x240) {
            s16 dx = g_PlayerEntity->render_object.model_matrix.translation[0] - shard->position.x;
            s16 dz = g_PlayerEntity->render_object.model_matrix.translation[2] - shard->position.z;
            if ((s16)Math_IntSqrt(dx * dx + dz * dz) < 0x80) {
                shard->state = 1;
                g_PlayerEntity->state->flags |= 0x4000;
                if (ANIM_OWNER_AS(RoomM273BossActor)->instance->owner)
                    ANIM_OWNER_AS(RoomM273BossActor)->instance->owner->flags |= 0x80000000;
                D_8019AF74.hits[shard->side].x = shard->position.x;
                D_8019AF74.hits[shard->side].y = shard->position.y;
                D_8019AF74.hits[shard->side].z = shard->position.z;
                D_8019AF74.hits[shard->side].pad = 2;
                Asset_Find08w(0x5FC, ANIM_OWNER_AS(RoomM273BossActor)->instance->owner->sound, shard->position.x,
                              shard->position.y, shard->position.z);
                D_8019AF74.cooldown = 0x28;
                return 1;
            }
        }
        shard->position.x += shard->velocity.x;
        shard->position.y += shard->velocity.y;
        shard->position.z += shard->velocity.z;
        if (shard->position.y >= (s16)g_RoomFloorY->raw) {
            D_8019AF74.hits[shard->side].x = shard->position.x;
            D_8019AF74.hits[shard->side].y = shard->position.y;
            D_8019AF74.hits[shard->side].z = shard->position.z;
            D_8019AF74.hits[shard->side].pad = 3;
            Asset_Find08w(0x5FC, ANIM_OWNER_AS(RoomM273BossActor)->instance->owner->sound, shard->position.x,
                          shard->position.y, shard->position.z);
            return 1;
        } else {
            s16 frame = D_800E27EC & 7;
            GteShortVector *trail;
            if (frame < 4) shard->frame = frame;
            else shard->frame = frame + 12;
            trail = func_800CE610(D_8019AF70);
            if (trail == 0) return 0;
            trail->x = shard->position.x;
            trail->y = shard->position.y;
            trail->z = shard->position.z;
        }
    } else if (mode == 2) {
        int i;
        int kind;
        int palette;
        position.x = shard->position.x;
        position.y = shard->position.y;
        position.z = shard->position.z;
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) palette += 4;
        func_800CEE20(&position, 0, 0xA00, 0xA00,
                      (s16)D_800F3368.parameter02 * shard->frame,
                      GetClut(0x10, palette), 1, 0x80, 0);
        spin.x = D_800E27EC * 0x300;
        spin.y = ANIM_OWNER_AS(RoomM273BossActor)->instance->yaw;
        spin.z = 0;
        spin.flags = 1;
        for (i = 0; i < 2; i++) {
            func_800D0728(&position, 0x60, 0x88, 8, &spin, 0x1000, 0x1000, &D_8019AE04,
                          (RenderColor *)D_8019AB70, 0x80, 1);
            spin.y = spin.x;
            spin.x = 0;
        }
        spin.x = 0;
        spin.y = 0;
        spin.flags = 0;
        spin.z = D_800E27EC << 8;
        func_800D004C(&position, 0x100, 0x100, 8, &spin, 0x1000, 0x1000, &D_8019AE08,
                      (RenderColor *)D_8019AB70, 0x80, 1);
        position.y = g_RoomFloorY->raw;
        func_800D004C(&position, 0xC0, 0xC0, 8, 0, 0x1000, 0x1000, &D_8019AE0C,
                      (RenderColor *)D_8019AB70, 0x80, 1);
    }
    return 0;
}

/* Spawns a shard from each sway point aimed at the player while animation
 * 15 swings through, and draws the shard and glow pools. */
int func_801986E8(int mode) {
    GteShortVector target;
    GteShortVector angles;
    GteMatrix matrix;
    int count;

    switch (mode) {
    case 0:
        count = func_800CE560(ANIM_POOL_AS(RoomM273PoolContext)->pool, 0x14, 4,
                              (FieldAnimCallbackListCallback)func_801981A4);
        count += func_800CE5AC(&D_8019AF70, count, 8, 0x14, func_80198060);
        return count;
    case 1:
        if (D_8019AF74.done) return 2;
        if (ANIM_OWNER_AS(RoomM273BossActor)->instance->animation == 15 && D_8019AF74.frame >= 4 &&
            D_8019AF74.frame_1A < 4) {
            int i;
            for (i = 0; i < 2; i++) {
                RoomM273SwayShard *shard = func_800CE610(ANIM_POOL_AS(RoomM273PoolContext)->pool);
                RoomM273BossPlayer *player;
                int dx;
                int dy;
                int dz;
                if (shard == 0) break;
                shard->side = i;
                shard->state = 0;
                player = (RoomM273BossPlayer *)g_PlayerEntity;
                shard->position.x = D_8019AF74.points[i].x;
                shard->position.y = D_8019AF74.points[i].y;
                shard->position.z = D_8019AF74.points[i].z;
                target.x = player->location[0].parts.integer;
                target.y = player->location[1].parts.integer;
                target.z = player->location[2].parts.integer;
                dx = target.x - shard->position.x;
                dz = target.z - shard->position.z;
                dy = target.y - shard->position.y;
                angles.x = (ratan2(dy, Math_IntSqrt(dx * dx + dz * dz)) & 0xFFF) - 0x30;
                angles.y = ANIM_OWNER_AS(RoomM273BossActor)->instance->yaw;
                angles.z = 0;
                if (angles.x < 0x100) angles.x = 0x100;
                if (angles.x > 0x280) angles.x = 0x280;
                RotMatrixYXZ(&angles, &matrix);
                ApplyMatrixSV(&matrix, &D_8019AE10, &shard->velocity);
            }
            target.x = (D_8019AF74.points[0].x + D_8019AF74.points[1].x) / 2;
            target.y = (D_8019AF74.points[0].y + D_8019AF74.points[1].y) / 2;
            target.z = (D_8019AF74.points[0].z + D_8019AF74.points[1].z) / 2;
            Asset_Find08w(0x5D2, ANIM_OWNER_AS(RoomM273BossActor)->instance->owner->sound, target.x, target.y,
                          target.z);
        }
        func_800CE688(D_8019AF70);
        break;
    case 2:
        D_800F3368.parameter00 = 0x10;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 0x10;
        D_800F3368.extent_y = 0x10;
        D_800F3368.extent_x = 0x10;
        D_800F3368.extent_y = 0x10;
        {
            int tpage = D_800E2850[D_800E11E4[2]];
            D_800F3368.palette = 2;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0;
            D_800F3368.tpage = tpage;
        }
        func_800CE78C(D_8019AF70);
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int tpage = D_800E2850[D_800E11E4[3]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}

/* Paired ground sprites and the boss ground ring share sway points and pool records. */

/* The emitter owns the eight-byte pool whose entries this callback renders. */

int func_80198B1C(int mode,Vector *input) {
    Vector position;
    if(mode==1) {
        if(D_800E27EC>=32) return 1;
    } else if(mode==2) {
        unsigned int phase=(unsigned int)(D_800E27EC-1)<<5;
        int shadeSample=*(short *)((char *)D_800966EE+(((phase+1024)&4095)*4));
        int sizeSample=*(short *)((char *)D_800966EE+((phase&4095)*4));
        unsigned int shade;
        int size,kind,palette;
        unsigned short clut;
        position=*input;
        kind=D_800F3368.palette;
        shade=(unsigned int)(shadeSample+4096)>>5;
        size=sizeSample*2+4096;
        palette=D_800E1204[kind];
        clut=GetClut(0,(kind==4 && D_800F3428) ? palette+8 : palette+4);
        func_800CEE20(&position,0,(short)size,(short)size,100,clut,1,(short)shade,0);
        position.y=g_RoomFloorY->raw;
        func_800D004C(&position,384,384,8,&D_8019AB68,4096,4096,
            (RenderColor *)D_8019AE18,(RenderColor *)D_8019AB70,(short)shade,1);
    }
    return 0;
}

int func_80198CD4(int mode) {
    switch(mode) {
    case 0:
        return func_800CE560(ANIM_POOL_AS(RoomM273PoolContext)->pool,8,4,
                             (FieldAnimCallbackListCallback)func_80198B1C);
    case 1: {
        Record *source;
        int i;
        if(D_8019AF74.done) return 2;
        if(D_8019AF74.animation!=15) return 0;
        if(D_8019AF74.frame<4) return 0;
        if(D_8019AF74.frame_1A>=4) break;
        i=0;
        source=(Record *)D_8019AF74.points;
        for(;i<2;++i) {
            Record *output=func_800CE610(ANIM_POOL_AS(RoomM273PoolContext)->pool);
            if(!output) break;
            *output=*source;
            ++source;
        }
        break;
    }
    case 2: {
        unsigned int index=D_800E11FA;
        unsigned short palette;
        D_800F3368.parameter00=32;
        D_800F3368.parameter02=2;
        D_800F3368.extent_x=32;
        D_800F3368.extent_y=32;
        D_800F3368.extent_x=32;
        D_800F3368.extent_y=32;
        palette=D_800E2850[index];

        D_800F3368.palette=3;
        D_800F3368.parameter06=1;
        D_800F3368.parameter0A=0;
        D_800F3368.depth=32;
        D_800F3368.tpage=palette;
        break;
    }
    }
    return 0;
}

/* Ground ring: while it opens (frames up to 24) it knocks the player back
 * once when the player stands inside the pulsing radius; while drawing it
 * shows the swelling ring model, a short flash and two rotating rings. */
int func_80198E94(int mode, GteShortVector *position) {
    GteRotation rotation;
    GteMatrix matrix;
    GteVector scale;
    s16 unused[4];
    int frame;
    int size;
    int fade;
    int value;
    int i;

    if (mode == 1) {
        int now = D_800E27EC;
        if (now >= 0x28) return 1;
        if (now >= 0x19) return 0;
        if (position->pad != 0) return 0;
        if (D_8019AF74.cooldown != 0) return 0;
        {
            int radius = D_800966EC[((now << 11) / 40) & 0xFFF].bits.sin / 16 + 0x80;
            int dx = g_PlayerEntity->render_object.model_matrix.translation[0] - position->x;
            int dz = g_PlayerEntity->render_object.model_matrix.translation[2] - position->z;
            if (radius < func_8005186C(dx * dx + dz * dz)) return 0;
        }
        position->pad = 1;
        g_PlayerEntity->state->flags |= 0x4000;
        if (ANIM_OWNER_AS(RoomM273BossActor)->instance->owner) {
            ANIM_OWNER_AS(RoomM273BossActor)->instance->owner->flags |= 0x80000000;
            D_8019AF74.cooldown = 0x28;
        }
        return 0;
    }
    if (mode != 2) return 0;
    frame = D_800E27EC - 1;
    if (D_800E27EC >= 5 && D_800E27EC < 0x25) {
        int page;
        int kind;
        int palette;
        int cosine;
        int sine;
        i = D_800E27EC - 5;
        if (i < 8) value = D_800966EC[(i << 7) & 0xF80].bits.sin;
        else value = D_800966EC[(((D_800E27EC - 13) << 10) / 24) & 0xFFF].bits.cos;
        page = (D_800E2850[D_800E11FA] | GetTPage(0, 1, 0, 0)) & 0xFFFF;
        fade = value >> 6;
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        size = value >> 2;
        FieldFx_ModelSetTexture(page, GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 6 : palette + 2));
        FieldFx_ModelSetSemiTrans(1);
        FieldFx_ModelSaveColors(D_8019AF04.model);
        FieldFx_ModelScaleColors(D_8019AF04.model, (u16)fade);
        cosine = rcos(frame << 8);
        sine = rsin(frame << 8);
        matrix.m[0][2] = sine;
        matrix.m[2][0] = -sine;
        matrix.m[0][0] = cosine;
        matrix.m[2][2] = cosine;
        matrix.t[2] = 0;
        matrix.t[1] = 0;
        matrix.t[0] = 0;
        matrix.m[2][1] = 0;
        matrix.m[1][2] = 0;
        matrix.m[1][0] = 0;
        matrix.m[0][1] = 0;
        matrix.m[1][1] = 0x1000;
        memset(&scale, 0, sizeof(scale));
        scale.x = size;
        scale.y = size;
        scale.z = size;
        ScaleMatrix(&matrix, &scale);
        matrix.t[0] = position->x;
        matrix.t[1] = position->y;
        matrix.t[2] = position->z;
        FieldFx_ModelDraw((union FieldTexturedModel *)D_8019AF04.model, &matrix);
        FieldFx_ModelRestoreColors(D_8019AF04.model);
    }
    D_800F3368.parameter00 = 0x20;
    D_800F3368.parameter02 = 2;
    D_800F3368.extent_x = 0x20;
    D_800F3368.extent_y = 0x20;
    D_800F3368.extent_x = 0x20;
    D_800F3368.extent_y = 0x20;
    D_800F3368.tpage = D_800E2850[TEXTURE_SLOT_RECORD(D_800E11EA)];
    D_800F3368.palette = 3;
    D_800F3368.parameter06 = 0;
    if (frame < 8) {
        int kind = D_800F3368.palette;
        int palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428 != 0) palette += 4;
        size = D_800966EC[(frame << 7) & 0xF80].bits.sin + 0x1800;
        func_800CEE20(position, 0, size, size, (s16)D_800F3368.parameter02 * frame + 0x80,
                      (u16)GetClut(0x10, palette), 1, 0x80, 0);
    }
    rotation.x = 0x400;
    rotation.y = -frame << 9;
    rotation.flags = 1;
    rotation.z = 0;
    fade = D_800966EC[(frame * 25) & 0xFFF].bits.cos >> 5;
    value = D_800966EC[(frame * 51) & 0xFFF].bits.sin / 16;
    for (i = 0; i < 2; i++) {
        func_800D004C(position, value, value, 8, &rotation, 0x1000, 0x1000,
                      &D_8019AE1C[0], &D_8019AE1C[1], fade, 1);
        value += 0x100;
    }
    return 0;
}

int func_801993F0(int mode) {
    switch (mode) {
    case 0:
        D_8019AF6C = Asset_FindTable08ByU32Key(D_800B0E64[0], 0xC5541704);
        FieldFx_ModelScrollUv(D_8019AF6C, 0, 0);
        return func_800CE560(ANIM_POOL_AS(RoomM273PoolContext)->pool, 8, 4,
                             (FieldAnimCallbackListCallback)func_80198E94);
    case 1: {
        int i;
        unsigned char *flags;
        Entry *source;
        unsigned char *anchor = (unsigned char *)&D_8019AF84 + 30;
        if (*anchor) return 2;
        i = 0;
        flags = anchor - 24;
        source = (Entry *)(anchor - 30);
        for (; i < 2; ++i) {
            asm("" : "=r"(source) : "0"(source));
            if (*(unsigned short *)flags & 1) {
                Entry *entry = func_800CE610(ANIM_POOL_AS(RoomM273PoolContext)->pool);
                if (!entry) return 0;
                entry->emitter.x = source->emitter.x;
                entry->emitter.y = source->emitter.y;
                entry->emitter.z = source->emitter.z;
                entry->emitter.flags = 0;
                *(unsigned short *)flags &= 0xFFFE;
            }
            flags += 8;
            ++source;
        }
        break;
    }
    case 2:
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0;
        break;
    }
    return 0;
}

int func_80199568(int mode, RoomM273RisingParticle *particle) {
    if (mode == 1) {
        int frame = D_800E27EC;
        if (frame >= 36) return 1;
        if (frame >= 4) {
            unsigned short y = ((volatile RoomM273RisingParticle *)particle)->y;
            s16 speed = ((volatile RoomM273RisingParticle *)particle)->speed;
            int delta = ((RoomM273RisingParticle *)particle)->speed;
            speed += 1;
            particle->y = y - delta;
            particle->speed = speed;
        }
    } else if (mode == 2) {
        int frame = D_800E27EC - 5;
        int kind, palette, clut;
        if (frame < 0) return 0;
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(16, palette);
        func_800CEE20((GteShortVector *)particle, 0, 12288, 12288,
            (s16)D_800F3368.parameter02 * (frame / 4) + 200, clut, 1,
            (short)((int *)D_800966EC)[(((unsigned int)frame << 8) & 0x3F00) / 4] >> 5, 0);
    }
    return 0;
}

int func_801996C4(int mode)
{
    RoomM273TemplatePoint *template;
    RoomM273DelayedBurstRecord *particle;
    RoomM273TrigEntry *entry;
    int i;
    int j;
    int angle;
    int offset;
    int flagOffset;
    int sixteen;
    u16 paletteIndex;
    u8 *stop;
    u16 *flag;
    RoomM273TemplatePoint *source;
    u16 palette;

    switch (mode) {
    case 0:
        return func_800CE560(ANIM_POOL_AS(RoomM273PoolContext)->pool, 8, 24, (int (*)())func_80199568);
    case 1:
        stop = &D_8019AFA2;
        if (*stop) return 2;
        i = 0;
        template = (RoomM273TemplatePoint *)(stop - 0x1E);
        offset = 0;
        do {
            flag = (u16 *)(D_8019AF8A + offset);
            if (*flag & 2) {
                j = 0;
                flagOffset = offset;
                source = template;
                do {
                    particle = (RoomM273DelayedBurstRecord *)func_800CE610(ANIM_POOL_AS(RoomM273PoolContext)->pool);
                    if (!particle) break;
                    particle->emitter = *source;
                    angle = ((j << 12) / 6) & 0xFFF;
                    entry = &D_800966EC[angle];
                    particle->emitter.x += (entry->low * 3 * 64) / 4096;
                    particle->emitter.z += (entry->high * 3 * 64) / 4096;
                    particle->emitter.flags = 0;
                    *(u16 *)(D_8019AF8A + flagOffset) &= ~2;
                    j++;
                } while (j < 6);
            }
            template++;
            i++;
            offset += 8;
        } while (i < 2);
        break;
    case 2:
        paletteIndex = D_800E11E8;
        sixteen = 16;
        D_800F3368.parameter00 = sixteen;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = sixteen;
        D_800F3368.extent_y = sixteen;
        D_800F3368.extent_x = sixteen;
        D_800F3368.extent_y = sixteen;
        palette = D_800E2850[paletteIndex];
        D_800F3368.palette = 2;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0;
        D_800F3368.tpage = palette;
        break;
    }
    return 0;
}

/* Pointer sprite pool and boss sway state share D_8019AF74 points. */

/* The emitter owns the pointer entries rendered by this callback. */

int func_80199950(int mode, RoomM273PointerPoolEffect *effect) {
    /* Layout only: the original purpose of these eight frame bytes is unknown. */
    int stack_pad[2];
    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
    } else if (mode == 2) {
        int frame = (short)(D_800E27EC - 1);
        int kind = D_800F3368.palette;
        unsigned int sizeOffset, shadeOffset;
        s16 size;
        s16 shade;
        int palette;
        unsigned short clut;
        sizeOffset = ((unsigned int)frame << 8) & 0x3F00;
        shadeOffset = ((unsigned int)frame << 9) & 0x3E00;
        size = D_800966EC[sizeOffset / 4].word + 2048;
        shade = (short)D_800966EC[shadeOffset / 4].word >> 6;

        palette = D_800E1204[kind];
        clut = GetClut(0, (kind == 4 && D_800F3428) ? palette + 7 : palette + 3);
        func_800CEE20(effect->position, 0, (short)size, (short)size,
            64, clut, 1, (short)shade, 0);
    }
    return 0;
}

int func_80199A90(int mode) {
    switch(mode) {
    case 0:
        D_8019AE88=0;
        return func_800CE560(ANIM_POOL_AS(RoomM273PoolContext)->pool,4,6,
                             (FieldAnimCallbackListCallback)func_80199950);
    case 1: {
        int i;
        Position *position;
        if(D_8019AE88) return 2;
        if(D_8019AF74.animation==15 && D_8019AF74.frame>=25) {
            D_8019AE88=1;
            return 2;
        }
        if(D_800E27EC&7) return 0;
        i=0;
        position=D_8019AF74.points;
        for(;i<2;++i) {
            RoomM273PointerPoolEffect *output=func_800CE610(ANIM_POOL_AS(RoomM273PoolContext)->pool);
            if(!output) break;
            output->position=position;
            ++position;
        }
        break;
    }
    case 2: {
        unsigned int index=D_800E11FA;
        unsigned short palette;
        D_800F3368.parameter00=64;
        D_800F3368.parameter02=4;
        D_800F3368.extent_x=64;
        D_800F3368.extent_y=64;
        D_800F3368.extent_x=64;
        D_800F3368.extent_y=64;
        palette=D_800E2850[index];

        D_800F3368.palette=3;
        D_800F3368.parameter06=1;
        D_800F3368.parameter0A=0;
        D_800F3368.depth=32;
        D_800F3368.tpage=palette;
        break;
    }
    }
    return 0;
}

/* Animation 15 controller: transforms two model points, then sways the
 * boss toward or away from the player for eight frames. */
int func_80199C50(int mode) {
    RoomM273BossInstance *instance = ANIM_OWNER_AS(RoomM273BossActor)->instance;
    GteShortVector offset;
    int value; /* loop index, then the player heading */
    int frame;
    int frame_1A;
    GteShortVector *point;

    if (mode == 0) {
        D_8019AF74.done = 0;
        instance->owner->flags |= 0x40000000;
        D_8019AF74.points[0].pad = 0;
        D_8019AF74.points[1].pad = 1;
        D_8019AF74.hits[0].pad = 0;
        D_8019AF74.hits[1].pad = 0;
        D_8019AF74.sway_timer = 0;
        D_8019AF74.cooldown = 0;
    } else if (mode == 1) {
        if (*instance->owner->status == 1) *instance->owner->status = 2;
        frame = instance->time.parts.frame;
        frame_1A = instance->previous.parts.frame;
        D_8019AF74.animation = instance->animation;
        D_8019AF74.frame = frame;
        D_8019AF74.frame_1A = frame_1A;
        offset.x = 0;
        offset.y = 0;
        offset.z = -0x130;
        point = D_8019AF74.points;
        for (value = 0; value < 2; value++) {
            GteMatrix *m = instance->transforms;
            if (value) m += 36;
            else m += 25;
            gte_ldrotmatrix(m);
            gte_ldtransmatrix(m);
            gte_ldv0(&offset);
            gte_rtv0tr_mac();
            gte_stsv(&point[value]);
        }
        if (instance->animation == 15) {
            if ((s16)frame >= instance->frame_count - 1) {
                D_8019AF74.done = 1;
                if (instance->owner) *instance->owner->status = 4;
                return 1;
            }
            if ((s16)frame >= 0x16) {
                if (D_8019AF74.repeat-- > 0) instance->time.fixed = 0;
            }
        }
        if (D_8019AF74.repeat && (s16)frame >= 0xF && (s16)frame_1A < 0xF) {
            int step;
            D_8019AF74.sway_timer = 8;
            value = (s16)FieldFx_VecToAngle((Vec3Fixed *)g_PlayerEntity->render_object.model_matrix.translation, (Vec3Fixed *)instance->position);
            step = 0x20;
            if ((value - instance->yaw) & 0x800) step = -0x20;
            D_8019AF74.sway_step = step;
        }
        if (D_8019AF74.sway_timer) {
            instance->yaw += D_8019AF74.sway_step *
                             D_800966EC[(D_8019AF74.sway_timer << 7) & 0xF80].bits.sin / 4096;
            D_8019AF74.sway_timer--;
        }
        if (D_8019AF74.cooldown) D_8019AF74.cooldown--;
    }
    return 0;
}

short *func_80199F70(void *arg0, void *arg1, short arg2) {
    short *ptr = &D_8019AE98;

    *ptr = arg2;
    return ptr - 6;
}

int func_80199F84(int mode, RoomM273PulseState *state) {
    RoomM273Vector position, rotation;
    s32 frame;
    s32 *source;
    s32 positionX;
    int scaleX, scaleY;
    char *texture;

    if (mode == 1) {
        if (D_800E27EC >= 9) return 1;
        state->value += state->step;
    } else if (mode == 2) {
        source = state->source;
        positionX = source[0];
        position.y = source[1];
        position.z = source[2];
        rotation.x = 0;
        rotation.y = state->value;
        rotation.z = 0;
        frame = D_800E27EC - 1;
        scaleX = state->scaleX;
        scaleY = state->scaleY;
        texture = D_8019AB70;
        position.x = positionX;
        func_800D0E88(&position, (GteRotation *)&rotation, scaleX, scaleY, (RenderColor *)state,
                       (RenderColor *)texture, (RenderColor *)texture,
                       D_800966EC[(frame << 7) & 0xF80].bits.cos >> 5, 1);
    }
    return 0;
}

void func_80194B5C(void);

s32 func_8019A068(s32 state) {
    GteShortVector *obj;
    RoomM273PlayerActorView *root;
    RoomM273ThresholdTransform *source;
    u8 status;

    if (state != 1) {
        if (state >= 2) {
            return 0;
        }
        if (state != 0) {
            return 0;
        }
        D_8019AE68 = 0;
        return func_800CE560(ANIM_POOL_AS(RoomM273PoolContext)->pool, 8, 5,
                             (FieldAnimCallbackListCallback)func_80194B5C);
    }
    if (D_8019AE68 != 0) {
        return 2;
    }

    root = ANIM_OWNER_AS(RoomM273EffectStateContext)->state.player;
    status = root->kind;
    if (status != 0x10 && status != 8) {
        D_8019AE68 = 1;
        return 2;
    }
    if ((D_800E27EC & 3) != 0) {
        return 0;
    }

    obj = func_800CE610(ANIM_POOL_AS(RoomM273PoolContext)->pool);
    if (obj != 0) {
        root = ANIM_OWNER_AS(RoomM273EffectStateContext)->state.player;
        source = (RoomM273ThresholdTransform *)root->tail.object;
        obj->x = source->x;
        obj->y = source->y;
        obj->z = source->z;
    }
    return 0;
}

int func_80052B2C(void);

void *func_8019A1A0(void *unused, int mode) {
    int direction;
    int value;

    D_8019AEFA = mode;
    switch (mode) {
    case 0:
        D_8019AEEE = 0;
        D_8019AEEA = 2;
        break;
    case 1:
        direction = (func_80052B2C() & 1) ? 42 : -42;
        D_8019AEF2 = 2;
        D_8019AEEA = 2;
        PE1_COMPILER_MEMORY_BARRIER();
        value = 0x61;
        D_8019AEEE = direction;
        D_8019AEF6 = value;
        break;
    case 2:
        direction = (func_80052B2C() & 1) ? 0x80 : -0x80;
        D_8019AEF2 = 4;
        D_8019AEEA = 5;
        PE1_COMPILER_MEMORY_BARRIER();
        value = 0xB0;
        D_8019AEEE = direction;
        D_8019AEF6 = value;
        break;
    }
    return &D_8019AE9C;
}

int func_8019A290(int mode, RoomM273PhaseState *state) {
    RoomM273Vector position;
    int frame, size;
    RoomM273TrigEntry *entry;

    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
    } else if (mode == 2) {
        frame = D_800E27EC - 1;
        entry = &D_800966EC[((frame << 8) & 0x3F00) >> 2];
        size = (s16)(entry->low * 2 + 2048);
        position.x = state->source[0];
        position.y = g_RoomFloorY->raw;
        position.z = state->source[2];
        func_800D004C(&position, 192, 192, 10,
                       (GteRotation *)(D_8019ACCC + ((frame & 1) << 3)), size, size,
                       (RenderColor *)D_8019AB70, (RenderColor *)D_8019ACC8, entry->high >> 5, 1);
    }
    return 0;
}
