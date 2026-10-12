#include "pe1/scene_grab_trigger.h"

int func_80192630(SceneGrabTrigger *obj, int mode, int event, int arg3) {
    if (event == 0x19 && mode == 1) {
        obj->control.signal = (int *)arg3;
        *(int *)arg3 = mode;
    }

    return 0;
}
