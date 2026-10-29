#include "common.h"
#include "pe1/aya.h"
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/menu_widget.h"
#include "pe1/resident_tables.h"
#include "pe1/text.h"


/* Location name of map `map` (-1 selects name 15); names are NUL-separated. */
u8 *Text_GetMapLocationName(int map) {
    u8 *name;

    name = RESIDENT_TABLE(u8, locationNames);
    if (map >= 0) {
        map = g_ResidentDataPack.bytes[map + g_ResidentDataPack.directory.mapLocationIds];
    } else {
        map = 0xF;
    }

    if (map == 0) {
        return 0;
    }

    do {
        if (*name++ == 0) {
            map--;
        }
    } while (map > 0);

    return name;
}

/* Memory-card icon block copied into each save file header. */
u8 *Text_GetSaveIconData(void) {
    return RESIDENT_TABLE(u8, saveIcon);
}