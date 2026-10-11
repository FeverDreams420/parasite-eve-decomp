#include "room_m350_shared.h"
#include "pe1/gte_types.h"
#include "pe1/room_floor.h"
#include "pe1/field_actor.h"
#include "pe1/player_entity.h"
#include "pe1/field_vector_math.h"
#include "pe1/akao/commands.h"
#include "pe1/field_anim_pool.h"
#include "pe1/asset_find.h"
#include "room_m350_data.h"
typedef RoomM350Vector Vector;
typedef RenderMatrix Matrix;
typedef FieldActor Instance;
typedef RoomM350Actor Actor;
typedef struct { int asset; short speed,targetSpeed,heightPhase,mode; } State;
#include "RoomEffect_CloudController_externs.h"
#include "pe1/gte_sine_table.h"
int func_80198CF8(int event,State *state)
{
    Instance *instance=(Instance *)ANIM_OWNER_AS(Actor)->instance;
    Vector offset;
    /* Match note: original purpose of unused sp+0x20..0x2F is unknown. */
    char frameGap[16];
    if(event==0) {
        Matrix *matrix;
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
        register int speed asm("$5");
        if(instance->mode!=9) return 0;
        if(instance->state && instance->state->action_state[0]==1)
            instance->state->action_state[0]=2;
        {
            register int current asm("$3")=instance->anim.parts.integer;
            frame=current;
        if(D_8019A8B8.repeats>0) {
            register int signedFrame asm("$2")=(short)current;
            if(signedFrame>=30) {
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
        angle=FieldEng_VecToAngle((Vec3Fixed *)&g_PlayerEntity->pos_x,(Vec3Fixed *)&instance->pos_x)&4095;
        D_8019A8B8.playerAngle=angle;
        delta=(angle-(unsigned short)instance->rot_y)&4095;
        do {
            if (state->mode != 0) {
                if (state->mode != 1) {
                    state->targetSpeed=0;
                    break;
                }
            } else {
                if(state->speed<=1280) instance->rot_y=FieldEng_TurnToward(instance->rot_y,angle,80);
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
            register int height asm("$2");
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
