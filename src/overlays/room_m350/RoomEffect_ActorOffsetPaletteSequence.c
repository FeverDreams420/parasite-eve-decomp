#include "pe1/field_sprite_params.h"
#include "pe1/room_m350_effects.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_anim_pool.h"
#include "room_m350_shared.h"
#include "pe1/psyq_gpu.h"
#include "pe1/asset_find.h"

extern int D_800966EC[], D_8019A56C[];

int func_80195B64(int event, GteShortVector *object)
{
    short position[4];
    if (event == 1) {
        if (D_800E27EC >= 8) {
            return 1;
        }
    } else if (event == 2) {
        int entry = D_800966EC[((unsigned int)D_800E27EC << 7) & 0xF80];
        int scale = entry >> 16;
        int shade = (short)entry >> 5;
        int *origin = ANIM_OWNER_AS(RoomM350EffectActor)->instance->transforms[33].t;
        int i;
        int kind, palette, handle;

        for (i = 0; i < 3; i++) {
            position[i] = ((short *)object)[i] * scale / 4096 + origin[i];
        }
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) {
            palette += 4;
        }
        handle = GetClut(32, palette);
        func_800CEE20((GteShortVector *)position, 0, object->pad, object->pad, (s16)D_800F3368.parameter02 + 216,
            (unsigned short)handle, 1, shade, (struct RenderColor *)D_8019A56C);
    }
    return 0;
}

/* MASPSX_FLAGS: --expand-div */
/* Event state: rotation, emission count, brightness, fade phase and size. */
typedef struct { GteShortVector rotation; short count,brightness,fade,size; } State;

extern unsigned short D_800E11E8;
extern char D_8019A570[];
extern int Inv_ScrambleGrid(void);

int func_80195CE0(int event,State *inputState)
{
    State *state=inputState;
    union { GteMatrix matrix; GteShortVector position; } local;
    GteShortVector vector;
    switch (event) {
    case 0:

        {
            RoomM350EffectInstance *instance=ANIM_OWNER_AS(RoomM350EffectActor)->instance;
            int *position=instance->transforms[33].t;
            RoomM350EffectEmitter *emitter;
            Asset_Find08w(0x5C9,(int)instance->owner->asset,(short)position[0],(short)position[1],(short)position[2]);
            *(int *)&state->rotation=0;
            emitter=ANIM_POOL_AS(RoomM350EffectEmitter);
            *(int *)((char *)&state->rotation+4)=0;
            state->count=0; state->fade=0;
            return func_800CE560(emitter->pool,8,18,
                                 (FieldAnimCallbackListCallback)func_80195B64);
        }
        case 1:
    {
        int phase,sample;
        register int i asm("$19");
        if(D_8019A82E) {
            unsigned short old=*(volatile unsigned short *)&state->fade;
            phase=(short)old*64+1024;
            state->fade=old+1;
            if(phase>=2048) return 2;
        } else {
            int timer=D_800E27EC;
            phase=timer<32 ? (unsigned int)timer<<5:1024;
        }
        sample=(short)D_800966EC[phase&4095];
        state->brightness=sample>>5;
        state->size=(unsigned int)sample<<2;
        if(state->count>=16) return 0;
        i=0;
        do {
            register GteShortVector *particle asm("$17")=func_800CE610(ANIM_POOL_AS(RoomM350EffectEmitter)->pool);
            int random;
            int highByte;
            int divisor;
            if(!particle) break;
            i++;
            vector.x=(unsigned int)Inv_ScrambleGrid()<<4;
            vector.y=(unsigned int)Inv_ScrambleGrid()<<4; vector.z=0;
            RotMatrixYXZ(&vector,&local.matrix);
            asm volatile("" : : "i"(&&radial));
radial:
            vector.x=0; vector.y=0;
            highByte=Inv_ScrambleGrid();
            random=((unsigned int)highByte<<8)|Inv_ScrambleGrid();
            divisor=44;
            vector.z=random%divisor+320;
            ApplyMatrixSV(&local.matrix,&vector,particle);
            particle->pad=(unsigned int)state->count*128+2048;
        } while(i<2);
        state->count=(unsigned short)state->count+1;
    }
        break;
    case 2:

    {
        int index=D_800E11E8;
        int i;
        int palette;
        RoomM350EffectActor *actor;
        register int specialKind asm("$17");
        register unsigned short *palettes asm("$19");
        volatile int *position;
        short *out;
        D_800F3368.parameter00=16; D_800F3368.parameter02=1;
        D_800F3368.extent_x=16; D_800F3368.extent_y=16; D_800F3368.extent_x=16; D_800F3368.extent_y=16;
        palette=D_800E2850[index];
        asm volatile("" : "=r"(palette) : "0"(palette) : "memory");
        actor=ANIM_OWNER_AS(RoomM350EffectActor);
        D_800F3368.palette=2; D_800F3368.parameter06=0; D_800F3368.parameter0A=0; D_800F3368.depth=0; D_800F3368.tpage=palette;

        i=0;
        position=actor->instance->transforms[33].t;
        out=&local.position.x;
        for(;i<3;i++) *out++=*position++;
        i=0;
        palettes=D_800E1204;
        state->rotation.z=(unsigned int)D_800E27EC<<8;
        specialKind=4;
        do {
            int kind=D_800F3368.palette;
            int clut,pal=palettes[kind];
            if(kind==specialKind && D_800F3428) pal+=4;
            clut=GetClut(32,pal);
            func_800CEE20(&local.position,(GteRotation *)state,state->size,state->size,(s16)D_800F3368.parameter02+216,(unsigned short)clut,1,state->brightness,(struct RenderColor *)D_8019A570);
            state->rotation.z=0u-(unsigned short)state->rotation.z;
        } while(++i<2);
    }
    }
    return 0;
}
