/* Boot-time disc loads called from Boot_MainLoop: the PE.IMG asset LBA
 * tables and the boot audio banks. */
#include "pe1/disc_load_state.h"
#include "common.h"
#include "pe1/cdrom.h"
#include "pe1/cd_poll.h"
#include "pe1/cdrom_buffers.h"
#include "pe1/asset_tim.h"
#include "pe1/scene_assets.h"
#include "include_asm.h"
#include "pe1/psyq_bios.h"
#include "pe1/psyq_gpu.h"
#include "pe1/boot_display.h"
#include "pe1/render_buffers.h"
#include "pe1/pe_image.h"
#include "pe1/boot.h"
#include "pe1/akao/commands.h"

void SetDispMask(int arg0);
void FlushCache(void);


void Akao_Cmd_F0(void);

int Overlay_LoadTables(void) {
    int status;
    int imageBase;
    Pe1GameState *state;
    u16 *range;
    TimRelativeTable *main_table;
    TimRelativeTable *large_table;
    int i;

    imageBase = g_PeImageBaseLba;

    VSync(0);
    SetDispMask(0);
    state = &g_GameState;
    CD_ReadSectors(1, 0xCC, 0, g_SceneLoadScratchBuffer[0], 0x21, 1);

retry_first_load:
    range = g_MainAssetLbaTbl;
    do {
        status = CdRom_ReadSectors(imageBase + range[0], 0, (int)state->scene_object_work, range[1] - range[0]);
    } while (status == -1);
    while ((status = CdRom_PollReady()) != 0) {
        if (status == -1) {
            goto retry_first_load;
        }
    }

    main_table = (TimRelativeTable *)state->scene_object_work;
    for (i = 0; i < 3; i++) {
        Asset_LoadTimImage(SceneAsset_ResolveOffset(main_table, main_table->offsets[(s16)i]));
    }

    if (D_800A77FC & 0x2000) {
retry_large_load_a:
        range = g_LargeTexLbaTbl;
        do {
            status = CdRom_ReadSectors(imageBase + range[0], 0, (int)state->object_texture_blob, range[1] - range[0]);
        } while (status == -1);
        while ((status = CdRom_PollReady()) != 0) {
            if (status == -1) {
                goto retry_large_load_a;
            }
        }
    } else {
retry_large_load_b:
        range = D_8009316C;
        do {
            status = CdRom_ReadSectors(imageBase + range[0], 0, (int)state->object_texture_blob, range[1] - range[0]);
        } while (status == -1);
        while ((status = CdRom_PollReady()) != 0) {
            if (status == -1) {
                goto retry_large_load_b;
            }
        }
    }

    large_table = (TimRelativeTable *)state->object_texture_blob;
    for (i = 0; i < 0x106; i++) {
        Asset_LoadTimImage(SceneAsset_ResolveOffset(large_table, large_table->offsets[(s16)i]));
    }

    DrawSync(0);

retry_final_load:
    range = g_FontUiLbaTbl;
    do {
        status = CdRom_ReadSectors(g_PeImageBaseLba + range[0], 0, g_StrFileDirBuffer, range[1] - range[0]);
    } while (status == -1);
    while ((status = CdRom_PollReady()) != 0) {
        if (status == -1) {
            goto retry_final_load;
        }
    }

    EnterCriticalSection();
    FlushCache();
    ExitCriticalSection();
    return 0;
}

int CD_LoadBootAudio(void) {
    int status;
    u16 *range;
    int retry;

    D_800B0DB5 = -1;
    D_800B0DB4 = -1;
    D_800B0DB7 = -1;
    D_800B0DB6 = -1;
    D_800B0DB3 = -1;
    D_800B0DB2 = -1;
    D_800B0CD8 &= -0xF1;
    Akao_Cmd_F0();

retry_first:
    range = D_8009315E;
    retry = -1;
    do {
        status = CdRom_ReadSectors(D_800B0DD8.baseLba + range[0], 0, D_8001160C, range[1] - range[0]);
    } while (status == retry);

    while ((status = CdRom_PollReady()) != 0) {
        if (status == -1) {
            goto retry_first;
        }
    }

    EnterCriticalSection();
    FlushCache();
    ExitCriticalSection();

retry_second:
    range = &D_80093166;
    retry = -1;
    do {
        status = CdRom_ReadSectors(D_800B0DD8.baseLba + range[0], 0, D_80011610, range[1] - range[0]);
    } while (status == retry);

    while ((status = CdRom_PollReady()) != 0) {
        if (status == -1) {
            goto retry_second;
        }
    }

    EnterCriticalSection();
    FlushCache();
    ExitCriticalSection();
    return 0;
}
