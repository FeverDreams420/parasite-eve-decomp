#include "common.h"
#include "pe1/aya.h"
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/menu_widget.h"
#include "pe1/resident_tables.h"
#include "pe1/text.h"




void *Text_LookupTable0(unsigned int index) {
    TextOffsetTable *table;
    s16 offset;

    table = RESIDENT_TABLE(TextOffsetTable, text);
    if (index < (s16)table->header.count) {
        offset = table->header.offsets[index];
        return &table->bytes[offset];
    }

    return 0;
}