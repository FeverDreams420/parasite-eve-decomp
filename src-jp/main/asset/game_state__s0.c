#include "common.h"
#include "pe1/akao.h"
#include "pe1/asset_find.h"
#include "pe1/asset_tim.h"
#include "pe1/boot_disc_check.h"
#include "pe1/boot_services.h"
#include "pe1/cdrom.h"
#include "pe1/cdrom_buffers.h"
#include "pe1/draw_buffers.h"
#include "pe1/font.h"
#include "pe1/game_state.h"
#include "pe1/pe_image.h"
#include "pe1/psyq_ds.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_animation_frame.h"
#include "pe1/render_buffers.h"
#include "pe1/scene_assets.h"
#include "pe1/script_queue.h"
extern RenderBufferPrefix D_800B08F8;

int DsPosToInt(CdlLOC *position);

int Asset_OpenPeImage(void)
{
    DslFILE file;
    int status;
    int disk_type;
    DslFILE *found;

    g_PeImageMountFlags = 0;
    status = DsSystemStatus();
    if (status != 1) return 1;
    if (DsQueueLen() != 0) return 1;
    disk_type = DsGetDiskType();
    if (disk_type == status) return 1;
    if (disk_type != 4) return -1;

    while (DsSystemStatus() != 1 || DsQueueLen() != 0)
        VSync(0);
    found = DsSearchFile(&file, g_PeDisc1MarkerPath);
    if (found != 0 && found != (DslFILE *)-1) {
        while (DsSystemStatus() != 1 || DsQueueLen() != 0)
            VSync(0);
        found = DsSearchFile(&file, g_PeImagePath);
        if (found != 0 && found != (DslFILE *)-1) {
            g_PeImageBaseLba = DsPosToInt(&file.pos);
            g_PeImageMountFlags = (u8)g_PeImageMountFlags | 1;
        }
    }
    while (DsSystemStatus() != 1 || DsQueueLen() != 0)
        VSync(0);
    found = DsSearchFile(&file, g_PeDisc2MarkerPath);
    if (found != 0) {
        asm volatile("" : : "r"(found));
        if (found != (DslFILE *)-1) {
            while (DsSystemStatus() != 1 || DsQueueLen() != 0)
                VSync(0);
            found = DsSearchFile(&file, g_PeImagePath);
            if (found != 0) {
                asm volatile("" : : "r"(found));
                if (found != (DslFILE *)-1) {
                    g_PeImageBaseLba = DsPosToInt(&file.pos);
                    g_PeImageMountFlags = (u8)g_PeImageMountFlags | 2;
                }
            }
        }
    }
    return (g_PeImageMountFlags == 0) ? -2 : 0;
}

/* Boot notice screen and disc check; mode 1 or 2 is the expected disc. */
int Render_InitDisplayLists(int mode)
{
    RECT rect;
    u8 result[8];
    short colors[8];
    int state;
    int done;
    int command;
    int wait;
    int status;

    state = 0;
    done = 0;
    command = -1;
    wait = 180;
    colors[0] = -1;
    VSync(0);
    SetDispMask(0);
    /* Recorded crutch debt: retail's CD retry loops restart through gotos,
     * not reproducible with structured loops in stock GCC 2.7.2 (loop.c
     * hoists the read constants out of the restart path). */
retryNotice:
    while (CdRom_ReadSectorsFromLba(g_GameState.pe_image_base_lba + g_PeImageSectorTable[0],
               g_GameState.loaded_scene_assets, g_PeImageSectorTable[1] - g_PeImageSectorTable[0]) == -1)
        ;
    for (;;) {
        status = CdRom_PollReady();
        if (status == 0)
            break;
        if (status == -1)
            goto retryNotice; /* Recorded debt: CD retry restart (goto). */
    }
retryFog:
    while (CdRom_ReadSectorsFromLba(g_GameState.pe_image_base_lba + g_PeImageSectorTable[1],
               g_GameState.scene_load_scratch, g_PeImageSectorTable[2] - g_PeImageSectorTable[1]) == -1)
        ;
    for (;;) {
        status = CdRom_PollReady();
        if (status == 0)
            break;
        if (status == -1)
            goto retryFog; /* Recorded debt: CD retry restart (goto). */
    }
    Boot_BuildRenderFlagTable();
    Render_SetupFogLayer(g_GameState.scene_load_scratch);
    rect.x = 0;
    rect.y = 0;
    rect.w = 320;
    rect.h = 448;
    ClearImage(&rect, 0, 0, 1);
    Asset_LoadTimImage(g_GameState.loaded_scene_assets);
    rect.x = 320;
    rect.y = 256;
    rect.w = 320;
    rect.h = 224;
    DrawSync(0);
    PutDispEnv(&g_RenderDispEnvArray[g_ActiveDrawSlot]);
    SetDispMask(1);
    while (!done) {
        ClearOTagR((u32 *)D_800B08F8.ordering[g_ActiveDrawSlot], 0x1000);
        switch (state) {
        case 0:
            if (DsSystemStatus() == 1 && DsQueueLen() == 0) {
                command = DsCommand(8, 0, 0, -1);
                state = 1;
            }
            break;
        case 1:
            status = DsSync(command, result);
            switch (status) {
            case 1:
                break;
            case 2:
                Render_SetupColorTable(mode == 1 ? 1 : 2, 0, colors);
                state = 2;
                break;
            case 5:
            case 6:
                state = 0;
                break;
            }
            break;
        case 2:
            if (DsStatus() & 0x10)
                state = 3;
            break;
        case 3:
            status = DsSystemStatus();
            switch (status) {
            case 2:
                break;
            case 1:
                Draw_ResetTextboxes();
                Render_SetupColorTable(mode == 1 ? 3 : 4, 0, colors);
                wait = 180;
                state = 5;
                break;
            case 3:
                wait = 180;
                state = 6;
                break;
            }
            break;
        case 5:
            if (wait != 0)
                wait--;
            else
                state = 4;
            break;
        case 4:
            status = Asset_OpenPeImage();
            switch (status) {
            case -1:
                Draw_ResetTextboxes();
                wait = 180;
                Render_SetupColorTable(5, 0, colors);
                state = 7;
                break;
            case -2:
                Draw_ResetTextboxes();
                wait = 180;
                Render_SetupColorTable(5, 0, colors);
                state = 8;
                break;
            case 0:
                if ((mode == 1 && (g_PeImageMountFlags & 1)) || (mode == 2 && (g_PeImageMountFlags & 2))) {
                    done = 1;
                } else {
                    wait = 180;
                    state = 9;
                }
                break;
            }
            break;
        case 6:
            if (wait != 0) {
                wait--;
                break;
            }
            Draw_ResetTextboxes();
            Render_SetupColorTable(mode == 1 ? 1 : 2, 0, colors);
            state = 2;
            break;
        case 7:
            if (wait != 0) {
                wait--;
                break;
            }
            Draw_ResetTextboxes();
            Render_SetupColorTable(mode == 1 ? 1 : 2, 0, colors);
            state = 2;
            break;
        case 8:
            if (wait != 0) {
                wait--;
                break;
            }
            Draw_ResetTextboxes();
            Render_SetupColorTable(mode == 1 ? 1 : 2, 0, colors);
            state = 2;
            break;
        case 9:
            if (wait != 0) {
                wait--;
                break;
            }
            Draw_ResetTextboxes();
            Render_SetupColorTable(mode == 1 ? 1 : 2, 0, colors);
            state = 2;
            break;
        }
        Menu_DrawTextboxEntries();
        DrawSync(0);
        VSync(0);
        ResetGraph(1);
        PutDispEnv(&g_RenderDispEnvArray[g_ActiveDrawSlot]);
        PutDrawEnv(&g_RenderDrawEnvArray[g_ActiveDrawSlot]);
        MoveImage(&rect, 0, g_ActiveDrawSlot ? 224 : 0);
        DrawOTag((u32 *)D_800B08F8.ordering[g_ActiveDrawSlot] + 0xFFF);
        g_ActiveDrawSlot ^= 1;
    }
    VSync(0);
    SetDispMask(0);
    rect.x = 0;
    rect.y = 0;
    rect.w = 320;
    rect.h = 448;
    ClearImage(&rect, 0, 0, 1);
    DrawSync(0);
    g_PeImageMountFlags = mode == 1 ? 1 : 2;
    /* `wait` is free again here and holds the drive status. */
    while ((wait = DsSystemStatus()) != 1)
        VSync(0);
    g_CdDiskType = DsShellOpen();
    return 0;
}

typedef struct AkaoVoiceActor {
    u8 pad_00[0x0C];
    u8 animGroup;
    u8 animSet;
    u8 animIndex;
    u8 frameSkip;
    u8 pad_10[0x06];
    u16 frame;
    u8 pad_18[0x02];
    u16 targetFrame;
    int frameStep;
    u8 pad_20[0x0A];
    s16 posXInteger; /* integer halves of the 16.16 position */
    u8 pad_2c[0x02];
    s16 posYInteger;
    u8 pad_30[0x02];
    s16 posZInteger;
} AkaoVoiceActor;

typedef struct AkaoVoiceBankState {
    int flags;
    u8 pad_04[0x0D];
    u8 first_dynamic_entry;
    u8 active_id_slot;
} AkaoVoiceBankState;