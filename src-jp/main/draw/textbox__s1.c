/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/boot_disc_check.h"
#include "pe1/field_pad.h"
#include "pe1/menu_flags.h"
#include "pe1/psyq_bios.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_prim.h"
#include "pe1/textbox.h"
#include "pe1/textbox_open.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_ScriptCollisionFlag;
u8 *g_TextboxMessageSource;
unsigned char g_TextboxKernGlyph0F;
TextboxRect g_TextboxPendingRect;
u8 g_DialogueChoiceCursor;
u8 g_PlayerMovementEnabled;

/* A draw-mode word pair followed by the sprite primitive it prefixes. */
typedef struct GpuModeSprtPacket {
    u32 tag;
    u32 mode;
    u8 sprt[1];
} GpuModeSprtPacket;

/* A draw-mode word pair followed by the tile primitive it prefixes. */
typedef struct GpuModeTilePacket {
    u32 tag;
    u32 mode;
    u8 tile[1];
} GpuModeTilePacket;

void SetDrawTPage(void *, int, int, int);
void SetSprt(void *);
int MargePrim(void *, void *);
void SetShadeTex(void *, int);
void SetSemiTrans(void *, int);
void exit(int);

/* Tentative small-data definition: stock maspsx must recognize this store
 * as GP-relative to retain its load-delay nop. The linker pins the COMMON
 * symbol to the retail address, as for other unplaced game globals. */
signed char g_MenuEquipSlotIndex;
u8 g_TextboxGlyphColorR;
u8 g_TextboxGlyphColorG;
u8 g_TextboxGlyphColorB;

/* Stored only by Draw_SetLightingParams, which nothing calls; no reader. */
unsigned char g_DrawLightingParamsSetWriteOnly;
short g_DrawLightingParam0WriteOnly;
short g_DrawLightingParam1WriteOnly;
short g_DrawLightingParam2WriteOnly;
unsigned char g_TextboxHighlightColorR;
unsigned char g_TextboxHighlightColorG;
unsigned char g_TextboxHighlightColorB;

unsigned char g_TextboxHighlightColorR;
unsigned char g_TextboxHighlightColorG;
unsigned char g_TextboxHighlightColorB;

void Draw_SetLightingParams(short arg0, short arg1, short arg2, unsigned char arg3, unsigned char arg4, unsigned char arg5) {
    g_DrawLightingParam0WriteOnly = arg0;
    g_DrawLightingParamsSetWriteOnly = 1;
    g_DrawLightingParam1WriteOnly = arg1;
    g_DrawLightingParam2WriteOnly = arg2;
    g_TextboxHighlightColorR = arg3;
    g_TextboxHighlightColorG = arg4;
    g_TextboxHighlightColorB = arg5;
}

void Menu_SetHighlightColor(int arg0, int arg1, int arg2, int arg3) {
    g_TextboxHighlightColorR = arg1;
    g_TextboxHighlightColorG = arg2;
    g_TextboxHighlightColorB = arg3;
}