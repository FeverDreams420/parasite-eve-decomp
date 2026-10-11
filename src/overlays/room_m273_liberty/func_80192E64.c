#include "common.h"
#include "pe1/room_liberty_choice.h"

int func_8005186C(int value);

int func_80192E64(char *base, int unused, unsigned int operation, int arg3,
                  int arg4, int arg5) {
    char *fields = base + 0xC;

    switch (operation) {
    case 0:
        *(s16 *)(fields + 0x14) = arg3;
        if (arg4 != 0) {
            *(s32 *)(fields + 0xC) = 0;
        }
        break;
    case 1:
        *(s16 *)(fields + 0x16) = arg3;
        *(s16 *)(fields + 0x18) = arg4;
        *(s32 *)(fields + 0x10) = arg5;
        break;
    case 2: {
        char *owner = *(char **)(base + 8);
        int dx = (*(s32 *)arg3 - *(s32 *)(owner + 0x28)) >> 16;
        int dy = (*(s32 *)arg4 - *(s32 *)(owner + 0x30)) >> 16;

        *(s32 *)arg5 = func_8005186C(dx * dx + dy * dy);
        break;
    }
    case 3:
        *(s32 *)arg4 = func_80193114(*(s32 *)arg3, (s32 **)fields);
        break;
    case 4:
        *(s32 *)fields = arg3;
        *(s32 *)(fields + 4) = arg4;
        break;
    case 5:
        *(s32 *)(fields + 8) = arg3;
        *(s32 *)arg3 = 1;
        break;
    }
    return 0;
}
