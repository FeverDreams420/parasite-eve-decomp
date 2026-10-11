/* Colours the vertices of an object's visible parts that lie below a height
 * limit. GTE operations and their explicit nop hazard slots use individual
 * macros. */

#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_object.h"
#include "pe1/render_buffers.h"

#define Render_LoadObjectMatrix(matrix) \
    { \
        gte_ldrotmatrix((const GteMatrixWords *)(matrix)); \
        gte_ldtransmatrix((const GteMatrixWords *)(matrix)); \
    }

#define Render_TransformVertex(src, dst)                                                           \
    {                                                                                              \
        gte_ldv0(src);                                                                             \
        gte_rt();                                                                                  \
        gte_stsv(dst);                                                                             \
    }

void Render_DrawObjectAlt(RenderObjectEntity *entity, s16 limit, u8 red, u8 green, u8 blue) {
    RenderVec3s *scratch = (RenderVec3s *)0x1F800000;
    RenderMatrix *matrix;
    int i, j;

    if (!entity->header || !entity->draw_count)
        return;
    matrix = entity->matrices;
    for (i = 0; i < entity->header->part_count; i++, matrix++) {
        RenderObjectPart *part;
        s32 y;

        if (entity->parts[i].visible != 1)
            continue;
        Render_LoadObjectMatrix(matrix);
        Render_TransformVertex(&entity->bounds_vertices[i * 2], scratch);
        part = &entity->parts[i];
        y = scratch->y;
        if (limit < y || limit < y - entity->bounds_vertices[i * 2].pad ||
            limit < y + entity->bounds_vertices[i * 2].pad) {
            RenderVec3s *vertex = &entity->vertices[part->vertex_start];
            RenderVertexColour *colour =
                (RenderVertexColour *)&D_800B1638[part->vertex_start];

            for (j = 0; j < part->vertex_count; colour++) {
                /* Retail advances the counters while the GTE works. */
                gte_ldv0(vertex);
                gte_rt();
                j++;
                vertex++;
                gte_stsv(scratch);
                if (limit < scratch->y) {
                    colour->r = red;
                    colour->g = green;
                    colour->b = blue;
                }
            }
        }
    }
    Render_UpdateClutTable(entity, 0, g_ActiveDrawSlot);
}
