#include "pe1/game_timers.h"
#include "pe1/save.h"

/* Restarts the game clock, counting up to 100 hours of frames. */
void GameTime_InitCounter0(void) {
    int value;

    value = 100 * 60 * 60 * 60;
    g_GameClockLimit = 0;
    g_GameClockLimit = value;
    value = 1;
    g_GameTimeTable = 0;
    D_800A76A0 = 0;
    D_800A76A0 = value;
}

/* Restarts the play time counter with no limit. */
void GameTime_InitCounter2(void) {
    int value;

    value = 1;
    g_PlayTimeSeconds = 0;
    g_PlayTimeLimit = 0;
    g_PlayTimeFlags = value;
}

void GameTime_InitCounter1(void) {
    int value;

    value = 1;
    D_800A76B0[0] = 0;
    g_GameTimer1Limit = 0;
    g_GameTimer1Flags = value;
}

/* Packed clock word with 20/6/6-bit fields, each filled from src[1]. */
typedef struct PackedTime {
    u32 frames : 20;
    u32 minutes : 6;
    u32 seconds : 6;
} PackedTime;

void Util_PackTime(u32 *src, PackedTime *out) {
    out->frames = src[1] % 60 * 60 * 60 * 60;
    out->minutes = src[1] % 60 * 60 * 60;
    out->seconds = src[1] % 60 * 60;
}
