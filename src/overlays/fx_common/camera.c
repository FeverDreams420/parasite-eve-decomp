/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/cdrom.h"
#include "pe1/game_state.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/script_queue.h"

#include "camera_externs.h"
#include "fx_common.h"
#include "fx_common_motion.h"
#include "fx_common_setup.h"

typedef struct FxAssetSource {
    u8 reserved[0x100];
    u8 *data;
} FxAssetSource;

/* Aim the effect camera from its position at its target: build the view
 * matrix, the look-at matrix and the billboard rotation used by sprites. */
void FxCommon_UpdateCameraMatrices(void)
{
    GteVector offset;
    GteVector unit;
    RoomSpriteMatrix view;
    RoomSpriteMatrix rotation;
    GteShortVector target;
    GteShortVector eye;
    GteVector up;

    view = D_8018EFF4;
    rotation = D_8018EFF4;
    offset.x = g_FxCommonCameraTarget.x - g_FxCommonCameraPosition.x;
    offset.y = g_FxCommonCameraTarget.y - g_FxCommonCameraPosition.y;
    offset.z = g_FxCommonCameraTarget.z - g_FxCommonCameraPosition.z;
    VectorNormal(&offset, &unit);
    view.t[0] = -g_FxCommonCameraPosition.x;
    view.t[1] = -g_FxCommonCameraPosition.y;
    view.t[2] = -g_FxCommonCameraPosition.z;
    D_8019BFC4->x = ratan2(unit.y, SquareRoot0(unit.x * unit.x + unit.z * unit.z));
    D_8019BFC4->y = ratan2(unit.z, unit.x) - 0x400;
    D_8019BFC4->z = 0;
    RotMatrix((RoomFxSeed8 *)D_8019BFC4, &rotation);
    CompMatrix(&rotation, &view, &D_8019CC30);

    up.x = 0;
    up.y = -20000;
    up.z = 0;
    target.x = g_FxCommonCameraTarget.x;
    target.y = g_FxCommonCameraTarget.y;
    target.z = g_FxCommonCameraTarget.z;
    eye.x = g_FxCommonCameraPosition.x;
    eye.y = g_FxCommonCameraPosition.y;
    eye.z = g_FxCommonCameraPosition.z;
    FxCommon_BuildLookAtMatrix((GteMatrix *)&D_8019CC30, &eye, &target, &up);

    D_8019BFC4->x = -ratan2(SquareRoot0(offset.x * offset.x + offset.z * offset.z),
                              -offset.y) + 0x400;
    D_8019BFC4->y = -ratan2(offset.z, offset.x) + 0x400;
    D_8019BFC4->z = 0;
    RotMatrixZYX(D_8019BFC4, &D_8019CDF0);
    D_8019CDF0.t[0] = g_FxCommonCameraPosition.x;
    D_8019CDF0.t[1] = g_FxCommonCameraPosition.y;
    D_8019CDF0.t[2] = g_FxCommonCameraPosition.z;
    SetTransMatrix(&D_8019CC30);
    SetRotMatrix(&D_8019CC30);
}

/* Build a camera matrix looking from eye toward target with the given up
 * vector: the rows are the normalised side, up and forward axes, and the
 * translation is the rotated eye position negated. */
void FxCommon_BuildLookAtMatrix(GteMatrix *out, GteShortVector *eye,
                   GteShortVector *target, GteVector *up)
{
    GteVector work;
    GteVector side;
    GteVector upAxis;
    GteVector forward;

    work.x = target->x - eye->x;
    work.y = target->y - eye->y;
    work.z = target->z - eye->z;
    VectorNormal(&work, &forward);
    if (forward.z == up->z)
        forward.z++;

    gte_ldopv1_psyq(&forward);
    gte_ldopv2(up);
    gte_op12_psyq();
    gte_stmac(&work);
    VectorNormal(&work, &side);

    gte_ldopv1_psyq(&forward);
    gte_ldopv2(&side);
    gte_op12_psyq();
    gte_stmac(&work);
    VectorNormal(&work, &upAxis);

    out->m[0][0] = side.x;
    out->m[0][1] = side.y;
    out->m[0][2] = side.z;
    out->m[1][0] = upAxis.x;
    out->m[1][1] = upAxis.y;
    out->m[1][2] = upAxis.z;
    out->m[2][0] = forward.x;
    out->m[2][1] = forward.y;
    out->m[2][2] = forward.z;

    gte_ldrotmatrix(out);
    gte_ldv0(eye);
    gte_mvmva();
    gte_stmac(out->t);
    out->t[0] = -out->t[0];
    out->t[1] = -out->t[1];
    out->t[2] = -out->t[2];
}

/* Sample a closed path of short points: position holds the segment index in
 * its upper bits and an 8-bit fraction below.  Writes the interpolated point
 * and a yaw/bank pair derived from the two neighbouring segment headings,
 * and returns the path's point count. */
int FxCommon_SamplePath(u32 position, int index, void *table,
                  FxCommonVec3 *out, GteShortVector *extra)
{
    GteVector start;
    GteVector middle;
    GteVector end;
    GteVector toMiddle;
    GteVector toEnd;
    FxCommonPathPoint point;
    FxCommonOffsetByte *base;
    FxCommonOffsetByte *cursor;
    FxCommonPathPoint *points;
    GteShortVector *angles;
    int fraction;
    int segment;
    int next;
    int after;
    s16 count;
    int bank;
    int delta;
    int span;
    s16 yaw;
    s16 nextYaw;

    base = table;
    cursor = base + index * sizeof(s32);
    points = (FxCommonPathPoint *)(base + *(s32 *)cursor);
    angles = extra;
    segment = position >> 8;
    count = points->count;
    fraction = position & 0xFF;
    points++;
    span = count - 2;
    if (span < segment)
        D_8019BFCC = 1;
    segment %= span;
    if (segment < 0)
        segment += count - 2;
    next = (segment + 1) % span;
    after = (segment + 2) % span;

    point = points[segment];
    start.x = point.x;
    start.y = point.y;
    start.z = point.z;
    point = points[next];
    middle.x = point.x;
    middle.y = point.y;
    middle.z = point.z;
    point = points[after];
    end.x = point.x;
    end.y = point.y;
    end.z = point.z;

    toMiddle.x = middle.x - start.x;
    toMiddle.y = middle.y - start.y;
    toMiddle.z = middle.z - start.z;
    toEnd.x = end.x - middle.x;
    toEnd.y = end.y - middle.y;
    toEnd.z = end.z - middle.z;

    yaw = -ratan2(toMiddle.z, toMiddle.x);
    nextYaw = -ratan2(toEnd.z, toEnd.x);
    if (yaw - nextYaw > 0x800)
        nextYaw += 0x1000;
    if (nextYaw - yaw > 0x800)
        yaw += 0x1000;

    delta = nextYaw - yaw;
    delta *= fraction;
    bank = (yaw - nextYaw) >> 3;
    angles->z = bank;
    angles->x = 0;
    angles->y = yaw + (delta >> 8);
    if (bank > 0x80)
        angles->z = 0x80;
    if (angles->z < -0x80)
        angles->z = -0x80;
    if (angles->z >= -3 && angles->z <= 3)
        angles->z = 0;

    start.x <<= 8;
    start.y <<= 8;
    start.z <<= 8;
    middle.x <<= 8;
    middle.y <<= 8;
    middle.z <<= 8;
    start.x += toMiddle.x * fraction;
    start.y += toMiddle.y * fraction;
    start.z += toMiddle.z * fraction;
    start.x >>= 8;
    start.y >>= 8;
    start.z >>= 8;

    out->x = start.x;
    out->y = start.y;
    out->z = start.z;
    return count;
}

/* Rotate the four frustum corners by the camera matrix and rebuild the
 * four side planes through the eye: normal, distance, the signed distance
 * of the next corner and the normal length. */
void FxCommon_UpdateFrustumPlanes(FxCommonVec3 *eye)
{
    GteVector corner0;
    GteVector corner1;
    GteVector corner2;
    GteVector corner3;
    GteVector a;
    GteVector b;
    s32 flag;

    PushMatrix();
    SetTransMatrix(&D_8019CDF0);
    SetRotMatrix(&D_8019CDF0);
    RotTrans(&D_8019BFD0, &corner0, &flag);
    RotTrans(&D_8019BFD8, &corner1, &flag);
    RotTrans(&D_8019BFE0, &corner2, &flag);
    RotTrans(&D_8019BFE8, &corner3, &flag);

    a.x = corner2.x - eye->x;
    a.y = corner2.y - eye->y;
    a.z = corner2.z - eye->z;
    b.x = corner0.x - eye->x;
    b.y = corner0.y - eye->y;
    b.z = corner0.z - eye->z;
    OuterProduct0(&a, &b, &D_8019CBB0);

    a.x = corner3.x - eye->x;
    a.y = corner3.y - eye->y;
    a.z = corner3.z - eye->z;
    b.x = corner1.x - eye->x;
    b.y = corner1.y - eye->y;
    b.z = corner1.z - eye->z;
    OuterProduct0(&a, &b, &D_8019CBD0);

    a.x = corner3.x - eye->x;
    a.y = corner3.y - eye->y;
    a.z = corner3.z - eye->z;
    b.x = corner2.x - eye->x;
    b.y = corner2.y - eye->y;
    b.z = corner2.z - eye->z;
    OuterProduct0(&a, &b, &D_8019CBF0);

    a.x = corner1.x - eye->x;
    a.y = corner1.y - eye->y;
    a.z = corner1.z - eye->z;
    b.x = corner0.x - eye->x;
    b.y = corner0.y - eye->y;
    b.z = corner0.z - eye->z;
    OuterProduct0(&a, &b, &D_8019CB50);

    D_8019CB48 = -(D_8019CBB0.x * corner2.x) - D_8019CBB0.y * corner2.y
               - D_8019CBB0.z * corner2.z;
    D_8019CB4C = -(D_8019CBD0.x * corner3.x) - D_8019CBD0.y * corner3.y
               - D_8019CBD0.z * corner3.z;
    D_8019CBA8 = -(D_8019CBF0.x * corner2.x) - D_8019CBF0.y * corner2.y
               - D_8019CBF0.z * corner2.z;
    D_8019CA90 = -(D_8019CB50.x * corner1.x) - D_8019CB50.y * corner1.y
               - D_8019CB50.z * corner1.z;
    D_8019CC04 = D_8019CBB0.x * corner1.x + D_8019CBB0.y * corner1.y
               + D_8019CBB0.z * corner1.z + D_8019CB48;
    D_8019CC0C = D_8019CBD0.x * corner0.x + D_8019CBD0.y * corner0.y
               + D_8019CBD0.z * corner0.z + D_8019CB4C;
    D_8019CC10 = D_8019CBF0.x * corner1.x + D_8019CBF0.y * corner1.y
               + D_8019CBF0.z * corner1.z + D_8019CBA8;
    D_8019CBC4 = D_8019CB50.x * corner2.x + D_8019CB50.y * corner2.y
               + D_8019CB50.z * corner2.z + D_8019CA90;

    D_8019CBC8 = SquareRoot0(D_8019CBB0.x * D_8019CBB0.x
                               + D_8019CBB0.y * D_8019CBB0.y
                               + D_8019CBB0.z * D_8019CBB0.z);
    D_8019CC00 = SquareRoot0(D_8019CBD0.x * D_8019CBD0.x
                               + D_8019CBD0.y * D_8019CBD0.y
                               + D_8019CBD0.z * D_8019CBD0.z);
    D_8019CC08 = SquareRoot0(D_8019CBF0.x * D_8019CBF0.x
                               + D_8019CBF0.y * D_8019CBF0.y
                               + D_8019CBF0.z * D_8019CBF0.z);
    D_8019CBAC = SquareRoot0(D_8019CB50.x * D_8019CB50.x
                               + D_8019CB50.y * D_8019CB50.y
                               + D_8019CB50.z * D_8019CB50.z);
    PopMatrix();
}

int FxCommon_IsPointInView(int *point)
{
    int first, second;
    int secondVisible;
    int firstVisible = 0;

    first = D_8019CBB0.x * point[0] + D_8019CBB4 * point[1]
          + D_8019CBB8 * point[2] + D_8019CB48;
    second = D_8019CBD0.x * point[0] + D_8019CBD4 * point[1]
           + D_8019CBD8 * point[2] + D_8019CB4C;

    secondVisible = 0;
    if (first > 0 && D_8019CC04 > 0)
        firstVisible = 1;
    if (first < 0 && D_8019CC04 < 0)
        firstVisible = 1;
    if (second > 0 && D_8019CC0C > 0)
        secondVisible = 1;
    if (second < 0 && D_8019CC0C < 0)
        secondVisible = 1;

    return firstVisible & secondVisible;
}

int FxCommon_IsShortPointInView(short *point)
{
    int first, second;
    int secondVisible;
    int firstVisible = 0;

    first = D_8019CBB0.x * point[0] + D_8019CBB4 * point[1]
          + D_8019CBB8 * point[2] + D_8019CB48;
    second = D_8019CBD0.x * point[0] + D_8019CBD4 * point[1]
           + D_8019CBD8 * point[2] + D_8019CB4C;

    secondVisible = 0;
    if (first > 0 && D_8019CC04 > 0)
        firstVisible = 1;
    if (first < 0 && D_8019CC04 < 0)
        firstVisible = 1;
    if (second > 0 && D_8019CC0C > 0)
        secondVisible = 1;
    if (second < 0 && D_8019CC0C < 0)
        secondVisible = 1;

    return firstVisible & secondVisible;
}

int FxCommon_CheckBoundsWithMargin(int *point, int margin)
{
    int first;
    int second;
    int distance;
    int limit;
    int secondVisible = 0;
    int firstVisible = 0;
    int threshold = margin;
    first = D_8019CBB0.x * point[0] + D_8019CBB4 * point[1]
          + D_8019CBB8 * point[2] + D_8019CB48;
    second = D_8019CBD0.x * point[0] + D_8019CBD4 * point[1]
           + D_8019CBD8 * point[2] + D_8019CB4C;
    if (D_8019CC04 > 0 && first >= 0)
    {
      firstVisible = 1;
    }
    if (D_8019CC04 < 0 && first <= 0)
    {
      firstVisible = 1;
    }
    if (D_8019CC0C > 0 && second >= 0)
    {
      secondVisible = 1;
    }
    if (D_8019CC0C < 0 && second <= 0)
    {
      secondVisible = 1;
    }
    limit = threshold * 8;
    threshold = limit + 30;
    if (!firstVisible)
    {
      if (first >= 0)
      {
        distance = first / D_8019CBC8;
      }
      else
      {
        distance = (-first) / D_8019CBC8;
      }
      limit = (short)threshold;
      if (distance < limit)
      {
        firstVisible = 1;
      }
    }
    if (!secondVisible)
    {
      if (second >= 0)
      {
        distance = second / D_8019CC00;
      }
      else
      {
        distance = (-second) / D_8019CC00;
      }
      limit = (short)threshold;
      if (distance < limit)
      {
        secondVisible = 1;
      }
    }
    return firstVisible & secondVisible;
}

int FxCommon_CheckFourBoundsWithMargin(int *point, int margin) {
    int first, second, third, fourth, distance;
    int limit;
    int fourthVisible = 0, thirdVisible = 0, secondVisible = 0, firstVisible = 0;
    int threshold = margin;
    first = D_8019CBB0.x * point[0] + D_8019CBB4 * point[1]
          + D_8019CBB8 * point[2] + D_8019CB48;
    second = D_8019CBD0.x * point[0] + D_8019CBD4 * point[1]
          + D_8019CBD8 * point[2] + D_8019CB4C;
    third = D_8019CBF0.x * point[0] + D_8019CBF4 * point[1]
          + D_8019CBF8 * point[2] + D_8019CBA8;
    fourth = D_8019CB50.x * point[0] + D_8019CB54 * point[1]
          + D_8019CB58 * point[2] + D_8019CA90;
    if (D_8019CC04 > 0 && first >= 0)
        firstVisible = 1;
    if (D_8019CC04 < 0 && first <= 0)
        firstVisible = 1;
    if (D_8019CC0C > 0 && second >= 0)
        secondVisible = 1;
    if (D_8019CC0C < 0 && second <= 0)
        secondVisible = 1;
    if (D_8019CC10 > 0 && third >= 0)
        thirdVisible = 1;
    if (D_8019CC10 < 0 && third <= 0)
        thirdVisible = 1;
    if (D_8019CBC4 > 0 && fourth >= 0)
        fourthVisible = 1;
    if (D_8019CBC4 < 0 && fourth <= 0)
        fourthVisible = 1;
    limit = threshold * 8;
    threshold = limit + 30;
    if (!firstVisible) {
        if (first >= 0)
            distance = first / D_8019CBC8;
        else
            distance = -first / D_8019CBC8;
        limit = (short)threshold;
        if (distance < limit)
            firstVisible = 1;
    }
    if (!secondVisible) {
        if (second >= 0)
            distance = second / D_8019CC00;
        else
            distance = -second / D_8019CC00;
        limit = (short)threshold;
        if (distance < limit)
            secondVisible = 1;
    }
    if (!thirdVisible) {
        if (third >= 0)
            distance = third / D_8019CC08;
        else
            distance = -third / D_8019CC08;
        limit = (short)threshold;
        if (distance < limit)
            thirdVisible = 1;
    }
    if (!fourthVisible) {
        if (fourth >= 0)
            distance = fourth / D_8019CBAC;
        else
            distance = -fourth / D_8019CBAC;
        limit = (short)threshold;
        if (distance < limit)
            fourthVisible = 1;
    }
    return firstVisible & secondVisible & thirdVisible & fourthVisible;
}

/* Sentinel nodes of the three node lists: each head node links back to
 * its tail node, each tail node on to its head. */

void FxCommon_InitNodePools(void)
{
    int i, offset;
    char *slot;

    i = 1;
    slot = D_801E4E6C;
    for (; i < 200; i++, slot += 108)
        FxCommon_ResetTransformNode(slot);

    FxCommon_ResetTransformNode(&D_8019C340);
    D_8019C39C = 0;
    D_8019CBC0 = 0;
    for (i = 0; i < 200; i++)
        D_8019C830[i] = i + 1;

    i = 0;
    {
        FxCommonNode *source = D_801EA5E8;
        FxCommonNode *dest = D_8019CC58;

        for (; i < 3;) {
            D_8019CC58[i].previous = source;
            source++;
            D_801EA5E8[i].next = dest;
            dest++;
            D_801EA5E8[i].previous = 0;
            D_8019CC58[i].next = 0;
            i++;
        }
    }

    {
        short empty = -1;

        offset = 0;
        for (; offset < 800; offset += 4) {
            *(short *)((char *)D_801E4A88 + offset) = empty;
            *(short *)((char *)D_801E4A88 + offset + 2) = empty;
        }
    }

    D_8019C9D0 = 200;
    D_801E4DA8 = -1;
    D_801E4DAA = 201;
    D_801E4DAC = 200;
    D_801E4DAE = -1;
}

int FxCommon_AllocIndexLink(void);
void FxCommon_LinkNodeToParent(FxCommonNode *node, FxCommonNode *parent);

FxCommonNode *func_80190AEC(FxCommonNode *parent, void *payload)
{
    int index = FxCommon_AllocIndexLink();
    FxCommonNode *node = (FxCommonNode *)&D_801E4E00[(s16)index];

    node->type = 0;
    node->args.payload = payload;
    node->id = index;
    FxCommon_LinkNodeToParent(node, parent);
    node->state[0] = 0;
    node->state[1] = 0;
    node->state[2] = 0;
    return node;
}

int FxCommon_GetOffsetTableValue(void *payload, int type);

FxCommonNode *func_80190B78(FxCommonNode *parent, s16 type, void *payload)
{
    int index = FxCommon_AllocIndexLink();
    FxCommonNode *node = (FxCommonNode *)&D_801E4E00[(s16)index];

    node->type = type;
    node->args.payload = payload;
    node->id = index;
    node->value32 = FxCommon_GetOffsetTableValue(payload, type);
    FxCommon_LinkNodeToParent(node, parent);
    node->state[0] = 0;
    node->state[1] = 0;
    node->state[2] = 0;
    return node;
}

int FxCommon_GetOffsetTableValue(void *payload, int subtype);

FxCommonNode *func_80190C1C(FxCommonNode *parent, s16 subtype, s16 variant,
                            s16 type, void *data38, void *data3C,
                            void *payload)
{
    int index = FxCommon_AllocIndexLink();
    FxCommonNode *node = (FxCommonNode *)&D_801E4E00[(s16)index];

    node->args.configured.subtype = subtype;
    node->args.configured.variant = variant;
    node->type = type;
    node->id = index;
    node->args.configured.payload = payload;
    node->value36 = FxCommon_GetOffsetTableValue(payload, subtype);
    node->data38 = data38;
    node->data3C = data3C;
    FxCommon_LinkNodeToParent(node, parent);
    node->state[1] = 0;
    node->state[2] = 0;
    node->state[3] = 0;
    return node;
}

void FxCommon_ReleaseNode(FxCommonNode *node)
{
    FxCommon_UnlinkNode(node);
    FxCommon_FreeIndexLink(node->id);
}

void FxCommon_DrawTransformNode(FxCommonTransformNode *node, void *context)
{
    RotMatrix(&node->seed.room, &node->matrix);
    *D_8019BFF0 = node->matrix;
    CompMatrix(&D_8019CC30, D_8019BFF0, D_8019BFF0);
    SetTransMatrix(D_8019BFF0);
    SetRotMatrix(D_8019BFF0);
    FxCommon_DrawPolyResource(context, node->resource);
}

/* Rebuild a transform node's matrix when it is on screen (or forced), apply
 * the mirror matrix for mirrored nodes, then draw it with the renderer that
 * matches its pass and mirror state.  `state` holds the visibility test and
 * then the pass. */
void FxCommon_DrawTransformNodeCulled(FxCommonTransformNode *node, FxCommonBuffer *context, u8 pass, u8 force,
                   u8 mirrored)
{
    GteVector position;
    GteMatrix mirror;
    int state;

    mirror = D_8018F014;
    position.x = node->matrix.t[0];
    position.y = node->matrix.t[1];
    position.z = node->matrix.t[2];
    state = FxCommon_CheckFourBoundsWithMargin(&position.x, node->margin) | force;
    if (state) {
        RotMatrix(&node->seed.room, &node->matrix);
        if (mirrored == 1) {
            gte_CompMatrix(&mirror, &node->matrix, &node->matrix);
        }
        *D_8019BFF0 = node->matrix;
        CompMatrix(&D_8019CC30, D_8019BFF0, D_8019BFF0);
        SetTransMatrix(D_8019BFF0);
        SetRotMatrix(D_8019BFF0);
        state = pass;
        if (state == 0) {
            if (mirrored == 0)
                FxCommon_DrawModel(context, node->resource, node->kind);
            if (mirrored == 1)
                FxCommon_DrawModelTinted(context, node->resource, node->kind);
        }
        if (pass == 1)
            FxCommon_DrawModelFlat(context, node->resource, node->kind);
    }
}

/* Rebuild a level-of-detail node's matrix (mode 2 composes it with the mirror
 * matrix and keeps the original translation), then draw the model kind that
 * matches its view depth when it is on screen or forced. */
void FxCommon_DrawLodNode(FxCommonLodNode *node, FxCommonBuffer *context, u8 force, u8 mode)
{
    GteVector position;
    GteMatrix mirror;
    RoomSpriteMatrix copy;
    RoomFxSeed8 seed;
    s16 margin;
    int depth;

    mirror = D_8018F014;
    position.x = node->matrix.t[0];
    position.y = node->matrix.t[1];
    position.z = node->matrix.t[2];
    margin = node->margin;
    if (mode == 2) {
        seed = node->seed;
        RotMatrix(&seed, &node->matrix);
        copy = node->matrix;
        gte_CompMatrix(&copy, &mirror, &node->matrix);
        node->matrix.t[0] = copy.t[0];
        node->matrix.t[1] = copy.t[1];
        node->matrix.t[2] = copy.t[2];
    } else {
        RotMatrix(&node->seed, &node->matrix);
    }
    *D_8019BFF0 = node->matrix;
    CompMatrix(&D_8019CC30, D_8019BFF0, D_8019BFF0);
    SetTransMatrix(D_8019BFF0);
    SetRotMatrix(D_8019BFF0);
    if (FxCommon_CheckBoundsWithMargin(&position.x, margin) | force) {
        depth = D_8019BFF0->t[2];
        if (depth < node->nearDepth) {
            if (depth > 0x180) {
                if (mode == 0)
                    FxCommon_DrawModel(context, node->resource, node->nearKind);
                if (mode == 1)
                    FxCommon_DrawModelTinted(context, node->resource, node->nearKind);
                if (mode == 2)
                    FxCommon_DrawModelTinted(context, node->resource, node->nearKind);
            }
        } else if (depth < node->farDepth) {
            if (mode == 0)
                FxCommon_DrawModel(context, node->resource, node->middleKind);
            if (mode == 1)
                FxCommon_DrawModelTinted(context, node->resource, node->middleKind);
        } else {
            if (mode == 0)
                FxCommon_DrawModel(context, node->resource, node->farKind);
            if (mode == 1)
                FxCommon_DrawModelTinted(context, node->resource, node->farKind);
            if (mode == 2)
                FxCommon_DrawModelTinted(context, node->resource, node->farKind);
        }
    }
}

void FxCommon_ResetTransformNode(FxCommonTransformNode *node)
{
    node->matrix.t[2] = 1000;
    node->matrix.m[0][0] = 0x1000;
    node->matrix.m[1][1] = 0x1000;
    node->matrix.m[2][2] = 0x1000;
    node->matrix.t[0] = 0;
    node->matrix.t[1] = 0;
    node->matrix.m[0][1] = 0;
    node->matrix.m[0][2] = 0;
    node->matrix.m[1][0] = 0;
    node->matrix.m[1][2] = 0;
    node->matrix.m[2][0] = 0;
    node->matrix.m[2][1] = 0;
    node->seed.component[0] = 0;
    node->seed.component[1] = 0;
    node->seed.component[2] = 0;
    node->record = &D_8019C340;
    node->previous = 0;
    node->next = 0;
}

int FxCommon_AllocIndexLink(void)
{
    s16 index = D_8019CBC0;
    s16 current = D_8019C9D0;
    u16 freeNext;
    u16 next;
    int nextIndex;

    freeNext = D_8019C830[index];
    D_801E4A88[index].previous = current;
    next = D_801E4A88[current].next;
    D_801E4A88[index].next = next;
    D_801E4A88[current].next = index;
    nextIndex = D_801E4A88[index].next;
    D_8019C9D0 = index;
    D_8019CBC0 = freeNext;
    D_801E4A88[nextIndex].previous = index;
    return index;
}

void FxCommon_FreeIndexLink(s16 index)
{
    int freeHead;

    D_801E4A88[D_801E4A88[index].previous].next =
        D_801E4A88[index].next;
    D_801E4A88[D_801E4A88[index].next].previous =
        D_801E4A88[index].previous;
    if (index == D_8019C9D0) {
        D_8019C9D0 = D_801E4A88[index].previous;
    }
    freeHead = D_8019CBC0;
    D_801E4A88[index].previous = -1;
    D_801E4A88[index].next = -1;
    D_8019CBC0 = index;
    D_8019C830[index] = freeHead;
}

void FxCommon_BeginRecordIteration(void)
{
    D_8019CC50 = 200;
}

FxCommonRecord *FxCommon_NextRecord(void)
{
    int record = D_801E4A8A[D_8019CC50].record;

    D_8019CC50 = record;
    if (record == 201) {
        record = -1;
    }
    if (record == -1) {
        return 0;
    }
    return &D_801E4E00[record];
}

void FxCommon_LinkNodeToParent(FxCommonNode *node, FxCommonNode *parent)
{
    int bucket;

    node->parent = parent;
    node->bucket = parent->bucket + 1;
    bucket = node->bucket;
    node->previous = D_8019CCBC[bucket].head;
    node->next = node->previous->next;
    D_8019CCBC[bucket].head->next = node;
    D_8019CCBC[bucket].head = node;
}

void FxCommon_UnlinkNode(FxCommonNode *node)
{
    node->previous->next = node->next;
    node->next->previous = node->previous;
}

/* Pick the effect variant, mirror pairs and texture flag for the current
 * room and area before the shared effect assets are set up. */
void FxCommon_SelectRoomVariant(void)
{

    FxCommon_InitDisplay();
    if (Save_GetMetadataWindowIndex() == 0)
        D_8019C008 = 0;
    else
        D_8019C008 = 0xFF;
    if (D_800B0CD8 & 0x40000000) {
        D_8019C008 = 0xFF;
        Menu_SetEquipSlotIndex(1);
    } else {
        D_8019C008 = 0;
        Menu_SetEquipSlotIndex(0);
    }

    D_8019CC52 = -1;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0x179)
        D_8019CC52 = 6;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0x18)
        D_8019CC52 = 6;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0x2E)
        D_8019CC52 = 7;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0x75)
        D_8019CC52 = 7;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0x67)
        D_8019CC52 = 7;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0xBF)
        D_8019CC52 = 9;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0xC0)
        D_8019CC52 = 9;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0x5D)
        D_8019CC52 = 2;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0x60)
        D_8019CC52 = 2;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0x3A)
        D_8019CC52 = 8;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0x176)
        D_8019CC52 = 8;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0x7D)
        D_8019CC52 = 4;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0x104)
        D_8019CC52 = 3;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0xA1)
        D_8019CC52 = 1;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0xB7)
        D_8019CC52 = 0;
    if (g_ScriptVariableBank[SCRIPT_VAR_ROOM] == 0x122)
        D_8019CC52 = 5;
    if (D_8019CC52 == -1)
        D_8019CC52 = 7;

    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0x80)
        D_8019BFF4 = 0;
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0x208)
        D_8019BFF4 = 7;
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0xB8) {
        D_8019C000 = 1;
        D_8019C004 = 9;
    }
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0xC0) {
        D_8019BFF8 = 1;
        D_8019BFFC = 9;
    }
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0xD0) {
        D_8019C000 = 1;
        D_8019C004 = 7;
    }
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0xD8) {
        D_8019BFF8 = 1;
        D_8019BFFC = 7;
    }
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0xE0) {
        D_8019C000 = 1;
        D_8019C004 = 8;
    }
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0xE4) {
        D_8019BFF8 = 1;
        D_8019BFFC = 8;
    }
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0x148) {
        D_8019C000 = 1;
        D_8019C004 = 9;
    }
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0x160) {
        D_8019BFF8 = 1;
        D_8019BFFC = 9;
    }
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0x178) {
        D_8019C000 = 1;
        D_8019C004 = 7;
    }
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0x180) {
        D_8019BFF8 = 1;
        D_8019BFFC = 7;
    }
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0x1C0) {
        D_8019C000 = 1;
        D_8019C004 = 4;
    }
    if (g_ScriptVariableBank[SCRIPT_VAR_AREA] == 0x1C8) {
        D_8019BFF8 = 1;
        D_8019BFFC = 4;
    }
    if (g_ScriptVariableBank[SCRIPT_VAR_FLAGS] & 0x2000)
        D_8019C1F0 = 1;
    else
        D_8019C1F0 = 0;
    FxCommon_LoadEffectSectors();
}

void FxCommon_LoadEffectSectors(void) {
    FxAssetSource *resource = (FxAssetSource *)&D_800B0CD8;
    u16 *range;
    int result;

first:
    range = D_80093170;
    do {
        result = CdRom_ReadSectorsFromLba(resource->data + range[0],
                               &D_801D0260, range[1] - range[0]);
    } while (result == -1);
    for (;;) {
        result = CdRom_PollReady();
        if (result == 0) break;
        if (result == -1) goto first;
    }

    if (!(D_8019C1F0)) {

second:
    range = D_80093174;
    do {
        result = CdRom_ReadSectorsFromLba(resource->data + range[0],
                               &D_8019CE10, range[1] - range[0]);
    } while (result == -1);
    for (;;) {
        result = CdRom_PollReady();
        if (result == 0) return;
        if (result == -1) goto second;
    }

    }
third:
    range = D_80093172;
    do {
        result = CdRom_ReadSectorsFromLba(resource->data + range[0],
                               &D_8019CE10, range[1] - range[0]);
    } while (result == -1);
    for (;;) {
        result = CdRom_PollReady();
        if (result == 0) break;
        if (result == -1) goto third;
    }
}

void FxCommon_SetAkao3dVolumeRange(int extended)
{
    if (extended == 0) {
        g_Akao3dVolumeMin = 0x10;
        g_Akao3dVolumeMax = 0x10;
    } else {
        g_Akao3dVolumeMin = 0x10;
        g_Akao3dVolumeMax = 0x40;
    }
    D_800B0DD0 = 0;
    D_800B0DD2 = 0xFFF;
}

int FxCommon_SendTableCommand3d(int id, s32 *state)
{
    int size = 0x300;
    void *resource;
    GteMatrix **matrixSlot;
    GteMatrix *savedMatrix;
    s32 *savedOutput;
    int result;

    resource = Draw_GetTimTableEntry(&D_801D0260, 3);
    PushMatrix();
    SetTransMatrix(&D_8019CC30);
    SetRotMatrix(&D_8019CC30);
    matrixSlot = &g_ViewMatrixSlot.value;
    savedMatrix = *matrixSlot;
    *matrixSlot = &D_8019CC30;
    savedOutput = g_ProjectionSlot.distance;
    g_ProjectionSlot.distance = &size;
    result = Akao_SendTableCommand(resource, id, 0, 0x80, 1);
    *matrixSlot = savedMatrix;
    g_ProjectionSlot.distance = savedOutput;
    PopMatrix();
    return result;
}

void FxCommon_Apply3dPan(int handle, volatile s32 *state)
{
    struct FxCommonApplyScratch {
        int size;
        int pad14;
        AkaoPackedRect3 vector;
        s16 pad1E;
        int valueA;
        int valueB;
    } scratch;
    GteMatrix **matrixSlot;
    GteMatrix *savedMatrix;
    s32 *savedOutput;

    scratch.size = 0x300;
    PushMatrix();
    SetTransMatrix(&D_8019CC30);
    SetRotMatrix(&D_8019CC30);
    matrixSlot = &g_ViewMatrixSlot.value;
    savedMatrix = *matrixSlot;
    *matrixSlot = &D_8019CC30;
    savedOutput = g_ProjectionSlot.distance;
    g_ProjectionSlot.distance = &scratch.size;
    scratch.vector.x = state[0];
    scratch.vector.y = state[1];
    scratch.vector.z = state[2];
    Akao_Calc3DPan(&scratch.vector, &scratch.valueA, &scratch.valueB);
    Akao_Cmd_A0(handle, 0, scratch.valueB);
    Akao_Cmd_A2(handle, 0, scratch.valueA);
    *matrixSlot = savedMatrix;
    g_ProjectionSlot.distance = savedOutput;
    PopMatrix();
}
