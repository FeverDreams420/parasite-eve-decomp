#include "common.h"
#include "pe1/aya.h"
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/menu_widget.h"
#include "pe1/resident_tables.h"
#include "pe1/text.h"


ItemDataRecord *Item_LookupBaseData(unsigned int index) {
    s32 *endSlot = &g_ResidentDataPack.directory.statGrowth;
    s32 end = *endSlot;
    s32 begin = g_ResidentDataPack.directory.itemBaseData;

    if (index >= (unsigned int)(end - begin) / sizeof(ItemDataRecord)) {
        return 0;
    }
    return (ItemDataRecord *)&g_ResidentDataPack.bytes[begin] + index;
}

void *Aya_GetStatGrowthTable(int category) {
    return &RESIDENT_TABLE(u8, statGrowth)[category << 9];
}

AyaLevelStats *Aya_LookupLevelStats(int level) {
    if (level < 0) {
        level = 0;
    } else if (level >= 0x63) {
        level = 0x62;
    }

    return &RESIDENT_TABLE(AyaLevelStats, levelStats)[level];
}

void *Aya_GetLevelExpTable(void) {
    return RESIDENT_TABLE(s32, levelExp);
}

void *Aya_GetParasiteSpellUnlockTable(void) {
    return RESIDENT_TABLE(ParasiteSpellEntry, parasiteSpells);
}

int Draw_LookupGlyphMetrics(int glyph) {
    return g_ResidentDataPack.bytes[glyph + g_ResidentDataPack.directory.glyphMetrics];
}