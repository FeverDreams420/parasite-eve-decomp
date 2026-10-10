#ifndef ROOM_M089_H
#define ROOM_M089_H

#include "../room_lib/room_lib.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_texture_tables.h"

extern short D_800966EC[][2];

extern void *D_800B0E64;
extern FieldActorNode *D_8009D20C;

extern int func_80071A54(void);
extern void func_8006DF50(void *channel, int id, int value, int volume,
                          int pan);
extern void func_801924F8(void);

#endif
