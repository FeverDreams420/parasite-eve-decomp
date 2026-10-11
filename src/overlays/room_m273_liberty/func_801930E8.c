#include "common.h"
#include "pe1/field_actor.h"
#include "room_m273.h"

int func_801930E8(RoomOscillationEffect *obj) {
    if (obj->params.signal != 0) {
        obj->params.signal = 0;
    }

    obj->status = 4;
    return 0;
}
