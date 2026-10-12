#include "common.h"
#include "pe1/field_actor.h"

int Task_SetEntityShadowFlag(void) {
    g_CurrentEntity->flags |= 0x20000000;
    return 1;
}

int Task_ClearEntityShadowFlag(void) {
    g_CurrentEntity->flags &= 0xDFFFFFFF;
    return 1;
}
