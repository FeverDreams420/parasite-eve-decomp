#include "pe1/room_m256_swing.h"
void func_80192654(void);

int func_80192584(RoomM256Swing *arg0) {
    arg0->cycle.callback = func_80192654;
    arg0->cycle.started = 0;
    arg0->cycle.signal = 0;
    arg0->cycle.segment = 0;
    arg0->cycle.angles[0] = 0;
    arg0->cycle.angles[3] = 0;
    return 0;
}
