#include "pe1/room_m350_effects.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_texture_tables.h"
#include "pe1/psyq_gpu.h"

#include "pe1/render_object.h"
#include "pe1/field_anim_pool.h"

#include "pe1/gte_sine_table.h"
extern RenderColor D_8019A3D0;
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

/* MASPSX_FLAGS: --expand-div */
extern unsigned char D_8019A8BE;
extern int Inv_ScrambleGrid(void);
int func_80198ABC(int event)
{
    GteMatrix matrix;
    GteShortVector vector;
    switch (event) {
    case 0:

        return func_800CE560(ANIM_POOL_AS(RoomM350EffectEmitter)->pool,20,16,
                             (FieldAnimCallbackListCallback)func_80198860);
        case 1:
    {
        int i;
        if(D_8019A8BE) return 2;
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
