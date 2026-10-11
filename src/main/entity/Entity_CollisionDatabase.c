/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
#include "common.h"
#include "pe1/collision_database.h"
#include "pe1/floor_polygon.h"
#include "pe1/floor_edge_point.h"
#include "pe1/scene_assets.h"
#include "pe1/field_collision.h"

/* The room collision database: clearing the per-room collision state and
 * relocating the loaded database's offsets into pointers. */

/* COMMON supplies small-data metadata to MASPSX for the words this unit
 * reaches through $gp; the edge points stay absolute. */
char *g_SceneDataTable2;
short g_FieldCollisionTriIndex;
unsigned short D_8009CE1C, D_8009CE20, D_8009CE24, D_8009CE28;
short g_EntityCollisionMoveDelta;
short g_EntityCollisionWallParam;
CollisionRegionEntry *g_RegionHeightTable;
struct CollisionPlane *g_CollisionPlaneTable;
CollisionDatabase *g_CollisionDb;
short *g_EntityCollisionWallSlot;
PolygonVertex *D_8009D248;
u16 D_8009D1CC;


void Entity_ResetStateGlobals(void) {
    unsigned int i;
    unsigned int count;
    unsigned int *ptr;

    g_RegionHeightTable = 0;

    for (i = 0; i < 8; i += 4) {
        *(short *)&((char *)&D_8009CE0C)[i] = 0;
        *(short *)&D_8009CE0E[i] = 0;
    }

    count = 0;
    g_SceneDataTable2 = 0;
    ptr = D_8009DFB0;

    while (count < 20) {
        *ptr = 0;
        count++;
        ptr++;
    }

    g_FieldCollisionTriIndex = 0;
    D_8009CE28 = 0;
    D_8009CE24 = 0;
    D_8009CE20 = 0;
    D_8009CE1C = 0;
    g_EntityCollisionMoveDelta = 0;
    g_CollisionPlaneTable = 0;
    g_CollisionDb = 0;
    g_EntityCollisionWallSlot = 0;
    D_8009D248 = 0;
    g_EntityCollisionWallParam = 0;
    D_8009D1CC = 0;
}

void Entity_RelocateSceneData(void) {
    CollisionDatabase *base;
    register char *loaded asm("$2");
    char frame[8];
    u32 offset;

    loaded = D_800B1620[0];
    base = (CollisionDatabase *)loaded;
    offset = *(u32 *)&base->vertices;
    g_CollisionDb = base;

    if (0x80000000U < offset) {
        struct CollisionPlane *planes;
        CollisionRegionEntry *heights;

        planes = base->planes.pointer;
        heights = base->regions;
        g_RegionHeightTable = heights;
        g_CollisionPlaneTable = planes;
        return;
    }

    base->vertices.pointer = SceneAsset_ResolveOffset(base, offset);
    base->triangles.pointer = SceneAsset_ResolveOffset(base, base->triangles.word);
    base->rampEdges.pointer = SceneAsset_ResolveOffset(base, base->rampEdges.word);
    if (base->planes.word != 0) {
        base->planes.pointer = SceneAsset_ResolveOffset(base, base->planes.word);
    }

    {
        CollisionDatabase *actor;
        u32 *dst;
        u32 i;
        u32 *cursor;
        u16 wordCount;
        u16 *regionCountAddress;
        u32 count;
        u32 entry;

        actor = g_CollisionDb;
        i = 0;
        cursor = (u32 *)actor->planes.pointer;
        wordCount = actor->visitedWordCount;
        entry = actor->rampEdges.word;
        dst = (u32 *)actor->regions;
        wordCount >>= 5;
        g_CollisionPlaneTable = (struct CollisionPlane *)cursor;
        regionCountAddress = &actor->regionCount;
        g_RegionHeightTable = (CollisionRegionEntry *)dst;
        count = *regionCountAddress;
        wordCount++;
        g_SceneDataTable2 = (char *)entry;
        actor->visitedWordCount = wordCount;

        if (count != 0) {
            cursor = (u32 *)actor;
            do {
                entry = cursor[10];
                cursor++;
                i++;
                entry = entry + (u32)actor;
                *dst = entry;
                dst++;
            } while (i < actor->regionCount);
        }
    }
}
