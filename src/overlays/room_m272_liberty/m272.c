#include "../room_lib/room_lib.h"
#include "pe1/boot_display.h"
#include "pe1/psyq_gpu.h"
#include "pe1/display_env.h"
#include "pe1/gpu_image.h"
#include "room_m272.h"
#include "room_m272_phase.h"

int func_8018F00C(void) {
    return 0;
}

int RoomM272_Cinematic(void) {
    RoomBlob8 b1 = RoomM272_BlobA;
    RoomBlob8 b2 = RoomM272_BlobB;
    RoomBlob8 b3 = RoomM272_BlobC;

    if (D_801991C8 == 0) {
        StoreImage(&b1, RoomM272_Rec);
        MoveImage(&b2, 0x300, 0x100);
        DrawSync(0);
    }
    if (D_801991C8 == 1) {
        LoadImage(&b1, RoomM272_Rec);
        MoveImage(&b3, 0x140, 0x100);
        DrawSync(0);
        VSync(2);
        PutDispEnv(&D_800BCE80[D_8009CDDC]);
        SetDispMask(1);
    }
    return 0;
}

int RoomM272_SetPhase(int a, int b, int c, int d) {
    D_801991C8 = d;
    return 0;
}

int RoomM272_Nop_8018F188(void) {
    return 0;
}

int RoomM272_Nop_8018F190(void) {
    return 0;
}

int RoomM272_Nop_8018F198(void) {
    return 0;
}

int RoomM272_Nop_8018F1A0(void) {
    return 0;
}
