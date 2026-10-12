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
int Boot_LoadOverlayTables(void);
int CdRom_LoadBootAudio(void);
int CdRom_LoadInitialImage(void);
int Draw_InitDisplay(int mode);
void Boot_RunFrame(void);
/* Entry points of the boot overlays: the title display, the movie player
 * and the display-mode selection that returns the mode for Draw_InitDisplay. */
void func_8019234C(void);
void func_801235DC(void);
int func_801909B4(void);

/* Disc table load (Boot_DisplayInitAndDiscTables.c): PE.IMG sector ranges of the
 * startup assets (entries of the D_800930D8 sector table) and the save
 * work word whose bit 0x2000 selects the large texture set. */
extern unsigned int D_800A77FC;
extern u16 g_MainAssetLbaTbl[];
extern u16 g_LargeTexLbaTbl[];
extern u16 D_8009316C[];
extern u16 g_FontUiLbaTbl[];
extern u16 D_8009315E[];

/* System start-up (Boot_InitSystemFlow.c). */
extern s32 g_FrameRngCounter[];
extern char g_AnalogStickState[];

/* Pad sampling (Boot_InitSystemFlow.c): render flag table, the
 * button mask per game bit, frames each bit has been held, the actuator
 * buffers and the cheat sequence masks. */
extern s32 g_FieldRenderFlagTable[];
extern u32 D_800A76F0[];
extern u32 D_800A7770[];
extern u8 D_8009D1C0[];
extern u8 D_800921F8[];
extern u32 D_80092200[];

/* Render pipeline set-up (Draw_InitPipeline.c): the projection distances
 * of the field and battle views. */
extern char D_800B1628[];
extern char D_800B162C[];
extern char D_800B8A18[];

void Draw_RenderFrame(void);
int Draw_CheckDrawStatus(void);

#endif
