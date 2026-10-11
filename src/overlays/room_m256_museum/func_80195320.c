#include "pe1/room_m256.h"

int func_80195320(int mode, u16 *out) {
    FieldActor *actor;
    FieldActorState *state;
    u8 *action;
    /* Keep the return register available for the retail mode dispatch. */
    register int ret asm("$2");
    RoomM256TrailEffect *task;
    u16 value;

    switch (mode) {
    case 0:

        *out = (u16)D_800E2368->variables[0];
        D_80195EF8 = Asset_FindTable08ByU32Key(D_800B0E64[0], 0xC5887704);
        func_800C6D5C((union FieldTexturedModel *)D_80195EF8, 0, 0);
        if (D_800E2368->flags) {
            actor = (FieldActor *)D_800F32D0->actor;
            if (actor) {
                state = actor->state;
                if (state) {
                    action = state->action_state;
                    if (*action == 1) *action = 2;
                }
            }
        }
        return func_800CE560(D_800F33E0->end, 0x18, 0x20,
                               func_801940B0);

        case 1:
    if (D_800E27EC == mode) {
        task = func_800CE610(D_800F33E0->end);
        if (task) {
            value = *out;
            task->kind = 0;
            task->frame = 0;
            task->index = value;
        }
        func_800D3F64(0x5A6, func_800D3FD8());
        func_800D3F64(0x5BE, 0x80);
    }
    ret = D_800E27EC < 8;
    if (ret) return 0;
    return 2;

    case 2:
    {
        unsigned palette_index = D_800E11EA;
        u16 palette;
        palette = D_800E2850[palette_index];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 8;
        D_800F3368.tpage = palette;
    }
    }
    return 0;
}
