#ifndef PE1_GAME_STATE_H
#define PE1_GAME_STATE_H

#include "pe1/game_state_types.h"

extern Pe1GameState g_GameState;
extern unsigned int g_GameStateFlags;
/* Field movement lock bits tested by the frame update, the actor list update
 * and the scene scripts (0x8009D2E8). */
extern int g_FieldMoveLock;

/* g_GameState.story_day_flags as its own byte symbol: Scene_SetStoryDay and
 * the battle update read and set it with absolute byte loads and stores. */
extern u8 D_800B0CE6;
extern u8 g_DiscChangeFlags;

/* g_GameState.flags as its own scalar symbol: the frame loop, the process
 * manager and the CD boot read update it with absolute loads and stores,
 * where the record keeps its address in a register. */
extern u32 D_800B0CD8;

/* g_GameState.scene_object.animation_source as its own scalar symbol:
 * Scene_UpdateBgDraw tests and sets it with absolute loads and stores. */
extern struct RenderObjectEntity *g_SceneObjectAnimationSource;

void Sys_Shutdown(void);

/* Records a started scene track in g_GameState.scene_audio. */
int Overlay_RegisterAudioSlot(int slot, int key, int bank, int volume);

#endif
