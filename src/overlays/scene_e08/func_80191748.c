#include "pe1/field_script_context.h"
#include "pe1/field_actor.h"
#include "pe1/room_fx.h"
#include "pe1/scene_grab_head.h"

extern struct BattleEntity *D_8009D254;

int *func_800C2B28(int arg0);
void func_80020D50(void);
void func_80020DD0(void);
int FieldEng_GetStatus(void *arg0);

void func_80191748(SceneGrabHead *arg0, RoomFxControl *arg1) {
    int *status;
    int *flags;
    int *objectFlags;

    status = func_800C2B28(1);
    if (*status == 1) {
        if (arg1->frame < 0xF0) {
            func_80020D50();
        }

        if (arg1->frame == 0xF0) {
            if (FieldEng_GetStatus(arg0) == 3) {
                flags = *(int **)D_8009D254;
                flags[0x13] |= 0x4000;

                objectFlags = (int *)&arg0->actor->state->core_flags;
                *objectFlags |= 0x80000000;
                func_80020DD0();
            }
        }
    }

    if (func_800C2B68() == 1) {
        arg1->state = 2;
    }
}
