/* Scene display clearing and CD read flow share the scene read flags. */
#include "pe1/game_state_types.h"
#include "pe1/game_state.h"
#include "common.h"
#include "pe1/render_camera.h"
#include "pe1/psyq_gpu.h"
#include "pe1/cdrom.h"
#include "pe1/cd_poll.h"
#include "pe1/cdrom_buffers.h"
#include "pe1/psyq_cd.h"
#include "pe1/psyq_bios.h"
#include "pe1/pe_image.h"
#include "pe1/psyq_ds_queue.h"
#include "pe1/gpu_image.h"
#include "pe1/akao/commands.h"

void Gpu_ClearOnFlag(void) {
    u32 *state = &g_GameState.flags;

    if (*state & 0x08000000) {
        RECT rect;
        unsigned int value;
        unsigned char byte;

        rect.x = 0;
        rect.y = 0;
        rect.w = 0x140;
        rect.h = 0x1C0;
        ClearImage(&rect, 0, 0, 1);
        DrawSync(0);
        Render_PrepareFrame();
        byte = g_DiscChangeFlags;
        value = *state;
        g_DiscChangeFlags = byte | 2;
        *state = value & 0xF7FFFDFF;
    }
}

int CdRom_ReadSectorsFromLba(u32 lba, void *destination, u32 size) {
    return CdRom_ReadSectors(lba, 0, destination, size);
}

int CdRom_ReadSectors(u32 lba, u32 offset, void *destination, u32 size) {
    Pe1GameState *state;
    CdlLOC loc;
    int ret;

    state = &g_GameState;

    if ((state->flags & 0x1000000) != 0) {
        return -1;
    }
    if (DsSystemStatus() != 1) {
        return -1;
    }
    if (DsQueueLen() != 0) {
        return -1;
    }
    if (DsShellOpen() != g_CdDiskType) {
        exit(1);
    }

    state->flags |= 0x1004000;
    lba += offset;
    DsIntToPos(lba, &loc);
    ret = DsRead(&loc, size, destination, 0x80);
    if (ret != 0) {
        return ret;
    }

    state->flags &= 0xFEFFBFFF;
    printf(g_CdReadErrorFormat, lba, size);
    return -1;
}
int CdRom_PollReady(void) {
    int scratch;
    int status;
    Pe1GameState *state;

    status = DsReadSync(&scratch);
    if ((unsigned int)(status + 1) < 2U) {
        state = &g_GameState;
        state->flags &= 0xFEFFBFFF;
    }
    return status;
}

s32 Overlay_LoadInitialImage(void) {
    s32 status;
    u16 *range;
    s32 result;
    DISPENV sp18;

    D_800B0DB5 = -1;
    D_800B0DB4 = -1;
    D_800B0DB7 = -1;
    D_800B0DB6 = -1;
    D_800B0DB3 = -1;
    D_800B0DB2 = -1;
    D_800B0CD8 &= 0xFFFFFF0F;
    Akao_Cmd_F0();

restart:
    range = g_StrFileDirLba;
    do {
        status = CdRom_ReadSectors(g_PeImageBaseLba + range[0], 0, g_StrFileDirBuffer, range[1] - range[0]);
    } while (status == -1);

    while ((status = CdRom_PollReady()) != 0) {
        if (status == -1) {
            goto restart;
        }
    }

    EnterCriticalSection();
    FlushCache();
    ExitCriticalSection();
    VSync(0);
    SetDispMask(0);
    SetDefDispEnv(&sp18, 0, 0, 0x140, 0xF0);
    sp18.isrgb24 = 1;
    PutDispEnv(&sp18);
    return 0;
}
