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
/* The same packets seen as the three shaded status-bar tiles of each draw
 * buffer (Battle_DrawHPBar). */
#define BATTLE_STATUS_TILES ((RenderColorTilePacket (*)[3])D_8009E358)
extern int g_ActiveDrawSlot;
extern BattleStatusMarkerBody D_8009E888[2];
extern RenderPagedSpritePacket D_8009E8B8[2][2];  /* ammo digits */
extern BattleGaugePrim D_8009E3B8[2][3];
extern BattleGaugePrim D_8009E460[2];
/* Status symbols use the same texture-page and sprite packet layout. */
extern BattleGaugePrim D_8009E730[2];
extern BattleGaugePrim D_8009E768[2];
extern RenderPagedSpritePacket D_8009E7A0[2][4];  /* ammo count digits */

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

extern u16 D_8009CE84;
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
/* Color a gauge quad: vertices 0 and 2 take the first color, 1 and 3 the
 * second (the PSY-Q setRGB0..setRGB3 sequence). A macro rather than an
 * inline function: the constants must stay shared with neighbouring stores. */
#define BATTLE_SET_GAUGE_GRADIENT(quad, r0_, g0_, b0_, r1_, g1_, b1_) \
    ((quad).r0 = (r0_), (quad).g0 = (g0_), (quad).b0 = (b0_),         \
     (quad).r1 = (r1_), (quad).g1 = (g1_), (quad).b1 = (b1_),         \
     (quad).r2 = (r0_), (quad).g2 = (g0_), (quad).b2 = (b0_),         \
     (quad).r3 = (r1_), (quad).g3 = (g1_), (quad).b3 = (b1_))

/* Color of a marker or label sprite (setRGB0). */
#define BATTLE_SET_SPRITE_COLOR(sprite, r_, g_, b_)                   \
    ((sprite).color.bytes.r = (r_), (sprite).color.bytes.g = (g_),    \
     (sprite).color.bytes.b = (b_))

/* HP digit packets: current HP has four per draw buffer, maximum HP five
 * (digits and slash). Battle_DrawEnemyHP addresses the sprites from the
 * sprite, each followed by the texture-page command of the next packet (the
 * BattleTileSlot view), so the sprite arrays keep their own symbols. */
typedef struct BattleDigitSlot {
    RenderSpritePacket sprite;
    RenderTexturePagePacket nextPage;
} BattleDigitSlot;
extern RenderPagedSpritePacket D_8009E0F0[2][4], D_8009E1D0[2][5];
extern BattleDigitSlot D_8009E0F8[2][4], D_8009E1D8[2][5];
/* HP label packets. */
extern RenderPagedSpritePacket D_8009E2E8[2];
/* Further HUD sprite packets Battle_DrawHPBar initializes. */
extern RenderPagedSpritePacket D_8009E500[2][10];
extern RenderPagedSpritePacket D_8009E880[2];
extern RenderPagedSpritePacket D_8009E928[2];
extern RenderPagedSpritePacket D_8009E960[2][13];  /* glyphs 0x6A..0x76 */
extern u8 D_8009E976[], D_8009EC38[];
/* The sprites of a paged-sprite array seen from the first sprite: each is
 * followed by the texture-page command of the next packet. The view keeps
 * the sprite base (array + 8) in its own register, as Battle_DrawHPBar's
 * loops do. */
#define BATTLE_SPRITE_SLOTS(packets, count) \
    ((BattleDigitSlot (*)[count])&(packets)[0][0].sprite)
/* Gray levels of the three status-bar tiles (Battle_DrawHPBar). */
typedef struct BattleHudShades {
    s8 values[3];
} BattleHudShades;
extern BattleHudShades D_8009CD90[]; /* one record; unsized keeps it off $gp */
/* Status-panel digit sprites: two draw buffers, ten panels of five digits. */
extern RenderPagedSpritePacket D_800B01C0[2][10][5];
/* End-of-battle panel quads, one per draw buffer. */
extern RenderTexturedQuad D_800BE9F0[2];
/* Corner coordinates of a quad (the PSY-Q setXY4 sequence). */
#define BATTLE_SET_QUAD_XY(quad, x0_, y0_, x1_, y1_, x2_, y2_, x3_, y3_)  \
    ((quad).x0 = (x0_), (quad).y0 = (y0_), (quad).x1 = (x1_),           \
     (quad).y1 = (y1_), (quad).x2 = (x2_), (quad).y2 = (y2_),           \
     (quad).x3 = (x3_), (quad).y3 = (y3_))
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
