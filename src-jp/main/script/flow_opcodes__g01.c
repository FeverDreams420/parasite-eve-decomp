/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/battle_update.h"
#include "pe1/entity_allocation.h"
#include "pe1/entity_animation.h"
#include "pe1/field_actor.h"
#include "pe1/field_collision.h"
#include "pe1/field_sfx.h"
#include "pe1/game_state.h"
#include "pe1/game_state_types.h"
#include "pe1/geom_state.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_state.h"
#include "pe1/player_entity.h"
#include "pe1/render_lighting.h"
#include "pe1/render_shadow.h"
#include "pe1/scene_room_load.h"
#include "pe1/scene_transition.h"
#include "pe1/script_node.h"
#include "pe1/script_queue.h"
#include "pe1/script_state.h"
#include "pe1/textbox.h"
#include "pe1/vector_types.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
u32 *g_ScriptCursor;
ScriptNode *g_ScriptNodePool;
u8 g_ScriptCameraBytes[3];
int g_ScriptCursorAddress;

typedef struct ScriptArgParts {
    s16 low;
    signed int high : 16;
} ScriptArgParts;

typedef struct ScriptCmdArgs {
    int *opcode;
    ScriptArgParts *arg1;
    ScriptArgParts *arg2;
    ScriptArgParts *arg3;
    ScriptArgParts *arg4;
    ScriptArgParts *arg5;
} ScriptCmdArgs;

#define ARG_WORD(ptr) (*(int *)(ptr))
#define ARG_BYTE(ptr) (*(u8 *)(ptr))

void FieldFx_SpawnScriptSprite(Pe1Vec3s *position, int mode, int arg2, int arg3,
                   int arg4, int arg5, int arg6);

void Menu_OpenSaveLoadEntryPoint(int arg0);

/* Script interpreter main loop and the jump-to-actor-script opcode; a
 * contiguous -G8 pair sharing the script cursor g_ScriptCursor. */

PE1_STATIC_ASSERT(sizeof(ScriptNode) == 0x2C, interpreter_script_node_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(ScriptNode, current) == 0, interpreter_script_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(ScriptNode, active) == 0x10, interpreter_wait_offset);

/* Tentative COMMON declarations preserve small-data metadata for stock
 * maspsx. Storage remains at the existing data/linker addresses. */
ScriptNode *g_ScriptNodePool;
u32 *g_ScriptCursor;

typedef struct {
    int value;
} ScriptJumpOffsetEntry;

int Math_FixedMul(int arg0, int arg1);

/* Script opcodes: random value in a range and a field sound effect on the
 * current actor. Contiguous default-profile handlers. */

int Draw_PackPrimColor(int start, int end);

int Draw_GetTextboxPageState(short arg0);



int Script_OpenSaveLoadMenu(int **arg0) {
    Pe1GameState *state = &g_GameState;
    ScriptNode *node;
    int flags;

    if (!(state->flags & 0x1000)) {
        node = g_ScriptNodePool;
        flags = node->flags;
        if (!(flags & 0x20)) {
            node->flags = flags | 0x20;
            node->active = 1;
            g_ScriptCursorAddress -= 0xC;
            return 0;
        }
        Render_BeginSceneLoad();
        Menu_OpenSaveLoadEntryPoint(**arg0);
        state->flags |= 0x9000;
        g_GameStateFlags |= 4;
        g_ScriptNodePool->flags &= 0xFFDF;
    }
    return 1;
}
