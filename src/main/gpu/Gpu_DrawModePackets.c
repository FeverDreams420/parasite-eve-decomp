#include "common.h"
#include "pe1/psyq_gpu.h"
#include "pe1/psyq_bios.h"

/* A draw-mode word pair followed by the sprite primitive it prefixes. */
typedef struct GpuModeSprtPacket {
    u32 tag;
    u32 mode;
    u8 sprt[1];
} GpuModeSprtPacket;

/* A draw-mode word pair followed by the tile primitive it prefixes. */
typedef struct GpuModeTilePacket {
    u32 tag;
    u32 mode;
    u8 tile[1];
} GpuModeTilePacket;

void Gpu_InitDrawModeSprtPacket(GpuModeSprtPacket *packet, int tpage) {
    u8 *prim = packet->sprt;

    SetDrawTPage(packet, 0, 1, tpage);
    SetSprt(prim);
    if (MargePrim(packet, prim) != 0) {
        exit(-1);
    }
}

void Gpu_InitDrawModeTilePacket(GpuModeTilePacket *packet, int tpage) {
    u8 *prim = packet->tile;

    SetDrawTPage(packet, 0, 1, tpage);
    SetTile(prim);
    if (MargePrim(packet, prim) != 0) {
        exit(-1);
    }
}
