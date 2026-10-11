/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/field_actor.h"
#include "room_m273.h"
#include "pe1/gte_types.h"

extern FieldActor *g_PlayerEntity;
extern s16 D_800966EE[];

int func_8005186C(int squaredDistance);

int func_80192F54(void) {
    return 0;
}

int func_80192F5C(RoomOscillationEffect *effect) {
    int **outs = &effect->angleOut;
    FieldActor *actor = effect->actor;

    if (effect->active) {
        int frame = effect->frame;
        int period = effect->period;
        int phase = ((frame << 12) / period - 2048) & 4095;
        register int sine asm("$2");
        register int displacement asm("$2");
        register int half asm("$4");
        FieldActor *target;

        effect->frame = frame + 1;
        sine = *(s16 *)((char *)D_800966EE + phase * 4);
        half = (sine + 4096) / 2;
        displacement = (half * effect->amplitude) / 4096 + 8;
        target = ((RoomOscillationEffect *)effect)->actor;
        {
            int height;
            register int scaled asm("$4");

            scaled = displacement << 12;
            height = effect->baseHeight - scaled;
            target->pos_y = height;
        }
    }

    if (outs[0]) {
        FieldActor *player = g_PlayerEntity;
        int angle = ratan2(actor->pos_x - player->pos_x,
                                  actor->pos_z - player->pos_z);

        *outs[0] = (angle - actor->rot_y) & 4095;
    }

    if (outs[1]) {
        FieldActor *player = g_PlayerEntity;
        int dx = (player->pos_x - actor->pos_x) >> 16;
        int dz = (player->pos_z - actor->pos_z) >> 16;

        *outs[1] = func_8005186C(dx * dx + dz * dz);
    }

    return 0;
}
