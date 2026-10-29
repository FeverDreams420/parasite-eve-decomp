#include "common.h"
#include "pe1/aya.h"
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/menu_widget.h"
#include "pe1/resident_tables.h"
#include "pe1/text.h"

MenuWidgetSimpleDescriptor *MenuWidget_LookupSimpleDescriptor(unsigned int index) {
    if (index >= 0x41) {
        return 0;
    }
    return &g_MenuWidgetSimpleDescriptors[index];
}

MenuWidgetGridDescriptor *MenuWidget_LookupGridDescriptor(unsigned int index) {
    if (index >= 0x41) {
        return 0;
    }
    return &g_MenuWidgetGridDescriptors[index];
}

DrawGlyphDescriptor *Draw_LookupGlyphDescriptor(int index) {
    s32 *slot = &g_ResidentDataPack.directory.glyphDescriptors;
    s32 offset = *slot;
    DrawGlyphDescriptor *table = (DrawGlyphDescriptor *)&g_ResidentDataPack.bytes[offset];

    return &table[index];
}