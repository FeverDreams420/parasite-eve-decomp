#include "pe1/room_m350_effects.h"
#include "pe1/field_sprite_params.h"
#include "room_m350_shared.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/gte_types.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_anim_pool.h"
#include "room_m350_data.h"
/* MASPSX_FLAGS: --expand-div */
typedef RoomM350Vector Vector;
typedef GteMatrix Matrix;
typedef struct { short position[3],reserved; } Particle;

#include "RoomEffect_PulsedPairSpawner_externs.h"
int func_80192508(int event)
{
    Vector offset;
    Matrix matrix;
    Vector vector;
    /* Match note: original purpose of unused sp+0x40..0x47 is unknown. */
    char frameGap[8];
    switch (event) {
    case 0:

        return func_800CE560(ANIM_POOL_AS(RoomM350Emitter)->pool,8,64,
                             (FieldAnimCallbackListCallback)func_80199B94);
        case 1:
    {
        int timer=D_800E27EC;
        int count;
        int i;
        Vector *out;
        if(timer>=33) return 2;
        count=(D_800966EE[((((unsigned int)timer<<7)+2048)&4095)*2]+4096)/2048+1;
        if(count*2<=0) return 0;
        i=0;
        out=&offset;
        do {
            int random;
            int highByte;
            int divisor;
            Vector *base;
            Particle *particle=func_800CE610(ANIM_POOL_AS(RoomM350Emitter)->pool);
            if(!particle) return 0;
            vector.x=(unsigned int)Inv_ScrambleGrid()<<4;
            vector.y=(unsigned int)Inv_ScrambleGrid()<<4;
            vector.z=0;
            RotMatrixYXZ(&vector,&matrix);
            asm volatile("" : : "i"(&&offset_start));
offset_start:
            vector.x=0; vector.y=0;
            highByte=Inv_ScrambleGrid();
            random=((unsigned int)highByte<<8)|(unsigned int)Inv_ScrambleGrid();
            divisor=192;
            vector.z=random%divisor+64;
            ApplyMatrixSV(&matrix,&vector,out);
            base=&D_8019A778[i&1];
            particle->position[0]=(unsigned short)offset.x+(unsigned short)base->x;
            particle->position[1]=(unsigned short)offset.y+(unsigned short)base->y;
            particle->position[2]=(unsigned short)offset.z+(unsigned short)base->z;
            i++;
        } while(i<count*2);
    }
        break;
    case 2:

    {
        int index=D_800E11E8;
        int palette;
        D_800F3368.parameter00=16; D_800F3368.parameter02=1;
        D_800F3368.extent_x=16; D_800F3368.extent_y=16;
        D_800F3368.extent_x=16; D_800F3368.extent_y=16;
        palette=D_800E2850[index];
        D_800F3368.palette=2; D_800F3368.parameter06=0;
        D_800F3368.parameter0A=0; D_800F3368.depth=0; D_800F3368.tpage=palette;
    }
    }
    return 0;
}
