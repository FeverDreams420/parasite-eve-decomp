#include "common.h"
#include "pe1/game_state.h"

/* Scene audio slots 0 and 1: each caches a track key, its bank and a volume,
 * and registering one marks that slot's track as loaded in the state flags. */
int Overlay_RegisterAudioSlot(int slot, int key, int bank, int volume) {
    Pe1GameState *state = &g_GameState;

    state->scene_audio.tracks.keys[slot][0] = key;
    state->scene_audio.tracks.keys[slot][1] = bank;
    state->track_volumes[slot] = volume;

    if (slot == 0) {
        state->flags |= 0x40;
    } else if (slot == 1) {
        state->flags |= 0x80;
    }

    return 0;
}

int Overlay_GetAudioSlotByKey(int key) {
    Pe1GameState *state = &g_GameState;
    int i;

    for (i = 0; i < 2; i++) {
        if (state->scene_audio.tracks.keys[i][0] == key) {
            return state->scene_audio.tracks.keys[i][1];
        }
    }

    return -1;
}

int Overlay_FindAudioSlotIndex(int key) {
    Pe1GameState *state = &g_GameState;
    int i = 0;

    do {
        if (state->scene_audio.tracks.keys[i][0] == key) {
            return i;
        }
        i++;
    } while (i < 2);

    return -1;
}
