#include "common.h"
#include "room_m350_data.h"

#include "func_80199CA4_externs.h"

/* Walks the actor list for the first live actor of the given type and sub
 * id, leaves it in D_8019A7F8 and returns the joint position slot. */
GteShortVector *func_80199CA4(s32 arg0, s32 arg1, s32 arg2) {
    FieldActor **slot;
    FieldActor **iter;
    FieldActor *node;

    slot = &D_8019A7F8;
    if ((*slot = D_8009D20C) != 0) {
        iter = slot;
        while (1) {
            node = D_8019A7F8;
            if ((node->type_id == arg1) && (node->sub_id == arg2) && ((node->flags & 0x10) == 0)) {
                break;
            }

            if ((*iter = D_8019A7F8->next) == 0) {
                break;
            }
        }
    }

    return &D_8019A7A0;
}
