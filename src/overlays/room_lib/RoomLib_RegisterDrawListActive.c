/*
 * Room module class, slot 3, in the modules that register their draw list
 * only while the field engine runs the object (status 3) rather than from
 * status 2 on (RoomLib_RegisterDrawList). The same function in every room
 * that links it; the list is the room's (pe1/room_module.h).
 */
#include "room_lib.h"
#include "pe1/room_module.h"

ROOMLIB_REGISTER_TABLE_AT3(RoomLib_RegisterDrawListActive, g_RoomDrawList)
