/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
#include "pe1/scene_transition.h"
#include "pe1/task_node.h"
#include "pe1/game_state_types.h"
#include "pe1/game_state.h"
#include "pe1/battle_runtime.h"

/* COMMON supplies small-data metadata to MASPSX; the extern words stay
 * absolute. */
int g_SceneDataTable0;
TaskNode *g_TaskNodePool;

void Menu_OpenEquipScreen(int arg0);

int Task_BeginSceneTransition(int **arg0) {
    Pe1GameState *state = &g_GameState;
    int **saved = arg0;
    TaskNode *node;
    unsigned short *node_flags;
    int flags;
    int cursor;

    if (!(state->flags & 0x1000)) {

    node = g_TaskNodePool;
    node_flags = &node->flags;
    flags = *node_flags;
    if ((flags & 0x20) == 0) {
        *node_flags = flags | 0x20;
        cursor = g_SceneDataTable0;
        node->active = 1;
        cursor -= 0xC;
        g_SceneDataTable0 = cursor;
        return 0;
    }

    Render_BeginSceneLoad();
    Menu_OpenEquipScreen(**saved);
    {
        TaskNode *tail_node = g_TaskNodePool;
        int tail_flags;

        state->flags |= 0x9000;
        tail_flags = tail_node->flags;
        g_GameStateFlags |= 4;
        tail_flags &= 0xFFDF;
        tail_node->flags = tail_flags;
    }

    }
ret_one:
    return 1;
}

void Menu_OpenSaveLoadEntryPoint(int arg0);

int Task_OpenSaveLoadMenu(int **arg0) {
    Pe1GameState *state = &g_GameState;
    int **saved = arg0;
    TaskNode *node;
    int flags;
    int cursor;

    if (!(state->flags & 0x1000)) {

    node = g_TaskNodePool;
    flags = node->flags;
    if (flags & 0x20) {
        Render_BeginSceneLoad();
        Menu_OpenSaveLoadEntryPoint(**saved);
        {
            TaskNode *tail_node = g_TaskNodePool;
            int tail_flags;

            state->flags |= 0x9000;
            tail_flags = tail_node->flags;
            g_GameStateFlags |= 4;
            tail_flags &= 0xFFDF;
            tail_node->flags = tail_flags;
        }
    } else {
        cursor = g_SceneDataTable0;
        node->flags = flags | 0x20;
        cursor -= 0xC;
        g_SceneDataTable0 = cursor;
    }

    {
        TaskNode *mark_node = g_TaskNodePool;
        int one = 1;

        mark_node->active = one;
    }

    }
ret_zero:
    return 0;
}
