/* MASPSX_FLAGS: --expand-div */

/*
 * The floor walker actor class driven by RoomLib_HandlerG: a sixth class
 * that 8 rooms (room_m145, m152, m153, m154, m332, m399, m401, m402) link
 * directly after the room library (RoomLib_ActorClasses.c), listed in the
 * same room class table with the same seven-method layout (no-op, Init,
 * Configure, no-op, Update, ..., Release, no-op).
 */
#include "room_lib.h"
#include "pe1/room_floor_walker.h"

int RoomLib_HandlerGNop0(void) {
    return 0;
}
int RoomLib_InitHandlerG(RoomEnt *o) {
    unsigned int m = 0x10002;
    RoomLink *l = o->link;
    o->t16 = -1;
    o->t17 = -1;
    o->t18 = -1;
    o->t19 = 3;
    o->sub.signal = 0;
    o->active = 0;
    o->t1A = 0;
    ((RoomFloorWalkerObject *)o)->state.rest = 0;
    ((RoomFloorWalkerObject *)o)->state.turning = 0;
    o->sub.cb = RoomLib_HandlerG;
    RW32(l, 0x98) |= m;
    l->h250 |= 0x400;
    return 0;
}

int RoomLib_ConfigureHandlerG(RoomEnt *o, int mode, unsigned int op,
                              int arg0, int arg1, int arg2) {
    RoomFloorWalkerState *p = &((RoomFloorWalkerObject *)o)->state;

    if (op != 4) {
    if (op < 5) {
        if (op == 0) {
            goto case0;
        }
        return 0;
    }
    if (op == 12) {
        goto case12;
    }
    if (op != 25) {
        return 0;
    }
    if (mode != 1) {
        return 0;
    }
    o->sub.signal = (int *)arg0;
    *(int *)arg0 = mode;

    }
    else
    {
case4:
    ((RoomFloorWalkerObject *)o)->state.speed = arg0;
    return 0;

case0:
    ((RoomFloorWalkerObject *)o)->state.leader = (RoomLink *)D_8009D20C;
    if (D_8009D20C != 0) {
        do {
            FieldActorNode *cur = (FieldActorNode *)p->leader;
            if (cur->b0C == arg0 && cur->b0D == arg1) {
                if ((cur->w98 & 0x10) == 0) {
                    break;
                }
            }
            p->leader = (RoomLink *)((FieldActorNode *)p->leader)->next;
        } while (p->leader != 0);
        if (p->leader != 0) {
            return 0;
        }
        RoomLib_ReleaseHandlerG(o);
    } else {
        RoomLib_ReleaseHandlerG(o);
    }
    return 0;

case12:
    ((RoomFloorWalkerObject *)o)->state.hopVelocity = arg0;
    ((RoomFloorWalkerObject *)o)->state.gravity = arg1;
    ((RoomFloorWalkerObject *)o)->state.restTime = arg2;

    }
ret:
    return 0;
}

int RoomLib_HandlerGNop3(void) {
    return 0;
}

#define ROOMLIB_UPDATE_TRANSFORM_SCALE_NAME RoomLib_UpdateHandlerG
#include "RoomLib_UpdateTransformScale.inc"

void RoomLib_HandlerG(RoomEnt *o) {
    RoomEnt *self = o;
    RoomLink *src = ((RoomFloorWalkerObject *)self)->state.leader;
    RoomLink *dst = self->link;
    dst->pos[0] = ((GteMatrix *)src->p238)[4].t[0] << 16;
    dst->pos[1] = ((GteMatrix *)src->p238)[4].t[1] << 16;
    dst->pos[2] = ((GteMatrix *)src->p238)[4].t[2] << 16;
    *(RoomLibPacked8 *)dst->rotation = *(RoomLibPacked8 *)src->rotation;
    if ((src->variant == 7) && (src->winLo >= 3) && (src->winHi < 3)) {
        self->sub.cb = RoomLib_HandlerGFloorWalker;
    }
    if (src->target != 0) {
        if (func_8003010C(src, 0x2C) <= 0) {
            func_80030220(dst, 0x2D, 0);
            func_80030220(dst, 0x2C, 0);
            func_80030220(dst, 0x5F, 0);
            RoomLib_ReleaseHandlerG(self);
        }
    } else {
        RoomLib_ReleaseHandlerG(self);
    }
}

#define ROOMLIB_FLOOR_WALKER_FUNC RoomLib_HandlerGFloorWalker
#include "RoomLib_FloorWalker.inc"

int RoomLib_ReleaseHandlerG(RoomEnt *o) {
    int *p = o->sub.signal;
    o->state = 4;
    if (p != 0) {
        *p = 0;
    }
    return 0;
}
int RoomLib_HandlerGNop6(void) {
    return 0;
}

