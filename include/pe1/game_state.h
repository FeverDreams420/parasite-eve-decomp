#ifndef PE1_GAME_STATE_H
#define PE1_GAME_STATE_H

#include "pe1/game_state_types.h"

#ifdef PE1_GAME_STATE_LEGACY_RAW_VIEW
extern Pe1GameState g_GameStateTyped __asm__("g_GameState");
#else
extern Pe1GameState g_GameState;
extern unsigned int g_GameStateFlags;
#endif

/* g_GameState.story_day_flags as its own byte symbol: Scene_SetStoryDay and
 * the battle update read and set it with absolute byte loads and stores. */
extern u8 D_800B0CE6;

void Sys_Shutdown(void);

#endif
