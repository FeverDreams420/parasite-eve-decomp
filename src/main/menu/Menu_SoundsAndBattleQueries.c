#include "pe1/akao.h"
#include "pe1/aya.h"
#include "pe1/battle.h"
#include "pe1/game_state.h"
#include "pe1/menu_sound.h"
#include "pe1/textbox.h"
#include "pe1/player_entity.h"


int Battle_GetEnemyEscapeFlag(void)
{
    Combatant *state;

    if (g_PlayerEntity != 0) {
        state = (Combatant *)g_PlayerEntity->state;
        if (state != 0) {
            return ((unsigned int)state->stateFlags >> 9) & 1;
        }
    }

    return 0;
}

int Battle_GetAgilityBonus(void)
{
    return g_AyaStatAgility[0];
}

unsigned int Aya_GetActiveTimeRate(void)
{
    return g_AyaStatActiveTimeRate[0];
}

int Battle_IsActiveWrapped(void)
{
    return (signed char)Battle_IsActive();
}

int Battle_IsInputAllowedWrapped(void)
{
    return (signed char)Pad_IsMenuConfirmAvailable();
}

int Battle_GetStateFlag1(void)
{
    return (g_GameStateFlags >> 1) & 1;
}

void Menu_CopyPromptCodes(unsigned char *src) {
    unsigned char *dst;
    unsigned char *end;
    int value;

    dst = D_80091694.glyphs;
    end = dst + 8;
    while (dst < end) {
        value = *src;
        if (value == 0xFF) {
            break;
        }
        *dst++ = value;
        src++;
    }

    D_80091694.count = dst - D_80091694.glyphs;
}

void Menu_PlayConfirmSound(void) {
    void * volatile *slot;

    slot = g_AkaoBgmHandle;
    if (*slot != 0) {
        Akao_SendTableCommand(*slot, 0x44C, 0x100, 0x80, 0x7F);
    }
}

void Menu_PlayCancelSound(void) {
    void * volatile *slot;

    slot = g_AkaoBgmHandle;
    if (*slot != 0) {
        Akao_SendTableCommand(*slot, 0x44D, 0x100, 0x80, 0x7F);
    }
}

void Menu_PlayMoveSound(void) {
    void * volatile *slot;

    slot = g_AkaoBgmHandle;
    if (*slot != 0) {
        Akao_SendTableCommand(*slot, 0x44E, 0x100, 0x80, 0x7F);
    }
}

void Menu_PlayErrorSound(void) {
    void * volatile *slot;

    slot = g_AkaoBgmHandle;
    if (*slot != 0) {
        Akao_SendTableCommand(*slot, 0x44F, 0x100, 0x80, 0x7F);
    }
}
