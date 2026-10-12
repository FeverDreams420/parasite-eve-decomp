#include "scene_e08.h"

void func_80191970(void *arg0, RoomFxControl *control, void *arg2) {
    if (control->frame >= 0x79) {
        control->state = 2;
    }
}
