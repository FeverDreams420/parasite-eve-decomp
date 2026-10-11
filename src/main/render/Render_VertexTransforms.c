/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
/* Per-frame model transforms: bone matrices from the matrix command list,
 * the skinned hit volumes and the projected vertex buffers. */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_object.h"

/* Builds the object's bone matrices from its matrix command list (-1 pushes
 * the current GTE matrix, -2 pops it, any other value is a part index) and
 * transforms the hit-sphere centres of the visible parts. */
void Render_TransformVertices(RenderObjectEntity *actor) {
    s32 *origin = (s32 *)0x1F800000;
    s32 *originY = (s32 *)0x1F800004;
    s32 *originZ = (s32 *)0x1F800008;
    RenderMatrix *stack = (RenderMatrix *)0x1F80000C;
    s8 *commands;
    RenderMatrix *out;
    RenderAnimationLookupEntry *centres;
    RenderMatrix *current;
    int mode;
    int i;

    mode = actor->animation_state;
    if (mode == 1) {
        Render_CopyFrameData(actor, actor->animation_source, actor->animation_id);
        current = &actor->model_matrix;
    } else if (mode == 3) {
        if (actor->flags_9C & 0x400) {
            actor->matrices[1].rotation[0][0] = actor->model_matrix.rotation[0][0];
            actor->matrices[1].rotation[0][1] = actor->model_matrix.rotation[0][1];
            actor->matrices[1].rotation[0][2] = actor->model_matrix.rotation[0][2];
            actor->matrices[1].rotation[1][0] = actor->model_matrix.rotation[1][0];
            actor->matrices[1].rotation[1][1] = actor->model_matrix.rotation[1][1];
            actor->matrices[1].rotation[1][2] = actor->model_matrix.rotation[1][2];
            actor->matrices[1].rotation[2][0] = actor->model_matrix.rotation[2][0];
            actor->matrices[1].rotation[2][1] = actor->model_matrix.rotation[2][1];
            actor->matrices[1].rotation[2][2] = actor->model_matrix.rotation[2][2];
            actor->matrices[1].translation[0] = actor->model_matrix.translation[0];
            actor->matrices[1].translation[1] = actor->model_matrix.translation[1];
            actor->matrices[1].translation[2] = actor->model_matrix.translation[2];
        } else {
            Render_CopyFrameDataDouble(actor, actor->animation_source, actor->animation_id);
        }
        return;
    } else {
        if (mode == 4) {
            gte_CompMatrix(&actor->animation_source->matrices[actor->animation_id],
                           &actor->model_matrix, &actor->model_matrix);
        }
        current = &actor->model_matrix;
    }

    *origin = 0;
    *originY = 0;
    out = actor->matrices;
    commands = actor->matrix_commands;
    centres = actor->animation_entries;
    gte_ldrotmatrix(current);
    gte_ldtransmatrix(current);
    for (i = 0; i < actor->header->matrix_command_bytes; i++, commands++) {
        int command = *commands;
        RenderMatrix *source;

        if (command == -1) {
            gte_ReadMatrix(stack);
            stack++;
            continue;
        }
        if (command == -2) {
            stack--;
            gte_ldrotmatrix(stack);
            gte_ldtransmatrix(stack);
            continue;
        }
        source = &actor->active_matrix[command];
        gte_ldclmv(source);
        gte_rtir();
        *originZ = actor->parts[command].translation_z;
        gte_stclmv(out);
        gte_ldclmv((u16 *)source + 1);
        gte_rtir();
        gte_stclmv((s16 *)out + 1);
        gte_ldclmv((u16 *)source + 2);
        gte_rtir();
        gte_stclmv((s16 *)out + 2);
        if (*commands == 0)
            gte_ldlv0(source->translation);
        else
            gte_ldlv0(origin);
        gte_rt();
        gte_stlvnl(out->translation);
        gte_ldrotmatrix(out);
        gte_ldtransmatrix(out);
        out++;
        if (actor->parts[*commands].visible == 1 &&
            actor->bounds_vertices[*commands * 2 + 1].pad >= 0) {
            gte_ldv0(&actor->bounds_vertices[*commands * 2]);
            gte_rt();
            gte_stsv(centres);
            centres++;
        }
    }
}

/* Places a skinned object's hit volumes and projects its origin and target
 * point; headerless objects take their transforms from their owner. */
void Render_TransformSkinnedVertices(RenderObjectEntity *actor, u32 *view) {
    u16 *offset = (u16 *)0x1F800020;
    GteMatrix *scratch = (GteMatrix *)0x1F800000;
    RenderObjectHeader *header = actor->header;

    if (!header) {
        Render_SetupBoneTransforms(actor, (s32 *)view);
        return;
    }
    gte_ldrotmatrix(&actor->matrices[header->anchor_matrix_index]);
    gte_ldtransmatrix(&actor->matrices[header->anchor_matrix_index]);
    D_8009CD9A = header->anchor_y;
    gte_ldv0(D_8009CD98);
    gte_rt();
    gte_stsv(&actor->hit_cylinder.value0);
    gte_ldrotmatrix(actor->matrices);
    gte_ldtransmatrix(actor->matrices);
    offset[0] = ((u16 *)actor->model_section14)[0] + (u16)actor->hit_body.value0;
    offset[1] = ((u16 *)actor->model_section14)[1] + (u16)actor->hit_body.value1;
    offset[2] = ((u16 *)actor->model_section14)[2] + (u16)actor->hit_body.value2;
    gte_ldv0(offset);
    gte_rt();
    gte_stsv(&actor->hit_body.value0);
    gte_ldrotmatrix(&actor->matrices[(s16)actor->table_index]);
    gte_ldtransmatrix(&actor->matrices[(s16)actor->table_index]);
    gte_ldv0(&actor->bounds_vertices[(s16)actor->table_index * 2]);
    gte_rt();
    gte_stsv(&actor->target_x);
    gte_CompMatrixMac(view, actor->matrices, scratch);
    gte_ldrotmatrix(scratch);
    gte_ldtransmatrix(scratch);
    gte_ldv0(actor->projection_origin);
    gte_rtps();
    gte_stsxy2(&actor->projected_x);
    gte_CompMatrixMac(view, &actor->matrices[(s16)actor->table_index], scratch);
    gte_ldrotmatrix(scratch);
    gte_ldtransmatrix(scratch);
    gte_ldv0(&actor->bounds_vertices[(s16)actor->table_index * 2]);
    gte_rtps();
    gte_stsxy2(&actor->projected_target_x);
}

/* Projects every vertex triple of the object's visible parts through the
 * view-composed part matrix into the screen and depth buffers. */
void Render_TransformMorphVertices(RenderObjectEntity *entity, u32 *view) {
    GteMatrix *scratch = (GteMatrix *)0x1F800000;
    RenderMatrix *matrix = entity->matrices;
    int i;

    for (i = 0; i < entity->header->part_count; i++) {
        RenderObjectPart *part = &entity->parts[i];

        gte_CompMatrixMac(view, matrix, scratch);
        gte_ldrotmatrix(scratch);
        gte_ldtransmatrix(scratch);
        matrix++;
        if (part->visible == 1) {
            RenderVec3s *vertices = &entity->vertices[part->vertex_start];
            u32 *screen = &D_800B1638[part->vertex_start];
            u32 *depth = &D_800A6360[part->vertex_start];
            int j;

            for (j = 0; j < part->vertex_count; ) {
                gte_ldv3c(vertices);
                gte_rtpt_padded();
                vertices += 3;
                j += 3;
                screen += 3;
                depth += 3;
                gte_stsxy3c(screen);
                gte_stsz3c(depth);
            }
        }
    }
}
