#include "pe1/field_sprite_params.h"
#include "pe1/field_actor.h"
#include "room_m273.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/psyq_gpu.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_model_draw.h"
#include "pe1/field_anim_pool.h"
#include "pe1/scene_archive.h"
#include "pe1/psyq_libc.h"
#include "pe1/gte_types.h"

extern FieldActor *g_PlayerEntity;
extern u8 D_8019AE9A;
extern RoomM273PulseSeed D_8019AC20[2];
extern int func_800CE688(void *);
extern int Inv_ScrambleGrid(void);

/* Threshold model triple callbacks and controller. */
typedef struct { short x,y,z,phase,scaleXZ,scaleY; unsigned short parameter12,unknown14; } ThresholdTripleEffect;
typedef GteMatrix ThresholdTripleMatrix;
typedef GteVector ThresholdTripleVector;
typedef struct { void *asset,*pool; } AssetState;

extern short D_8019AC18[];
extern int D_8019AC0C[];
extern AssetState D_8019AE8C;
extern void *D_8019AE90;
extern unsigned char D_8019AE9A;
extern void GsSetOrign(int,int);
void *Asset_FindTable08ByU32Key(void *base, s32 key);

extern int func_80193CB8(),func_80193B5C();

int func_80193B5C(int mode, void **arg1) {
    short params[4];
    short i;
    void **objp = arg1;
    short *x;
    short *x_next;
    int *z;
    int *z_next;

    if (mode == 1) {
        if (D_800E27EC < 0x20) {
            goto ret0;
        }
        return 1;
    }
    if (mode != 2) {
        return 0;
    }

    i = 0;
    x = D_8019AC18;
    x_next = x + 1;
    z = D_8019AC0C;
    z_next = z + 1;

    params[0] = 0x400;
    params[1] = D_800E27EC << 7;
    params[2] = 0;
    params[3] = 1;

    do {
        func_800D0728(*objp, x[i], x_next[i], 0x10, (GteRotation *)params,
                      *(short *)((char *)*objp + 8), *(short *)((char *)*objp + 8),
                      (RenderColor *)((((int)i * 4) + (int)z)), (RenderColor *)((((int)i * 4) + (int)z_next)),
                      *(short *)((char *)*objp + 0xC), 1);
        i++;
    } while ((short)i < 2);

ret0:
    return 0;
ret1:
    return 1;
}

int func_80193CB8(int mode,ThresholdTripleEffect *effect) {
    ThresholdTripleMatrix matrix;
    ThresholdTripleVector copy,scale;
    if(mode==1) {
        int frame=D_800E27EC;
        int *wave;
        register int *shifted asm("$4");
        register int *table asm("$6");
        unsigned int index;
        if(frame>=32) return 1;
        frame*=32;
        index=frame&0xFFF;
        asm("" : "=r"(index) : "0"(index) : "$6");
        table=(int *)D_800966EC;
        wave=&table[index];
        shifted=&table[(frame+1024)&0xFFF];
        {
            register int sample asm("$2")=*wave;
            effect->scaleXZ=(short)sample/2;
        }
        effect->scaleY=(((short *)shifted)[1]+4096)*2;
        asm("" : : : "memory");
        {
            unsigned int phase=(unsigned short)effect->phase;
            int parameter=((short *)wave)[1];
            asm("" : "=r"(phase), "=r"(parameter) : "0"(phase), "1"(parameter));
            effect->phase=phase+256;
            effect->parameter12=parameter>>6;
        }
    } else if(mode==2) {
        int flags=((int (*)(int,int,int,int))GetTPage)(0,1,0,0);
        int page=(unsigned short)(D_800E2850[D_800E11FA]|flags);
        int kind=D_800F3368.palette,palette;
        int cosine,sine;
        register void **asset asm("$17");
        palette=D_800E1204[kind];
        GsSetOrign(page,GetClut(0, (kind == 4 && D_800F3428) ? palette + 11 : palette + 7));
        func_800C6ED8(1);
        asset=&D_8019AE8C.asset;
        func_800C6EF8(*asset);
        func_800C6FA0(*asset,effect->parameter12);
        cosine=rcos(effect->phase);
        sine=rsin(effect->phase);
        matrix.m[0][2]=sine; matrix.m[2][0]=-sine;
        matrix.m[0][0]=cosine; matrix.m[2][2]=cosine;
        matrix.t[0]=matrix.t[1]=matrix.t[2]=0;
        matrix.m[0][1]=matrix.m[1][0]=matrix.m[1][2]=matrix.m[2][1]=0;
        matrix.m[1][1]=4096;
        memset(&scale,0,16);
        scale.x=effect->scaleXZ; scale.y=effect->scaleY; scale.z=effect->scaleXZ;
        copy=scale;
        ScaleMatrix(&matrix,&copy);
        {
            int x=effect->x;
            void *model=*asset;
            matrix.t[0]=x; matrix.t[1]=effect->y; matrix.t[2]=effect->z;
            func_800C71E4(model,&matrix);
        }
        func_800C6F4C(*asset);
    }
    return 0;
}

int func_80193F30(int mode) {
    switch(mode) {
    case 0: {
        int size;
        D_8019AE8C.asset=Asset_FindTable08ByU32Key(D_800B0E64[0],0xC5941704);
        func_800C6D5C(D_8019AE8C.asset,0,0);
        size=func_800CE560(ANIM_POOL_AS(RoomM273PoolContext)->pool,16,6,
                           (FieldAnimCallbackListCallback)func_80193CB8);
        asm("" : : "r"(size));
        return size+func_800CE5AC(&D_8019AE8C.pool,size,4,2,func_80193B5C);
    }
    case 1: {
        RoomM273ThresholdEntityState *state;
        if(D_8019AE9A) return 2;
        state=ANIM_OWNER_AS(RoomM273EffectStateContext)->state.threshold;
        if(state->mode.kind==9) {
            unsigned short value=state->mode.value26;
            if(state->mode.value22>0 && (short)value<=0) {
                short i=0;
                do {
                    ThresholdTripleEffect *effect=func_800CE610(ANIM_POOL_AS(RoomM273PoolContext)->pool);
                    if(!effect) break;
                    effect->x=state->transform->x;
                    asm("" : : : "memory");
                    effect->y=g_RoomFloorY->raw;
                    effect->z=state->transform->z;
                    effect->phase=i*384;
                    if(i==0) *(ThresholdTripleEffect **)func_800CE610(D_8019AE90)=effect;
                    i++;
                } while(i<3);
            }
        }
        func_800CE688(D_8019AE90);
        break;
    }
    case 2:
        func_800CE78C(D_8019AE90);
        break;
    }
    return 0;
}

int func_80194128(int mode, GteRotation *rotation) {
    GteShortVector position;
    int frame, size, kind, palette;
    u16 clut;

    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
    } else if (mode == 2) {
        RoomM273PlayerActorView *player;
        /* The loop index is reused for the sampled scale below. */
        size = 0;
        player = (RoomM273PlayerActorView *)g_PlayerEntity;
        for (; size < 3; size++) {
            RoomM273PlayerTransform *transform = player->tail.transform;
            ((s16 *)&position)[size] = ((s32 *)&transform->position.x)[size];
        }

        frame = D_800E27EC - 1;
        kind = D_800F3368.palette;
        size = *(s16 *)((char *)D_800966EC + ((frame << 8) & 0x3F00));
        palette = D_800E1204[kind];

        clut = GetClut(0, (kind == 4 && D_800F3428) ? palette + 11 : palette + 7);
        func_800CEE20(&position, rotation, 8192, size * 3, 5, clut, 1,
                       /* The upper half of the packed trig entry is signed. */
                       (s16)(*(s32 *)((char *)D_800966EC +
                           ((frame << 8) & 0x3F00)) >> 16) >> 5, 0);
    }
    return 0;
}

int func_80194284(int mode) {
    switch (mode) {
    case 0:
        return func_800CE560(ANIM_POOL_AS(RoomM273PoolContext)->pool, 8, 4,
                             (FieldAnimCallbackListCallback)func_80194128);
    case 1: {
        RoomM273EffectModeState *state;
        if (D_8019AE9A) return 2;
        state = ANIM_OWNER_AS(RoomM273EffectStateContext)->state.mode;
        if (state->kind == 9) {
            unsigned short value = state->value26;
            if (state->value22 > 0 && (short)value <= 0) {
                short i = 0;
                int angle = Inv_ScrambleGrid() << 4;
                do {
                    RoomM273PulseSeed *seed = func_800CE610(ANIM_POOL_AS(RoomM273PoolContext)->pool);
                    if (!seed) break;
                    *seed = D_8019AC20[i];
                    i++;
                    seed->angle += angle;
                } while (i < 2);
            }
        }
        break;
    }
    case 2: {
        int index;
        int palette;
        D_800F3368.parameter00 = 16;
        D_800F3368.extent_x = 16; D_800F3368.extent_y = 16; D_800F3368.extent_x = 16;
        index = D_800E11FA;
        D_800F3368.parameter02 = 1; D_800F3368.extent_y = 64;
        palette = D_800E2850[index];
        D_800F3368.palette = 3; D_800F3368.parameter06 = 1; D_800F3368.parameter0A = 3; D_800F3368.depth = 0;
        D_800F3368.tpage = palette;
        break;
    }
    }
    return 0;
}

extern short D_8019AE60, D_8019AE64;

typedef GteShortVector Vector;

int func_80194470(int mode, Vector *input) {
    Vector *position = input;
    if (mode == 1) {
        if (D_800E27EC >= 9) return 1;
    } else if (mode == 2) {
        int firstFrame = D_800E27EC - 1;
        int frame;
        unsigned int offset;
        int kind = D_800F3368.palette;
        s16 size;
        int palette;
        unsigned short clut;

        offset = ((unsigned int)firstFrame << 9) & 0x3E00;
        frame = firstFrame;
        size = *(short *)((char *)D_800966EC + offset) * 2 + 4096;

        palette = D_800E1204[kind];
        clut = GetClut(0, (kind == 4 && D_800F3428) ? palette + 9 : palette + 5);
        func_800CEE20(position, 0, (short)size, (short)size, 102, clut, 1,
            (short)((int *)D_800966EC)[(((unsigned int)frame << 10) & 0x3C00) / 4] >> 6, 0);
    }
    return 0;
}

int func_801945A8(int mode) {
    switch(mode) {
    case 0:
        D_8019AE60=0; D_8019AE64=0;
        return func_800CE560(ANIM_POOL_AS(RoomM273PoolContext)->pool,8,4,
                             (FieldAnimCallbackListCallback)func_80194470);
    case 1: {
        RoomM273EffectModeState *state;
        if(D_8019AE9A) return 2;
        state=ANIM_OWNER_AS(RoomM273EffectStateContext)->state.mode;
        if(state->kind==9) {
            unsigned short value=state->value26;
            if(state->value22>=2 && (short)value<2) {
                D_8019AE60=4; D_8019AE64=0;
            }
        }
        if(D_8019AE60) {
            short old=D_8019AE64--;
            if(old<=0) {
                Vector *position=func_800CE610(ANIM_POOL_AS(RoomM273PoolContext)->pool);
                if(position) {
                    RoomM273PlayerTransform *transform=(RoomM273PlayerTransform *)g_PlayerEntity->render_object.matrices;
                    position->x=transform->position.x;
                    position->y=transform->position.y;
                    position->z=transform->position.z;
                    D_8019AE64=2;
                    D_8019AE60--;
                }
            }
        }
        break;
    }
    case 2: {
        int index=D_800E11FA;
        int palette;
        D_800F3368.parameter00=32; D_800F3368.parameter02=2;
        D_800F3368.extent_x=32; D_800F3368.extent_y=32; D_800F3368.extent_x=32; D_800F3368.extent_y=32;
        palette=D_800E2850[index];
        D_800F3368.palette=3; D_800F3368.parameter06=1; D_800F3368.parameter0A=0; D_800F3368.depth=32;
        D_800F3368.tpage=palette;
        break;
    }
    }
    return 0;
}

int func_801947CC(int mode, RoomM273PaletteInput *input)
{
    RoomM273TrigEntry *entry;
    RoomM273PaletteEffect *effect;
    GteShortVector position;
    GteVector *source;
    int x;
    int y;
    int z;
    GteRotation rotation;
    int random;

    if (mode == 1) {
        int frame;
        frame = D_800E27EC;
        if (frame >= 33) return 1;
        entry = &D_800966EC[(((unsigned int)frame << 7) & 0x3F80) / sizeof(RoomM273TrigEntry)];
        input->size = (entry->low * 3 * 2048) / 4096 + 4096;
        if (frame >= 24) return 0;
        effect = func_800CE610(D_8019AE94);
        if (!effect) return 0;
        effect->source = input->source;
        effect->size = (input->size * 3 * 256) / 4096;
        effect->depth = Inv_ScrambleGrid() + 256;
        effect->color = D_8019AC30[D_800E27EC & 3];
        effect->x = Inv_ScrambleGrid() << 4;
        random = Inv_ScrambleGrid();
        effect->y = (128 - random) >> 2;
    } else if (mode == 2) {
        int frame;
        source = input->source;
        frame = D_800E27EC;
        y = source->y;
        x = source->x;
        frame = frame - 1;
        position.y = y;
        z = source->z;
        entry = &D_800966EC[(((unsigned int)frame << 7) & 0x3F80) / sizeof(RoomM273TrigEntry)];
        position.z = z;
        position.x = x;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = 170 * frame;
        rotation.flags = 0;
        func_800D004C(&position, 256, 256, 12, &rotation,
                       input->size, input->size,
                       (RenderColor *)D_8019AB70,
                       (RenderColor *)&D_8019ABFC[D_8019AE98],
                       entry->high >> 5, 1);
    }
    return 0;
}

extern int func_80199F84();
extern int func_800CE688(void *);

int func_801949EC(int mode) {
    switch (mode) {
    case 0: {
        int size = func_800CE560(ANIM_POOL_AS(RoomM273PoolContext)->pool, 8, 2,
                                 (FieldAnimCallbackListCallback)func_801947CC);
        asm("" : : "r"(size));
        return size + func_800CE5AC(&D_8019AE94, size, 16, 9, func_80199F84);
    }
    case 1: {
        RoomM273EffectModeState *state;
        if (D_8019AE9A) return 2;
        state = ANIM_OWNER_AS(RoomM273EffectStateContext)->state.mode;
        if (state->kind == 9) {
            unsigned short value = state->value26;
            if (state->value22 >= 4 && (short)value < 4) {
                RoomM273PointerPoolEffect *effect = (RoomM273PointerPoolEffect *)func_800CE610(ANIM_POOL_AS(RoomM273PoolContext)->pool);
                if (effect) effect->position = (u8 *)g_PlayerEntity->render_object.matrices->translation;
            }
        }
        func_800CE688(D_8019AE94);
        break;
    }
    case 2:
        func_800CE78C(D_8019AE94);
        break;
    }
    return 0;
}
