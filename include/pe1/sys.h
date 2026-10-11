#ifndef PE1_SYS_H
#define PE1_SYS_H

#include "common.h"

/* Error reporting (Sys_ReportError.c): prints the severity name and the two
 * messages; severity 1 halts. */
extern char D_80010EB0[]; /* severity/message format */
extern char D_80010EC8[]; /* detail format */
extern char *D_80091A2C[]; /* severity names */
void Sys_ReportError(char *message, char *detail, int value, unsigned char severity);

/* Soft-reset and start-up services (Sys_GameStateAndVoiceBanks.c). */
extern int g_SceneDispatchToken;
extern int D_800A77F4; /* dispatch token saved by Sys_Shutdown */
void Sys_SyncShutdown(void);
int Draw_SetBaseY(int unused, u32 y);
void Boot_InitMemoryLayout(void);
void Boot_InitGameState(void);

/* Fixed work areas carved up by Boot_InitMemoryLayout. */
extern u8 D_800F34F8[];
extern u8 D_8010BD00[];
extern u8 D_80120D08[];
extern u8 D_801ED800[];

/* Absolute views of the g_GameState buffer-pointer fields that
 * Boot_InitMemoryLayout writes (0x14C..0x190). Matching debt: volatile keeps
 * the retail store order against the render buffer tables. */
extern volatile s32 D_800B0E24;
extern volatile s32 D_800B0E28;
extern volatile s32 D_800B0E2C;
extern volatile s32 D_800B0E30;
extern volatile s32 D_800B0E34;
extern volatile s32 D_800B0E40;
extern volatile s32 D_800B0E44;
extern volatile s32 D_800B0E48;
extern volatile s32 D_800B0E5C;
extern volatile s32 D_800B0E60;
extern volatile s32 D_800B0E68;

#endif /* PE1_SYS_H */
