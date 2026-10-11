#include "room_m273_effects.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/room_floor.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_anim_pool.h"
typedef GteShortVector Vector;
typedef struct { Vector position; unsigned char parameter[4]; } Effect;
typedef RoomM273ThresholdTransform Transform;
typedef RoomM273ScaledLayerOwner Owner;

/* The emitter owns the twelve-byte records rendered by this callback. */
#include "pe1/gte_sine_table.h"
extern short D_800966EE[];
extern unsigned char D_8019AB70[];

int func_8019353C(int mode,Effect *effect) {
    Vector position,rotation;
    /* Layout only: purpose of these eight original frame bytes is unknown. */
    int unknownFrame[2];
    if(mode==1) {
        if(D_800E27EC>=8) return 1;
    } else if(mode==2) {
        short frame=D_800E27EC-1;
        /* The input vector's fourth halfword supplies the initial scale. */
        int product=(*(short *)((char *)D_800966EC+(((unsigned int)frame<<9)&0x3E00))*2+4096)*effect->position.pad;
        int sample=*(int *)((char *)D_800966EC+(((unsigned int)frame<<9)&0x3E00));
        short size=product/4096;
        short shade=sample>>21;
        short i;
        rotation.x=1024; rotation.y=0; rotation.z=frame*170; rotation.pad=-1;
        position=effect->position;
        i=0;
        do {
            func_800D004C(&position,128,128,12,(GteRotation *)&rotation,size,size,(struct RenderColor *)D_8019AB70,(struct RenderColor *)effect->parameter,shade,1);
            shade>>=1;
            size=size*3/2;
            i++;
            position.y=g_RoomFloorY->raw;
        } while(i<2);
    }
    return 0;
}

int func_801936F0(int mode) {
    /* Layout only: purpose of these eight original frame bytes is unknown. */
    int unknownFrame[2];
    switch(mode) {
    case 0:
        return func_800CE560(ANIM_POOL_AS(RoomM273PoolContext)->pool,12,9,
                             (FieldAnimCallbackListCallback)func_8019353C);
    case 1: {
        Effect *effect;
        Transform *transform;
        int scale;
        if(D_800E27EC>=12) return 2;
        effect=func_800CE610(ANIM_POOL_AS(RoomM273PoolContext)->pool);
        if(!effect) break;
        transform=ANIM_OWNER_AS(RoomM273EffectStateContext)->state.scaled_layer->transform;
        effect->position.x=transform->x;
        asm("" : : : "memory");
        effect->position.y=transform->y;
        effect->position.z=transform->z;
        scale=D_800966EE[((D_800E27EC*1024/12)&0xFFF)*2];
        effect->parameter[0]=40;
        effect->position.pad=scale;
        effect->parameter[1]=((short *)D_800966EC)[((D_800E27EC*1024/12)&0xFFF)*2]*40/4096;
        effect->parameter[2]=0;
        break;
    }
    case 2:
        break;
    }
    return 0;
}
