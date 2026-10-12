#include "common.h"
#include "pe1/field_actor.h"
#include "pe1/player_entity.h"
#include "pe1/battle_runtime.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/entity_frame_update.h"


int Task_CopyEntityRenderObject(int **args) {
    FieldActor *source;
    int type_id;

    type_id = *args[0];
    if (type_id == 0) {
        FieldActor *player;

        player = g_PlayerEntity;
        if (player == 0) {
            return 1;
        }
        source = player;
    } else {
        source = D_8009D20C;
        if (source == 0) {
            return 1;
        }

        {
            int wanted_type;

            wanted_type = type_id;

            do {
                if (source->type_id != wanted_type) {
                    source = source->next;
                } else if (source->sub_id != *args[1]) {
                    source = source->next;
                } else if ((source->flags & 0x10) == 0) {
                    break;
                } else {
                    source = source->next;
                }
            } while (source != 0);
        }

        if (source == 0) {
            return 1;
        }
    }

    Render_InitObjectFromTable(&g_CurrentEntity->render_object,
                               &source->render_object, (s16)*args[2]);
    Render_TransformSkinnedVertices(&g_CurrentEntity->render_object,
                                    D_800B89F8);

    g_CurrentEntity->parent = source;
    g_CurrentEntity->pos_x =
        (s16)g_CurrentEntity->render_object.rotation_overrides[0].x << 16;
    g_CurrentEntity->pos_y =
        (s16)g_CurrentEntity->render_object.rotation_overrides[0].y << 16;
    g_CurrentEntity->pos_z =
        (s16)g_CurrentEntity->render_object.rotation_overrides[0].z << 16;
    g_CurrentEntity->flags |= 0x2000;

    return 1;
}
