#include "common.h"
#include "pe1/game_state.h"
#include "pe1/map_selection.h"
#include "pe1/font.h"
#include "pe1/scene_dispatch.h"

extern u8 D_8009CD78[];
extern u8 D_8009CD80[];
extern u8 g_MapIdTable[];
extern unsigned int D_8009D280;
extern u32 D_8009D1A0;

int Menu_GetEquipSlotStateOrIndex(void);
int Task_GpuPackPrimColor(int start, int end);
int Str_ParseBase32Id(u8 *text);

int Scene_SelectMapById(int **args) {
    int raw_selection;
    u8 selection;
    u8 state;
    MapSelectionIndexRow *select_index;
    u32 *game_state;
    u32 map_flags;
    u32 state_flags;

    if ((u8)Menu_IsEquipSlotActive() != 0) {
        raw_selection = ((int (*)(int))Render_GetOrLoadFontGlyph)(*(u8 *)args[0]);
    } else {
        raw_selection = ((int (*)(void))Render_LoadFontGlyphAlt)();
    }
    selection = raw_selection;

    if (selection == 0xFF) {
        state = Menu_GetEquipSlotStateOrIndex();
        if (state < 2) {
            Menu_ResetEquipSlotState();
            D_8009D280 = Str_ParseBase32Id(D_8009CD78);
        } else {
            Menu_ResetEquipSlotState();
            D_8009D280 = Str_ParseBase32Id(D_8009CD80);
        }
        return 1;
    }

    if ((u8)(raw_selection - 6) < 2 || selection == 8) {
        if ((s16)Task_GpuPackPrimColor(0, 100) < 60) {
            D_8009D280 = Str_ParseBase32Id(g_MapIdTable);
        } else {
            D_8009D280 = Str_ParseBase32Id(g_MapIdTable + ((s16)Task_GpuPackPrimColor(1, 4) << 3));
        }
    } else {
        state = (u8)(Menu_GetEquipSlotStateOrIndex() - 1);
        state /= 10;
        select_index = g_MapSelectIndexTable + state;
        D_8009D280 = Str_ParseBase32Id(
            g_MapFilenameTable[select_index->filename_index[selection]].name);
    }

    raw_selection = 1;
    if (D_8009D1C4 == D_8009D280) {
        game_state = &g_GameState.flags;
        state_flags = D_8009D1A0;
        map_flags = game_state[0];
        D_8009D1A0 = state_flags | 0x2000;
        game_state[0] = map_flags | 0x800;
    } else {
        return raw_selection;
    }
    return raw_selection;
}

extern u8 g_MapNameOverrideA[];
extern u8 g_MapNameOverrideB[];
extern int g_SceneDispatchToken;
extern u32 g_GameStateFlags;

int Task_SetFloorByEntityRoll(u8 **args) {
    int selection;
    u8 state;
    MapSelectionIndexRow *select_index;
    u32 *game_state;
    u32 map_flags;
    u32 state_flags;

    if (!(Menu_IsEquipSlotActive() & 0xFF)) {
        Render_LoadFontGlyphAlt();
    }
    selection = ((int (*)(int))Render_SetFontGlyphByCode)(**args) & 0xFF;

    if (selection == 0xFF) {
        if ((u8)Menu_GetEquipSlotStateOrIndex() < 2) {
            Menu_ResetEquipSlotState();
            g_SceneDispatchToken = Str_ParseBase32Id(g_MapNameOverrideA);
        } else {
            Menu_ResetEquipSlotState();
            g_SceneDispatchToken = Str_ParseBase32Id(g_MapNameOverrideB);
        }
        return 1;
    }

    state = (u8)(Menu_GetEquipSlotStateOrIndex() - 1);
    state /= 10;
    select_index = g_MapSelectIndexTable + state;
    g_SceneDispatchToken =
        Str_ParseBase32Id(
            g_MapFilenameTable[select_index->filename_index[selection]].name);

    selection = 1;
    if (g_SceneDispatchCur == g_SceneDispatchToken) {
        game_state = &g_GameState.flags;
        state_flags = g_GameStateFlags;
        map_flags = game_state[0];
        g_GameStateFlags = state_flags | 0x2000;
        game_state[0] = map_flags | 0x800;
    } else {
        return selection;
    }
    return selection;
}
