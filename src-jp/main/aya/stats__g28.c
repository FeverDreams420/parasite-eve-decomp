/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "pe1/akao.h"
#include "pe1/aya.h"
#include "pe1/battle.h"
#include "pe1/battle_modifiers.h"
#include "pe1/battle_runtime.h"
#include "pe1/game_state.h"
#include "pe1/inventory_slots.h"
#include "pe1/inventory_state.h"
#include "pe1/menu_sound.h"
#include "pe1/player_entity.h"
#include "pe1/stat_modifiers.h"
#include "pe1/textbox.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_InvBonusSlotCount;



/* JP keeps the prompt name as two separate globals: a 6-glyph buffer and a
 * small-data count byte, instead of USA's 9-glyph TextboxNameGlyphs. */
extern u8 g_TextboxNameGlyphBuf[6];
extern s8 g_TextboxNameGlyphCount;

void Menu_CopyPromptCodes(unsigned char *src) {
    unsigned char *dst;
    unsigned char *end;
    int value;

    dst = g_TextboxNameGlyphBuf;
    end = dst + 6;
    while (dst < end) {
        value = *src;
        if (value == 0xFF) {
            break;
        }
        *dst++ = value;
        src++;
    }

    g_TextboxNameGlyphCount = dst - g_TextboxNameGlyphBuf;
}