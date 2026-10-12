#include "pe1/game_timers.h"

int GameTime_GetZero(void)
{
    return 0;
}

unsigned int GameTime_GetCounterSeconds(int timer) {
    return (unsigned int)g_GameTimers[timer].current / 60U;
}

void GameTime_SetCounterSeconds(int timer, int seconds) {
    g_GameTimers[timer].current = seconds * 60;
}
