#include "common.h"
#include "pe1/game_state.h"
#include "pe1/entity_frame_update.h"

/* Draws the scene's background object animated from the player's render
 * object. */
int Scene_UpdateBgDraw(void) {
    Pe1GameState *state = &g_GameState;
    FieldActor *entity;

    if (state->flags & 0xC0000) {
        return 0;
    }

    entity = g_PlayerEntity;
    if (entity == 0) {
        return 0;
    }

    if (entity->flags & 0x20000040) {
        return 0;
    }

    if (!(g_GameStateFlags & 2) && (g_FieldMoveLock & 2)) {
        return 0;
    }

    if (g_SceneObjectAnimationSource == 0) {
        g_SceneObjectAnimationSource = &entity->render_object;
        g_GameState.scene_object.animation_state = 3;
        g_GameState.scene_object.animation_id = 0x12;
    }

    Render_SetGteScreenOffset();
    Render_TransformVertices(&state->scene_object);
    Render_TransformMorphVertices(&state->scene_object, D_800B89F8);

    if ((g_GameStateFlags & 2) || g_PlayerEntity->render_object.variant_visible != 0) {
        Render_DrawEntity(&state->scene_object, D_800B89F8);
    }

    Render_ResetGteScreenOffset();
    return 0;
}
