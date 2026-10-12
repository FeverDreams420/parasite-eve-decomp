/*
 * The drop field effect set's module class: the seven methods the class
 * table lists in front of the set (RoomEffect_DropFieldEffects.c). The
 * registration method clears the target's 0x40000000 flag before it
 * registers the draw list, and both it and the start method act only while
 * the field engine runs the object (status 3).
 *
 * room_m348 links the class as its only module; room_m174 and room_m383
 * link it after the glow orb's class. The script and the lists are each
 * room's own data (pe1/room_drop_field_class.h).
 */
#include "room_lib.h"
#include "pe1/room_drop_field_class.h"

ROOMLIB_RETURN_ZERO(RoomEffect_DropFieldNop0)

ROOMLIB_PLANT_TABLE(RoomEffect_DropFieldPlantScript, g_RoomDropFieldScript)

ROOMLIB_SPAWN6(RoomEffect_DropFieldSpawn6)

int RoomEffect_DropFieldRegister(RoomEnt *o) {
    RoomRenderNode *target;

    if (FieldFx_GetStatus(o) == 3) {
        target = o->link->target;
        target->flags &= 0xBFFFFFFF;
        FieldFx_Register(o, g_RoomDropFieldDrawList);
    }
    return 0;
}

ROOMLIB_START_AT3(RoomEffect_DropFieldStart, g_RoomDropFieldUpdateList,
                  g_RoomDropFieldSpawnData, g_RoomDropFieldSpawnScript,
                  RoomEffect_DropFieldClose)

ROOMLIB_CLOSE_TARGET(RoomEffect_DropFieldClose)

ROOMLIB_RETURN_ZERO(RoomEffect_DropFieldNop6)
