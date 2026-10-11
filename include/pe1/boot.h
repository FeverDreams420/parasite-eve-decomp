#ifndef PE1_BOOT_H
#define PE1_BOOT_H

#include "common.h"

void InitSystem(void);
void Boot_VsyncCallback(void);

/* Frame counter Boot_FrameLoop advances once per frame (an array view, so
 * -G8 units still address it absolutely). */
extern unsigned int D_8009CDA4[];

/* Boot sequence (Boot_MainLoop.c) and the services it starts. */
void Boot_InitSubsystems(void);
void Scene_LoadSceneData(void);
int Overlay_LoadTables(void);
int CD_LoadBootAudio(void);
int Overlay_LoadInitialImage(void);
int Gpu_InitDisplay(int mode);
void Boot_RunFrame(void);
/* Entry points of the boot overlays: the title display, the movie player
 * and the display-mode selection that returns the mode for Gpu_InitDisplay. */
void func_8019234C(void);
void func_801235DC(void);
int func_801909B4(void);

/* Disc table load (Boot_DiscTableLoad.c): PE.IMG sector ranges of the
 * startup assets (entries of the D_800930D8 sector table) and the save
 * work word whose bit 0x2000 selects the large texture set. */
extern unsigned int D_800A77FC;
extern u16 g_MainAssetLbaTbl[];
extern u16 g_LargeTexLbaTbl[];
extern u16 D_8009316C[];
extern u16 g_FontUiLbaTbl[];
extern u16 D_8009315E[];

/* Frame loop (Boot_FrameLoop.c): the scene flag bytes of g_GameState. */
extern u8 D_800B0CEA[];

/* System start-up (Boot_InitSystemFlow.c). */
extern s32 g_FrameRngCounter[];
/* Display and draw environment bytes and halfwords set for each buffer:
 * fields of g_RenderDispEnvArray (0x800BCE80) and g_RenderDrawEnvArray
 * (0x800BCDC8) written as their own symbols. */
extern u8 D_800BCE91;
extern u8 D_800BCEA5;
extern u16 D_800BCE9E;
extern u16 D_800BCE8A;
extern u16 D_800BCE9C;
extern u16 D_800BCE88;
extern u16 D_800BCEA2;
extern u16 D_800BCE8E;
extern u8 D_800BCE3C;
extern u8 D_800BCDE0;
extern u8 D_800BCE3A;
extern u8 D_800BCDDE;
extern u8 D_800BCE3B;
extern u8 D_800BCDDF;
extern u16 D_800BCE38;
extern u16 D_800BCDDC;
extern u8 D_800BCE3D;
extern u8 D_800BCDE1;
extern u8 D_800BCE3E;
extern u8 D_800BCDE2;
extern u8 D_800BCE3F;
extern u8 D_800BCDE3;
extern char g_AnalogStickState[];

/* Pad sampling (Boot_RenderFlagsAndPadInput.c): render flag table, the
 * button mask per game bit, frames each bit has been held, the actuator
 * buffers and the cheat sequence masks. */
extern s32 g_FieldRenderFlagTable[];
extern u32 D_800A76F0[];
extern u32 D_800A7770[];
extern u8 D_8009D1C0[];
extern u8 D_800921F8[];
extern u32 D_80092200[];

/* Render pipeline set-up (Gpu_InitPipeline.c): the task node sequence
 * counter and the projection distances of the field and battle views. */
extern u16 g_TaskNodeSeqCounter[];
extern char D_800B1628[];
extern char D_800B162C[];
extern char D_800B8A18[];

#endif
