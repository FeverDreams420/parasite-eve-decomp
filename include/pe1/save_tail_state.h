#ifndef PE1_SAVE_TAIL_STATE_H
#define PE1_SAVE_TAIL_STATE_H

#include "common.h"
#include "pe1/save_blob.h"

/* Byte-block views of the globals the save tail copies; each names the
 * same storage as the typed global its asm label points at. */

extern SaveBytes800 g_SaveEntityWorkBlock __asm__("D_800A77F0");
/* Battle records the save tail copies as raw bytes; their layouts live in
 * battle_saved_state.h and battle.h. */
struct AyaBattleState;
union BattleStateTail;
struct BattleAttributes;
extern struct AyaBattleState g_AyaBattleState __asm__("D_800B8A20");
extern union BattleStateTail g_SavedBattleStateTail __asm__("D_800B0CB0");
extern struct BattleAttributes g_BattleEquipStateBlock;
extern SaveBytes4 g_SaveFieldMoveLockBytes __asm__("D_8009D2E8");
extern SaveBytes4 g_SaveSceneDispatchTokenBytes __asm__("D_8009D280");
extern SaveBytes4 g_SaveGameStateFlagsBytes __asm__("D_8009D1A0");
extern SaveBytes4 D_800B0CDC;
extern s8 D_800B0CE0;
extern s8 g_LoadedTexturePageId __asm__("D_800B0CE1");
extern u8 g_SceneAreaType __asm__("D_800B0CE2");
extern u8 g_SavedSceneAreaType __asm__("D_800B0CE3");
extern s8 g_PendingStoryDay __asm__("D_800B0CE5");
extern s8 g_CurrentStoryDay __asm__("D_800B0CE4");
extern u8 g_DiscChangeFlags __asm__("D_800B0CE6");
extern u8 g_ScreenTransitionState[] __asm__("D_800BCFEE");
extern u8 *g_SaveIoCursor __asm__("D_800A0ED0");
extern u32 g_GameStateFlagsWord __asm__("D_8009D1A0");
extern u32 g_FieldMoveLockWord __asm__("D_8009D2E8");
extern u8 *volatile g_SaveIoCursorRead __asm__("D_800A0ED0");

#endif
