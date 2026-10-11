#ifndef PE1_GAME_STATE_H
#define PE1_GAME_STATE_H

#include "pe1/game_state_types.h"

extern Pe1GameState g_GameState;
extern unsigned int g_GameStateFlags;

/* g_GameState.story_day_flags as its own byte symbol: Scene_SetStoryDay and
 * the battle update read and set it with absolute byte loads and stores. */
extern u8 D_800B0CE6;
extern u8 g_DiscChangeFlags;

/* g_GameState.flags as its own scalar symbol: the frame loop, the process
 * manager and the CD boot read update it with absolute loads and stores,
 * where the record keeps its address in a register. */
extern u32 D_800B0CD8;

void Sys_Shutdown(void);

#endif
