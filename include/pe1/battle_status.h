#ifndef PE1_BATTLE_STATUS_H
#define PE1_BATTLE_STATUS_H

#include "pe1/battle.h"
#include "pe1/render_prim.h"
#include "pe1/battle_hud_colors.h"

typedef struct BattleStatusLinePrim {
    u8 pad00[8];
    u16 x0;
    u16 y0;
    u16 x1;
    u16 y1;
} BattleStatusLinePrim;

typedef struct BattleStatusMarkerBody {
    u8 pad00[8];
    u16 x;
    u16 y;
    u8 pad0C[0x10];
} BattleStatusMarkerBody;

/* Texture-page command followed by a variable-size UI sprite. */
typedef RenderPagedSpritePacket BattleGaugePrim;

extern BattleStatusLinePrim D_8009E358[6];
extern int g_ActiveDrawSlot;
extern BattleStatusMarkerBody D_8009E888[2];
extern u8 D_8009E8B8[2][0x38];
extern BattleGaugePrim D_8009E3B8[2][3];
extern BattleGaugePrim D_8009E460[2];
/* Status symbols use the same texture-page and sprite packet layout. */
extern BattleGaugePrim D_8009E730[2];
extern BattleGaugePrim D_8009E768[2];
extern u8 D_8009E7A0[2][0x70];

void AddPrim(unsigned int *orderingEntry, unsigned int *primitive);
void Battle_LayoutStatusPrimRow(int bottomY);
void Battle_DrawTargetHighlight(void);
void Battle_DrawActiveStatus(void);
void Battle_DrawEnemyHP(s16 maximum, s16 current);
void Gpu_DrawStatusIcons(void);
int Menu_GetItemContextFlag(void);

/* Active menu window origin (Menu_ActiveWindowMode): x at D_8009CE84 and y
 * in the next halfword. The HUD reads it as one record, so the stores to the
 * packets make GCC reload it. */
typedef struct BattleHudOrigin {
    u16 x;
    u16 y;
} BattleHudOrigin;

extern volatile u16 D_8009CE84;
#define g_HudOrigin (*(BattleHudOrigin *)&D_8009CE84)

/* Per draw buffer HUD packets (Battle_DrawHPBar fills them once). */
typedef struct BattleModeTilePacket {
    RenderDrawModePacket mode;
    RenderColorTilePacket tile;
} BattleModeTilePacket;

PE1_STATIC_ASSERT(sizeof(BattleModeTilePacket) == 0x18,
                  battle_mode_tile_packet_size);

extern BattleModeTilePacket D_8009E068[2];      /* status background */
/* The background tiles seen from the tile: each is followed by the mode
 * words of the next packet. Battle_DrawActiveStatus addresses them this way,
 * keeping the tile base in a register. */
typedef struct BattleTileSlot {
    RenderColorTilePacket tile;
    RenderDrawModePacket nextMode;
} BattleTileSlot;
extern BattleTileSlot D_8009E070[2];
extern RenderColorTilePacket D_8009E098[2];     /* ATB gauge track */
extern RenderPagedSpritePacket D_8009E0B8[2];   /* ATB label */
extern RenderPagedSpritePacket D_8009E320[2];   /* PE label */
extern RenderGouraudQuad D_800B00E8[2];         /* ATB gauge fill */
extern RenderGouraudQuad D_800B0130[2][2];      /* PE gauge fill, remainder */
extern RenderPagedSpritePacket D_800B6920[2];   /* ATB gauge marker */
/* HP digit packets: interior symbols retain independent relocations. */
extern u8 D_8009E0F0[], D_8009E0F8[], D_8009E1D0[], D_8009E1D8[];
extern u8 D_8009E2F0[];
/* End-of-battle panel; color bytes also have independent retail symbols. */
extern RenderTexturedQuad D_800BE9F0[2];
extern u8 D_800BE9F4[], D_800BE9F5[], D_800BE9F6[];
void Battle_DrawStatusValue(int value, int yOffset);
/* Returns the highest digit index (number of rendered digits minus one). */
int Battle_DrawDecimalNumber(void *buffer, s16 x, s16 y, s16 value, s16 mode);

PE1_STATIC_ASSERT(sizeof(BattleStatusLinePrim) == 0x10,
                  battle_status_line_prim_size);
PE1_STATIC_ASSERT(sizeof(BattleStatusMarkerBody) == 0x1C,
                  battle_status_marker_body_stride);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleGaugePrim, sprite) == 8,
                  battle_gauge_sprite_offset);
PE1_STATIC_ASSERT(sizeof(BattleGaugePrim) == 0x1C,
                  battle_gauge_prim_stride);

extern s32 D_8009D230;
extern RenderSpritePacket D_800B01C8[];
/* Interior byte symbols of the first panel sprite, independently relocated. */
extern u8 D_800B01CC[], D_800B01CD[], D_800B01CE[];
extern u8 D_800B01D4[], D_800B01D5[];
void Battle_DrawStatusPanel(int mode, BattleStatusPanel *panel);


/* Target pointer drawn by Battle_BuildStatusPrimHeader: a flat-shaded
 * triangle (POLY_F3 layout) plus two flat lines per draw slot that join the
 * pointer to the status panel. */
typedef struct BattleStatusPointerPrim {
    u32 tag;
    u8 r, g, b, code;
    s16 x0, y0, x1, y1, x2, y2;
} BattleStatusPointerPrim;

extern RenderLinePacket D_8009E498[2][2];
extern BattleStatusPointerPrim D_8009E4D8[2];
/* Frame counter; drives the pointer's rotation. */
extern u32 D_8009D250;

void RotTrans(const GteShortVector *v, GteVector *out, s32 *flag);

PE1_STATIC_ASSERT(sizeof(BattleStatusPointerPrim) == 0x14,
                  battle_status_pointer_prim_stride);

#endif
