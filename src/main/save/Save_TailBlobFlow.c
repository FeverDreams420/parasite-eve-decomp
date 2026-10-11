#include "common.h"

#include "pe1/save_tail_state.h"

/* The save tail is a packed byte stream: each record is copied as an
 * unaligned byte block at the IO cursor, which then advances past it. */
#define SAVE_PUT(type, src)                                \
    *(type *)g_SaveIoCursor = *(type *)(src);              \
    g_SaveIoCursor += sizeof(type)

void Save_SerializeTail(void) {
    SAVE_PUT(SaveBytes800, g_EntityWorkBuffer);
    SAVE_PUT(SaveBytes4, &D_8009D2E8);
    SAVE_PUT(SaveBytes4, &D_8009D280);
    SAVE_PUT(SaveBytes4, &D_8009D1A0);
    SAVE_PUT(SaveBytes4, &D_800B0CDC);
    SAVE_PUT(SaveBytes2, &D_800B0CE0);
    SAVE_PUT(SaveBytes2, &g_SceneAreaType);
    SAVE_PUT(SaveBytes2, &g_CurrentStoryDay);
    SAVE_PUT(SaveU8, &g_DiscChangeFlags);
    SAVE_PUT(SaveU8, &g_ScreenTransitionState[0]);
    SAVE_PUT(SaveBytes70, &g_AyaBattleState);
    SAVE_PUT(SaveBytes18, &g_SavedBattleStateTail);
    SAVE_PUT(SaveBytes8, &g_BattleEquipStateBlock);
}

#define SAVE_GET(type, dst)                                \
    *(type *)(dst) = *(type *)g_SaveIoCursor;              \
    g_SaveIoCursor += sizeof(type)

void Save_DeserializeTail(void) {
    SAVE_GET(SaveBytes800, g_EntityWorkBuffer);
    SAVE_GET(SaveBytes4, &D_8009D2E8);
    SAVE_GET(SaveBytes4, &D_8009D280);
    SAVE_GET(SaveBytes4, &D_8009D1A0);
    SAVE_GET(SaveBytes4, &D_800B0CDC);
    SAVE_GET(SaveBytes2, &D_800B0CE0);
    SAVE_GET(SaveBytes2, &g_SceneAreaType);
    SAVE_GET(SaveBytes2, &g_CurrentStoryDay);
    SAVE_GET(SaveU8, &g_DiscChangeFlags);
    SAVE_GET(SaveU8, &g_ScreenTransitionState[0]);
    SAVE_GET(SaveBytes70, &g_AyaBattleState);
    SAVE_GET(SaveBytes18, &g_SavedBattleStateTail);
    SAVE_GET(SaveBytes8, &g_BattleEquipStateBlock);
    D_8009D1A0 &= 0xFFFF2679;
    D_8009D2E8 &= ~9;
}
