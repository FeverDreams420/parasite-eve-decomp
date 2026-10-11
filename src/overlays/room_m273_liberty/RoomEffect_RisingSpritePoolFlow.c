/* Sprite callback and poll/reset controller share one pool and layout. */
#include "pe1/field_sprite_params.h"
#include "room_m273_effects.h"
#include "pe1/field_anim_callback_list.h"
#include "room_m273_sweep.h"
#include "pe1/room_floor.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"
#include "pe1/psyq_gpu.h"
typedef GteShortVector Vector;
#include "pe1/gte_sine_table.h"

int func_80197230(int mode, Vector *position) {
    if (mode == 1) {
        if (D_800E27EC >= 8) return 1;
    } else if (mode == 2) {
        int frame = D_800E27EC - 1;
        int kind, palette;
        int size;
        int sample;
        unsigned short clut;
        sample = D_800966EC[(((unsigned int)frame << 9) & 0x3E00) / 4].word;
        kind = D_800F3368.palette;
        size = sample + 2048;
        palette = D_800E1204[kind];
        clut = GetClut(0, (kind == 4 && D_800F3428) ? palette + 9 : palette + 5);
        func_800CEE20(position, 0, (short)size, (short)size, 102, clut, 1,
            (short)D_800966EC[(((unsigned int)frame << 10) & 0x3C00) / 4].word >> 5, 0);
    }
    return 0;
}

#define ROOMLIB_POLL_RESET_POOL_CONTEXT_DECL
#define ROOMLIB_POLL_RESET_POOL_EXPR ANIM_POOL_AS(RoomM273PoolContext)->pool
#define ROOMLIB_POLL_RESET_FUNC func_80197360
#define ROOMLIB_POLL_RESET_CALLBACK func_80197230
#define ROOMLIB_POLL_RESET_FLAG D_8019AF69
#define ROOMLIB_POLL_RESET_COUNTER D_8019AF0A
#define ROOMLIB_POLL_RESET_SEED D_8019AF04
#include "../room_lib/RoomLib_PollAndResetActor.inc"

#include "room_m273_effects.h"
typedef RoomM273RisingParticle Particle;

/* The timed emitter fills the eight-byte records rendered by this callback. */
#include "pe1/gte_sine_table.h"
extern short D_8019AE84;
extern unsigned char D_8019AD70[],D_8019AF6A,D_8019AF69;
extern unsigned short D_800E11E8;
extern volatile unsigned short D_8019AEFE,D_8019AF00;

int func_801974DC(int mode, Particle *input) {
    Particle *particle = input;
    /* Layout only: original purpose of these eight frame bytes is unknown. */
    int stack_pad[2];
    if (mode == 1) {
        if (D_800E27EC >= 32) return 1;
        else {
            unsigned short y = ((volatile Particle *)particle)->y;
            s16 speed = ((volatile Particle *)particle)->speed;
            int delta = ((Particle *)particle)->speed;
            speed += 1;
            particle->y = y - delta;
            particle->speed = speed;
        }
    } else if (mode == 2) {
        int frame = D_800E27EC - 1;
        int kind = D_800F3368.palette;
        int sample, palette, size, shade;
        int paletteOffset, sizeOffset;
        unsigned int sampleOffset;
        int clut;
        sampleOffset = ((unsigned int)frame << 8) & 0x3F00;
        asm("" : : "r"(sampleOffset), "r"(frame));
        paletteOffset = kind * 2;
        sample = D_800966EC[sampleOffset / 4].word;
        asm("" : : "r"(sample), "r"(paletteOffset));
        sizeOffset = ((unsigned int)frame << 7) & 0x3F80;
        palette = *(unsigned short *)((char *)D_800E1204 + paletteOffset);
        shade = (short)sample >> 5;
        size = *(short *)((char *)D_800966EC + sizeOffset) + 8192;
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(16, palette);
        func_800CEE20((GteShortVector *)particle, 0, size, size, (s16)D_800F3368.parameter02 * (frame / 4) + 200,
            clut, 1, shade, (struct RenderColor *)D_8019AD70);
    }
    return 0;
}

int func_80197648(int mode) {
    switch(mode) {
    case 0:
        D_8019AE84=0;
        return func_800CE560(ANIM_POOL_AS(RoomM273PoolContext)->pool,8,12,
                             (FieldAnimCallbackListCallback)func_801974DC);
    case 1: {
        Particle *output;
        if(D_8019AF69) return 2;
        if(!D_8019AF6A) return 0;
        if(D_8019AE84-- > 0) return 0;
        output=func_800CE610(ANIM_POOL_AS(RoomM273PoolContext)->pool);
        if(!output) return 0;
        ((volatile Particle *)output)->x=D_8019AEFC.x;
        ((volatile Particle *)output)->y=D_8019AEFE-256;
        output->z=D_8019AF00;
        D_8019AE84=2;
        ((volatile Particle *)output)->speed=0;
        break;
    }
    case 2: {
        unsigned int index=D_800E11E8;
        unsigned short palette;
        D_800F3368.parameter00=16;
        D_800F3368.parameter02=1;
        D_800F3368.extent_x=16;
        D_800F3368.extent_y=16;
        D_800F3368.extent_x=16;
        D_800F3368.extent_y=16;
        palette=D_800E2850[index];
        D_800F3368.palette=2;
        D_800F3368.parameter06=0;
        D_800F3368.parameter0A=0;
        D_800F3368.depth=0;
        D_800F3368.tpage=palette;
        break;
    }
    }
    return 0;
}

/* The position emitter fills the eight-byte records rendered by this callback. */
#include "pe1/gte_sine_table.h"
extern unsigned char D_8019AB70[],D_8019AD74[],D_8019AD78[],D_8019AF68,D_8019AF69;
extern unsigned short D_800E11EA;
extern volatile unsigned short D_8019AEFE,D_8019AF00;

int func_801977F8(int mode,Vector *input) {
    Vector rotation,position;
    if(mode==1) {
        if(D_800E27EC>=16) return 1;
    } else if(mode==2) {
        int counter=D_800E27EC;
        int frame=counter-1;
        int kind=D_800F3368.palette;
        int firstSample=*(int *)((char *)D_800966EC+(((unsigned int)(u8)frame<<8)&0x3F00));
        int shade,size,palette;
        int secondSample;
        unsigned short clut;
        rotation.x=0; rotation.y=0; rotation.z=(unsigned int)frame<<8; rotation.pad=0;
        position=*input;
        palette=D_800E1204[kind];
        shade=firstSample>>21;
        size=(short)firstSample*3+2048;
        if(kind==4 && D_800F3428) palette+=4;
        clut=GetClut(80,palette);
        func_800CEE20(&position,(GteRotation *)&rotation,size,size,74,clut,1,shade,0);
        rotation.z>>=1;
        secondSample=*(int *)((char *)D_800966EC+(((unsigned int)(u8)frame<<8)&0x3F00));
        shade=secondSample>>21;
        size=(short)secondSample+1024;
        func_800D004C(&position,512,512,16,(GteRotation *)&rotation,size,size,(struct RenderColor *)D_8019AB70,(struct RenderColor *)D_8019AD74,shade,1);
        rotation.x=1024; rotation.y=0; rotation.z=(unsigned int)frame<<8; rotation.pad=1;
        position.y=g_RoomFloorY->raw;
        func_800D004C(&position,768,768,8,(GteRotation *)&rotation,4096,4096,(struct RenderColor *)D_8019AD78,(struct RenderColor *)D_8019AB70,shade,1);
    }
    return 0;
}

int func_80197A48(int mode) {
    switch(mode) {
    case 0:
        return func_800CE560(ANIM_POOL_AS(RoomM273PoolContext)->pool,8,4,
                             (FieldAnimCallbackListCallback)func_801977F8);
    case 1: {
        Vector *output;
        if(D_8019AF69) return 2;
        if(!D_8019AF68) return 0;
        output=func_800CE610(ANIM_POOL_AS(RoomM273PoolContext)->pool);
        if(!output) return 0;
        ((volatile Vector *)output)->x=D_8019AEFC.x;
        ((volatile Vector *)output)->y=D_8019AEFE;
        output->z=D_8019AF00;
        break;
    }
    case 2: {
        unsigned int index=D_800E11EA;
        unsigned short palette;
        D_800F3368.parameter00=32;
        D_800F3368.parameter02=2;
        D_800F3368.extent_x=32;
        D_800F3368.extent_y=32;
        D_800F3368.extent_x=32;
        D_800F3368.extent_y=32;
        palette=D_800E2850[index];

        D_800F3368.palette=3;
        D_800F3368.parameter06=0;
        D_800F3368.parameter0A=0;
        D_800F3368.depth=64;
        D_800F3368.tpage=palette;
        break;
    }
    }
    return 0;
}
