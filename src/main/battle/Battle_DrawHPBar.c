/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "common.h"
#include "pe1/render_prim.h"
#include "pe1/draw_state.h"
#include "pe1/battle_status.h"
void Gpu_InitDrawModeSprtPacket(void *, int);
void Gpu_InitDrawModeTilePacket(void *, int);
void SetSemiTrans(void *, int);
void SetShadeTex(void *, int);
void SetPolyF3(void *);
void SetPolyG4(void *);
void SetPolyFT4(RenderTexturedQuad *);
void SetTile(void *);
u16 GetClut(int, int);


/* Initialize the HUD packet pools for both draw buffers. The legacy symbol
 * name is retained; this routine sets up more than the HP bar.
 * Loops that write a sprite after initializing its packet address the sprite
 * through BATTLE_SPRITE_SLOTS, which keeps the sprite base in its own
 * register as the retail code does.
 */
void Battle_DrawHPBar(void) {
    BattleHudShades shades;
    s32 i;
    s32 j;
    u16 pageCode;
    u32 tileIndex;
    s32 highBlue;
    u8 glyphHeight;
    u8 markerV;
    u8 bufferIndex;
    u16 hudClut;
    RenderLinePacket *upperLine;
    RenderLinePacket *lowerLine;
    RenderTexturedQuad *labelQuad;
    s32 hudPage;
    u8 shade;
    RenderSpritePacket *hpLabel;
    RenderSpritePacket *pointerSprite;
    RenderSpritePacket *ammoIcon;
    RenderSpritePacket *statusIcon;
    RenderSpritePacket *ammoLabel;
    RenderSpritePacket *weaponIcon;
    u8 gridRow;
    RenderSpritePacket *cursorSprite;
    RenderColorTilePacket *background;
    RenderGouraudQuad *atbGradient;
    RenderColorTilePacket *atbTrack;
    RenderSpritePacket *atbMarker;
    RenderGouraudQuad *greenGradient;
    RenderSpritePacket *peLabel;
    RenderGouraudQuad *pinkGradient;
    DrawGlyphDescriptor *glyph;
    RenderSpritePacket *atbLabel;

    shades = g_BattleStatusShades[0];
    hudPage = GetTPage(0, 1, 0x100, 0x1E0);
    hudClut = GetClut(0x130, 0x1F8);
    bufferIndex = 0;
    do {
        glyph = Draw_LookupGlyphDescriptor(0x8B);
        labelQuad = &g_BattleEndPanels[bufferIndex];
        SetPolyFT4(labelQuad);
        labelQuad->u0 = glyph->u;
        labelQuad->v0 = glyph->v;
        labelQuad->u1 = glyph->u + glyph->width;
        labelQuad->v1 = glyph->v;
        labelQuad->u2 = glyph->u;
        labelQuad->v2 = glyph->v + glyph->height;
        labelQuad->u3 = glyph->u + glyph->width;
        labelQuad->v3 = glyph->v + glyph->height;
        g_BattleEndPanels[bufferIndex].tpage = GetTPage(0, 0, 0x1C0, 0);
        g_BattleEndPanels[bufferIndex].clut = glyph->clut;
        labelQuad->x0 = 0;
        highBlue = 0x82;
        labelQuad->y0 = 0;
        labelQuad->x1 = 0;
        labelQuad->y1 = 0;
        labelQuad->x2 = 0;
        labelQuad->y2 = 0;
        labelQuad->x3 = 0;
        labelQuad->y3 = 0;
        labelQuad->color.bytes.r = 0;
        labelQuad->color.bytes.g = 0;
        labelQuad->color.bytes.b = 0;
        SetSemiTrans(labelQuad, 1);
        {
            u8 buffer;
            buffer = bufferIndex;

            for (i = 0; (u8)i < 10; i++) {
                gridRow = (u8)i;
                for (j = 0; (u8)j < 4; j++) {
                    Gpu_InitDrawModeSprtPacket(&g_BattlePanelDigits[buffer][gridRow][(u8)j], hudPage);
                    g_BattlePanelDigits[buffer][gridRow][(u8)j].sprite.clut = hudClut;
                }
            }
        }
        pageCode = GetTPage(0, 0, 0, 0);
        Gpu_InitDrawModeTilePacket(&g_BattleStatusBackgrounds[bufferIndex], pageCode);
        background = &g_BattleStatusBackgrounds[bufferIndex].tile;
        background->r0 = 0x30;
        background->g0 = 0x30;
        background->b0 = 0x30;
        SetSemiTrans(background, 1);
        atbTrack = &g_BattleAtbTracks[bufferIndex];
        SetTile(atbTrack);
        atbGradient = &g_BattleAtbGauges[bufferIndex];
        atbTrack->r0 = 0x1D;
        atbTrack->g0 = 0x3E;
        atbTrack->b0 = 0x32;
        atbTrack->w = 0x38;
        atbTrack->h = 3;
        SetPolyG4(atbGradient);
        Gpu_InitDrawModeSprtPacket(&g_BattleAtbMarkers[bufferIndex], hudPage);
        atbMarker = &g_BattleAtbMarkers[bufferIndex].sprite;
        atbMarker->u = 0xC8;
        atbMarker->v = 0xE0;
        g_BattleAtbMarkers[bufferIndex].sprite.clut = hudClut;
        atbMarker->width = 4;
        atbMarker->height = 8;
        atbGradient->b0 = highBlue;
        atbGradient->g1 = 0xFF;
        atbGradient->r0 = 0;
        atbGradient->g0 = 0x46;
        atbGradient->r1 = 0x9F;
        atbGradient->b2 = highBlue;
        atbGradient->b1 = 0xF9;
        atbGradient->r2 = 0;
        atbGradient->g2 = 0x46;
        atbGradient->r3 = 0x9F;
        atbGradient->g3 = 0xFF;
        atbGradient->b3 = 0xF9;
        atbMarker->color.bytes.r = 0x9F;
        atbMarker->color.bytes.g = 0xFF;
        atbMarker->color.bytes.b = 0xF9;

        for (i = 0; (u8)i < 4; i++) {
            Gpu_InitDrawModeSprtPacket(&g_BattleHpDigits[bufferIndex][(u8)i], hudPage);
            g_BattleHpDigits[bufferIndex][(u8)i].sprite.clut = hudClut;
            BATTLE_SPRITE_SLOTS(g_BattleHpDigits, 4)[bufferIndex][(u8)i].sprite.width = 6;
            BATTLE_SPRITE_SLOTS(g_BattleHpDigits, 4)[bufferIndex][(u8)i].sprite.height = 0xA;
        }
        for (i = 0; (u8)i < 5; i++) {
            Gpu_InitDrawModeSprtPacket(&g_BattleMaxHpDigits[bufferIndex][(u8)i], hudPage);
            g_BattleMaxHpDigits[bufferIndex][(u8)i].sprite.clut = hudClut;
            BATTLE_SPRITE_SLOTS(g_BattleMaxHpDigits, 5)[bufferIndex][(u8)i].sprite.width = 6;
            BATTLE_SPRITE_SLOTS(g_BattleMaxHpDigits, 5)[bufferIndex][(u8)i].sprite.height = 0xA;
        }
        greenGradient = &g_BattlePeGauges[bufferIndex][0];
        SetPolyG4(greenGradient);
        pinkGradient = &g_BattlePeGauges[bufferIndex][1];
        SetPolyG4(pinkGradient);
        {
            u8 red;
            u8 blue;
            red = 0x4A;
            blue = 0x3B;
            greenGradient->g0 = highBlue;
            greenGradient->r0 = 0;
            greenGradient->b0 = 0x36;
            greenGradient->r1 = red;
            greenGradient->g1 = 0xFF;
            greenGradient->b1 = blue;
            greenGradient->g2 = highBlue;
            greenGradient->r2 = 0;
            greenGradient->b2 = 0x36;
            greenGradient->r3 = red;
            greenGradient->g3 = 0xFF;
            greenGradient->b3 = blue;
        }
        BATTLE_SET_GAUGE_GRADIENT(*pinkGradient, 0xFF, 0x3D, 0x81, 0x83, 0x13, 1);
        Gpu_InitDrawModeSprtPacket(&g_BattleAtbLabels[bufferIndex], hudPage);
        atbLabel = &g_BattleAtbLabels[bufferIndex].sprite;
        SetShadeTex(atbLabel, 1);
        markerV = 0xF4;
        atbLabel->u = 0x50;
        atbLabel->v = markerV;
        BATTLE_SET_SPRITE_COLOR(*atbLabel, 0x80, 0x80, 0x80);
        g_BattleAtbLabels[bufferIndex].sprite.clut = hudClut;
        atbLabel->width = 8;
        atbLabel->height = 4;
        Gpu_InitDrawModeSprtPacket(&g_BattleHpLabels[bufferIndex], hudPage);
        hpLabel = &g_BattleHpLabels[bufferIndex].sprite;
        SetShadeTex(hpLabel, 1);
        hpLabel->u = 0x58;
        hpLabel->v = markerV;
        BATTLE_SET_SPRITE_COLOR(*hpLabel, 0x80, 0x80, 0x80);
        g_BattleHpLabels[bufferIndex].sprite.clut = hudClut;
        hpLabel->width = 8;
        hpLabel->height = 4;
        Gpu_InitDrawModeSprtPacket(&g_BattlePeLabels[bufferIndex], hudPage);
        peLabel = &g_BattlePeLabels[bufferIndex].sprite;
        SetShadeTex(peLabel, 1);
        peLabel->u = 0x60;
        peLabel->v = markerV;
        BATTLE_SET_SPRITE_COLOR(*peLabel, 0x80, 0x80, 0x80);
        g_BattlePeLabels[bufferIndex].sprite.clut = hudClut;
        peLabel->width = 8;
        peLabel->height = 4;

        for (i = 0; (u8)i < 3; i++) {
            tileIndex = (u8)i;
            SetTile(&BATTLE_STATUS_TILES[bufferIndex][tileIndex]);
            BATTLE_STATUS_TILES[bufferIndex][tileIndex].r0 = shade = shades.values[tileIndex];
            BATTLE_STATUS_TILES[bufferIndex][tileIndex].g0 = shade;
            BATTLE_STATUS_TILES[bufferIndex][tileIndex].b0 = shade;
        }
        Gpu_InitDrawModeSprtPacket(&D_8009E460[bufferIndex], hudPage);
        upperLine = &D_8009E498[bufferIndex][0];
        pointerSprite = &D_8009E460[bufferIndex].sprite;
        pointerSprite->u = 0xE8;
        pointerSprite->v = 0xE0;
        D_8009E460[bufferIndex].sprite.clut = hudClut;
        pointerSprite->width = 0x18;
        pointerSprite->height = 0x18;
        SetLineF2(upperLine);
        lowerLine = &D_8009E498[bufferIndex][1];
        SetLineF2(lowerLine);
        upperLine->r = 0xE0;
        upperLine->g = 0xE0;
        upperLine->b = 0xE0;
        lowerLine->r = 0x60;
        lowerLine->g = 0x60;
        lowerLine->b = 0x60;
        SetPolyF3(&D_8009E4D8[bufferIndex]);
        for (i = 0; (u8)i < 3; i++) {
            Gpu_InitDrawModeSprtPacket(&D_8009E3B8[bufferIndex][(u8)i], hudPage);
            D_8009E3B8[bufferIndex][(u8)i].sprite.clut = hudClut;
            BATTLE_SPRITE_SLOTS(D_8009E3B8, 3)[bufferIndex][(u8)i].sprite.width = 0x18;
            BATTLE_SPRITE_SLOTS(D_8009E3B8, 3)[bufferIndex][(u8)i].sprite.height = 8;
            BATTLE_SET_SPRITE_COLOR(BATTLE_SPRITE_SLOTS(D_8009E3B8, 3)[bufferIndex][(u8)i].sprite,
                                    0x80, 0x80, 0x80);
        }
        for (i = 0; (u8)i < 10; i++) {
            Gpu_InitDrawModeSprtPacket(&D_8009E500[bufferIndex][(u8)i], hudPage);
            BATTLE_SET_SPRITE_COLOR(BATTLE_SPRITE_SLOTS(D_8009E500, 10)[bufferIndex][(u8)i].sprite,
                                    0x80, 0x80, 0x80);
        }
        Gpu_InitDrawModeSprtPacket(&D_8009E768[bufferIndex], hudPage);
        ammoLabel = &D_8009E768[bufferIndex].sprite;
        ammoLabel->u = 0x58;
        ammoLabel->v = 0xEF;
        D_8009E768[bufferIndex].sprite.clut = hudClut;
        ammoLabel->width = 0x24;
        ammoLabel->height = 5;
        BATTLE_SET_SPRITE_COLOR(*ammoLabel, 0x80, 0x80, 0x80);
        for (i = 0; (u8)i < 4; i++) {
            Gpu_InitDrawModeSprtPacket(&D_8009E7A0[bufferIndex][(u8)i], hudPage);
            D_8009E7A0[bufferIndex][(u8)i].sprite.clut = hudClut;
            BATTLE_SPRITE_SLOTS(D_8009E7A0, 4)[bufferIndex][(u8)i].sprite.width = 6;
            BATTLE_SPRITE_SLOTS(D_8009E7A0, 4)[bufferIndex][(u8)i].sprite.height = 6;
            BATTLE_SET_SPRITE_COLOR(BATTLE_SPRITE_SLOTS(D_8009E7A0, 4)[bufferIndex][(u8)i].sprite,
                                    0x80, 0x80, 0x80);
        }
        Gpu_InitDrawModeSprtPacket(&D_8009E730[bufferIndex], hudPage);
        statusIcon = &D_8009E730[bufferIndex].sprite;
        statusIcon->u = 0x68;
        statusIcon->v = 0xF4;
        D_8009E730[bufferIndex].sprite.clut = GetClut(0x130, 0x1F9);
        statusIcon->width = 0x18;
        statusIcon->height = 4;
        BATTLE_SET_SPRITE_COLOR(*statusIcon, 0x80, 0x80, 0x80);
        Gpu_InitDrawModeSprtPacket(&D_8009E880[bufferIndex], hudPage);
        ammoIcon = &D_8009E880[bufferIndex].sprite;
        ammoIcon->u = 0x7C;
        ammoIcon->v = 0xEF;
        D_8009E880[bufferIndex].sprite.clut = hudClut;
        ammoIcon->width = 0x24;
        ammoIcon->height = 5;
        BATTLE_SET_SPRITE_COLOR(*ammoIcon, 0x80, 0x80, 0x80);
        for (i = 0; (u8)i < 2; i++) {
            Gpu_InitDrawModeSprtPacket(&D_8009E8B8[bufferIndex][(u8)i], hudPage);
            D_8009E8B8[bufferIndex][(u8)i].sprite.clut = hudClut;
            BATTLE_SPRITE_SLOTS(D_8009E8B8, 2)[bufferIndex][(u8)i].sprite.width = 6;
            BATTLE_SPRITE_SLOTS(D_8009E8B8, 2)[bufferIndex][(u8)i].sprite.height = 6;
        }
        Gpu_InitDrawModeSprtPacket(&D_8009E928[bufferIndex], hudPage);
        D_8009E928[bufferIndex].sprite.clut = hudClut;
        weaponIcon = &D_8009E928[bufferIndex].sprite;
        BATTLE_SET_SPRITE_COLOR(*weaponIcon, 0x80, 0x80, 0);

        for (i = 0; (u8)i < 13; i++) {
            glyph = Draw_LookupGlyphDescriptor(((u8)i) + 0x6A);
            pageCode = GetTPage(0, 0, 0x1C0, 0);
            Gpu_InitDrawModeSprtPacket(&D_8009E960[bufferIndex][(u8)i], pageCode);
#define GLYPH_SPRITE BATTLE_SPRITE_SLOTS(D_8009E960, 13)[bufferIndex][(u8)i].sprite
            GLYPH_SPRITE.u = glyph->u;
            GLYPH_SPRITE.v = glyph->v;
            D_8009E960[bufferIndex][(u8)i].sprite.clut = glyph->clut;
            GLYPH_SPRITE.width = glyph->width;
            glyphHeight = glyph->height;
            BATTLE_SET_SPRITE_COLOR(GLYPH_SPRITE, 0x80, 0x80, 0x80);
            GLYPH_SPRITE.height = glyphHeight;
#undef GLYPH_SPRITE
        }
        pageCode = GetTPage(0, 0, 0x1C0, 0);
        Gpu_InitDrawModeSprtPacket(&((RenderPagedSpritePacket *)D_8009EC38)[bufferIndex], pageCode);
        cursorSprite = &((RenderPagedSpritePacket *)D_8009EC38)[bufferIndex].sprite;
        bufferIndex += 1;
        cursorSprite->width = 0x10;
        cursorSprite->height = 0x10;
        BATTLE_SET_SPRITE_COLOR(*cursorSprite, 0x80, 0x80, 0x80);
    } while (bufferIndex < 2U);
}
