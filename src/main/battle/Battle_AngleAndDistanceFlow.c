#include "pe1/battle.h"
#include "pe1/battle_runtime.h"
#include "pe1/gte_types.h"

int Battle_CalcDistToPlayer(BattleEntity *actor, BattleEntity *player)
{
    int x;
    int y;

    x = actor->renderObject.target_x - player->posX.parts.integer;
    y = actor->renderObject.target_z - player->posZ.parts.integer;
    return Math_IntSqrt((x * x) + (y * y));
}

s16 Battle_CalcAngleToTarget(RenderObjectEntity *object, void *target)
{
    int *position = target;
    int x;
    int y;
    int angle;

    x = object->target_x << 16;
    y = object->target_z << 16;
    angle = ratan2(x - position[0], y - position[2]);
    return angle + 0x800;
}

int Battle_CalcRelativeAngle(BattleEntity *target, BattleEntity *viewer)
{
    int angle;
    int wrapped;

    angle = 0x800 - ratan2(target->posX.fixed - viewer->posX.fixed,
                           target->posZ.fixed - viewer->posZ.fixed);
    angle = angle << 16;
    angle = angle >> 16;
    angle += viewer->facingAngle;
    if (angle >= 0)
    {
        wrapped = angle;
    }
    else
    {
        wrapped = angle + 0xFFF;
    }
    wrapped >>= 12;
    wrapped <<= 12;
    return (s16) (angle - wrapped);
}
