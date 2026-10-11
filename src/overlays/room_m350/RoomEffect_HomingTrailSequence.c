#include "pe1/gte.h"
#include "pe1/field_actor.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/room_m350_effects.h"
#include "pe1/render_object.h"
#include "pe1/room_floor.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_vector_math.h"
#include "pe1/field_anim_pool.h"
#include "pe1/psyq_gpu.h"
/* Moving effect with collision, queued trail positions and ground rendering. */
#include "RoomEffect_HomingTrailSequence_externs.h"
#include "pe1/gte_sine_table.h"
#include "RoomEffect_HomingTrailSequence_externs_1.h"
int func_801947BC(int event,RoomM350Particle *p)
{
    GteMatrix matrix;
    GteShortVector offset,ground;
    /* Match note: target saves registers 16 bytes later; original local unknown. */
    char frameGap[16];
    if(event==1) {
        int timer=D_800E27EC;
        int brightness;
        if(timer>=48) {
            int result=1;
            unsigned short *count=(unsigned short *)&D_8019A802;
            *count=*count-1;
            return result;
        }
        if(p->tail.value.active.frame>=48) return 0;
        if(!p->tail.value.active.hit) {
            if(p->motion.value.rotation.x>0) p->motion.value.rotation.x-=64;
            else {
                GteSineWord *trig=D_800966EC;
                int speed=trig[(timer<<4)&0xFF0].part.sin/4;
                int angle=FieldEng_VecToAngle((Vec3Fixed *)g_PlayerEntity->render_object.model_matrix.translation,(Vec3Fixed *)ANIM_OWNER_AS(RoomM350EffectActor)->instance->transform.t);
                int yaw=FieldEng_TurnToward(p->motion.value.rotation.y,(short)angle,speed);
                int sum,product;
                p->motion.value.rotation.y=yaw;
                product=(short)trig[(D_800E27EC+2048)&4095].word*160;
                sum=yaw+80;
                p->motion.value.rotation.y=sum-product/4096;
            }
            RotMatrixYXZ(&p->motion.value.rotation,&matrix);
            matrix.t[0]=p->position.x;
            matrix.t[1]=p->position.y;
            matrix.t[2]=p->position.z;
            offset.x=0; offset.y=0; offset.z=0u-(unsigned short)p->tail.value.active.speed;
            gte_ldrotmatrix(&matrix); gte_ldtransmatrix(&matrix);
            gte_ldv0(&offset); gte_rtv0tr_mac();
            gte_stsv(&p->position);
        }
        if(p->tail.value.active.frame<40) brightness=((short)D_800966EC[(p->tail.value.active.frame<<9)&0xE00].word>>6)+128;
        else {
            unsigned int value=D_800966EC[((p->tail.value.active.frame-40)<<7)&0xF80].part.cos;
            brightness=value>>5;
        }
        p->tail.value.active.brightness=brightness;
        if(!p->tail.value.active.hit && g_PlayerEntity->mode>=4) {
            int dx=g_PlayerEntity->render_object.model_matrix.translation[0]-p->position.x;
            int dz=g_PlayerEntity->render_object.model_matrix.translation[2]-p->position.z;
            if(Math_IntSqrt((unsigned int)dx*dx+(unsigned int)dz*dz)<128) {
                short *count;
                short n;
                p->tail.value.active.hit=1;
                ((RoomM350EffectInstance *)g_PlayerEntity)->owner->status|=0x4000;
                if(ANIM_OWNER_AS(RoomM350EffectActor)->instance->owner) ANIM_OWNER_AS(RoomM350EffectActor)->instance->owner->flags|=0x80000000;
                p->tail.value.active.speed=0;
                if(p->tail.value.active.frame<40) p->tail.value.active.frame=40;
                count=&D_8019A7FE; n=*count;
                if(n<4) {
                    register int next asm("$4")=n;
                    asm volatile("" : "=r"(next) : "0"(next));
                    *count=next+1;
                    asm volatile("" : : "r"(count), "r"(next));
                    {
                        RoomM350PointerQueue *queue=(RoomM350PointerQueue *)((char *)count-PE1_OFFSETOF(RoomM350PointerQueue,count));
                        queue->positions[n]=&p->position;
                    }
                }
            }
        }
        if(p->tail.value.active.frame>=36 && !(p->tail.value.active.frame&3)) {
            short *count=&D_8019A7FE;
            short n=*count;
            if(n<4) {
                register int next asm("$4")=n;
                asm volatile("" : "=r"(next) : "0"(next));
                *count=next+1;
                asm volatile("" : : "r"(count), "r"(next));
                {
                    RoomM350PointerQueue *queue=(RoomM350PointerQueue *)((char *)count-PE1_OFFSETOF(RoomM350PointerQueue,count));
                    queue->positions[n]=&p->position;
                }
            }
        }
        {
            short *count=&D_8019A800;
            unsigned short n=*(unsigned short *)count;
            int next=n+1;
            int shifted=(unsigned int)n<<16;
            *count=next;
            asm volatile("" : : "r"(count));
            count-=0x24;
            *(GteShortVector *)((shifted>>13)+(unsigned int)count)=p->position;
        }
        p->tail.value.active.frame=(unsigned short)p->tail.value.active.frame+1;
    } else if(event==2) {
        int clut,palette;
        if(p->tail.value.active.frame>=48) return 0;
        palette=D_800E1204[D_800F3368.palette];
        if(D_800F3368.palette==4 && D_800F3428) palette+=4;
        clut=GetClut(32,palette);
        func_800CEE20(&p->position,0,4096,4096,108,(unsigned short)clut,1,p->tail.value.active.brightness,0);
        ground.x=p->position.x; ground.y=g_RoomFloorY->raw; ground.z=p->position.z;
        func_800D004C(&ground,192,192,8,&D_8019A3C0,4096,4096,&D_8019A4E4,&D_8019A3C8,p->tail.value.active.brightness,1);
    }
    return 0;
}

#include "RoomEffect_HomingTrailSequence_externs_2.h"
int func_80194CFC(int event) {
    switch (event) {
    case 0:

        return func_800CE560(ANIM_POOL_AS(RoomM350EffectEmitter)->pool,28,3,
                             (FieldAnimCallbackListCallback)func_801947BC);
        case 1:
    {
        RoomM350EffectInstance *instance;
        RoomM350Particle *particle;
        int frame, previous;
        if (D_8019A804) return 2;
        instance=ANIM_OWNER_AS(RoomM350EffectActor)->instance;
        if (instance->animation != 13) return 0;
        frame=instance->frame;
        previous=instance->previousFrame;
        if (frame<15 || previous>=15) return 0;
        particle=func_800CE610(ANIM_POOL_AS(RoomM350EffectEmitter)->pool);
        *(GteShortVector *)particle=D_8019A7A0;
        particle->position.pad=16;
        particle->motion.value.spawn.speed=512;
        {
            int yaw=ANIM_OWNER_AS(RoomM350EffectActor)->instance->yaw;
            particle->motion.value.spawn.phase=0;
            particle->motion.value.spawn.yaw=yaw;
        }
        asm volatile("" : : : "memory");
        {
            int count=D_8019A802;
            particle->tail.value.spawn.duration=16;
            particle->tail.value.spawn.frame=0;
            particle->tail.value.spawn.state=0;
            particle->tail.value.spawn.size=100-count*20;
        }
        asm volatile("" : : : "memory");
        D_8019A802++;
    }
        break;
    case 2:

    D_800F3368.parameter00 = 32;
    D_800F3368.parameter02 = 2;
    D_800F3368.extent_x = 32;
    D_800F3368.extent_y = 32;
    D_800F3368.extent_x = 32;
    D_800F3368.extent_y = 32;
    D_800F3368.tpage = D_800E2850[D_800E11E4[3]];
    D_800F3368.palette = 3;
    D_800F3368.parameter06 = 0;
    D_800F3368.parameter0A = 0;
    D_800F3368.depth = 64;
    }
    return 0;
}
