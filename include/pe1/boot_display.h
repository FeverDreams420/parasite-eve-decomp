#ifndef PE1_BOOT_DISPLAY_H
#define PE1_BOOT_DISPLAY_H

#include "common.h"
#include "pe1/asset_tim.h"
#include "pe1/cdrom_buffers.h"
#include "pe1/display_env.h"
#include "pe1/gpu_image.h"
#include "pe1/psyq_callbacks.h"
#include "pe1/psyq_cd.h"
#include "pe1/psyq_ds.h"
#include "pe1/psyq_ds_queue.h"
#include "pe1/psyq_gpu.h"

/* Three byte channels in the boot screen image buffer. */
typedef struct BootRgb24Pixel {
    u8 red, green, blue;
} BootRgb24Pixel;

PE1_STATIC_ASSERT(sizeof(BootRgb24Pixel) == 3, boot_rgb24_pixel_size);

typedef struct {
    s32 tag;
    s32 code;
} BootDisplayDrawModePrim;

typedef struct {
    s32 tag;
    u8 r0;
    u8 g0;
    u8 b0;
    u8 code;
    u16 x0;
    u16 y0;
    u8 u0;
    u8 v0;
    u16 clut;
    u16 w;
    u16 h;
} BootDisplaySpritePrim;

typedef struct {
    BootDisplayDrawModePrim draw_mode;
    BootDisplaySpritePrim sprite;
} BootDisplaySpritePacket;

/* One 16-byte record per display buffer: GPU position, pixels, and decode target. */
typedef struct {
    u16 x, y;
    char *pixels;
    u16 transferX, transferY;
    char *decodeTarget;
} BootDisplayImageBuffer;
extern BootDisplayImageBuffer g_BootDisplayImageBuffers[2];

typedef struct {
    void (*init)(void);
    void (*step)(void);
    s32 (*text)(void);
    void (*glyph)(s16, s16, u8);
    void (*transition)(u8);
    void (*present)(void);
    void (*shutdown)(void);
} BootDisplayCallbacks;
extern BootDisplayCallbacks *g_BootDisplayCallbacks;
extern u16 g_BootDisplayFontStyle;
extern const u8 D_80125B1C[][4];
extern const u8 D_80125B1E[][4];
extern const u8 g_BootDisplayLowercaseWidths[][4];
extern const u8 *D_80125C88[];
extern const u8 **g_BootDisplayTextCursor;

extern char *volatile g_BootDisplayOrderingTable;
extern u16 g_BootDisplayTPage;
extern u16 g_BootDisplayClut;
extern char *volatile g_BootDisplayPrimitiveCursor;
extern s32 g_BootDisplayTextScrollX;
extern s32 g_BootDisplayTextScrollStepX;
extern s32 g_BootDisplayTransition;
extern s32 g_BootDisplayBufferIndex;
extern volatile s16 g_BootDisplayStateTimer;
extern s16 g_BootDisplayTransferProgress;
extern s32 g_BootDisplayState;
extern s16 *D_80172C90;
extern s16 *D_80172C94;
extern s32 D_80172C98;
extern s32 D_80172C9C;
extern u16 D_80093166;
extern u16 g_MainAssetLbaTbl[];
extern char D_8012682C[];
/* Two buffer records, each holding four image addresses. */
extern char *D_80125B88[8];
extern char *D_80125B90;
extern char *D_80125B98;
extern char *D_80125BA0;
extern volatile s32 D_80172CF0;
extern volatile s32 D_80172CEC;
extern s32 D_80172CE8;
extern s32 D_80172CE4;
extern u8 D_80125AF0[];
extern u16 g_CdDiskType;
extern volatile u16 D_80172CF8;
extern volatile u16 D_80172CFA;
extern char *D_80172D00;
extern volatile s32 D_80172CF4;
extern char *D_80125B48;
extern s32 g_ActiveDrawSlot;

void SetSemiTrans(void *primitive, s32 code);
void ResetGraph(s32 value);
s32 VSync(s32 value);
void Spu_SetVoiceVolume(s32 value);
int func_801258EC(int command, int parameter);
s32 DsSync(s32, void *);
s32 BootDisplay_StartPlayback(void);
void BootDisplay_CheckPlaybackTime(void);
s32 DsSystemStatus(void);
s32 DsShellOpen(void);
/* PsyQ exit: the failure path never returns to the transfer callback. */
void exit(s32) __attribute__((noreturn));
s32 BootDisplay_StepPlaybackTransfer(u8 mode);
void DsInit(void);
s32 DsQueueLen(void);
s32 BootDisplay_InitPlaybackFile(void);
void SetGraphDebug(s32 value);
void SetDispMask(s32 value);
void func_8010BE3C(s32 value);
void func_8010BD4C(void *address);
void BootDisplay_PresentFrame(void);
void func_80124F40(s32 value);

/* Callable entry points inside neighboring boot-display code. */
s32 BootDisplay_GetGlyphIndex(u8 character);
u16 BootDisplay_GetGlyphWidth(u8 character);
void func_8012462C(void *primitive, u16 x, u16 y, s32 u, s32 v);

#endif
