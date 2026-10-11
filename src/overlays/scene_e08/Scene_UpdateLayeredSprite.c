#include "scene_e08.h"
#include "pe1/scene_e08_layered_sprite.h"
#include "pe1/room_floor.h"
#include "pe1/floor_polygon.h"
#include "pe1/gte_types.h"
#include "pe1/field_engine_calls.h"


int func_80071A54(void);

void func_801946D4(void *unused, char *control, char *state)
{
    char *owner = (char *)func_800C2B50();
    /* Keeps the later history-copy loop based at its original offset. */
    register char *cursor asm("$6");
    s16 point[3];
    u32 count;
    int random;
    int x;
    int y;
    int z;
    int nextX;
    s32 xVelocity;
    s32 zBase;
    s32 zVelocity;
    s32 yBase;
    int nextY;
    register s32 yVelocity asm("$7");
    int nextZ;
    int zArg;
    /* The Y velocity and later screen limit share the original argument register. */
    register u16 screenLimit asm("$7");
    int pointValue;
    s16 *ground;
    count = 4;
    for (; count != 0; --count)
    {
        ((SceneLayeredSpriteState *)state)->layers[count] = ((SceneLayeredSpriteState *)state)->layers[count - 1];
        ((SceneLayeredSpriteState *)state)->layerSeeds[count] = ((SceneLayeredSpriteState *)state)->layerSeeds[count - 1];
    }

    count = 7;
    x = ((SceneLayeredSpriteState *)state)->center[0].parts.integer;
    y = ((SceneLayeredSpriteState *)state)->center[1].parts.integer;
    z = ((SceneLayeredSpriteState *)state)->center[2].parts.integer;
    cursor = state + 0x38;
    ((SceneLayeredSpriteState *)state)->layers[0].x = x;
    ((SceneLayeredSpriteState *)state)->layers[0].y = y;
    ((SceneLayeredSpriteState *)state)->layers[0].z = z;
    ((SceneLayeredSpriteState *)state)->layerSeeds[0] = ((SceneLayeredSpriteState *)state)->initialSeed;
    for (; count != 0; --count, cursor -= 8)
    {
        *((RoomFxSeed8 *) (cursor + 0x86)) = *((RoomFxSeed8 *) (cursor + 0x7E));
    }

    *((RoomFxSeed8 *) (state + 0x86)) = *((RoomFxSeed8 *) (state + 0x50));
    /* The volatile reads preserve the retail load order. */
    zBase = ((SceneLayeredSpriteState *)state)->center[2].fixed;
    zVelocity = ((SceneLayeredSpriteState *)state)->velocity[2];
    xVelocity = *((volatile s32 *)&((SceneLayeredSpriteState *)state)->velocity[0]);
    yBase = *((volatile s32 *)&((SceneLayeredSpriteState *)state)->center[1].fixed);
    yVelocity = *((volatile s32 *)&((SceneLayeredSpriteState *)state)->velocity[1]);
    nextZ = zBase + zVelocity;
    nextY = yBase + yVelocity;
    nextX = ((SceneLayeredSpriteState *)state)->center[0].fixed + xVelocity;
    screenLimit = D_8009D1CC;
    ((SceneLayeredSpriteState *)state)->center[0].fixed = nextX;
    *((s32 *) (state + 4)) = nextY;
    *((s32 *) (state + 8)) = nextZ;
    zArg = (unsigned)(nextZ >> 16) << 16;
    if (Geo_PointInPoly(((unsigned)((SceneLayeredSpriteState *)state)->center[0].parts.integer) << 16, zArg, D_8009D248, screenLimit) == 0)
    {
        *((u8 *) (control + 1)) = 2;
    }
    if ((*((s16 *) (control + 2))) < 50)
    {
        random = func_80071A54();
        ((SceneLayeredSpriteState *)state)->initialSeed.vector.y = (random % 1024) - 0x200;
        ground = &g_RoomFloorY->y;
        ((SceneLayeredSpriteState *)state)->initialSeed.vector.x = 0x400;
        *((u16 *) (state + 0x24)) += 0x270;
        if ((((SceneLayeredSpriteState *)state)->center[1].fixed) < (*ground))
        {
            int gravity;
            gravity = 0xC350;
            ((SceneLayeredSpriteState *)state)->velocity[1] += gravity;
        }
        else
        {
            /* Preserve the second ground-height read after clearing velocity. */
            PE1_COMPILER_MEMORY_BARRIER();
            ((SceneLayeredSpriteState *)state)->velocity[0] = 0;
            ((SceneLayeredSpriteState *)state)->velocity[1] = 0;
            ((SceneLayeredSpriteState *)state)->velocity[2] = 0;
            ((SceneLayeredSpriteState *)state)->center[1].fixed = *ground;
        }
    }
    else
    {
        ((SceneLayeredSpriteState *)state)->initialSeed.vector.x = 0x400;
        ((SceneLayeredSpriteState *)state)->initialSeed.vector.y = 0;
        ++(((SceneLayeredSpriteState *)state)->phase);
        *((u16 *) (state + 0x24)) += 0x270;
        ((SceneLayeredSpriteState *)state)->velocity[0] -= (((SceneLayeredSpriteState *)state)->drift) * 8;
        ((SceneLayeredSpriteState *)state)->velocity[2] = rsin((((SceneLayeredSpriteState *)state)->phase) << 6) * 1500;
    }
    pointValue = ((SceneLayeredSpriteState *)state)->center[0].parts.integer;
    point[0] = pointValue;
    pointValue = ((SceneLayeredSpriteState *)state)->center[1].parts.integer;
    point[1] = pointValue;
    pointValue = ((SceneLayeredSpriteState *)state)->center[2].parts.integer;
    point[2] = pointValue;
    if (func_800C6B90(point, 0xC8))
    {
        *((s16 *) (owner + 0x2C)) = 1;
    }
}
