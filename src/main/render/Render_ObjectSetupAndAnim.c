#include "common.h"
#include "pe1/render_setup.h"
#include "pe1/entity_frame_update.h"
#include "pe1/render_matrix_copy.h"
#include "pe1/gte.h"

#include "pe1/render_buffers.h"

#define NULL ((void *)0)

/* Texture-page transition deltas, indexed by old and requested page. */

/* Moves every packet of the object to texture page `page`: the current page
 * is read from the first packet and the per-page delta added to all. */
void Render_SetObjectTexturePage(RenderObjectEntity *obj, s32 page) {
    int slot = g_ActiveDrawSlot;
    int current = 0;
    RenderPrimCursor prim;
    int delta;
    int i;

    if (obj->header == NULL || obj->draw_count == 0)
        return;
    prim.bytes = obj->primitive_buffer;
    if (obj->header->packet34_count != 0)
        current = (prim.p34[slot].page_bits & 0x7F) >> 5;
    else if (obj->header->packet28_count != 0)
        current = (prim.p28[slot].page_bits & 0x7F) >> 5;
    if ((s16)page == current)
        return;
    delta = g_RenderTexturePageDeltas[current][(s16)page];
    for (i = 0; i < obj->header->packet34_count; i++) {
        RenderPacket34 *packet = &prim.p34[slot];

        packet->page_bits += delta;
        prim.p34 += 2;
    }
    for (i = 0; i < obj->header->packet28_count; i++) {
        RenderPacket28 *packet = &prim.p28[slot];

        packet->page_bits += delta;
        prim.p28 += 2;
    }
}

/* Parses a model block into the render object: section pointers, the
 * double-buffered GPU packets (command codes and texture coordinates), the
 * animated part lookup entries and one identity matrix per part. */
int Render_SetupEntityPrims(RenderObjectEntity *obj, RenderObjectHeader *model,
                            u8 *packetBuffer, s16 x, s16 y, s16 unused,
                            s16 paletteRow, s16 initCount, s8 **textureOut,
                            int setup)
{
    RenderModelCursor src;
    RenderPrimitiveDescriptor *desc;
    RenderPrimCursor prim;
    RenderModelCursor cursor;
    RenderObjectHeader *header;
    RenderAnimationLookupEntry *entry;
    RenderMatrix *matrix;
    int bytes, words;
    s16 skipped;
    s16 i, j;

    cursor.header = model;
    obj->header = model;
    header = model;
    i = 0;
    skipped = 0;
    cursor.header++;
    obj->parts = cursor.parts;
    cursor.parts += model->part_count;
    obj->vertices = cursor.vectors;
    cursor.vectors += header->vertex_count;
    obj->vertex_colours = cursor.colours;
    cursor.colours += header->vertex_count;
    obj->primitive_descriptors = cursor.descriptors;
    prim.bytes = packetBuffer;
    obj->primitive_buffer = packetBuffer;
    desc = cursor.descriptors;
    obj->draw_count = setup;

    if (setup) {
        for (; i < header->packet34_count; i++, desc++) {
            prim.head->length = 12;
            prim.head->colour.bytes.command = 0x3C;
            if (desc->kind == 0x0B)
                prim.head->colour.bytes.command = 0x3E;
            prim.p34++;
            prim.head->length = 12;
            prim.head->colour.bytes.command = 0x3C;
            if (desc->kind == 0x0B)
                prim.head->colour.bytes.command = 0x3E;
            prim.p34++;
        }
        for (i = 0; i < header->packet28_count; i++, desc++) {
            prim.head->length = 9;
            prim.head->colour.bytes.command = 0x34;
            if (desc->kind == 0x10)
                prim.head->colour.bytes.command = 0x36;
            prim.p28++;
            prim.head->length = 9;
            prim.head->colour.bytes.command = 0x34;
            if (desc->kind == 0x10)
                prim.head->colour.bytes.command = 0x36;
            prim.p28++;
        }
        for (i = 0; i < header->packet24_count; i++, desc++) {
            prim.head->length = 8;
            prim.head->colour.bytes.command = 0x38;
            if (desc->kind == 0x15)
                prim.head->colour.bytes.command = 0x3A;
            prim.p24++;
            prim.head->length = 8;
            prim.head->colour.bytes.command = 0x38;
            if (desc->kind == 0x15)
                prim.head->colour.bytes.command = 0x3A;
            prim.p24++;
        }
        for (i = 0; i < header->packet1c_count; i++, desc++) {
            prim.head->length = 6;
            prim.head->colour.bytes.command = 0x30;
            if (desc->kind == 0x1A)
                prim.head->colour.bytes.command = 0x32;
            prim.p1c++;
            prim.head->length = 6;
            prim.head->colour.bytes.command = 0x30;
            if (desc->kind == 0x1A)
                prim.head->colour.bytes.command = 0x32;
            prim.p1c++;
        }
    } else {
        desc += header->packet34_count + header->packet28_count
              + header->packet24_count + header->packet1c_count;
    }
    cursor.descriptors = desc;

    obj->model_section14 = cursor.vectors;
    cursor.vectors += 2;
    obj->bounds_vertices = cursor.vectors;
    cursor.vectors += header->part_count * 2;
    obj->projection_origin = cursor.vectors;
    cursor.vectors++;
    obj->matrix_commands = cursor.commands;
    /* Matrix command block, rounded up to whole words. */
    bytes = header->matrix_command_bytes;
    words = (u16)bytes;
    words >>= 2;
    if ((bytes & 3) > 0) {
        bytes = words;
        bytes++;
        bytes *= 4;
    } else {
        bytes = words;
        bytes *= 4;
    }
    cursor.commands += bytes;

    if (setup) {
        RenderPrimCursor out;

        src = cursor;
        *textureOut = src.commands;
        out.bytes = obj->primitive_buffer;
        for (i = 0; i < header->packet34_count; i++) {
            src.quad = cursor.quad;

            for (j = 0; j < 2; j++) {
                out.p34->u0 = src.quad->u0;
                out.p34->v0 = src.quad->v0;
                out.p34->u1 = src.quad->u1;
                out.p34->v1 = src.quad->v1;
                out.p34->u2 = src.quad->u2;
                out.p34->v2 = src.quad->v2;
                out.p34->u3 = src.quad->u3;
                out.p34->v3 = src.quad->v3;
                out.p34->clut = src.quad->clut;
                out.p34->page_bits = src.quad->page_bits;
                out.p34++;
            }
            cursor.quad++;
        }
        for (i = 0; i < header->packet28_count; i++) {
            src.tri = cursor.tri;

            for (j = 0; j < 2; j++) {
                out.p28->u0 = src.tri->u0;
                out.p28->v0 = src.tri->v0;
                out.p28->u1 = src.tri->u1;
                out.p28->v1 = src.tri->v1;
                out.p28->u2 = src.tri->u2;
                out.p28->v2 = src.tri->v2;
                out.p28->clut = src.tri->clut;
                out.p28->page_bits = src.tri->page_bits;
                out.p28++;
            }
            cursor.tri++;
        }
        for (i = 0; i < header->packet24_count; i++) {
        }
        for (i = 0; i < header->packet1c_count; i++) {
        }
        if (initCount > 0)
            Render_InitPrimBlock(obj, x, y, unused, paletteRow);
    }

    {
        RenderVec3s *section = obj->model_section14;

        obj->hit_cylinder.radius = section->pad;
    }
    obj->hit_cylinder.bounds_value1 = obj->projection_origin->pad;
    obj->animation_entries = (RenderAnimationLookupEntry *)prim.head;
    entry = (RenderAnimationLookupEntry *)prim.head;
    for (i = 0; i < obj->header->part_count; i++) {
        if (obj->parts[i].visible == 1) {
            RenderVec3s *bounds = &obj->bounds_vertices[i * 2];

            if (bounds[1].pad >= 0) {
                entry->radius = bounds[0].pad;
                entry->bounds_value1 = bounds[1].pad;
                entry->animation_id = i;
                entry++;
            }
        } else {
            skipped++;
        }
    }

    obj->matrices = (RenderMatrix *)entry;
    matrix = (RenderMatrix *)entry;
    for (i = 0; i < obj->header->part_count;) {
        i++;
        matrix->rotation[0][0] = 0x1000;
        matrix->rotation[0][1] = 0;
        matrix->rotation[0][2] = 0;
        matrix->rotation[1][0] = 0;
        matrix->rotation[1][1] = 0x1000;
        matrix->rotation[1][2] = 0;
        matrix->rotation[2][0] = 0;
        matrix->rotation[2][1] = 0;
        matrix->rotation[2][2] = 0x1000;
        matrix->translation[0] = 0;
        matrix->translation[1] = 0;
        matrix->translation[2] = 0;
        matrix++;
    }

    obj->header->shadow_radius = 0;
    obj->table_value2c = 0;
    obj->table_value2e = 0;
    obj->table_value30 = 0;
    obj->table_index = 1;
    obj->table_value2c = 0;
    obj->table_value2e = 0;
    obj->table_value30 = 0;
    RotMatrix((GteShortVector *)&obj->table_value2c, (GteMatrix *)&obj->model_matrix);
    obj->fade_remaining = -1;
    Anim_SetInterpRate(obj, 0x32);
    obj->primitive_red = 0x80;
    obj->primitive_green = 0x80;
    obj->primitive_blue = 0x80;
    obj->flags_9C = 0;
    obj->variant_visible = 1;
    obj->reserved9f = D_8009CDDC;
    obj->animation_state = 0;
    obj->animation_source = 0;
    obj->animation_id = 0;
    obj->header->visible_part_count = obj->header->vertex_count - skipped;
    obj->hit_cylinder.animation_id = obj->projection_origin->y + (obj->hit_cylinder.bounds_value1 >> 4);
    for (i = 0; i < 2; i++) {
        obj->rotation_overrides[i].matrix_index = 0;
        obj->rotation_overrides[i].flags = 0;
    }
    obj->animation_data = 0;
    return 1;
}

int Util_ReturnTrue(void *unused);

int Util_ReturnTrue(void *unused) {
    return 1;
}

/* Draws an object with an optional animation frame applied on top of its
 * saved model matrix, then refreshes the packets of both draw buffers. */
void Render_DrawWithAnim(RenderObjectEntity *obj, void *animation, s16 frame,
                         union RenderLightingMatrix *light) {
    if (animation != 0) {
        obj->animation_data = animation;
        Render_CopyMatrixBlock(&obj->model_matrix, (RenderMatrix *)g_RenderClutLookupTable, 1);
        Render_CopyMatrixBlock((RenderMatrix *)g_IdentityMatrixSource, &obj->model_matrix, 1);
        Anim_BuildRotationMatrices(obj, animation, frame, 0);
    }
    Render_TransformVertices(obj);
    Render_CopyMatrixBlock((RenderMatrix *)g_RenderClutLookupTable, &obj->model_matrix, 1);
    Render_DrawObject(obj, light);
    Render_UpdateClutTable(obj, 1, g_ActiveDrawSlot);
    g_ActiveDrawSlot ^= 1;
    Render_UpdateClutTable(obj, 1, g_ActiveDrawSlot);
    g_ActiveDrawSlot ^= 1;
}

/* Equivalent branches preserve the stock compiler\'s register allocation.
 * No packet operation depends on which of these identical branches is taken. */

void Render_InitPrimBlock(RenderObjectEntity *object, s16 x, s16 y, int unused, unsigned int palette_row)
{
    unsigned int page;
    int page_offset;
    unsigned int scaled;
    u16 signed_page;
    unsigned int palette_offset;
    unsigned int row_copy;
    s16 half_page;
    int flag_copy;
    RenderPacket34 *quad;
    RenderPacket28 *tri;
    s16 i;
    s16 side;
    half_page = y == 128;
    page = ((((x >> 6) - 8) * 2) + (y >> 7)) + ((y >> 8) * 30);
    signed_page = page;
    row_copy = palette_row;
    quad = (RenderPacket34 *) object->primitive_buffer;
    if (x != 960)
    {
        page_offset = page >> 1;
        scaled = palette_row * 64;
    }
    else
    {
        if (quad->page_bits == 31)
        {
            return;
        }
        if (quad)
        {
            page_offset = ((int) (((unsigned int) signed_page) << 16)) >> 17;
        }
        else
        {
            page_offset = ((int) (((unsigned int) signed_page) << 16)) >> 17;
        }
        if (quad)
        {
            scaled = row_copy * 64;
        }
        else
        {
            scaled = row_copy * 64;
        }
    }
    signed_page = page;
    if (signed_page)
    {
        palette_offset = scaled - 0x7080;
    }
    else
    {
        palette_offset = scaled - 0x7080;
    }
    for (i = 0; i < object->header->packet34_count; i++)
    {
        flag_copy = half_page;
        for (side = 0; side < 2; side++, quad++)
        {
            if (flag_copy)
            {
                u8 v = quad->v0;
                if (v < 128)
                {
                    quad->v0 = v + 128;
                    quad->v1 += 128;
                    quad->v2 += 128;
                    quad->v3 += 128;
                    quad->page_bits += page_offset;
                }
                else
                    if (object)
                {
                    quad->v0 = v + 128;
                    quad->v1 += 128;
                    quad->v2 += 128;
                    quad->v3 += 128;
                    quad->page_bits += page_offset + 1;
                }
                else
                {
                    quad->v0 = v + 128;
                    quad->v1 += 128;
                    quad->v2 += 128;
                    quad->v3 += 128;
                    quad->page_bits += page_offset + 1;
                }
            }
            else
            {
                quad->page_bits += page_offset;
            }
            quad->clut += palette_offset;
        }

    }

    tri = (RenderPacket28 *) quad;
    for (i = 0; i < object->header->packet28_count; i++)
    {
        for (side = 0; side < 2; side++, tri++)
        {
            if (half_page)
            {
                u8 v = tri->v0;
                if (v < 128)
                {
                    tri->v0 = v + 128;
                    tri->v1 += 128;
                    tri->v2 += 128;
                    scaled = palette_row * 64;
                    tri->page_bits += page_offset;
                }
                else
                {
                    tri->v0 = v + 128;
                    tri->v1 = tri->v1 + 128;
                    tri->v2 += 128;
                    tri->page_bits += page_offset + 1;
                }
            }
            else
            {
                tri->page_bits += page_offset;
            }
            tri->clut += palette_offset;
        }

    }

}

void Render_CopyFrameData(RenderObjectEntity *dst, RenderObjectEntity *src, s32 frame) {
    s16 index = frame;

    dst->model_matrix.rotation[0][0] = src->matrices[index].rotation[0][0];
    dst->model_matrix.rotation[0][1] = src->matrices[index].rotation[0][1];
    dst->model_matrix.rotation[0][2] = src->matrices[index].rotation[0][2];
    dst->model_matrix.rotation[1][0] = src->matrices[index].rotation[1][0];
    dst->model_matrix.rotation[1][1] = src->matrices[index].rotation[1][1];
    dst->model_matrix.rotation[1][2] = src->matrices[index].rotation[1][2];
    dst->model_matrix.rotation[2][0] = src->matrices[index].rotation[2][0];
    dst->model_matrix.rotation[2][1] = src->matrices[index].rotation[2][1];
    dst->model_matrix.rotation[2][2] = src->matrices[index].rotation[2][2];
    dst->model_matrix.translation[0] = src->matrices[index].translation[0];
    dst->model_matrix.translation[1] = src->matrices[index].translation[1];
    dst->model_matrix.translation[2] = src->matrices[index].translation[2];
}

void Render_CopyFrameDataDouble(RenderObjectEntity *dst, RenderObjectEntity *src, s32 frame) {
    s16 index = frame;

    dst->matrices[0].rotation[0][0] = src->matrices[index].rotation[0][0];
    dst->matrices[0].rotation[0][1] = src->matrices[index].rotation[0][1];
    dst->matrices[0].rotation[0][2] = src->matrices[index].rotation[0][2];
    dst->matrices[0].rotation[1][0] = src->matrices[index].rotation[1][0];
    dst->matrices[0].rotation[1][1] = src->matrices[index].rotation[1][1];
    dst->matrices[0].rotation[1][2] = src->matrices[index].rotation[1][2];
    dst->matrices[0].rotation[2][0] = src->matrices[index].rotation[2][0];
    dst->matrices[0].rotation[2][1] = src->matrices[index].rotation[2][1];
    dst->matrices[0].rotation[2][2] = src->matrices[index].rotation[2][2];
    dst->matrices[0].translation[0] = src->matrices[index].translation[0];
    dst->matrices[0].translation[1] = src->matrices[index].translation[1];
    dst->matrices[0].translation[2] = src->matrices[index].translation[2];

    dst->matrices[1].rotation[0][0] = src->matrices[index].rotation[0][0];
    dst->matrices[1].rotation[0][1] = src->matrices[index].rotation[0][1];
    dst->matrices[1].rotation[0][2] = src->matrices[index].rotation[0][2];
    dst->matrices[1].rotation[1][0] = src->matrices[index].rotation[1][0];
    dst->matrices[1].rotation[1][1] = src->matrices[index].rotation[1][1];
    dst->matrices[1].rotation[1][2] = src->matrices[index].rotation[1][2];
    dst->matrices[1].rotation[2][0] = src->matrices[index].rotation[2][0];
    dst->matrices[1].rotation[2][1] = src->matrices[index].rotation[2][1];
    dst->matrices[1].rotation[2][2] = src->matrices[index].rotation[2][2];
    dst->matrices[1].translation[0] = src->matrices[index].translation[0];
    dst->matrices[1].translation[1] = src->matrices[index].translation[1];
    dst->matrices[1].translation[2] = src->matrices[index].translation[2];
}



/* Bounds vectors come in pairs per table entry; the first one stores the
 * hit radius in its pad word. */
void Render_InitObjectFromTable(RenderObjectEntity *obj, RenderObjectEntity *owner, int index) {
    obj->table_index = index;
    obj->header = 0;
    obj->animation_source = owner;
    obj->hit_cylinder.radius = owner->bounds_vertices[(short)index * 2].pad;
    obj->table_value2c = owner->bounds_vertices[(short)index * 2].x;
    obj->table_value2e = owner->bounds_vertices[(short)index * 2].y;
    obj->table_value30 = owner->bounds_vertices[(short)index * 2].z + obj->hit_cylinder.radius;
}

void Render_Noop(void) {
}

int Render_ReturnZero(void) {
    return 0;
}

void Render_CopyMatrixBlock(RenderMatrix *src, RenderMatrix *dst, s16 count) {
    int i;

    for (i = 0; i < count; i++, src++, dst++) {
        dst->rotation[0][0] = src->rotation[0][0];
        dst->rotation[0][1] = src->rotation[0][1];
        dst->rotation[0][2] = src->rotation[0][2];
        dst->rotation[1][0] = src->rotation[1][0];
        dst->rotation[1][1] = src->rotation[1][1];
        dst->rotation[1][2] = src->rotation[1][2];
        dst->rotation[2][0] = src->rotation[2][0];
        dst->rotation[2][1] = src->rotation[2][1];
        dst->rotation[2][2] = src->rotation[2][2];
        dst->translation[0] = src->translation[0];
        dst->translation[1] = src->translation[1];
        dst->translation[2] = src->translation[2];
    }
}

void Render_SetObjectAnim(RenderObjectEntity *arg0, RenderObjectEntity *arg1, short arg2) {
    if (arg0->header->part_count == 2) {
        arg0->animation_source = arg1;
        arg0->animation_state = 3;
    } else {
        arg0->animation_source = arg1;
        arg0->animation_state = 1;
    }
    arg0->animation_id = arg2;
}

void Render_ClearObjectAnim(RenderObjectEntity *arg0) {
    arg0->animation_source = 0;
    if (arg0->header->part_count == 2) {
        arg0->animation_state = 2;
    } else {
        arg0->animation_state = 0;
    }
}

int Render_FindAnimEntry(RenderObjectEntity *obj, int id, s32 *out) {
    RenderAnimationLookupEntry *entry = obj->animation_entries;
    int i;

    for (i = 0; i < obj->header->animation_entry_count; i++, entry++) {
        if (entry->animation_id == (s16)id) {
            out[0] = entry->value0;
            out[1] = entry->value1;
            out[2] = entry->value2;
            return 1;
        }
    }
    return 0;
}

/* Matching debt: transfer and scratch pointers are pinned, with empty
 * address/memory constraints. Physical scratchpad addresses and pointer
 * casts remain. CPU loads, stores and vector packing are C; GTE transfers
 * and commands use individual macros. */
#define BoneLoadRotMatrix(matrix) gte_ldrotmatrix((const GteMatrixWords *)(matrix))

#define BoneLoadTrans(matrix) gte_ldtransmatrix((const GteMatrixWords *)(matrix))

#define BoneLoadFullMatrix(matrix)                                                                 \
    {                                                                                              \
        BoneLoadRotMatrix(matrix);                                                                 \
        BoneLoadTrans(matrix);                                                                     \
    }

void Render_SetupBoneTransforms(RenderObjectEntity *actor, s32 *view) {
    RenderMatrix *scratch = (RenderMatrix *)0x1F800000;
    RenderObjectEntity *source = actor->animation_source;
    int index = (s16)actor->table_index;
    RenderMatrix *matrix = &source->matrices[index];

    BoneLoadFullMatrix(matrix);
    gte_ldv0(&source->bounds_vertices[index * 2]);
    gte_rt();
    /* Headerless objects reuse the first override XYZ as a position output. */
    gte_stsv(&actor->rotation_overrides[0].x);
    gte_stsv(&actor->hit_cylinder.value0);
    gte_stsv(&actor->target_x);
    gte_CompMatrixMac(view, matrix, scratch);
    BoneLoadFullMatrix(scratch);
    /* In this mode the halfwords at +0x2C form the projection input vector. */
    gte_ldv0(&actor->table_value2c);
    gte_rtps();
    actor->hit_body.radius = source->hit_body.radius;
    gte_stsxy2(&actor->projected_target_x);
    actor->table_value2c += actor->hit_cylinder.radius;
    gte_ldv0(&actor->table_value2c);
    gte_rtps();
    actor->hit_body.value0 = source->hit_body.value0;
    actor->hit_body.value1 = source->hit_body.value1;
    actor->hit_body.value2 = source->hit_body.value2;
    gte_stsxy2(&actor->projected_x);
    actor->table_value2c -= actor->hit_cylinder.radius;
}

void Render_OffsetObjectTextureCoordinates(RenderObjectEntity *object, int du, int dv, int clutOffset) {
    u8 savedClut = clutOffset;
    RenderPacket34 *quad = (RenderPacket34 *)object->primitive_buffer;
    RenderPacket28 *tri;
    int i, j;
    u8 triClut;
    for (i = 0; i < object->header->packet34_count; i++) {
        for (j = 0; j < 2; j++, quad++) {
            quad->v0 += dv;
            quad->u0 += du;
            quad->v1 += dv;
            quad->u1 += du;
            quad->v2 += dv;
            quad->u2 += du;
            /* Retail leaves u3 unchanged. */
            quad->v3 += dv;
            quad->clut += (s8)clutOffset;
        }
    }
    triClut = savedClut;
    /* Byte-sized copies preserve the two signed CLUT conversions in plain C. */
    tri = (RenderPacket28 *)quad;
    for (i = 0; i < object->header->packet28_count; i++) {
        for (j = 0; j < 2; j++, tri++) {
            tri->v0 += dv;
            tri->u0 += du;
            tri->v1 += dv;
            tri->u1 += du;
            tri->v2 += dv;
            tri->u2 += du;
            tri->clut += (s8)triClut;
        }
    }
}


/* The counter word sits in .text right after this routine (main.yaml
 * render/Render_Counter) and the routine works in $a0/$a1, both signs of a
 * hand-written assembler original; the two pins reproduce those registers. */
void Render_IncrementCounter(void) {
    register int *ptr asm("$4");
    register int value asm("$5");

    ptr = &g_RenderCounter;
    value = *ptr;
    value++;
    *ptr = value;
}
