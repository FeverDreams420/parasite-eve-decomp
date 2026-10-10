#include "room_m350_shared.h"
#include "pe1/gte_types.h"
#include "pe1/room_floor.h"
#include "pe1/field_actor.h"
#include "pe1/player_entity.h"
#include "pe1/field_vector_math.h"
#include "pe1/akao/commands.h"
#include "pe1/field_anim_pool.h"
typedef RoomM350Vector Vector;
typedef RenderMatrix Matrix;
typedef FieldActor Instance;
typedef RoomM350Actor Actor;
typedef struct { int asset; short speed,targetSpeed,heightPhase,mode; } State;
extern unsigned char D_8019A8BE,D_8019A8C0;
extern short D_8019A89E,D_8019A8B8,D_8019A8BA,D_8019A8BC;
extern short D_800966EC[];
extern short D_8019A870,D_8019A872,D_8019A874,D_8019A876,D_8019A878,D_8019A87A,D_8019A87C,D_8019A87E,D_8019A880;
extern int D_8019A884,D_8019A888,D_8019A88C;
extern int Asset_Find08w(int,int,int,int,int);
int func_80198CF8(int event,State *state)
{
    Instance *instance=(Instance *)ANIM_OWNER_AS(Actor)->instance;
    Vector offset;
    /* Match note: original purpose of unused sp+0x20..0x2F is unknown. */
    char frameGap[16];
    if(event==0) {
        Matrix *matrix;
        state->speed=0; state->targetSpeed=0; state->heightPhase=0; state->mode=0;
        D_8019A8BE=0; D_8019A8C0=0; D_8019A89E=0;
        asm volatile("" : : : "memory");
        matrix=instance->render_object.matrices;
        state->asset=Asset_Find08w(0x5CB,instance->state->progress,
            (short)matrix->translation[0],(short)matrix->translation[1],(short)matrix->translation[2]);
    } else if(event==1) {
        int frame,angle,delta,cosine,sine;
        int phase;
        int target;
        register int speed asm("$5");
        short *repeat;
        if(instance->mode!=9) return 0;
        if(instance->state && instance->state->action_state[0]==1)
            instance->state->action_state[0]=2;
        repeat=&D_8019A8B8;
        {
            register int current asm("$3")=instance->anim.parts.integer;
            frame=current;
        if(*repeat>0) {
            register int signedFrame asm("$2")=(short)current;
            if(signedFrame>=30) {
                instance->anim.fixed=0x50000;
                *repeat=*(volatile unsigned short *)repeat-1;
            }
        } else if((short)current>=instance->action-1) {
            if(instance->state) instance->state->action_state[0]=4;
            D_8019A8BE=1;
            asm volatile("" : : : "memory");
            Akao_Cmd_21(state->asset,0);
            return 1;
        }
        }
        D_8019A8BA=func_800DFE20((Vec3Fixed *)&g_PlayerEntity->pos_x,(Vec3Fixed *)&instance->pos_x);
        angle=FieldEng_VecToAngle((Vec3Fixed *)&g_PlayerEntity->pos_x,(Vec3Fixed *)&instance->pos_x)&4095;
        D_8019A8BC=angle;
        asm volatile("" : : : "memory");
        delta=(angle-(unsigned short)instance->rot_y)&4095;
        if (state->mode != 0) {
            if(state->mode==1) goto steer;
            state->targetSpeed=0;
            goto adjust;
        }
accelerate:
        {
            if(state->speed<=1280) instance->rot_y=FieldEng_TurnToward(instance->rot_y,angle,80);
            if(state->speed<=state->targetSpeed && ((instance->rot_y-D_8019A8BC+64)&4095)<=256) {
                state->mode=1; state->targetSpeed=1536; D_8019A8C0=0;
            }
        }
        goto adjust;
steer:
        {
            if(state->targetSpeed>=state->speed && ((delta-1536)&4095)<1024) {
                state->mode=0; state->targetSpeed=512;
            }
        }
adjust:
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
        register Matrix *matrix asm("$4")=(Matrix *)&D_8019A870;
        register Vector *vector asm("$5")=&offset;
        *(short *)matrix=cosine; D_8019A874=sine; D_8019A87C=0u-(unsigned int)sine;
        D_8019A880=cosine;
        D_8019A88C=0; D_8019A888=0; D_8019A884=0;
        D_8019A87E=0; D_8019A87A=0; D_8019A876=0; D_8019A872=0;
        D_8019A878=4096;
        asm volatile("" : : "r"(matrix), "r"(vector) : "memory");
        offset.x=0; offset.y=0; offset.z=state->speed;
        ApplyMatrixSV(matrix,vector,vector);
        }
        instance->pos_x=(unsigned int)instance->pos_x+((unsigned int)offset.x<<12);
        instance->pos_z=(unsigned int)instance->pos_z+((unsigned int)offset.z<<12);
        phase=state->heightPhase;
        {
        int nextPhase=phase;
        asm volatile("" : "=r"(nextPhase) : "0"(nextPhase), "r"(phase));
        if(phase<16) {
            int sample,floor;
            register int height asm("$2");
            phase=nextPhase+1;
            state->heightPhase=phase;
            sample=*(short *)((char *)D_800966EC+(((unsigned int)phase<<8)&0x3F00));
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
