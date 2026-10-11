#include "pe1/room_m256_swing.h"
int func_801925AC(RoomM256Swing *arg0, int arg1, int arg2, int arg3, int arg4, int arg5) {
    switch ((unsigned int)arg2) {
    case 0x19:
        if (arg1 != 1) {
            return 0;
        }
        arg0->cycle.signal = (int *)arg3;
        *(int *)arg3 = arg1;
        break;
    case 0x100:
        arg0->cycle.angles[1] = arg3;
        arg0->cycle.angles[2] = arg4;
        break;
    case 0x101:
        arg0->cycle.durations[0] = arg3;
        arg0->cycle.durations[1] = arg4;
        arg0->cycle.durations[2] = arg5;
        break;
    default:
        return 0;
    }
    return 0;
}
