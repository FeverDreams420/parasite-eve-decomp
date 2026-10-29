#include "common.h"
#include "pe1/aya.h"
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/menu_widget.h"
#include "pe1/resident_tables.h"
#include "pe1/text.h"
/* JP: selects which run of four table offsets (after the reserved word) the
 * text lookups use. */
extern int g_TextTableSet;

void *Text_LookupTable4(unsigned int index) {
    int slot = g_TextTableSet * 4 + 1;
    ResidentDataPack *pack = &g_ResidentDataPack;
    TextTableDirectory *directory;
    TextOffsetTable *table;
    s16 offset;

    directory = (TextTableDirectory *)&pack->bytes[pack->directory.text];
    table = (TextOffsetTable *)&directory->bytes[((s32 *)directory)[slot]];
    if (index >= (s16)table->header.count) {
        return 0;
    }

    offset = table->header.offsets[index];
    return &table->bytes[offset];
}

void *Text_LookupTable8(unsigned int index) {
    int slot = g_TextTableSet * 4 + 2;
    ResidentDataPack *pack = &g_ResidentDataPack;
    TextTableDirectory *directory;
    TextOffsetTable *table;
    s16 offset;

    directory = (TextTableDirectory *)&pack->bytes[pack->directory.text];
    table = (TextOffsetTable *)&directory->bytes[((s32 *)directory)[slot]];
    if (index >= (s16)table->header.count) {
        return 0;
    }

    offset = table->header.offsets[index];
    return &table->bytes[offset];
}

void *Text_LookupTableC(unsigned int index) {
    int slot = g_TextTableSet * 4 + 3;
    ResidentDataPack *pack = &g_ResidentDataPack;
    TextTableDirectory *directory;
    TextOffsetTable *table;
    s16 offset;

    directory = (TextTableDirectory *)&pack->bytes[pack->directory.text];
    table = (TextOffsetTable *)&directory->bytes[((s32 *)directory)[slot]];
    if (index >= (s16)table->header.count) {
        return 0;
    }

    offset = table->header.offsets[index];
    return &table->bytes[offset];
}

void *Text_LookupTable10(unsigned int index) {
    int slot = g_TextTableSet * 4 + 4;
    ResidentDataPack *pack = &g_ResidentDataPack;
    TextTableDirectory *directory;
    TextOffsetTable *table;
    s16 offset;

    directory = (TextTableDirectory *)&pack->bytes[pack->directory.text];
    table = (TextOffsetTable *)&directory->bytes[((s32 *)directory)[slot]];
    if (index >= (s16)table->header.count) {
        return 0;
    }

    offset = table->header.offsets[index];
    return &table->bytes[offset];
}

/* Text-table-3 entry naming the location of map `map`. */
void *Text_LookupMapLocationText(int map) {
    TextTableDirectory *directory;
    TextOffsetTable *table;
    int index;
    s16 offset;
    int slot;

    index = g_ResidentDataPack.bytes[map + g_ResidentDataPack.directory.mapLocationIds];
    if (index == 0) {
        return 0;
    }

    slot = g_TextTableSet * 4 + 4;
    directory = RESIDENT_TABLE(TextTableDirectory, text);
    table = (TextOffsetTable *)&directory->bytes[((s32 *)directory)[slot]];
    index += 0x7F;
    if ((unsigned int)index >= (s16)table->header.count) {
        return 0;
    }

    offset = table->header.offsets[index];
    return &table->bytes[offset];
}