#include "common.h"
#include "pe1/akao/commands.h"
#include "pe1/asset_find.h"
#include "pe1/field_actor.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_sprite_params.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_vector_math.h"
#include "pe1/fieldfx_callback_list.h"
#include "pe1/fieldfx_task_pool.h"
#include "pe1/gte.h"
#include "pe1/gte_sine_table.h"
#include "pe1/gte_types.h"
#include "pe1/player_entity.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_object.h"
#include "pe1/room_floor.h"
#include "pe1/room_m350_drop.h"
#include "pe1/room_m350_effects.h"

#include "m350_callbacks_externs.h"
#include "room_m350_data.h"
#include "room_m350_shared.h"

typedef struct {
    char reserved[24];
    unsigned char *state;
} Actor;

typedef struct {
    Actor *actor;
    char reserved04[10];
    unsigned char animation;
    unsigned char frameCount;
    int reserved10;
    union { int word; short halves[2]; } frame;
} Animation;

typedef struct { int reserved[19]; unsigned int flags; } ActorFlags;

typedef struct {
    ActorFlags *actor;
    char reserved04[504];
    int x, y, z;
} WorldActor;

typedef struct { unsigned int *actor; } RoomActor;

typedef RoomM350Vector Vector;

typedef GteMatrix Matrix;

typedef RoomM350Drop Particle;

typedef RenderMatrix Transform;

typedef FieldActor Instance;

typedef FieldActor Player;

typedef struct { int asset; short speed,targetSpeed,heightPhase,mode; } State;

typedef struct { short count,delay; } FadingModelState;

typedef struct { short count, delay; } EmissionState;

typedef struct { signed int scale:16; signed int shade:16; } EffectEntry;

int func_80196E04(int event) {
    Animation *animation = ANIM_OWNER_AS(Animation *)[2];
    if (event == 0) {
        D_8019A828 = 4;
        D_8019A82E = 0;
        D_8019A82F = 0;
        D_8019A830 = 0;
    } else if (event == 1) {
        register short frame;
        if (animation->actor) {
            unsigned char *state = animation->actor->state;
            if (*state == 1) *state = 2;
        }
        frame = animation->frame.halves[1];
        if (animation->animation == 9) {
            short *remaining = &D_8019A828;
            if (*remaining > 0) {
                if (frame >= 22) {
                    animation->frame.word = 0x30000;
                    *remaining = *(volatile unsigned short *)remaining - 1;
                }
            } else if (frame >= animation->frameCount - 1) {
                if (animation->actor) *animation->actor->state = 4;
                D_8019A82E = 1;
                return 1;
            }
        }
    }
    return 0;
}

/* Falling drop: moves until it passes the floor (splash) or touches the
 * player, otherwise queues its position for the trail; draws a glow and a
 * floor ring. */
int func_80196F2C(int mode, RoomM350Drop *drop) {
    GteShortVector position;

    if (mode == 1) {
        if (drop->landed) return 1;
        drop->x += drop->velocity.x;
        drop->y += drop->velocity.y;
        drop->z += drop->velocity.z;
        if (drop->y > g_RoomFloorY->y) {
            g_RoomEffectTrailPositions.splashed = 1;
            g_RoomEffectTrailPositions.splash_x = drop->x;
            g_RoomEffectTrailPositions.splash_z = drop->z;
            g_RoomEffectTrailPositions.touched = 1;
            g_RoomEffectTrailPositions.hit_x = drop->x;
            g_RoomEffectTrailPositions.hit_y = drop->y - 0xA0;
            g_RoomEffectTrailPositions.hit_z = drop->z;
            func_8019721C((short *)drop, 200);
            return 1;
        }
        if (func_8019721C((short *)drop, 0x50)) {
            g_RoomEffectTrailPositions.touched = 1;
            g_RoomEffectTrailPositions.hit_x = drop->x;
            g_RoomEffectTrailPositions.hit_y = drop->y;
            g_RoomEffectTrailPositions.hit_z = drop->z;
            drop->landed = 1;
        } else if (g_RoomEffectTrailPositions.count < 4) {
            GteShortVector *point =
                &g_RoomEffectTrailPositions.points[g_RoomEffectTrailPositions.count++];
            point->x = drop->x;
            point->y = drop->y;
            point->z = drop->z;
        }
    } else if (mode == 2) {
        int kind;
        int palette;
        position.x = drop->x;
        position.y = drop->y;
        position.z = drop->z;
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int tpage = D_800E2850[D_800E11E4[11]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0;
            D_800F3368.tpage = tpage;
        }
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        func_800CEE20(&position, 0, 0xA00, 0xA00,
                      (s16)D_800F3368.parameter02 * 6 + 0x80,
                      GetClut(0, (kind == 4 && D_800F3428) ? palette + 8 : palette + 4),
                      1, 0x80, &D_8019A61C[0]);
        position.y = g_RoomFloorY->y;
        func_800D004C(&position, 0x60, 0x60, 6, &D_8019A3C0, 0x1000, 0x1000,
                      &D_8019A61C[1], &D_8019A61C[2], 0x80, 1);
    }
    return 0;
}

int func_8019721C(short *point, int radius) {
    short *gate = &D_8019A86C;
    int dx, dz;
    WorldActor *world;
    if (*gate != 0) return 0;
    if (point[5] < g_RoomFloorY->y - 512) return 0;
    world = (WorldActor *)g_PlayerEntity;
    dx = world->x - point[4];
    if (dx < 0) dx = point[4] - world->x;
    if (dx > radius) return 0;
    dz = world->z - point[6];
    if (dz < 0) dz = point[6] - world->z;
    if (dz > radius) return 0;
    if (Math_IntSqrt(dx * dx + dz * dz) > radius) return 0;
    g_PlayerEntity->state->flags |= 0x4000;
    if (ANIM_OWNER_AS(RoomActor *)[2]->actor) *ANIM_OWNER_AS(RoomActor *)[2]->actor |= 0x80000000U;
    *gate = 30;
    return 1;
}

int func_80197364(int event)
{
    Matrix matrix;
    Vector vector;
    if (event != 1) {
        if (event < 2) {
            if (event != 0) {
                return 0;
            }
        } else {
        goto done;
        }
        return func_800CE560(ANIM_POOL_AS(RoomM350Emitter)->pool, 16, 4,
                             (FieldFxCallbackListCallback)func_80196F2C);
    }
update:
    {
        Particle *effect;
        Instance *instance;
        Transform *transform;
        Player *player;
        int dx, dz, dy;
        int angle;
        if (D_8019A86E) return 2;
        if (!D_8019A855) return 0;
        effect = func_800CE610(ANIM_POOL_AS(RoomM350Emitter)->pool);
        if (!effect) return 0;
        instance = (Instance *)ANIM_OWNER_AS(RoomM350Actor)->instance;
        transform = instance->render_object.matrices;
        player = g_PlayerEntity;
        effect->x = transform[7].translation[0];
        effect->y = transform[7].translation[1];
        effect->z = transform[7].translation[2];
        effect->landed = 0;
        dx = transform[7].translation[0] - player->render_object.model_matrix.translation[0];
        dz = transform[7].translation[2] - player->render_object.model_matrix.translation[2];
        dy = transform[7].translation[1] - player->render_object.model_matrix.translation[1];
        angle = ratan2(dy, Math_IntSqrt(dx * dx + dz * dz));
        angle += (signed char)Inv_ScrambleGrid() / 2;
        angle &= 0xFFF;
        vector.x = angle;
        if (angle < 0xD00) vector.x = 0xD00;
        {
            int offset = (signed char)Inv_ScrambleGrid();
            Vector *v = &vector;
            register Vector *input asm("$4") = v;
            register Matrix *m asm("$5") = &matrix;
            int yaw = (u16)instance->rot_y;
            /* Keep the rotation arguments ready before adding the yaw offset. */
            asm("" : : "r"(input), "r"(m));
            offset += 0x800;
            yaw += offset;
            vector.y = yaw;
            vector.z = 0;
            RotMatrixYXZ(input, m);
        }
        vector.x = 0;
        vector.y = 0;
        vector.z = 128;
        ApplyMatrixSV(&matrix, &vector, &effect->velocity);
        {
            RoomM350Actor *actor = ANIM_OWNER_AS(RoomM350Actor);
            D_8019A855 = 0;
            /* Publish consumption before loading the sound's position and owner. */
            asm volatile("" : : : "memory");
            {
                int x = effect->x;
                Asset_Find08w(0x60A, ((Instance *)actor->instance)->state->progress,
                    x, effect->y, effect->z);
            }
        }
        D_8019A86A = 8;
    }
done:
    return 0;
}

int func_80197594(int event, short *position) {
    if (event == 1) {
        if (D_800E27EC >= 16) return 1;
        position[1] -= position[3];
        position[3] += 2;
    } else if (event == 2) {
        int kind = D_800F3368.palette;
        int texture = D_800E1204[kind];
        int handle;
        handle = GetClut(0, (kind == 4 && D_800F3428 != 0) ? texture + 6 : texture + 2);
        func_800CEE20((GteShortVector *)position, 0, 6144, 6144,
            (s16)D_800F3368.parameter02 * (((D_800E27EC - 1) >> 1) & 7),
            (unsigned short)handle, 1,
            D_800966EC[((unsigned int)(D_800E27EC - 1) << 6) & 0xFC0].bits.cos >> 5,
            &D_8019A62C);
    }
    return 0;
}

int func_801976C8(int event)
{
    switch (event) {
    case 0:

        return func_800CE560(ANIM_POOL_AS(RoomM350Emitter)->pool, 8, 4,
                             (FieldFxCallbackListCallback)func_80197594);
        case 1:
    if (D_8019A86E) return 2;
    if (D_8019A857) {
        short *effect = func_800CE610(ANIM_POOL_AS(RoomM350Emitter)->pool);
        if (!effect) return 0;
        do {
            effect[0] = D_8019A864[0];
            effect[1] = D_8019A864[1];
            effect[2] = D_8019A864[2];
            effect[3] = 0;
        } while (0);
        D_8019A857 = 0;
    }
        break;
    case 2:

    D_800F3368.parameter00 = 32;
    D_800F3368.parameter02 = 2;
    D_800F3368.extent_x = 32;
    D_800F3368.extent_y = 32;
    D_800F3368.extent_x = 32;
    D_800F3368.extent_y = 32;
    D_800F3368.tpage = D_800E2850[D_800E11E4[11]];
    D_800F3368.palette = 3;
    D_800F3368.parameter06 = 1;
    D_800F3368.parameter0A = 0;
    D_800F3368.depth = 0;
    }
    return 0;
}

/* Retail writes both fields twice during configuration. */

int func_8019784C(int event)
{
    switch (event) {
    case 0:
        return func_800CE560(ANIM_POOL_AS(RoomM350Emitter)->pool, 8, 16,
                             (FieldFxCallbackListCallback)func_8019A014);
    case 1:
    if (D_8019A86E) return 2;
    {
        int i;
        for (i = 0; i < D_8019A85C; i++) {
            short *effect = func_800CE610(ANIM_POOL_AS(RoomM350Emitter)->pool);
            if (!effect) break;
            effect[0] = g_RoomEffectTrailPositions.points[i].x;
            effect[1] = g_RoomEffectTrailPositions.points[i].y;
            effect[2] = g_RoomEffectTrailPositions.points[i].z;
            /* Finish the copy before reloading the possibly aliased count. */
            asm("" : "=m"(D_8019A85C) : "m"(D_8019A85C), "m"(effect[2]));
        }
        D_8019A85C = 0;
    }
    break;
    case 2:
    {
        int unit = 16;
        int index = D_800E11E8;
        int palette;
        D_800F3368.parameter00 = unit;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = unit;
        D_800F3368.extent_y = unit;
        D_800F3368.extent_x = unit;
        D_800F3368.extent_y = unit;
        palette = D_800E2850[index];
        D_800F3368.palette = 2;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0;
        D_800F3368.tpage = palette;
    }
    }
    return 0;
}

int func_80197A04(int event, short *object)
{
    short position[4];
    if (event == 1) {
        if (D_800E27EC >= 8) {
            return 1;
        }
    } else if (event == 2) {
        int scale = (4096 - D_800966EC[((unsigned int)(D_800E27EC - 1) << 7) & 0xF80].part.sin) * 2;
        int *origin = ANIM_OWNER_AS(RoomM350EffectActor)->instance->transforms[7].t;
        int i;
        int kind, palette, handle;

        for (i = 0; i < 3; i++) {
            position[i] = object[i] * scale / 4096 + origin[i];
        }
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) {
            palette += 4;
        }
        handle = GetClut(32, palette);
        func_800CEE20((GteShortVector *)position, 0, 5120, 5120, (s16)D_800F3368.parameter02 * 2 + 216,
            (unsigned short)handle, 1,
            (short)D_800966EC[((unsigned int)(D_800E27EC - 1) << 7) & 0xF80].word >> 5,
            (struct RenderColor *)D_8019A634);
    }
    return 0;
}
/* Retail writes both fields twice during configuration. */

int func_80197B98(int event)
{
    switch (event) {
    case 0:

        return func_800CE560(ANIM_POOL_AS(RoomM350Emitter)->pool, 8, 16,
                             (FieldFxCallbackListCallback)func_80197A04);
        case 1:
    if (D_8019A86E) return 2;
    if (D_8019A859) {
        register int i asm("$18") = 0;
        while (i < 2) {
            Matrix matrix;
            Vector vector;
            int random;
            int highByte;
            int divisor;
            short *effect = func_800CE610(ANIM_POOL_AS(RoomM350Emitter)->pool);
            if (!effect) break;
            i++;
            vector.x = Inv_ScrambleGrid() << 4;
            vector.y = Inv_ScrambleGrid() << 4;
            vector.z = 0;
            RotMatrixYXZ(&vector, &matrix);
            /* Keep the two phases separate in CSE without emitting instructions. */
            asm volatile("" : : "i"(&&offset));
offset:
            vector.x = 0;
            vector.y = 0;
            highByte = Inv_ScrambleGrid();
            random = (highByte << 8) | Inv_ScrambleGrid();
            divisor = 64;
            vector.z = random % divisor + 256;
            ApplyMatrixSV(&matrix, &vector, (Vector *)effect);
        }
        {
            unsigned char *request = &D_8019A859;
            *request = *request - 1;
        }
    }
        break;
    case 2:

    {
        int unit = 16;
        int index = D_800E11E8;
        int palette;
        D_800F3368.parameter0A = 0;
        D_800F3368.parameter00 = unit;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = unit;
        D_800F3368.extent_y = unit;
        D_800F3368.extent_x = unit;
        D_800F3368.extent_y = unit;

        palette = D_800E2850[index];
        D_800F3368.palette = 2;
        D_800F3368.parameter06 = 0;
        D_800F3368.depth = 0;
        D_800F3368.tpage = palette;
    }
    }
    return 0;
}

int func_80197DA8(int event,short *cursor) {
    Events events=D_8018F1CC;
    Vector movement;
    Instance *instance=(Instance *)ANIM_OWNER_AS(RoomM350Actor)->instance;
    int frame,previous;
    short phase;
    if (event==0) {
        *(int *)&D_8019A854=0;
        D_8019A858=0;
        D_8019A86E=0;
        D_8019A85C=0;
        D_8019A86A=0;
        D_8019A86C=0;
        *cursor=0;
    } else {
        if (event!=1) return 0;
        frame=instance->anim.parts.integer;
        previous=(u16)(instance->anim_prev >> 16);
        if (instance->state && *instance->state->action_state==1)
        *instance->state->action_state=2;
        {
        short *timer=&D_8019A86C;
        if (*timer) --*timer;
        }
        if (instance->mode==6) {
        if ((short)frame<6) return 0;
        if ((short)previous<6) D_8019A859=5;
        return 0;
        }
        if (D_8019A85E>0) {
        if ((short)frame>=31) {
        instance->anim.fixed=0x60000;
        asm volatile("" ::: "memory");
        D_8019A85E--;
        }
        } else if ((short)frame>=instance->action-1) {
        if (instance->state) *instance->state->action_state=4;
        D_8019A86E=1;
        return 1;
        }
        if ((unsigned int)(frame-7)<8) phase=15-frame;
        else if ((unsigned int)(frame-19)<8) phase=27-frame;
        else phase=0;
        if (phase) {
        int speed=(short)D_800966EC[(((unsigned int)phase<<10)&0x3C00)/4].word>>6;
        int angle=FieldFx_VecToAngle(g_PlayerEntity->render_object.model_matrix.translation,instance->render_object.model_matrix.translation);
        instance->rot_y=FieldFx_TurnToward(instance->rot_y,(short)angle,speed);
        }
        while ((short)frame>=events.entries[*cursor].frame) {
        Event *entry=&events.entries[*cursor];
        if ((short)previous>=entry->frame) break;
        (&D_8019A854)[entry->slot]=entry->value;
        *cursor+=1;
        if ((unsigned short)*cursor>=8) *cursor=2;
        }
        {
        short *timer=&D_8019A86A;
        int remaining;
        remaining=*timer;
        if (remaining>0) {
        movement.y=0;
        movement.x=0;
        movement.z=D_800966EE[(((8-remaining)<<9)&0x3E00)/2]/256;
        ApplyMatrixSV(&instance->render_object.model_matrix,&movement,&movement);
        instance->pos_x+=(unsigned int)(int)movement.x<<16;
        instance->pos_z+=(unsigned int)(int)movement.z<<16;
        --*timer;
        }
        }
    }
done:
    return 0;
}

int func_801981D0(int event, RoomM350FlareParticle *particle)
{
    GteShortVector position;
    GteRotation rotation;
    if (event == 1) {
        int timer = D_800E27EC;
        if (timer >= 8) return 1;
        particle->shade = (short)D_800966EC[((unsigned int)timer << 8) & 0xF00].word >> 5;
    } else if (event == 2) {
        int kind, palette;
        unsigned short clut;
        position.x = 0;
        position.y = particle->y;
        position.z = particle->z;
        {
            register GteMatrix *matrix asm("$8") = &D_8019A870;
            GteShortVector *out;
            gte_ldrotmatrix(matrix);
            gte_ldtransmatrix(matrix);
            out = &position;
            gte_ldv0(out);
            gte_rtv0tr_mac();
            gte_stsv(out);
        }
        position.x += D_8019A890[0];
        position.y += D_8019A890[1];
        position.z += D_8019A890[2];
        rotation.x = (unsigned int)D_800E27EC * 384;
        rotation.y = ((FieldActor *)ANIM_OWNER_AS(RoomM350EffectActor)->instance)->rot_y;
        rotation.z = 0;
        rotation.flags = 1;
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(0, palette);
        {
            int timer = D_800E27EC;
            register int scale asm("$4") = (s16)D_800F3368.parameter02;
            register int phase asm("$3") = timer & 7;
            register int product asm("$8") = scale * phase;
            int texture = product + 64;
            asm("" : : "r"(product), "r"(texture));
            func_800CEE20(&position, &rotation, 0x2200, 4096,
                texture, clut, 1, particle->shade, (struct RenderColor *)&D_8019A690[timer & 3]);
        }
    }
    return 0;
}

/* Spawns flares and tests the quad swept by the actor's two side endpoints. */

/* The endpoint records' X words, read whole through GteShortVector views. */

int func_80198400(int event) {
    GteShortVector offset,vertices[4];
    GteShortVector *previousEndpoint;
    const GteShortVector *currentEndpoint;
    GteShortVector *previousOppositeEndpoint;
    const GteShortVector *currentOppositeEndpoint;
    int area;
    FieldActor *instance=(FieldActor *)ANIM_OWNER_AS(RoomM350EffectActor)->instance;
    switch (event) {
    case 0:

        return func_800CE560(ANIM_POOL_AS(RoomM350EffectEmitter)->pool,8,4,
                             (FieldFxCallbackListCallback)func_801981D0);
        case 1:
    previousEndpoint=&D_8019A8A8;
    previousOppositeEndpoint=&D_8019A8B0;
    currentOppositeEndpoint=(const GteShortVector *)&D_8019A8A0;
    if(instance->anim.parts.integer>=31) return 2;
    currentEndpoint=(const GteShortVector *)&D_8019A898;
    if(!(D_800E27EC&1)) {
        RoomM350FlareParticle *particle=func_800CE610(ANIM_POOL_AS(RoomM350EffectEmitter)->pool);
        if(particle) {
            int random=Inv_ScrambleGrid()&255;
            particle->y=random;
            particle->z=(random>>2)+64;
        }
    }
    if(D_8019A8C0 || instance->mode!=9) return 0;
    *previousEndpoint=*currentEndpoint;
    *previousOppositeEndpoint=*currentOppositeEndpoint;
    offset.x=256; offset.y=0; offset.z=0;
    ApplyMatrixSV((GteMatrix *)&instance->render_object.model_matrix,&offset,&offset);
    {
        int positionX=instance->render_object.model_matrix.translation[0];
        unsigned short x;
        unsigned short z;
        int positionZ;
        x=offset.x;
        z=offset.z;
        D_8019A898=x+positionX;
        positionZ=((volatile FieldActor *)instance)->render_object.model_matrix.translation[2];
        D_8019A89E=1;
        positionZ=z+positionZ;
        D_8019A89C=positionZ;
        D_8019A8A0=((volatile FieldActor *)instance)->render_object.model_matrix.translation[0]-x;
        D_8019A8A4=((volatile FieldActor *)instance)->render_object.model_matrix.translation[2]-z;
    }
    asm volatile("" : : "m"(D_8019A8A4));
    vertices[0]=*(GteShortVector *)&D_8019A898;
    vertices[1]=*(GteShortVector *)&D_8019A8A0;
    vertices[2]=D_8019A8B0;
    {
        /* The saved endpoint uses its fourth halfword as a validity flag. */
        int valid=D_8019A8A8.pad;
        vertices[3]=D_8019A8A8;
        if(!valid) return 0;
        {
            int i=0;
            int previous=((unsigned int)vertices[3].z<<16)|(unsigned short)vertices[3].x;
            int point=((unsigned int)g_PlayerEntity->render_object.model_matrix.translation[2]<<16)|(unsigned short)g_PlayerEntity->render_object.model_matrix.translation[0];
            int *out=&area;
            for(;i<4;) {
                int current=((unsigned int)vertices[i].z<<16)|(unsigned short)vertices[i].x;
                gte_ldsxy0(point); gte_ldsxy2(previous); gte_ldsxy1(current);
                gte_nclip(); gte_stmac0(out);
                if(area<0) break;
                i++;
                previous=current;
            }
            if(i<4) return 0;
            {
                FieldActor *player=g_PlayerEntity;
                asm("" : : "r"(player));
                D_8019A8C0=1;
                asm volatile("" : : "m"(D_8019A8C0));
                player->state->flags|=0x4000;
                if(instance->state) instance->state->core_flags|=0x80000000u;
            }
        }
    }
        break;
    case 2:

    {
        short i=0;
        int index,color;
        short *destination=D_8019A890;
        GteMatrix *transforms;
        transforms=(GteMatrix *)instance->render_object.matrices;
        for(;i<3;i++) destination[i]=(transforms[11].t[i]+transforms[15].t[i])>>1;
        index=D_800E11EA;
        D_800F3368.parameter00=32; D_800F3368.parameter02=2;
        D_800F3368.extent_x=32; D_800F3368.extent_y=32;
        D_800F3368.extent_x=32; D_800F3368.extent_y=32;
        color=D_800E2850[index];
        D_800F3368.palette=3; D_800F3368.parameter06=0;
        D_800F3368.parameter0A=0; D_800F3368.depth=0;
        D_800F3368.tpage=color;
    }
    }
    return 0;
}

int func_80198860(int event,RoomM350CloudParticle *particle)
{
    if(event==1) {
        int timer=D_800E27EC;
        int shade;
        int size;
        int product;
        int tripledProduct;
        if(timer>=8) return 1;
        shade=D_800966EC[((unsigned int)timer*3<<6)&0xFC0].bits.cos >> 5;
        particle->state.value.active.secondShade=128-((unsigned int)timer<<4);
        size=particle->size;
        particle->state.value.active.firstShade=shade;
        product=D_800966EC[((unsigned int)timer<<7)&0xF80].part.sin*size;
        {
            int twice=(unsigned int)product<<1;
            tripledProduct=(unsigned int)twice+(unsigned int)product;
        }
        particle->state.value.active.animatedSize=tripledProduct/4096;
    } else if(event==2) {
        GteShortVector world;
        /* Match note: original purpose of unused sp+0x30..0x37 is unknown. */
        char frameGap[8];
        world.x=(unsigned int)particle->anchor[0]+(unsigned short)particle->position[0];
        world.y=(unsigned int)particle->anchor[1]+(unsigned short)particle->position[1];
        world.z=(unsigned int)particle->anchor[2]+(unsigned short)particle->position[2];
        if(particle->state.value.active.firstShade>0) {
            int kind=D_800F3368.palette;
            int palette=D_800E1204[kind];
            unsigned short clut;
            if(kind==4 && D_800F3428) palette+=4;
            clut=GetClut(32,palette);
            func_800CEE20(&world,0,particle->size,particle->size,(s16)D_800F3368.parameter02*2+216,clut,1,particle->state.value.active.firstShade,&D_8019A3D0);
        }
        {
            int kind=D_800F3368.palette;
            int palette=D_800E1204[kind];
            unsigned short clut;
            if(kind==4 && D_800F3428) palette+=4;
            clut=GetClut(32,palette);
            func_800CEE20(&world,0,particle->state.value.active.animatedSize,particle->state.value.active.animatedSize,(s16)D_800F3368.parameter02*2+216,clut,1,particle->state.value.active.secondShade,&D_8019A3D0);
        }
    }
    return 0;
}
int func_80198ABC(int event)
{
    GteMatrix matrix;
    GteShortVector vector;
    switch (event) {
    case 0:

        return func_800CE560(ANIM_POOL_AS(RoomM350EffectEmitter)->pool,20,16,
                             (FieldFxCallbackListCallback)func_80198860);
        case 1:
    {
        int i;
        if(D_8019A8B8.complete) return 2;
        for(i=0;i<2;i++) {
            int firstRandom;
            int combinedRandom;
            int divisor;
            RoomM350CloudParticle *particle=func_800CE610(ANIM_POOL_AS(RoomM350EffectEmitter)->pool);
            if(!particle) return 0;
            vector.x=(unsigned int)Inv_ScrambleGrid()<<4;
            vector.y=(unsigned int)Inv_ScrambleGrid()<<4;
            vector.z=0;
            RotMatrixYXZ(&vector,&matrix);
            asm volatile("" : : "i"(&&offset_start));
offset_start:
            vector.x=0; vector.y=0;
            firstRandom=Inv_ScrambleGrid();
            combinedRandom=((unsigned int)firstRandom<<8)|(unsigned int)Inv_ScrambleGrid();
            divisor=128;
            vector.z=combinedRandom%divisor;
            ApplyMatrixSV(&matrix,&vector,(GteShortVector *)particle->position);
            particle->position[1]=(unsigned short)particle->position[1]*2+128;
            particle->size=((unsigned int)Inv_ScrambleGrid()<<4)+4096;
            particle->anchor=ANIM_OWNER_AS(RoomM350EffectActor)->instance->transforms[i ? 11:15].t;
        }
    }
        break;
    case 2:

    D_800F3368.parameter0A = 0;
    D_800F3368.parameter00 = 16;
    D_800F3368.parameter02 = 1;
    D_800F3368.extent_x = 16;
    D_800F3368.extent_y = 16;
    D_800F3368.extent_x = 16;
    D_800F3368.extent_y = 16;
    D_800F3368.tpage = D_800E2850[D_800E11E4[2]];
    D_800F3368.palette = 2;
    D_800F3368.parameter06 = 0;
    D_800F3368.depth = 16;
    }
    return 0;
}

int func_80198CF8(int event,State *state)
{
    Instance *instance=(Instance *)ANIM_OWNER_AS(RoomM350Actor)->instance;
    Vector offset;
    /* Match note: original purpose of unused sp+0x20..0x2F is unknown. */
    char frameGap[16];
    if(event==0) {
        RenderMatrix *matrix;
        state->speed=0; state->targetSpeed=0; state->heightPhase=0; state->mode=0;
        D_8019A8B8.complete=0; D_8019A8C0=0; D_8019A89E=0;
        asm volatile("" : : : "memory");
        matrix=instance->render_object.matrices;
        state->asset=Asset_Find08w(0x5CB,instance->state->progress,
            (short)matrix->translation[0],(short)matrix->translation[1],(short)matrix->translation[2]);
    } else if(event==1) {
        int frame,angle,delta,cosine,sine;
        int phase;
        int target;
        register int height asm("$2");
        register int speed asm("$5");
        if(instance->mode!=9) return 0;
        if(instance->state && instance->state->action_state[0]==1)
            instance->state->action_state[0]=2;
        {
            register int current asm("$3")=instance->anim.parts.integer;
            frame=current;
        if(D_8019A8B8.repeats>0) {
            height=(short)current;
            if(height>=30) {
                instance->anim.fixed=0x50000;
                D_8019A8B8.repeats--;
            }
        } else if((short)current>=instance->action-1) {
            if(instance->state) instance->state->action_state[0]=4;
            D_8019A8B8.complete=1;
            Akao_Cmd_21(state->asset,0);
            return 1;
        }
        }
        D_8019A8B8.playerDistance=func_800DFE20((Vec3Fixed *)&g_PlayerEntity->pos_x,(Vec3Fixed *)&instance->pos_x);
        angle=FieldFx_VecToAngle((Vec3Fixed *)&g_PlayerEntity->pos_x,(Vec3Fixed *)&instance->pos_x)&4095;
        D_8019A8B8.playerAngle=angle;
        delta=(angle-(unsigned short)instance->rot_y)&4095;
        do {
            if (state->mode != 0) {
                if (state->mode != 1) {
                    state->targetSpeed=0;
                    break;
                }
            } else {
                if(state->speed<=1280) instance->rot_y=FieldFx_TurnToward(instance->rot_y,angle,80);
                if(state->speed<=state->targetSpeed && ((instance->rot_y-D_8019A8B8.playerAngle+64)&4095)<=256) {
                    state->mode=1; state->targetSpeed=1536; D_8019A8C0=0;
                }
                break;
            }
            if(state->targetSpeed>=state->speed && ((delta-1536)&4095)<1024) {
                state->mode=0; state->targetSpeed=512;
            }
        } while (0);
        speed=state->speed; target=state->targetSpeed;
        {
            register int next asm("$3")=speed;
            int result;
            if(speed<target) {
                result=next+128;
                state->speed=result;
            } else {
                result=target<speed;
                if(result) {
                    result=next-32;
                    state->speed=result;
                }
            }
        }
        cosine=rcos(instance->rot_y+2048);
        sine=rsin(instance->rot_y+2048);
        {
        GteMatrix *matrix=&D_8019A870;
        Vector *vector=&offset;
        matrix->m[0][0]=cosine; D_8019A870.m[0][2]=sine; D_8019A870.m[2][0]=0u-(unsigned int)sine;
        D_8019A870.m[2][2]=cosine;
        D_8019A870.t[2]=0; D_8019A870.t[1]=0; D_8019A870.t[0]=0;
        D_8019A870.m[2][1]=0; D_8019A870.m[1][2]=0; D_8019A870.m[1][0]=0; D_8019A870.m[0][1]=0;
        D_8019A870.m[1][1]=4096;
        offset.x=0; offset.y=0; offset.z=state->speed;
        ApplyMatrixSV(matrix,vector,vector);
        }
        instance->pos_x=(unsigned int)instance->pos_x+((unsigned int)offset.x<<12);
        instance->pos_z=(unsigned int)instance->pos_z+((unsigned int)offset.z<<12);
        phase=state->heightPhase;
        {
        int nextPhase=phase;
        asm("" : "=r"(nextPhase) : "0"(nextPhase));
        if(phase<16) {
            int sample,floor;
            phase=nextPhase+1;
            state->heightPhase=phase;
            sample=D_800966EC[((unsigned int)phase<<6)&0xFC0].part.sin;
            height=4096-sample;
            floor=g_RoomFloorY->y;
            height=height/8-128;
            height=floor-height;
            instance->pos_y=(unsigned int)height<<16;
        } else if((short)frame>=31) {
            int floor;
            state->mode=2;
            floor=(unsigned int)g_RoomFloorY->y<<16;
            if(floor<instance->pos_y) instance->pos_y=(unsigned int)instance->pos_y-0x100000;
            else instance->pos_y=floor;
        }
        }
    }
    return 0;
}

int func_80199148(int event, RoomM350FadingParticle *object)
{
    /* Retail initializes two unused stack words; their original type is unknown. */
    struct { int x, y; } unused;

    unused.x = 0;
    unused.y = 0;
    if (event == 1) {
        short phase = object->counter >> 1;
        object->phase = phase;
        if (phase >= 8) {
            return 1;
        }
        if (phase >= 4) {
            object->brightness -= 8;
        }
        {
            unsigned int counter = (unsigned short)object->counter;
            register short velocity;
            velocity = object->velocity >> 1;
            object->counter = counter + 1;
            object->position.y -= velocity;
            object->velocity += object->acceleration;
        }
    } else if (event == 2) {
        int kind = D_800F3368.palette;
        int palette = D_800E1204[kind];
        int handle;
        handle = GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 6 : palette + 2);
        func_800CEE20(&object->position, 0, object->size, object->size, (s16)D_800F3368.parameter02 * object->phase,
            (unsigned short)handle, 1, object->brightness, 0);
    }
    return 0;
}
int func_801992A0(int event,FadingModelState *state)
{
    Vector offset;
    RenderMatrix rotation;
    /* Match note: original purpose of unused sp+0x40..0x47 is unknown. */
    char frameGap[8];
    switch (event) {
    case 0:

        {
            Instance *instance=(Instance *)ANIM_OWNER_AS(RoomM350Actor)->instance;
            RenderMatrix *matrix=instance->render_object.matrices;
            Asset_Find08w(0x549,instance->state->progress,(short)matrix->translation[0],(short)matrix->translation[1],(short)matrix->translation[2]);
            state->count=0; state->delay=0; D_8019A8C4=0;
            return func_800CE560(ANIM_POOL_AS(RoomM350Emitter)->pool,20,10,
                                 (FieldFxCallbackListCallback)func_80199148);
        }
        case 1:
    {
        RoomM350FadingParticle *particle;
        RoomM350Actor *actor;
        Instance *instance;
        int random;
        int scatterRandom;
        int index;
        int halfSize;
        int sizeBits;
        int cosine,divisor;
        int sine;
        int product;
        if(D_8019A8C4) return 2;
        state->delay=(unsigned short)state->delay+1;
        if(state->delay<4) return 0;
        particle=func_800CE610(ANIM_POOL_AS(RoomM350Emitter)->pool);
        if(!particle) return 0;
        rand();
        particle->brightness=64;
        {
            int count=state->count;
                        particle->size=((unsigned int)count<<7)+4096;
        }
        random=Inv_ScrambleGrid();
        actor=ANIM_OWNER_AS(RoomM350Actor);
        instance=(Instance *)actor->instance;
        {
            register int low asm("$5")=random&255;
            register int parts asm("$3")=instance->render_object.header->part_count-1;
            register int scaled asm("$2")=low*parts;
                        scaled=scaled/256;
                        {
                parts=scaled+1;
                asm volatile("" : : "r"(parts));
                index=parts;
            }
        {
        Instance *ySource;
        Instance *zSource;
        particle->position.x=instance->render_object.matrices[index].translation[0];
                ySource=(Instance *)actor->instance;
        asm volatile("" : : "r"(ySource));
        parts=particle->size;
        asm volatile("" ::: "memory");
        sizeBits=(unsigned short)particle->size;
        particle->position.y=ySource->render_object.matrices[index].translation[1];
        asm volatile("" : : : "$2");
        {
            u8 low7=random&127;
            product=low7*parts;
        }
        zSource=(Instance *)actor->instance;
        asm volatile("" : : "r"(zSource));
        sizeBits=(unsigned int)sizeBits<<16;
                {
        RenderMatrix *transforms=zSource->render_object.matrices;
        asm volatile("" : : "r"(transforms));
        halfSize=sizeBits>>17;
        asm volatile("" : : "r"(transforms));
        particle->position.z=transforms[index].translation[2];
        }
        }
        particle->velocity=0; particle->acceleration=0; particle->counter=0;
        {
            int y=(unsigned short)particle->position.y;
            asm volatile("" : "=l"(product) : "0"(product), "r"(y), "r"(halfSize));
            particle->position.y=y-(product>>13);
        }
        offset.x=0;
        scatterRandom=rand(); divisor=96;
        offset.z=((scatterRandom%divisor+64)*halfSize)>>12;
        cosine=rcos(rand());
        sine=rsin(rand());
        rotation.rotation[0][2]=sine;
        sine=0u-(unsigned int)sine;
        rotation.rotation[2][0]=sine;
        sine=4096;
        rotation.rotation[1][1]=sine;
                rotation.rotation[0][0]=cosine; rotation.rotation[2][2]=cosine;
        rotation.translation[2]=0; rotation.translation[1]=0; rotation.translation[0]=0;
        rotation.rotation[2][1]=0; rotation.rotation[1][2]=0;
        rotation.rotation[1][0]=0; rotation.rotation[0][1]=0;
        {
            RenderMatrix *matrix=&rotation;
            Vector *out;
            gte_ldrotmatrix(matrix); gte_ldtransmatrix(matrix);
            out=&offset;
            gte_ldv0(out); gte_rtv0tr_mac();
            gte_stsv(out);
        }
        particle->position.x=(unsigned short)particle->position.x+(unsigned short)offset.x;
        particle->position.z=(unsigned short)particle->position.z+(unsigned short)offset.z;
        state->delay=0; state->count=(unsigned short)state->count+1;
        Asset_Find08w(0x5FE,((Instance *)ANIM_OWNER_AS(RoomM350Actor)->instance)->state->progress,particle->position.x,particle->position.y,particle->position.z);
        }
    }
        break;
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
    }
    return 0;
}

int func_801996BC(int event, RoomM350SineParticle *object)
{
    GteShortVector position;
    GteRotation texture;
    if (event == 1) {
        int wave = rsin((unsigned int)D_800E27EC << 4);
        object->size = wave * object->amplitude >> 12;
        if (D_800E27EC >= 8) {
            object->brightness -= 8;
            if (object->brightness < 16) return 1;
        }
    } else if (event == 2) {
        int kind = D_800F3368.palette;
        int palette = D_800E1204[kind];
        unsigned short handle;
        handle = GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 7 : palette + 3);
        func_800CEE20(&object->position, &object->rotation, object->size, object->size,
            128, handle, 1, object->brightness, 0);
        position.x = object->position.x;
        position.y = ANIM_OWNER_AS(RoomM350EffectActor)->instance->transform.t[1];
        position.z = object->position.z;
        texture.x = 1024;
        texture.y = D_800E27EC * 192;
        texture.z = 0;
        texture.flags = 1;
        func_800D004C(&position, 384, 384, 10, &texture, object->size,
            object->size, &D_8019A738, &D_8019A734, object->brightness, 1);
    }
    return 0;
}

int func_8019988C(int event, EmissionState *state)
{
    switch (event) {
    case 0:

        state->count = 0;
        state->delay = 0;
        return func_800CE560(ANIM_POOL_AS(RoomM350EffectEmitter)->pool, 24, 8,
                             (FieldFxCallbackListCallback)func_801996BC);
        case 1:
    if (state->count < 24) {
        RoomM350SineParticle *effect;
        int random;
        int index;
        RoomM350EffectActor *actor;
        FieldActor *instance;
        state->delay--;
        if (state->delay != -1) return 0;
        effect = func_800CE610(ANIM_POOL_AS(RoomM350EffectEmitter)->pool);
        if (!effect) return 0;
        random = Inv_ScrambleGrid();
        actor = ANIM_OWNER_AS(RoomM350EffectActor);
        instance = (FieldActor *)actor->instance;
        index = (unsigned char)random;
        {
            register int byte asm("$4") = index;
            int count = instance->render_object.header->part_count;
            register int range asm("$3") = count - 1;
            asm("" : "=r"(range) : "0"(range), "r"(count));
            index = byte * range / 256 + 1;
        }
        effect->position.x = instance->render_object.matrices[index].translation[0];
        effect->position.y = ((FieldActor *)actor->instance)->render_object.matrices[index].translation[1];
        effect->position.z = ((FieldActor *)actor->instance)->render_object.matrices[index].translation[2];
        {
            int count = state->count;
            effect->brightness = 128;
            effect->amplitude = count * 2048 + 2048;
        }
        /* Keep the position/amplitude stores before the delay/count update. */
        asm volatile("" : : "m"(effect->amplitude));
        {
            int delay = random & 7;
            unsigned short count = state->count;
            delay += 12;
            state->delay = delay;
            state->count = count + 1;
        }
        {
            register int random asm("$4") = rand();
            int high;
            int y;
            int rawHigh;
            int bit;
            effect->rotation.x = (random & 255) + 512;
            asm volatile("" : : "m"(effect->rotation.x));
            rawHigh = random >> 8;
            high = rawHigh & 255;
            asm("" : : "r"(high));
            bit = random & 2;
            asm("" : : "r"(bit), "r"(random));
            if (bit) y = high + 768;
            else y = high - 1024;
            effect->rotation.y = y;
        }
        effect->rotation.z = 0;
        effect->rotation.flags = 0;
    } else {
        D_8019A8C4 = 1;
        return 2;
    }
        break;
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
    }
    return 0;
}

/* Walks the actor list for the first live actor of the given type and sub
 * id and leaves it in D_8019A774. */
FieldActor **func_80199B0C(s32 arg0, s32 arg1, s32 arg2) {
    FieldActor **slot;
    FieldActor **iter;
    FieldActor *node;

    slot = &D_8019A774;
    if ((*slot = g_FieldActorListHead) != 0) {
        iter = slot;
        while (1) {
            node = D_8019A774;
            if ((node->type_id == arg1) && (node->sub_id == arg2) && ((node->flags & 0x10) == 0)) {
                break;
            }

            if ((*iter = D_8019A774->next) == 0) {
                break;
            }
        }
    }

    return &D_8019A774;
}

s32 func_80199B94(s32 mode, GteShortVector *ctx) {
    s32 tex;

    if (mode == 1) {
        if (D_800E27EC < 8) {
            goto ret0;
        }
        return 1;
    }

    if (mode != 2) {
        return 0;
    }

    tex = D_800E1204[D_800F3368.palette];
    if (D_800F3368.palette == 4 && D_800F3428 != 0) {
        tex += 4;
    }

    func_800CEE20(ctx, 0, 0x1800, 0x1800, (s16)D_800F3368.parameter02 + 0xDC, GetClut(0x30, tex), 1,
                  (D_800966EC[((D_800E27EC - 1) << 8) & 0xF00].word << 16) >> 21, &D_8019A410);
ret0:
    return 0;

}

/* Walks the actor list for the first live actor of the given type and sub
 * id, leaves it in D_8019A7F8 and returns the joint position slot. */
GteShortVector *func_80199CA4(s32 arg0, s32 arg1, s32 arg2) {
    FieldActor **slot;
    FieldActor **iter;
    FieldActor *node;

    slot = &D_8019A7F8;
    if ((*slot = g_FieldActorListHead) != 0) {
        iter = slot;
        while (1) {
            node = D_8019A7F8;
            if ((node->type_id == arg1) && (node->sub_id == arg2) && ((node->flags & 0x10) == 0)) {
                break;
            }

            if ((*iter = D_8019A7F8->next) == 0) {
                break;
            }
        }
    }

    return &D_8019A7A0;
}

char *func_80199D2C(void *arg0, char arg1, short arg2, short arg3) {
    char *ptr = &D_8019A831;

    *ptr = arg1;
    D_8019A82A[0] = arg2;
    D_8019A82C = arg3;
    return ptr - 0x29;
}

s32 func_80199D50(s32 arg0) {
    char *obj;
    char *actor;

    if (arg0 != 1) {
        if (arg0 >= 2) {
            return 0;
        }
        if (arg0 != 0) {
            return 0;
        }
        obj = ANIM_POOL_AS(char *)[2];
        return func_800CE560(obj, 8, 2,
                             (FieldFxCallbackListCallback)func_80196C64);
    } else if ((char)D_8019A82E != 0) {
        return 2;
    } else if ((char)D_8019A830 != 0) {
        obj = func_800CE610(ANIM_POOL_AS(char *)[2]);
        if (obj != 0) {
            actor = (char *)D_8009D254;
            *(short *)(obj + 0) = *(s32 *)(actor + 0x1FC);
            *(short *)(obj + 2) = *(s32 *)(actor + 0x200) - 0x140;
            {
                s32 value = *(s32 *)(actor + 0x204);
                *(short *)(obj + 4) = value;
            }
            D_8019A830 = 0;
        }
    }

    return 0;
}

short *func_80199E3C(void *arg0, short arg1) {
    short *ptr = &D_8019A85E;

    *ptr = arg1;
    return ptr - 0x15;
}

s32 func_80199E50(s32 arg0) {
    char *obj;

    if (arg0 != 1) {
        if (arg0 >= 2) {
            return 0;
        }
        if (arg0 != 0) {
            return 0;
        }
        return func_800CE560(D_800F33E0->end, 4, 2,
                             (FieldFxCallbackListCallback)func_8019A134);
    } else if ((char)D_8019A86E != 0) {
        return 2;
    } else if (D_8019A854 != 0) {
        obj = func_800CE610(D_800F33E0->end);
        if (obj != 0) {
            /* The translation of the owner's eighth matrix. */
            *(s32 **)obj = D_800F32D0->actor->render_object.matrices[7].translation;
            D_8019A854 = 0;
        }
    }

    return 0;
}

s32 func_80199F2C(s32 arg0) {
    char *obj;

    if (arg0 != 1) {
        if (arg0 >= 2) {
            return 0;
        }
        if (arg0 != 0) {
            return 0;
        }
        return func_800CE560(ANIM_POOL_AS(char *)[2], 8, 6,
                             (FieldFxCallbackListCallback)func_8019A21C);
    } else if ((char)D_8019A86E != 0) {
        return 2;
    } else if (D_8019A856 != 0) {
        obj = func_800CE610(ANIM_POOL_AS(char *)[2]);
        if (obj != 0) {
            *(u16 *)(obj + 0) = D_8019A860;
            *(u16 *)(obj + 2) = g_RoomFloorY->raw;
            *(u16 *)(obj + 4) = D_8019A862;
            D_8019A856 = 0;
        }
    }

    return 0;
}

s32 func_8019A014(s32 mode, void *ctx) {
    s32 tex;

    if (mode == 1) {
        if (D_800E27EC < 5) {
            goto ret0;
        }
        return 1;
    }

    if (mode != 2) {
        return 0;
    }

    if (D_800E27EC < 2) {
        return 0;
    }

    tex = D_800E1204[D_800F3368.palette];
    if (D_800F3368.palette == 4 && D_800F3428 != 0) {
        tex += 4;
    }

    func_800CEE20(ctx, 0, 0x1000, 0x1000, 0xDC, GetClut(0x30, tex), 1, (6 - D_800E27EC) << 5, &D_8019A630);
ret0:
    return 0;
}

short *func_8019A120(void *arg0, short arg1) {
    short *ptr = &D_8019A8B8.repeats;

    *ptr = arg1;
    return ptr - 0x24;
}

int func_8019A134(int event, int **position) {
    short point[4];
    int shade;
    int *colors;
    if (event == 1) {
        if (D_800E27EC >= 4) return 1;
    } else if (event == 2) {
        point[0] = (*position)[0];
        point[1] = (*position)[1];
        point[2] = (*position)[2];
        shade = D_800966EE[((D_800E27EC - 1) << 9) & 0x1E00];
        colors = D_8019A614;
        func_800D004C((struct GteShortVector *)point, 512, 512, 16, 0, (short)shade, (short)shade,
                     (struct RenderColor *)colors, (struct RenderColor *)(colors + 1), (short)shade >> 5, 1);
    }
    return 0;
}

int func_8019A21C(int event, void *context) {
    short rotation[4];
    short *entry;
    int scale;
    if (event == 1) {
        if (D_800E27EC >= 16) return 1;
    } else if (event == 2) {
        int angle = (D_800E27EC << 12) / 20;
        unsigned int offset = (unsigned int)(D_800E27EC - 1) << 8;
        short *base = ((short *)D_800966EC);
        entry = (short *)((offset & 0x3F00) + (char *)base);
        scale = entry[0];
        rotation[0] = 1024;
        rotation[1] = 0;
        rotation[2] = angle;
        rotation[3] = 1;
        func_800D004C(context, 384, 384, 10, (struct GteRotation *)rotation, scale, scale,
                     &D_8019A3C8, (struct RenderColor *)D_8019A628,
                     ((EffectEntry *)entry)->shade >> 5, 1);
    }
    return 0;
}
