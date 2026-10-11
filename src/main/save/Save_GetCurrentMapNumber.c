#include "common.h"
#include "pe1/scene_dispatch.h"
#include "pe1/scene_room.h"

int Save_GetCurrentMapNumber(void) {
    char roomName[8];

    Str_EncodeBase32(roomName, g_SceneDispatchToken);
    return Str_ParseMapNumber(roomName);
}
