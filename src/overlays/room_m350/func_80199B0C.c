#include "common.h"
#include "room_m350_data.h"

extern FieldActor *D_8009D20C;

/* Walks the actor list for the first live actor of the given type and sub
 * id and leaves it in D_8019A774. */
FieldActor **func_80199B0C(s32 arg0, s32 arg1, s32 arg2) {
    FieldActor **slot;
    FieldActor **iter;
    FieldActor *node;

    slot = &D_8019A774;
    if ((*slot = D_8009D20C) != 0) {
        iter = slot;
        while (1) {
            node = D_8019A774;
            if ((node->type_id == arg1) && (node->sub_id == arg2) && ((node->flags & 0x10) == 0)) {
                break;
            }

            if ((*iter = D_8019A774->next) == 0) {
                break;
            }
        }
    }

    return &D_8019A774;
}
