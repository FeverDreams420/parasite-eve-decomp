#include "pe1/room_m256_swing.h"

int func_80192808(RoomM256Swing *obj) {
    int *state;

    obj->actor->render_object.rotation_overrides[0].matrix_index = 0;
    obj->actor->render_object.rotation_overrides[0].flags = 0;
    state = obj->cycle.signal;
    obj->status = 4;
    if (state != 0) {
        *state = 0;
    }

    return 0;
}
