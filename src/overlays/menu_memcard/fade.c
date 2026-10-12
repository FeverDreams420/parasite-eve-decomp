#include "menu_memcard_fade.h"
#include "menu_memcard_menu.h"
#include "menu_memcard_node.h"

#include "fade_externs.h"

void MenuCard_AdvanceFade(MenuMemcardFadeState *state) {
    s32 level = state->level;

    if ((level <= 0 && state->velocity < 0) ||
        (level >= 0x100 && state->velocity > 0)) {
        state->velocity = 0;
    }
    state->level += state->velocity;
}

void MenuCard_UpdateFadeDisplay(MenuMemcardFadeState *state) {
    {
        s32 velocity = state->velocity;
        s32 increase = 0;
        s32 level;

        if (velocity != 0) {
            increase = velocity < 0x54;
        }
        level = state->level;
        velocity += increase;
        state->velocity = velocity;
        if (level < 0x100) {
            level += state->increment << 3;
        }
        state->level = level;
        if (level == 0 || level == 0x100) {
            state->increment = 0;
        }
    }
    {
        s32 current = state->velocity;
        s32 top = 0x3C - current;

        if (top >= 0) {
            top = current + 0x50;
        } else {
            top = 0x8C;
        }
        state->displayTop = top;
    }
    {
        s32 current = state->velocity;
        s32 delta = 0x54 - current;
        s32 bottom;

        /* Keep the floor constant in the branch delay slot. */
                if (delta >= 0x18) {
            bottom = delta;
        } else {
            bottom = 0x18;
        }
        state->displayBottom = bottom;
    }
}

void MenuCard_DecreaseFade(MenuMemcardFadeState *state) {
    s32 level = state->level - 0x10;

    if (level < 0) {
        level = 0;
    }
    state->level = level;
    if (level == 0) {
        state->increment = 0;
        state->velocity = 0;
    }
}

void func_8012B6F0(void) {
    func_8018FBC0(2);
}

void func_8012B710(void) {
    MenuMemcardListNode *node;
    int kind;

    node = ((MenuMemcardListNode *)D_801D1370);
    kind = 2;
    while (node != 0) {
        if (node->kind == kind) {
            break;
        }
        node = node->next;
    }

    node->payload = &D_80193200;
}

void func_8018FE1C(void);

void func_8012B754(MenuMemcardFadeState *state) {
    int step_value;

    step_value = state->level - 0x10;
    state->level = step_value;

    if (step_value == 0x80) {
        func_8018FE1C();
    }

    if (state->level == 0) {
        state->finished = 1;
    }
}
