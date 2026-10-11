#include "scene_e08.h"

void func_80192CCC(void *arg0, RoomFxControl *control, SceneE08ScreenFade *fade) {
    if (control->frame < 0x3D) {
        fade->phase = 1;
    } else {
        fade->phase = 2;
    }

    if (fade->phase == 1) {
        fade->level += 8;
        if (fade->level >= 0x80) {
            fade->level = 0x7F;
        }
    }

    if (fade->phase == 2) {
        fade->level -= 8;
        if (fade->level < 0) {
            fade->level = 0;
        }
    }

    if (control->frame >= 0x79) {
        control->status = 2;
    }
}
