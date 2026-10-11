#include "../room_lib/room_lib.h"
#include "pe1/boot_display.h"
#include "pe1/psyq_gpu.h"
#include "pe1/display_env.h"
#include "pe1/gpu_image.h"

extern RoomBlob8 RoomM272_BlobA;
extern RoomBlob8 RoomM272_BlobB;
extern RoomBlob8 RoomM272_BlobC;
extern char RoomM272_Rec[];
extern int D_801991C8;
extern int D_8009CDDC;

int RoomM272_Cinematic_8018F014(void) {
    RoomBlob8 b1 = RoomM272_BlobA;
    RoomBlob8 b2 = RoomM272_BlobB;
    RoomBlob8 b3 = RoomM272_BlobC;

    if (D_801991C8 == 0) {
        StoreImage(&b1, RoomM272_Rec);
        MoveImage(&b2, 0x300, 0x100);
        func_80074DC0(0);
    }
    if (D_801991C8 == 1) {
        LoadImage(&b1, RoomM272_Rec);
        MoveImage(&b3, 0x140, 0x100);
        func_80074DC0(0);
        func_80073A44(2);
        PutDispEnv(&D_800BCE80[D_8009CDDC]);
        func_80074D28(1);
    }
    return 0;
}
