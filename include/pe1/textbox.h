#ifndef PE1_TEXTBOX_H
#define PE1_TEXTBOX_H

#include "pe1/draw_state.h"
#include "pe1/render_packets.h"

/* Textbox / on-screen dialogue entry, walked by Menu_DrawTextboxEntries.
 * g_TextboxEntries (= D_800BCEA8) is an array of these, stride 0x38. Each
 * frame the engine walks the active entries and types out / draws their text.
 * See include/pe1/text.h for the surrounding text subsystem notes. */

typedef struct TextboxNumber {
    u8 digits[5];
    s8 count;
} TextboxNumber;
typedef union TextboxControl {
    s32 flags;
    struct {
        u8 delay, elapsed;
        u16 bits;
    } parts;
} TextboxControl;
typedef struct TextboxEntry {
    u8 state;
    u8 pad01[3];
    u8 *message;
    u8 style, background;
    u8 pad0A[2];
    TextboxControl control;
    s16 page_id;
    u16 x, y, width, height;
    TextboxNumber numbers[5];
} TextboxEntry;

typedef struct TextboxFontPage {
    u32 reserved;
    u16 textureX, textureY, paletteX, paletteY;
    u16 tpage, clut;
} TextboxFontPage;

typedef struct TextboxNameGlyphs {
    u8 glyphs[9];
    s8 count;
} TextboxNameGlyphs;
extern TextboxNameGlyphs D_80091694;
typedef struct TextboxGlyphSpacing {
    u8 left, right;
} TextboxGlyphSpacing;
typedef struct TextboxPoint {
    u16 x, y;
} TextboxPoint;
typedef struct TextboxGlyphPacket {
    u32 tag, drawMode;
    RenderSpritePacket sprite;
} TextboxGlyphPacket;

extern TextboxEntry g_TextboxEntries[]; /* = D_800BCEA8 */

/* Font pages, the per-glyph spacing table and the textbox sprite/tile
 * packets for both frame buffers (glyph packets at 0x8009EC70, 28 bytes;
 * fog tiles at 0x8009ECA8, 24 bytes). */
extern TextboxFontPage D_80091644[4];
extern u16 D_80091680[];
extern TextboxGlyphSpacing D_800916A0[256];

/* A draw-table address as a pointer or a word: Menu_DrawTextboxEntries
 * adds byte offsets it computed once (draw slot * 4, glyph * 2) to the
 * table address as integers. */
typedef union TextboxTableAddress {
    void *pointer;
    u8 *bytes;
    RenderSpritePacket *sprite;
    TextboxGlyphPacket **packets;
    u32 **ordering;
    TextboxGlyphSpacing *spacing;
    u32 address;
} TextboxTableAddress;
/* A glyph's spacing entry addressed by the doubled glyph index. */
typedef union TextboxGlyphSpacingAddress {
    TextboxGlyphSpacing *spacing;
    u32 word;
} TextboxGlyphSpacingAddress;
extern u8 D_8009EC70[];
extern u8 D_8009EC78[];
extern u8 D_8009EC86[];
extern u8 D_8009ECA8[];
/* Text colour bytes of the textbox glyphs. */
extern u8 D_8009CEC4;
extern u8 D_8009CEC8;
extern u8 D_8009CECC;

PE1_STATIC_ASSERT(sizeof(TextboxEntry) == 0x38, textbox_entry_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(TextboxEntry, control) == 0xC, textbox_control_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(TextboxEntry, numbers) == 0x1A, textbox_numbers_offset);
PE1_STATIC_ASSERT(sizeof(TextboxNumber) == 6, textbox_number_size);
PE1_STATIC_ASSERT(sizeof(TextboxGlyphPacket) == 28, textbox_glyph_packet_size);

void Menu_DrawTextboxEntries(void);
void Draw_ResetTextboxes(void);
void Menu_SetTextCursorRect(int x, int y, int width, int height);
void Render_SetupColorTable(int index, int mode, short *colors);
void Draw_SetCursor(int x, int y);

void Draw_CloseTextboxPage(int arg0);
void Script_SetCollisionFlag(int value);
void Script_EnableMovement(void);
void Script_DisableMovement(void);
int Menu_GetEquipSlotIndex(void);

#endif /* PE1_TEXTBOX_H */
