#ifndef PE1_BOOT_SERVICES_H
#define PE1_BOOT_SERVICES_H

#include "common.h"

/* Error reporting (draw/error.c): prints the severity name and the two
 * messages; severity 1 halts. */
extern char D_80010EB0[]; /* severity/message format */
extern char D_80010EC8[]; /* detail format */
extern char *D_80091A2C[]; /* severity names */
void Draw_ReportError(char *message, char *detail, int value, unsigned char severity);

/* Soft-reset and start-up services (asset/game_state.c). */
extern int g_SceneDispatchToken;
extern int D_800A77F4; /* dispatch token saved by Boot_Shutdown */
void Boot_SyncShutdown(void);
int Draw_SetBaseY(int unused, u32 y);
void Boot_InitMemoryLayout(void);
void Boot_InitGameState(void);
/* Resets g_GameState and the scene voice-bank entries to start-up values. */
void Asset_ResetGameState(void);
/* End-of-frame pass: draws the fade primitives and sends queued AKAO commands. */
void Boot_FinishFrame(void);

/* Work areas carved up by Boot_InitMemoryLayout. g_BootWorkArea follows the
 * field engine and moves with it; the other three are buffers inside the
 * overlay window block, at fixed addresses (configs/USA/main.yaml). */
extern u8 g_BootWorkArea[];
extern u8 D_8010BD00[];
extern u8 D_80120D08[];
extern u8 D_801ED800[];

/* Absolute views of the g_GameState buffer-pointer fields that
 * Boot_InitMemoryLayout writes (0x14C..0x190). */
extern u8 *volatile D_800B0E24;
extern u8 *volatile D_800B0E28;
extern u8 *volatile D_800B0E2C;
extern u8 *volatile D_800B0E30;
extern u8 *volatile D_800B0E34;
extern u8 *volatile D_800B0E40;
extern u8 *volatile D_800B0E44;
extern u8 *volatile D_800B0E48;
extern u8 *volatile D_800B0E5C;
extern u8 *volatile D_800B0E60;
extern u8 *volatile D_800B0E68;

#endif /* PE1_BOOT_SERVICES_H */
