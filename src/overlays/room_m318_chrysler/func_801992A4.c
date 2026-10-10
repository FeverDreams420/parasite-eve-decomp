#include "common.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_anim_pool.h"
extern char **D_8009D254;

s32 func_801992A4(s32 arg0, void *arg1, s32 *arg2) {
    char *actor;

    if (arg0 != 1) {
        if (arg0 >= 2) {
            return 0;
        }
        if (arg0 != 0) {
            return 0;
        }
        ANIM_TASK_AS(s16)[10] = 0;
        ANIM_TASK_AS(s16)[11] = 0;
        {
            char *actor0 = *D_8009D254;
            s32 mask = 0xEFFFFFFF;
            *(s32 *)(actor0 + 0x4C) &= mask;
        }
        return 0;
    } else {
        actor = *D_8009D254;
        if (*(s32 *)(actor + 0x4C) & 0x10000000) {
            ANIM_TASK_AS(s16)[10] = 1;
            ANIM_TASK_AS(s16)[11] = 1;
            return 1;
        }
        if (D_800E27EC < *arg2) {
            return 0;
        }
        ANIM_TASK_AS(s16)[10] = 1;
        ANIM_TASK_AS(s16)[11] = 0;
        return 1;
    }
}
