/* Boot-time system initialization: InitSystem, the subsystem and global
 * reset it is followed by, the start-up display reset, the vsync callback and
 * the pad start-up (G0341/G0342). */
#include "pe1/psyq_cd.h"
#include "pe1/psyq_spu_api.h"
#include "pe1/psyq_callbacks.h"
#include "pe1/psyq_gpu.h"
#include "common.h"
#include "pe1/display_env.h"
#include "../../../tools/m2c/m2c_macros.h"
#include "pe1/font.h"
#include "pe1/field_sfx.h"
#include "pe1/scene_dispatch.h"
#include "pe1/battle_status.h"
#include "pe1/battle_runtime.h"
#include "pe1/boot.h"
#include "pe1/draw_buffers.h"

void Render_ResetScene(int arg0, int arg1);
void InitGeom(void);
void SetGeomOffset(int x, int y);
void SetGeomScreen(int h);
void MemCard_InitManager(void);
void Boot_InitMemCard(void);
void DsInit(void);

void InitSystem(void) {
    ResetCallback();
    Render_ResetScene(0x140, 0xE0);
    SpuInit();
    InitGeom();
    SetGeomOffset(0xA0, 0x70);
    SetGeomScreen(0xF0);
    MemCard_InitManager();
    Boot_InitMemCard();
    DsInit();
    DsSetDebug(0);
}

#define NULL ((void *)0)
u32 Task_GpuFlushPrimQueue(void);
M2C_UNK Save_InitSystem();
M2C_UNK Task_InitNodePool();
M2C_UNK Entity_ResetStateGlobals();
M2C_UNK Entity_ResetAllPools();
M2C_UNK SsInit();
void Menu_SetEquipSlotIndex(int index);
void Boot_BuildRenderFlagTable(void);
M2C_UNK Save_PostInitStub();
M2C_UNK CdRom_InitScreenState();
M2C_UNK Task_InitGpuHwRegs();
#define g_FrameRngCounter (g_FrameRngCounter[0])
void Boot_VsyncCallback(void);

void Boot_InitSubsystems(void) {
    u32 var_s0;

    g_SceneDispatchCur = 0;
    g_SceneDispatchToken = 0;
    g_GameStateFlags = 0;
    g_FrameRngCounter = 0;
    g_ActiveDrawSlot = 0;
    Task_InitGpuHwRegs();
    for (var_s0 = 0; var_s0 < 0x7D0U; var_s0++) {
        Task_GpuFlushPrimQueue();
    }
    Boot_BuildRenderFlagTable();
    SsInit();
    VSyncCallback(NULL);
    VSyncCallback(&Boot_VsyncCallback);
    Menu_SetEquipSlotIndex(0);
    Save_InitSystem();
    Save_PostInitStub();
    CdRom_InitScreenState();
    Task_InitNodePool();
    Entity_ResetStateGlobals();
    Entity_ResetAllPools();
    Task_ClearSfxTable();
    Menu_ConsumeEquipSlotFlag();
}



void ResetGraph(int arg0);
void SetGraphDebug(int level);
void SetDispMask(int mask);
void ClearImage(RECT *rect, int r, int g, int b);
DISPENV *SetDefDispEnv(DISPENV *env, int x, int y, int w, int h);
DRAWENV *SetDefDrawEnv(DRAWENV *env, int x, int y, int w, int h);

void Render_ResetScene(int width, int height)
{
    s16 rect[4];

    ResetGraph(0);
    SetGraphDebug(0);
    SetDispMask(0);

    rect[2] = 0x3FF;
    rect[3] = 0x200;
    rect[0] = 0;
    rect[1] = 0;
    ClearImage(rect, 0, 0, 1);

    g_RenderDispEnvArray[0].isrgb24 = 1;
    g_RenderDispEnvArray[1].isrgb24 = 1;
    SetDefDispEnv(&g_RenderDispEnvArray[0], 0, height, width, height);
    SetDefDispEnv(&g_RenderDispEnvArray[1], 0, 0, width, height);

    g_RenderDispEnvArray[1].screen.y = 8;
    g_RenderDispEnvArray[0].screen.y = 8;
    g_RenderDispEnvArray[1].screen.x = 0;
    g_RenderDispEnvArray[0].screen.x = 0;
    g_RenderDispEnvArray[0].screen.h = g_RenderDispEnvArray[1].screen.h = height;
    SetDefDrawEnv(&g_RenderDrawEnvArray[0], 0, 0, width, height);
    SetDefDrawEnv(&g_RenderDrawEnvArray[1], 0, height, width, height);

    g_RenderDrawEnvArray[1].isbg = 1;
    g_RenderDrawEnvArray[0].isbg = 1;
    g_RenderDrawEnvArray[1].dtd = 1;
    g_RenderDrawEnvArray[0].dtd = 1;
    g_RenderDrawEnvArray[1].dfe = 0;
    g_RenderDrawEnvArray[0].dfe = 0;
    g_RenderDrawEnvArray[1].tpage = 0;
    g_RenderDrawEnvArray[0].tpage = 0;
    g_RenderDrawEnvArray[1].r0 = 0;
    g_RenderDrawEnvArray[0].r0 = 0;
    g_RenderDrawEnvArray[1].g0 = 0;
    g_RenderDrawEnvArray[0].g0 = 0;
    g_RenderDrawEnvArray[1].b0 = 0;
    g_RenderDrawEnvArray[0].b0 = 0;
    g_ActiveDrawSlot = 0;

    PutDispEnv(&g_RenderDispEnvArray[0]);
}

void Scene_TickTimers(void);


void PadInitDirect(char *arg0, char *arg1);
void PadStartCom(void);

void Boot_VsyncCallback(void) {
    Task_GpuFlushPrimQueue();
    Scene_TickTimers();
}

void Boot_InitMemCard(void) {
    PadInitDirect(g_AnalogStickState, g_AnalogStickState + 0x22);
    PadStartCom();
}
