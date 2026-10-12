/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/field_rotated_triangle.h"
#include "pe1/field_history_trail.h"
#include "pe1/field_tile.h"
#include "pe1/geom_state.h"
#include "pe1/field_rotated_quad.h"
#include "pe1/render_object.h"
#include "pe1/field_glow_line.h"
#include "pe1/field_textured_ribbon.h"
#include "pe1/render_prim.h"
#include "pe1/field_pulled_sprite.h"

/* Triangle with its apex at `position` and a `breadth` wide base `length`
 * away along local Z, turned by `rotation` and shaded per corner from
 * color0..color2 at intensity/128; mode 0xFF draws it opaque, anything
 * else adds a semi-transparent draw mode.
 * The camera and local matrix uploads are gte_ldrotmatrix and
 * gte_ldtransmatrix; the corners go to the GTE through gte_ldv3. */
void func_800D0E88(GteShortVector *position, GteRotation *rotation, int length,
                   int breadth, RenderColor *color0, RenderColor *color1,
                   RenderColor *color2, int intensity, int mode)
{
    GteShortVector corners[3];
    RenderColor black;
    GteMatrix matrix;
    u32 depth;
    FieldG3Packet *packet;
    RenderTintMode *drawMode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    GteMatrix *view;
    int bias;

    black = D_800C22A0;
    bias = (u16)D_800F3368.depth;
    /* Keep intensity in s6 without pinning the later draw-mode lifetime. */
    
    packet = (FieldG3Packet *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(FieldG3Packet);
    view = D_800BCFA4.value;
    {
        const GteMatrixWords *words = (const GteMatrixWords *)(view);
        gte_ldrotmatrix(words);
        gte_ldtransmatrix(words);
    }
    gte_lwc2_0_0(position);
    gte_lwc2_1_4(position);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();
    corners[1].x = -breadth / 2;
    corners[1].y = corners[2].y = 0;
    corners[0].x = corners[0].y = corners[0].z = 0;
    corners[1].z = corners[2].z = length;
    corners[2].x = breadth / 2;
    {
        s32 *translation = matrix.t;
        gte_swc2_25_0(translation);
        gte_swc2_26_4(translation);
        gte_swc2_27_8(translation);
    }
    RotMatrixYXZ((GteShortVector *)rotation, &matrix);
    MulRotMatrix(&matrix);
    {
        const GteMatrixWords *words = (const GteMatrixWords *)(&matrix);
        gte_ldrotmatrix(words);
        gte_ldtransmatrix(words);
    }
    gte_ldv3(&corners[0], &corners[1], &corners[2]);
    gte_rtpt_padded();
    SetPolyG3(packet);
    if (color0 == 0) {
        color0 = &black;
    }
    packet->r0 = color0->r * (s16)intensity / 128;
    packet->g0 = color0->g * (s16)intensity / 128;
    packet->b0 = color0->b * (s16)intensity / 128;
    if (color1 == 0) {
        color1 = &black;
    }
    packet->r1 = color1->r * (s16)intensity / 128;
    packet->g1 = color1->g * (s16)intensity / 128;
    packet->b1 = color1->b * (s16)intensity / 128;
    if (color2 == 0) {
        color2 = &black;
    }
    packet->r2 = color2->r * (s16)intensity / 128;
    packet->g2 = color2->g * (s16)intensity / 128;
    packet->b2 = color2->b * (s16)intensity / 128;
    gte_stmac0(&depth);
    if (depth != 0) {
        gte_stsxy3(&packet->x0, &packet->x1, &packet->x2);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_avsz3_command();
        gte_stszotz(&depth);
        depth -= bias;
        if (depth < 0x1000) {
            TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[D_8009CDDC], depth);
            if (mode != 0xFF) {
                drawMode = (RenderTintMode *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
                D_8009CDD8 += sizeof(RenderTintMode);
                SetDrawTPage((char *)drawMode, 0, 1, GetTPage(0, mode, 0, 0));
                if (packet) {
                    packet->code |= 2;
                    TILE_OT_ADDPRIM(ot, packet, link);
                }
                TILE_OT_ADDPRIM(ot, drawMode, link);
            } else if (packet) {
                TILE_OT_ADDPRIM(ot, packet, link);
            }
        }
    }
}

/* Push the pair (head, tail) onto the `count`-deep history and draw the
 * ribbon through it: quad i joins pairs i and i+1 and fades from
 * (count - i)/count to (count - i - 1)/count of color0 (head side) and
 * color1 (tail side) at scale/128. A missing head repeats the previous
 * pair; count >= 1000 instead clears the first count - 999 pairs. Mode
 * 0xFF draws opaque, anything else semi-transparent.
 * The camera upload is gte_ldrotmatrix and gte_ldtransmatrix. */
void func_800D1384(GteShortVector *head, GteShortVector *tail, u32 count,
                   u8 *color0, u8 *color1, int scale, FieldTrailPair *history,
                   int mode)
{
    u8 headColor[3];
    u8 tailColor[3];
    u32 depth;
    FieldG4Packet *packet;
    RenderTintMode *drawMode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    GteMatrix *view;
    int bias;
    u32 i;
    u32 last;
    FieldTrailPair *pair;
    s16 fade0;
    s16 fade1;

    view = D_800BCFA4.value;
    bias = (u16)D_800F3368.depth;
    if (count >= 1000) {
        count -= 999;
        for (i = 0; i < count; i++) {
            history[i].head.vector.pad = 0;
            history[i].tail.vector.pad = 0;
        }
        return;
    }
    last = count - 1;
    pair = &history[last];
    for (i = 0; i < count; i++) {
        pair[1].head.words[0] = pair[0].head.words[0];
        pair[1].head.words[1] = pair[0].head.words[1];
        pair[1].tail.words[0] = pair[0].tail.words[0];
        pair[1].tail.words[1] = pair[0].tail.words[1];
        pair--;
    }
    if (head != 0) {
        history[0].head.vector.x = head->x;
        history[0].head.vector.y = head->y;
        history[0].head.vector.z = head->z;
        history[0].head.vector.pad = 1;
        history[0].tail.vector.x = tail->x;
        history[0].tail.vector.y = tail->y;
        history[0].tail.vector.z = tail->z;
        history[0].tail.vector.pad = 1;
    } else {
        history[0].head.vector.x = history[1].head.vector.x;
        history[0].head.vector.y = history[1].head.vector.y;
        history[0].head.vector.z = history[1].head.vector.z;
        history[0].tail.vector.x = history[1].tail.vector.x;
        history[0].tail.vector.y = history[1].tail.vector.y;
        history[0].tail.vector.z = history[1].tail.vector.z;
    }
    packet = (FieldG4Packet *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += count * sizeof(FieldG4Packet);
    if (color0 == 0) {
        headColor[0] = headColor[1] = headColor[2] = 0;
    } else {
        headColor[0] = color0[0] * scale / 128;
        headColor[1] = color0[1] * scale / 128;
        headColor[2] = color0[2] * scale / 128;
    }
    if (color1 == 0) {
        tailColor[0] = tailColor[1] = tailColor[2] = 0;
    } else {
        tailColor[0] = color1[0] * scale / 128;
        tailColor[1] = color1[1] * scale / 128;
        tailColor[2] = color1[2] * scale / 128;
    }
    gte_ldrotmatrix(view);
    gte_ldtransmatrix(view);
    for (i = 0; i < count; i++, history++, packet++) {
        if (history[0].head.vector.pad & history[0].tail.vector.pad & history[1].head.vector.pad &
            history[1].tail.vector.pad) {
            gte_ldv3(&history[0].head.vector, &history[0].tail.vector, &history[1].head.vector);
            gte_rtpt_padded();
            gte_stmac0(&depth);
            if (depth == 0) {
                return;
            }
            gte_stsxy3(&packet->x0, &packet->x1, &packet->x2);
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_avsz3_command();
            gte_stszotz(&depth);
            depth -= bias;
            if (depth >= 0x1000) {
                return;
            }
            gte_ldv0(&history[1].tail.vector);
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_rtps_command();
            SetPolyG4(packet);
            gte_stsxy2(&packet->x3);
            fade0 = 128 - i * 128 / count;
            fade1 = 128 - (i + 1) * 128 / count;
            packet->r0 = headColor[0] * fade0 / 128;
            packet->g0 = headColor[1] * fade0 / 128;
            packet->b0 = headColor[2] * fade0 / 128;
            packet->r1 = tailColor[0] * fade0 / 128;
            packet->g1 = tailColor[1] * fade0 / 128;
            packet->b1 = tailColor[2] * fade0 / 128;
            packet->r2 = headColor[0] * fade1 / 128;
            packet->g2 = headColor[1] * fade1 / 128;
            packet->b2 = headColor[2] * fade1 / 128;
            packet->r3 = tailColor[0] * fade1 / 128;
            packet->g3 = tailColor[1] * fade1 / 128;
            packet->b3 = tailColor[2] * fade1 / 128;
            TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[D_8009CDDC], depth);
            if (mode != 0xFF) {
                drawMode = (RenderTintMode *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
                D_8009CDD8 += sizeof(RenderTintMode);
                SetDrawTPage((char *)drawMode, 0, 1, GetTPage(0, mode, 0, 0));
                if (packet) {
                    packet->code |= 2;
                    TILE_OT_ADDPRIM(ot, packet, link);
                }
                TILE_OT_ADDPRIM(ot, drawMode, link);
            } else if (packet) {
                TILE_OT_ADDPRIM(ot, packet, link);
            }
        }
    }
}

/* Full-screen 320x240 tile in `color` scaled by scale/128, linked at
 * `depth`; any blend mode other than 0xFF adds a semi-transparent draw mode. */
void func_800D1AE0(u8 *color, int scale, int abr, u32 depth)
{
    RenderTintTile *tile;
    RenderTintMode *mode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;

    tile = (RenderTintTile *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(RenderTintTile);
    SetTile(tile);
    tile->r = color[0] * scale / 128;
    tile->g = color[1] * scale / 128;
    tile->b = color[2] * scale / 128;
    if (depth < 0x1000) {
        tile->x = 0;
        tile->y = 0;
        tile->width = 320;
        tile->height = 240;
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[D_8009CDDC], depth);
        if (abr != 0xFF) {
            mode = (RenderTintMode *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
            D_8009CDD8 += sizeof(RenderTintMode);
            SetDrawTPage((char *)mode, 0, 1, GetTPage(0, abr, 0, 0));
            if (tile) {
                tile->command |= 2;
                TILE_OT_ADDPRIM(ot, tile, link);
            }
            TILE_OT_ADDPRIM(ot, mode, link);
        } else if (tile) {
            TILE_OT_ADDPRIM(ot, tile, link);
        }
    }
}

int Geo_ClipAlternatingArcPoint(int radius, int divisor, int segment) {
    int angle = (segment << 10) / divisor;
    int direction = ((segment & 1) << 1) - 1;
    int product = rcos(angle) * radius;
    int y;
    GeomState *state;

    if (product < 0) {
        product += 0xFFF;
    }
    product >>= 12;
    y = product * direction;

    state = D_800B1624;
    state->clip_min_x = state->clip_min_y = -128;
    state = D_800B1624;
    state->clip_max_x = state->clip_max_y = 128;

    return Geo_ClipPoint(0, (s16)y, 0);
}

/* Projected one-pixel tile plus a 3x3 halo at a quarter of its colour. */
void func_800D1DEC(void *position, void *colorArg, int scale, int abr)
{
    u8 *color = colorArg;
    FieldTilePoint *point;
    RenderTintTile *glow;
    RenderTintMode *mode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    u32 depth;

    point = (FieldTilePoint *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(FieldTilePoint);
    glow = (RenderTintTile *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(RenderTintTile);
    gte_lwc2_0_0(position);
    gte_lwc2_1_4(position);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtps_command();
    SetTile1(point);
    SetSemiTrans(point, 1);
    SetTile(glow);
    glow->r = point->r = color[0] * scale / 128;
    glow->g = point->g = color[1] * scale / 128;
    glow->b = point->b = color[2] * scale / 128;
    glow->r >>= 2;
    glow->g >>= 2;
    glow->b >>= 2;
    gte_stsxy2(&point->x);
    gte_stszotz(&depth);
    depth -= (u16)D_800F3368.depth;
    if (depth < 0x1000) {
        glow->x = point->x - 1;
        glow->y = point->y - 1;
        glow->height = 3;
        glow->width = 3;
        AddPrim((u32 *)D_800B0E38.ordering[D_8009CDDC] + depth, (u32 *)point);
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[D_8009CDDC], depth);
        if (abr != 0xFF) {
            mode = (RenderTintMode *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
            D_8009CDD8 += sizeof(RenderTintMode);
            SetDrawTPage((char *)mode, 0, 1, GetTPage(0, abr, 0, 0));
            if (glow) {
                glow->command |= 2;
                TILE_OT_ADDPRIM(ot, glow, link);
            }
            TILE_OT_ADDPRIM(ot, mode, link);
        } else if (glow) {
            TILE_OT_ADDPRIM(ot, glow, link);
        }
    }
}

/* One-pixel tile at the projected `position` in `color` scaled by
 * scale/128, linked at its view depth; any blend mode other than 0xFF adds a
 * semi-transparent draw mode. */
void func_800D2104(GteShortVector *position, u8 *color, int scale, int abr)
{
    FieldTilePoint *tile;
    RenderTintMode *mode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    u32 depth;

    tile = (FieldTilePoint *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(FieldTilePoint);
    gte_lwc2_0_0(position);
    gte_lwc2_1_4(position);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtps_command();
    SetTile1(tile);
    tile->r = color[0] * scale / 128;
    tile->g = color[1] * scale / 128;
    tile->b = color[2] * scale / 128;
    gte_stsxy2(&tile->x);
    {
        s32 *out = (s32 *)&depth;
        gte_stszotz(out);
    }
    depth -= (u16)D_800F3368.depth;
    if (depth < 0x1000) {
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[D_8009CDDC], depth);
        if (abr != 0xFF) {
            mode = (RenderTintMode *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
            D_8009CDD8 += sizeof(RenderTintMode);
            SetDrawTPage((char *)mode, 0, 1, GetTPage(0, abr, 0, 0));
            if (tile) {
                tile->command |= 2;
                TILE_OT_ADDPRIM(ot, tile, link);
            }
            TILE_OT_ADDPRIM(ot, mode, link);
        } else if (tile) {
            TILE_OT_ADDPRIM(ot, tile, link);
        }
    }
}

/* Rectangle `length` deep and `breadth` wide in the local XZ plane at
 * `position`, turned by `rotation`, gouraud shaded from color0 (near edge)
 * to color1 (far edge) at intensity/128 and textured with the cell at
 * (u, v); mode 0xFF draws it opaque, anything else semi-transparent.
 * The camera and local matrix uploads are gte_ldrotmatrix and
 * gte_ldtransmatrix. Matching debt: one empty constraint on rotation and
 * length (see docs/CODE_ORGANIZATION.md). */
void func_800D2370(GteShortVector *position, GteRotation *rotation,
                   int length, int breadth, int u, int v, int texture_width,
                   int texture_height, int clut, RenderColor *color0,
                   RenderColor *color1, int intensity, int mode)
{
    GteShortVector corners[4];
    RenderColor black;
    GteMatrix matrix;
    u32 depth;
    FieldGt4Packet *packet;
    s16 scale;
    int bias;
    GteMatrix *view;

    view = D_800BCFA4.value;
    scale = intensity;
    bias = (u16)D_800F3368.depth;
    black = D_800C22A0;
    packet = (FieldGt4Packet *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(FieldGt4Packet);
    SetPolyGT4(packet);
    if (mode == 0xFF) {
        packet->code &= ~2;
        packet->tpage = D_800F3368.tpage;
    } else {
        packet->code |= 2;
        packet->tpage = D_800F3368.tpage | GetTPage(0, mode, 0, 0);
    }
    /* Keep rotation and length in registers across the packet setup calls. */
    asm("" : : "r"(rotation), "r"(length));
    packet->clut = clut;
    gte_ldrotmatrix(view);
    gte_ldtransmatrix(view);
    gte_lwc2_0_0(position);
    gte_lwc2_1_4(position);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_mvmva_rotation_v0_translation_sf12();
    breadth /= 2;
    corners[0].x = corners[2].x = -breadth;
    corners[1].x = corners[3].x = breadth;
    corners[2].z = corners[3].z = length;
    corners[0].y = corners[1].y = corners[2].y = corners[3].y = 0;
    corners[0].z = corners[1].z = 0;
    {
        s32 *translation = matrix.t;
        gte_swc2_25_0(translation);
        gte_swc2_26_4(translation);
        gte_swc2_27_8(translation);
    }
    RotMatrixYXZ((GteShortVector *)rotation, &matrix);
    MulRotMatrix(&matrix);
    {
        const GteMatrixWords *words = (const GteMatrixWords *)(&matrix);
        gte_ldrotmatrix(words);
        gte_ldtransmatrix(words);
    }
    gte_ldv3(&corners[0], &corners[1], &corners[2]);
    gte_rtpt_padded();
    if (color0 == 0) {
        color0 = &black;
    }
    packet->r0 = packet->r1 = color0->r * scale / 128;
    packet->g0 = packet->g1 = color0->g * scale / 128;
    packet->b0 = packet->b1 = color0->b * scale / 128;
    if (color1 == 0) {
        color1 = &black;
    }
    packet->r2 = packet->r3 = color1->r * scale / 128;
    packet->g2 = packet->g3 = color1->g * scale / 128;
    packet->b2 = packet->b3 = color1->b * scale / 128;
    gte_stmac0(&depth);
    if (depth != 0) {
        {
            s16 *xy0 = &packet->x0;
            s16 *xy1 = &packet->x1;
            s16 *xy2 = &packet->x2;
            gte_stsxy3(xy0, xy1, xy2);
        }
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_avsz3_command();
        {
            u32 *out = &depth;
            gte_stszotz(out);
        }
        depth -= bias;
        if (depth < 0x1000) {
            {
                GteShortVector *last = &corners[3];
                gte_lwc2_0_0(last);
                gte_lwc2_1_4(last);
            }
            gte_cop2_hazard_slot();
            gte_cop2_hazard_slot();
            gte_rtps_command();
            packet->u0 = packet->u1 = u;
            packet->u2 = packet->u3 = u + texture_width - 1;
            packet->v0 = packet->v2 = v;
            packet->v1 = packet->v3 = v + texture_height - 1;
            gte_stsxy2(&packet->x3);
            AddPrim((unsigned int *)D_800B0E38.ordering[D_8009CDDC] + depth,
                    (unsigned int *)packet);
        }
    }
}

/* Haloed point at distance `x` from the view origin rotated by `y` about Z.
 * The halo is always drawn semi-transparent; `mode` is not read.
 * The rotation upload is gte_ldrotmatrix. */
void func_800D27FC(int x, int y, void *color, int scale, int mode)
{
    GteShortVector offset;
    GteMatrix matrix;
    FieldTilePoint *point;
    RenderTintTile *glow;
    RenderTintMode *blend;
    u8 *rgb = color;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    u32 depth;

    memset(&offset, 0, sizeof(offset));
    offset.x = x;
    offset.y = 0;
    offset.z = 0;
    point = (FieldTilePoint *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(FieldTilePoint);
    glow = (RenderTintTile *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(RenderTintTile);
    matrix.m[0][0] = matrix.m[1][1] = matrix.m[2][2] = 0x1000;
    matrix.m[0][1] = matrix.m[0][2] = matrix.m[1][0] = matrix.m[1][2] =
        matrix.m[2][0] = matrix.m[2][1] = 0;
    RotMatrixZ(y, &matrix);
    gte_ldrotmatrix(&matrix);
    gte_ldv0(&offset);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtps_command();
    SetTile1(point);
    SetTile(glow);
    SetSemiTrans(point, 1);
    glow->r = point->r = rgb[0] * scale / 128;
    glow->g = point->g = rgb[1] * scale / 128;
    glow->b = point->b = rgb[2] * scale / 128;
    glow->r >>= 2;
    glow->g >>= 2;
    glow->b >>= 2;
    gte_stsxy2(&point->x);
    gte_stszotz(&depth);
    depth -= (u16)D_800F3368.depth;
    if (depth < 0x1000) {
        glow->x = point->x - 1;
        glow->y = point->y - 1;
        glow->height = 3;
        glow->width = 3;
        AddPrim((u32 *)D_800B0E38.ordering[D_8009CDDC] + depth, (u32 *)point);
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[D_8009CDDC], depth);
        blend = (RenderTintMode *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
        D_8009CDD8 += sizeof(RenderTintMode);
        SetDrawTPage((char *)blend, 0, 1, GetTPage(0, 1, 0, 0));
        if (glow) {
            glow->command |= 2;
            TILE_OT_ADDPRIM(ot, glow, link);
        }
        TILE_OT_ADDPRIM(ot, blend, link);
    }
}

/* Gouraud line from `from` (color0 at scale0/128) to `to` (color1 at
 * scale1/128) linked at the mean depth, with a 16x16 glow cell stretched
 * along it; mode 0xFF draws both opaque, anything else semi-transparent. */
void func_800D2B58(GteShortVector *from, GteShortVector *to, u8 *color0,
                   u8 *color1, int scale0, int scale1, int mode)
{
    u32 depth[2];
    FieldLineG2Packet *line;
    FieldGlowQuadPacket *glow;
    RenderTintMode *drawMode;
    FieldTileAddress table;
    FieldTileAddress ot;
    FieldTileAddress link;
    int angle;
    int across;
    int acrossY;
    int along;
    int alongY;

    line = (FieldLineG2Packet *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(FieldLineG2Packet);
    glow = (FieldGlowQuadPacket *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(FieldGlowQuadPacket);
    gte_ldv0(from);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtps_command();
    line->r0 = color0[0] * scale0 / 128;
    line->g0 = color0[1] * scale0 / 128;
    line->b0 = color0[2] * scale0 / 128;
    {
        s32 *out;
        out = (s32 *)&depth[0];
        gte_stszotz(out);
    }
    gte_stsxy2(&line->x0);
    line->tag.length = 4;
    line->code = 0x50;
    gte_ldv0(to);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtps_command();
    line->r1 = color1[0] * scale1 / 128;
    line->g1 = color1[1] * scale1 / 128;
    line->b1 = color1[2] * scale0 / 128;
    {
        s32 *out;
        out = (s32 *)&depth[1];
        gte_stszotz(out);
    }
    gte_stsxy2(&line->x1);
    depth[0] = (int)(depth[0] + depth[1]) / 2 - (u16)D_800F3368.depth;
    if (depth[0] < 0x1000) {
        TILE_OT_ENTRY(ot, table, D_800B0E38.ordering[D_8009CDDC], depth[0]);
        if (mode != 0xFF) {
            drawMode = (RenderTintMode *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
            D_8009CDD8 += sizeof(RenderTintMode);
            SetDrawTPage((char *)drawMode, 0, 1, GetTPage(0, mode, 0, 0));
            if (line) {
                line->code |= 2;
                TILE_OT_ADDPRIM(ot, line, link);
            }
            TILE_OT_ADDPRIM(ot, drawMode, link);
        } else if (line) {
            TILE_OT_ADDPRIM(ot, line, link);
        }
        angle = ratan2(line->y1 - line->y0, line->x1 - line->x0);
        across = rsin(-angle) / 512;
        acrossY = rcos(-angle) / 512;
        along = rcos(angle) / 256;
        alongY = rsin(angle) / 256;
        glow->tag.length = 12;
        glow->code = 0x3C;
        if (mode == 0xFF) {
            glow->code = 0x3C;
            glow->tpage = D_800E2852;
        } else {
            glow->code = 0x3E;
            glow->tpage = D_800E2852 | GetTPage(0, mode, 0, 0);
        }
        glow->clut = GetClut(0x20, 0x1D6);
        glow->x0 = line->x0 - along + across;
        glow->y0 = line->y0 - alongY + acrossY;
        glow->x1 = line->x0 - along - across;
        glow->y1 = line->y0 - alongY - acrossY;
        glow->x2 = line->x1 + along + across;
        glow->y2 = line->y1 + alongY + acrossY;
        glow->x3 = line->x1 + along - across;
        glow->y3 = line->y1 + alongY - acrossY;
        glow->u0 = 0x90;
        glow->u2 = 0x90;
        glow->v0 = 0xD0;
        glow->v1 = 0xD0;
        glow->u1 = 0x9F;
        glow->u3 = 0x9F;
        glow->v2 = 0xDF;
        glow->v3 = 0xDF;
        glow->r0 = line->r0;
        glow->g0 = line->g0;
        glow->b0 = line->b0;
        glow->r1 = line->r0;
        glow->g1 = line->g0;
        glow->b1 = line->b0;
        glow->r2 = line->r1;
        glow->g2 = line->g1;
        glow->b2 = line->b1;
        glow->r3 = line->r1;
        glow->g3 = line->g1;
        glow->b3 = line->b1;
        AddPrim((unsigned int *)D_800B0E38.ordering[D_8009CDDC] + depth[0],
                (unsigned int *)glow);
    }
}

/* Draw a textured ribbon `width` wide through `count` points: every joint
 * is widened across the screen direction of the segment that leaves it,
 * shaded from color0 to color1 (each at scale/128) along the ribbon and
 * textured from u..u+du by v..v+height. Mode 0xFF draws opaque, anything
 * else semi-transparent. */
void func_800D3114(GteShortVector *points, s16 count, int width, int u, int v,
                   int du, int height, int tpage, int clut, int scale,
                   RenderColor *color0, RenderColor *color1, int mode)
{
    GteShortVector *point;
    GteShortVector angles;
    GteShortVector right;
    GteShortVector left;
    FieldRibbonColor color0Scaled;
    FieldRibbonColor color1Scaled;
    FieldRibbonColor color;
    FieldScreenPoint screen0;
    FieldScreenPoint screen1;
    GteMatrix matrix;
    u32 unused;
    u32 depth;
    FieldRibbonPacket *packet;
    int i;
    u32 previousDepth;
    int bias;
    int t;
    u32 shade;
    int dx;
    int dy;
    int angle;
    int joint;

    packet = (FieldRibbonPacket *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += count * sizeof(FieldRibbonPacket);
    bias = (u16)D_800F3368.depth;
    color0Scaled.bytes.rgb[0] = color0->r * scale / 128;
    color0Scaled.bytes.rgb[1] = color0->g * scale / 128;
    color0Scaled.bytes.rgb[2] = color0->b * scale / 128;
    color1Scaled.bytes.rgb[0] = color1->r * scale / 128;
    color1Scaled.bytes.rgb[1] = color1->g * scale / 128;
    color1Scaled.bytes.rgb[2] = color1->b * scale / 128;
    right.x = width / 2;
    previousDepth = 0;
    right.y = right.z = 0;
    left.y = left.z = 0;
    left.x = -width / 2;
    angles.x = angles.y = 0;
    point = points;
    for (i = 0; i < count; i++) {
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        gte_ldv3(&point[0], &point[1], &point[0]);
        gte_rtpt_padded();
        packet->tag.length = 12;
        packet->c0.bytes.code = 0x3C;
        if (mode == 0xFF) {
            packet->c0.bytes.code = 0x3C;
            packet->tpage = tpage;
        } else {
            packet->c0.bytes.code = 0x3E;
            packet->tpage = GetTPage(0, mode, 0, 0) | tpage;
        }
        packet->clut = clut;
        gte_stsxy3(&screen0, &screen1, &unused);
        gte_avsz3_padded();
        gte_stszotz(&depth);
        depth -= bias;
        if (depth >= 0x1000) {
            return;
        }
        dx = screen1.x - screen0.x;
        dy = screen1.y - screen0.y;
        angle = ratan2(dy, dx) + 0x400;
        t = (i << 12) / count;
        LoadAverageCol(&color0Scaled, &color1Scaled, 0x1000 - t, t, &color);
        if (i == 0) {
            joint = angle;
            angles.z = joint;
            RotMatrix(&angles, &matrix);
            gte_ldv0(&point[0]);
            gte_rt();
            packet->c0.bytes.rgb[0] = packet->c1.bytes.rgb[0] = color.bytes.rgb[0];
            packet->c0.bytes.rgb[1] = packet->c1.bytes.rgb[1] = color.bytes.rgb[1];
            packet->c0.bytes.rgb[2] = packet->c1.bytes.rgb[2] = color.bytes.rgb[2];
            gte_stmac(matrix.t);
            gte_ldrotmatrix(&matrix);
            gte_ldtransmatrix(&matrix);
            gte_ldv3(&right, &left, &right);
            gte_rtpt_padded();
            packet->u0 = packet->u1 = u;
            packet->v0 = v;
            packet->v1 = v + height;
            previousDepth = depth;
            gte_stsxy3(&packet->p0, &packet->p1, &unused);
            packet++;
        } else if (i < count - 1) {
            joint = angle;
            angles.z = joint;
            RotMatrix(&angles, &matrix);
            gte_ldv0(&point[0]);
            gte_rt();
            shade = color.word;
            packet[-1].c3.word = shade;
            packet[-1].c2.word = shade;
            gte_stmac(matrix.t);
            gte_ldrotmatrix(&matrix);
            gte_ldtransmatrix(&matrix);
            gte_ldv3(&right, &left, &right);
            gte_rtpt_padded();
            packet[-1].v2 = v;
            packet[-1].v3 = v + height;
            packet[-1].u2 = packet[-1].u3 = u + du * i / count;
            gte_stsxy3(&packet[-1].p2, &packet[-1].p3, &unused);
            AddPrim((unsigned int *)D_800B0E38.ordering[D_8009CDDC] + previousDepth,
                    (unsigned int *)&packet[-1]);
            packet->p0.word = packet[-1].p2.word;
            packet->p1.word = packet[-1].p3.word;
            packet->c0.bytes.rgb[0] = packet->c1.bytes.rgb[0] = color.bytes.rgb[0];
            packet->c0.bytes.rgb[1] = packet->c1.bytes.rgb[1] = color.bytes.rgb[1];
            packet->c0.bytes.rgb[2] = packet->c1.bytes.rgb[2] = color.bytes.rgb[2];
            packet->u0 = packet->u1 = packet[-1].u2;
            packet->v0 = v;
            packet->v1 = v + height;
            previousDepth = depth;
            packet++;
        } else {
            joint = angle;
            angles.z = joint;
            RotMatrix(&angles, &matrix);
            gte_ldv0(&point[0]);
            gte_rt();
            shade = color.word;
            packet[-1].c3.word = shade;
            packet[-1].c2.word = shade;
            gte_stmac(matrix.t);
            gte_ldrotmatrix(&matrix);
            gte_ldtransmatrix(&matrix);
            gte_ldv3(&right, &left, &right);
            gte_rtpt_padded();
            packet[-1].v2 = v;
            packet[-1].v3 = v + height;
            packet[-1].u2 = packet[-1].u3 = u + du * i / count;
            gte_stsxy3(&packet[-1].p2, &packet[-1].p3, &unused);
            AddPrim((unsigned int *)D_800B0E38.ordering[D_8009CDDC] + previousDepth,
                    (unsigned int *)&packet[-1]);
            packet->p0.word = packet[-1].p2.word;
            packet->p1.word = packet[-1].p3.word;
            packet->c0.bytes.rgb[0] = packet->c1.bytes.rgb[0] = color.bytes.rgb[0];
            packet->c0.bytes.rgb[1] = packet->c1.bytes.rgb[1] = color.bytes.rgb[1];
            packet->c0.bytes.rgb[2] = packet->c1.bytes.rgb[2] = color.bytes.rgb[2];
            packet->u0 = packet->u1 = packet[-1].u2;
            packet->v0 = v;
            packet->v1 = v + height;
            angles.z = joint;
            RotMatrix(&angles, &matrix);
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            gte_ldv0(&point[1]);
            gte_rt();
            packet->c2.word = packet->c3.word = color1Scaled.word;
            gte_stmac(matrix.t);
            gte_ldrotmatrix(&matrix);
            gte_ldtransmatrix(&matrix);
            gte_ldv3(&right, &left, &right);
            gte_rtpt_padded();
            packet->v2 = v;
            packet->u2 = packet->u3 = u + du;
            packet->v3 = v + height;
            gte_stsxy3(&packet->p2, &packet->p3, &unused);
            AddPrim((unsigned int *)D_800B0E38.ordering[D_8009CDDC] + depth,
                    (unsigned int *)packet);
        }
        point++;
    }
}

void FieldEng_UpdatePointHistory(RenderHistoryPoint *history, s16 count,
                                 RenderHistoryPoint *value, int reset) {
    RenderHistoryPoint *cursor;
    int i;

    if (reset) {
        count++;
        for (i = 0; i < count; i++) {
            history->vector.x = value->vector.x;
            history->vector.y = value->vector.y;
            history->vector.z = value->vector.z;
            history++;
        }
    } else {
        cursor = history + (count - 1);
        for (i = 0; i < count;) {
            i++;
            cursor[1].words[0] = cursor[0].words[0];
            cursor[1].words[1] = cursor[0].words[1];
            cursor--;
        }
        history->words[0] = value->words[0];
        history->words[1] = value->words[1];
    }
}

/* Matching debt: two empty barriers and five volatile stack parameters keep
 * every register save before the stack argument loads (see
 * docs/CODE_ORGANIZATION.md). */
void func_800D3BC8(GteShortVector *position, int scale_x, int scale_y,
                   int texture, volatile int input_clut, volatile int input_page,
                   volatile int input_intensity, RenderColor *volatile input_color,
                   volatile int input_pull)
{
    GteShortVector center;
    GteShortVector screen;
    s32 depth;
    FieldStripPacket *packet;
    int r, g, b;
    int u, v;
    int width, height;
    int clut;
    int page;
    int intensity;
    register RenderColor *color;
    int pull;

    asm volatile("" : : : "memory");
    pull = input_pull;
    clut = input_clut;
    page = input_page;
    intensity = input_intensity;
    color = input_color;
    asm volatile("" : : : "$6");
    center.x = 0xA0;
    center.y = 0x78;
    packet = (FieldStripPacket *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += sizeof(FieldStripPacket);
    if (color == 0) {
        r = g = b = intensity;
    } else {
        r = intensity * color->r / 128;
        g = intensity * color->g / 128;
        b = intensity * color->b / 128;
    }
    packet->r0 = r;
    packet->g0 = g;
    packet->b0 = b;
    if (D_800F3368.parameter06 != 0) {
        u = texture & 0xF;
        v = 0;
        if (u >= 8) {
            u -= 8;
            v = 0x20;
        }
        u <<= 4;
        v += texture / 16 * 16;
    } else {
        u = (texture & 0xF) << 4;
        v = texture / 16 * 16;
    }
    gte_ldrotmatrix(D_800BCFA4.value);
    gte_ldtransmatrix(D_800BCFA4.value);
    gte_ldv0(position);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtps_command();
    packet->tag.length = 9;
    packet->code = 0x2C;
    if (page == 0xFF) {
        packet->code &= ~2;
        packet->tpage = D_800F3368.tpage;
    } else {
        packet->code |= 2;
        packet->tpage = D_800F3368.tpage | GetTPage(0, page, 0, 0);
    }
    packet->clut = clut;
    width = D_800F3368.extent_x;
    height = D_800F3368.extent_y;
    packet->u0 = packet->u2 = u;
    packet->v1 = packet->v0 = v;
    packet->u1 = packet->u3 = u + width - 1;
    packet->v2 = packet->v3 = v + height - 1;
    width = width * scale_x / 8192;
    height = height * scale_y / 8192;
    gte_stszotz(&depth);
    depth -= (u16)D_800F3368.depth;
    if (pull != 0) {
        depth = 14;
    }
    if ((u32)depth < 0x1000) {
        gte_stsxy2(&screen);
        pull *= 2;
        LoadAverageShort12(&screen, &center, 0x1000 - pull, pull, &screen);
        packet->x0 = packet->x2 = screen.x - width;
        packet->y0 = packet->y1 = screen.y - height;
        packet->x1 = packet->x3 = screen.x + width;
        packet->y2 = packet->y3 = screen.y + height;
        AddPrim((unsigned int *)STRIP_OT(depth), (unsigned int *)packet);
    }
}
