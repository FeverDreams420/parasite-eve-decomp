#ifndef PE1_SAVE_TAIL_STATE_H
#define PE1_SAVE_TAIL_STATE_H

#include "common.h"
#include "pe1/save_blob.h"
#include "pe1/battle_saved_state.h"
#include "pe1/battle_modifiers.h"

/* Save tail cursor: Save_SerializeTail/Save_DeserializeTail copy each
 * record below through it and advance it by the record size. */
extern u8 *g_SaveIoCursor;

/* Records the save tail copies as raw bytes, in file order. */
extern char g_EntityWorkBuffer[]; /* 0x800 bytes */
extern u32 D_8009D2E8;           /* field movement lock flags */
extern u32 D_8009D280;           /* scene dispatch token */
extern u32 D_8009D1A0;           /* game state flags */
extern SaveBytes4 D_800B0CDC;
extern s8 D_800B0CE0;
extern s8 g_LoadedTexturePageId;
extern u8 g_SceneAreaType;
extern u8 g_SavedSceneAreaType;
extern s8 g_CurrentStoryDay;
extern s8 g_PendingStoryDay;
extern u8 g_DiscChangeFlags;
extern u8 g_ScreenTransitionState[];

#endif
