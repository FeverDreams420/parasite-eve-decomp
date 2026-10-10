#include "room_m350_shared.h"
#include "pe1/gte_types.h"
#include "pe1/field_actor.h"
#include "pe1/player_entity.h"
#include "pe1/field_vector_math.h"
/* Dispatches frame-table events and the actor's turning/recoil motion. */
typedef RoomM350Vector Vector;
typedef struct { unsigned char frame,slot,value,pad; } Event;
typedef struct { Event entries[8]; } Events;
typedef FieldActor Instance;
typedef RoomM350Actor Actor;
extern Actor *D_800F32D0;
extern Events D_8018F1CC;
extern unsigned char D_8019A854[],D_8019A859,D_8019A86E;
extern int D_8019A858;
extern short D_8019A85C,D_8019A85E,D_8019A86A,D_8019A86C;
extern int D_800966EC[];
extern short D_800966EE[];

int func_80197DA8(int event,short *cursor) {
    Events events=D_8018F1CC;
    Vector movement;
    Instance *instance=(Instance *)D_800F32D0->instance;
    int frame,previous;
    short phase;
    if (event==0) {
        *(int *)D_8019A854=0;
        D_8019A858=0;
        D_8019A86E=0;
        D_8019A85C=0;
        D_8019A86A=0;
        D_8019A86C=0;
        *cursor=0;
        goto done;
    }
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
        int speed=(short)D_800966EC[(((unsigned int)phase<<10)&0x3C00)/4]>>6;
        int angle=FieldEng_VecToAngle(g_PlayerEntity->render_object.model_matrix.translation,instance->render_object.model_matrix.translation);
        instance->rot_y=FieldEng_TurnToward(instance->rot_y,(short)angle,speed);
    }
    while ((short)frame>=events.entries[*cursor].frame) {
        Event *entry=&events.entries[*cursor];
        if ((short)previous>=entry->frame) break;
        D_8019A854[entry->slot]=entry->value;
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
done:
    return 0;
}
