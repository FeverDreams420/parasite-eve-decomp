#include "common.h"
#include "pe1/akao/commands.h"
#include "pe1/asset_tim.h"
#include "pe1/battle_status.h"
#include "pe1/boot.h"
#include "pe1/boot_disc_check.h"
#include "pe1/boot_display.h"
#include "pe1/boot_services.h"
#include "pe1/cd_poll.h"
#include "pe1/cdrom.h"
#include "pe1/cdrom_buffers.h"
#include "pe1/disc_load_state.h"
#include "pe1/draw_buffers.h"
#include "pe1/draw_state.h"
#include "pe1/font.h"
#include "pe1/game_state.h"
#include "pe1/geom_state.h"
#include "pe1/menu_queue.h"
#include "pe1/pe_image.h"
#include "pe1/psyq_bios.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_buffers.h"
#include "pe1/render_camera.h"
#include "pe1/render_tint.h"
#include "pe1/save_tail_state.h"
#include "pe1/scene_assets.h"
#include "pe1/scene_dispatch.h"
#include "pe1/script_queue.h"
#include "include_asm.h"
extern u8 D_801207C8[];
extern u8 D_8010B7C0[];


/* Boot-time disc loads called from boot/main.c: the PE.IMG asset LBA
 * tables and the boot audio banks. */

void SetDispMask(int arg0);


int Draw_InitDisplay(int mode) {
    int mode_reg;
    int one;
    register u8 **slots asm("$18");
    register u8 *ptr asm("$2");
    u8 *ptr2;
    register int const_8000 asm("$4");
    int const_48000;
    RECT rect;
    u8 *tmp;

    mode_reg = mode;
    VSync(0);
    SetDispMask(0);
    PutDispEnv(g_RenderDispEnvArray);

    rect.x = 0;
    rect.y = 0;
    rect.w = 0x140;
    rect.h = 0x1C0;
    one = 1;
    ClearImage(&rect, 0, 0, one);
    DrawSync(0);

    const_48000 = 0x48000;
    slots = g_OtBufferTable;
    asm volatile("" : "=r"(slots) : "0"(slots));

    ptr = g_BootWorkArea;
    ptr2 = ptr + 0x1800;
    g_SceneBankAssetSource = ptr;
    ptr += 0x6000;
    g_VoiceBankBase = ptr2;
    g_EntityBankBuffer = ptr;
    ptr += 0xE000;
    g_EntityTextureBlob = ptr;

    ptr = D_8010B7C0;
    ptr2 = D_801207C8;
    g_GameState.texture_load_scratch = (union SceneAssetView *)ptr;
    ptr = ptr2 + 0x1C98;
    g_SceneObjectWork = ptr2;
    ptr2 += 0x5C98;
    const_8000 = 0x8000;
    g_OtBufferTable[0] = ptr;
    ptr += const_8000;
    g_RenderOtBufferBaseAlt[0] = ptr2;
    ptr2 = ptr + 0x1C00;
    g_GameState.object_texture_blob = (struct SceneAssetBlob *)ptr;
    ptr += 0x3800;
    g_RenderScratchBufferBase[0] = ptr;
    ptr += const_48000;
    g_ObjectTextureBlobMidWriteOnly = ptr2;
    ptr2 = ptr + 0x4000;
    g_RenderFrontBufferBase[0] = ptr;
    ptr += const_8000;
    g_RenderBackBufferBase[0] = ptr2;
    ptr2 = ptr + 0x3800;
    g_ScriptSpriteBuffer = ptr2;
    tmp = g_StrFileDirBuffer;
    g_RenderAnimBufferBase[0] = ptr;
    ptr += 0x7000;
    g_ScenePmSlotBanks = ptr;
    g_SceneLoadScratchBuffer[0] = g_SceneLoadScratchArea;
    g_LoadedSceneAssetBlock[0] = tmp - 8;
    g_StrFileDirectory = tmp;

    Draw_InitBuffers();
    CdRom_SetSeekPos(2);

    do {
        ClearOTagR(slots[g_ActiveDrawSlot], 0x1000);
        Render_SetCDDCSlot();
        Draw_RenderFrame();
    } while ((g_ScreenTint.fadeMode & 3) != one);

    Menu_ConsumeEquipSlotFlag();

    if (mode_reg == 1) {
        g_SceneDispatchToken = 0xA80830C8;
    } else if (mode_reg == 3) {
        g_SceneDispatchToken = 0xA80651C8;
    }
    return 0;
}