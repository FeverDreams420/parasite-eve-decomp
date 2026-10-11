#include "scene_e08.h"
#include "pe1/field_sprite_draw.h"
typedef struct {
    char bytes[4];
} Ovl178_FourBytes;

#include "func_80192C84_externs.h"

void func_80192C84(void *arg0, void *arg1, SceneE08ScreenFade *fade) {
    Ovl178_FourBytes local;

    local = D_8018F04C;
    func_800D1AE0((unsigned char *)&local, fade->level, 1, 8);
}
