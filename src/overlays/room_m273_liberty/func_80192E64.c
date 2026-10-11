#include "common.h"
#include "pe1/field_actor.h"
#include "room_m273.h"
#include "pe1/room_liberty_choice.h"

int func_8005186C(int value);

int func_80192E64(RoomOscillationEffect *base, int unused, unsigned int operation, int arg3,
                  int arg4, int arg5) {
    RoomOscillationParams *fields = &base->params;

    switch (operation) {
    case 0:
        fields->active = arg3;
        if (arg4 != 0) {
            fields->frame = 0;
        }
        break;
    case 1:
        fields->amplitude = arg3;
        fields->period = arg4;
        fields->baseHeight = arg5;
        break;
    case 2: {
        FieldActor *owner = base->actor;
        int dx = (*(s32 *)arg3 - owner->pos_x) >> 16;
        int dy = (*(s32 *)arg4 - owner->pos_z) >> 16;

        *(s32 *)arg5 = func_8005186C(dx * dx + dy * dy);
        break;
    }
    case 3:
        *(s32 *)arg4 = func_80193114(*(s32 *)arg3, (RoomLibertyChoiceInputs *)fields);
        break;
    case 4:
        fields->angleOut = (int *)arg3;
        fields->distanceOut = (int *)arg4;
        break;
    case 5:
        fields->signal = (int *)arg3;
        *(s32 *)arg3 = 1;
        break;
    }
    return 0;
}
