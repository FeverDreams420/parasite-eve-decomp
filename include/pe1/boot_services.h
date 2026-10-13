#ifndef PE1_BOOT_SERVICES_H
#define PE1_BOOT_SERVICES_H

#include "common.h"

/* Error reporting (draw/error.c): prints the severity name and the two
 * messages; severity 1 halts. */
extern char g_ErrorMessageFormat[]; /* severity/message format */
extern char g_ErrorDetailFormat[]; /* detail format */
extern char *g_ErrorSeverityNames[]; /* severity names */
void Draw_ReportError(char *message, char *detail, int value, unsigned char severity);

/* Soft-reset and start-up services (asset/game_state.c). */
extern int g_SceneDispatchToken;
/* Scene return state: the room word (Boot_Shutdown saves the dispatch token
 * there, the effect sequences set 999 and pick their room by it), the
 * flags word and the area word. */
typedef struct SceneReturnIds {
    s32 room;          /* 0x000 */
    s32 pad04;
    u32 flags;         /* 0x008 */
    u8 pad0C[0x118];
    s32 area;          /* 0x124 */
} SceneReturnIds;
extern SceneReturnIds D_800A77F4;
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
extern u8 g_SceneLoadScratchArea[];

/* Absolute views of the g_GameState buffer-pointer fields that
 * Boot_InitMemoryLayout writes (0x14C..0x190). */
extern u8 *volatile g_SceneBankAssetSource;
extern u8 *volatile g_VoiceBankBase;
extern u8 *volatile D_800B0E2C;
extern u8 *volatile g_EntityTextureBlob;
extern u8 *volatile g_SceneObjectWork;
extern u8 *volatile D_800B0E40;
extern u8 *volatile D_800B0E44;
extern u8 *volatile D_800B0E48;
extern u8 *volatile D_800B0E5C;
extern u8 *volatile g_ScenePmSlotBanks;
extern u8 *volatile D_800B0E68;

#endif /* PE1_BOOT_SERVICES_H */
