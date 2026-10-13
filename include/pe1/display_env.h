#ifndef PE1_DISPLAY_ENV_H
#define PE1_DISPLAY_ENV_H

#include "common.h"
#include "pe1/psyq_gpu.h"

/* Double-buffered display and draw environments, indexed by the active
 * draw slot (g_ActiveDrawSlot). Boot fills them, the frame loop puts them, and
 * the memcard video player saves, retunes and restores them. */
extern DISPENV D_800BCE80[2];
extern DRAWENV D_800BCDC8[2];

#endif /* PE1_DISPLAY_ENV_H */
