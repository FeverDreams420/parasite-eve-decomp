#include "common.h"
#include "pe1/text.h"
#include "pe1/menu_widget.h"
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/aya.h"

extern MenuWidgetSimpleDescriptor D_80092478[];

extern MenuWidgetGridDescriptor D_80092888[];

extern int D_800A8030;

extern int g_StrLookupTableOffset;

MenuWidgetSimpleDescriptor *MenuWidget_LookupSimpleDescriptor(unsigned int index) {
    if (index >= 0x41) {
        return 0;
    }
    return &D_80092478[index];
}

MenuWidgetGridDescriptor *MenuWidget_LookupGridDescriptor(unsigned int index) {
    if (index >= 0x41) {
        return 0;
    }
    return &D_80092888[index];
}

DrawGlyphDescriptor *Draw_LookupGlyphDescriptor(int index) {
    register char *base = (char *)&D_800A8030;
    register int offset = *(int *)base;

    base -= 8;
    return (DrawGlyphDescriptor *)(offset + base + index * sizeof(DrawGlyphDescriptor));
}

void *Str_LookupTable0(unsigned int arg0) {
    u8 *end;
    u8 *base;
    TextOffsetTable *table;
    s16 offset;

    end = (u8 *)&g_StrLookupTableOffset;
    base = end - 4;
    table = (TextOffsetTable *)(base + *(int *)end);
    if (arg0 < table->count) {
        offset = table->offsets[arg0];
        return (u8 *)table + offset;
    }

    return 0;
}

extern int D_800A8034[];
extern int D_800A8038[];

ItemDataRecord *Item_LookupBaseData(unsigned int index)
{
    int *endPtr;
    int base;
    unsigned int offset;
    int end;

    endPtr = D_800A8038;
    end = endPtr[0];
    base = D_800A8034[0];

    if (index >= ((unsigned int)(end - base) >> 5)) {
        return 0;
    }

    offset = index << 5;
    return (ItemDataRecord *)((char *)D_800A8038 - 0x10 + base + offset);
}

extern int g_StatGrowthTable[];

void *Stat_GetGrowthTable(int arg0) {
    return (void *)(g_StatGrowthTable[0] + (int)((char *)g_StatGrowthTable - 0x10) + (arg0 << 9));
}

extern int D_800A803C[];

extern int D_800A8040;

extern int D_800A8048;

extern u8 g_GlyphMetricsTable[];
extern int D_800A8050;

AyaLevelStats *Aya_LookupLevelStats(int arg0) {
    if (arg0 < 0) {
        arg0 = 0;
    } else if (arg0 >= 0x63) {
        arg0 = 0x62;
    }

    return (AyaLevelStats *)(D_800A803C[0] + (int)((char *)D_800A803C - 0x14) + (arg0 * 0x18));
}

void *Aya_GetLevelExpTable(void) {
    register char *base = (char *)&D_800A8040;
    register int offset = *(int *)base;

    base -= 0x18;
    return (void *)(offset + base);
}

void *Aya_GetParasiteSpellUnlockTable(void) {
    register char *base = (char *)&D_800A8048;
    register int offset = *(int *)base;

    base -= 0x20;
    return (void *)(offset + base);
}

int Draw_LookupGlyphMetrics(int arg0) {
    arg0 += D_800A8050;
    return g_GlyphMetricsTable[arg0];
}

void *Str_LookupTable4(unsigned int arg0) {
    TextTableDirectory *directory;
    TextOffsetTable *table;
    s16 offset;

    directory = (TextTableDirectory *)(g_GlyphMetricsTable + g_StrLookupTableOffset);
    table = (TextOffsetTable *)((u8 *)directory + directory->tableOffsets[0]);
    if (arg0 >= table->count) {
        return 0;
    }

    offset = table->offsets[arg0];
    return (u8 *)table + offset;
}

void *Str_LookupTable8(unsigned int arg0) {
    TextTableDirectory *directory;
    TextOffsetTable *table;
    s16 offset;

    directory = (TextTableDirectory *)(g_GlyphMetricsTable + g_StrLookupTableOffset);
    table = (TextOffsetTable *)((u8 *)directory + directory->tableOffsets[1]);
    if (arg0 >= table->count) {
        return 0;
    }

    offset = table->offsets[arg0];
    return (u8 *)table + offset;
}

void *Str_LookupTableC(unsigned int arg0) {
    TextTableDirectory *directory;
    TextOffsetTable *table;
    s16 offset;

    directory = (TextTableDirectory *)(g_GlyphMetricsTable + g_StrLookupTableOffset);
    table = (TextOffsetTable *)((u8 *)directory + directory->tableOffsets[2]);
    if (arg0 >= table->count) {
        return 0;
    }

    offset = table->offsets[arg0];
    return (u8 *)table + offset;
}

void *Str_LookupTable10(unsigned int arg0) {
    TextTableDirectory *directory;
    TextOffsetTable *table;
    s16 offset;

    directory = (TextTableDirectory *)(g_GlyphMetricsTable + g_StrLookupTableOffset);
    table = (TextOffsetTable *)((u8 *)directory + directory->tableOffsets[3]);
    if (arg0 >= table->count) {
        return 0;
    }

    offset = table->offsets[arg0];
    return (u8 *)table + offset;
}
extern int D_800A804C;

void *Str_LookupTableEntry(int arg0) {
    TextTableDirectory *directory;
    TextOffsetTable *table;
    int index;
    s16 offset;

    index = g_GlyphMetricsTable[arg0 + D_800A804C];
    if (index == 0) {
        return 0;
    }

    directory = (TextTableDirectory *)(g_GlyphMetricsTable + g_StrLookupTableOffset);
    table = (TextOffsetTable *)((u8 *)directory + directory->tableOffsets[3]);
    index += 0x7F;
    if ((u32)index >= table->count) {
        return 0;
    }

    offset = table->offsets[index];
    return (u8 *)table + offset;
}

extern int D_800A8054;

extern int D_800A8044;

u8 *Tbl_FindNthNonEmpty(int arg0) {
    u8 *end;
    u8 *base;
    u8 *ptr;
    int offset;

    end = (u8 *)&D_800A8054;
    offset = *(int *)end;
    base = end - 0x2C;
    ptr = base + offset;
    if (arg0 >= 0) {
        arg0 = base[arg0 + D_800A804C];
    } else {
        arg0 = 0xF;
    }

    if (arg0 == 0) {
        return 0;
    }

    do {
        if (*ptr++ == 0) {
            arg0--;
        }
    } while (arg0 > 0);

    return ptr;
}

void *Str_ResolveDataPtr(void) {
    register char *base = (char *)&D_800A8044;
    register int offset = *(int *)base;

    base -= 0x1C;
    return (void *)(offset + base);
}
