#include "pe1/field_actor.h"
#include "pe1/field_sprite_draw.h"

extern FieldActor *D_8009D20C;
extern struct BattleEntity *D_8009D254;
extern char D_8018F1CC[];

/* printf. */
int func_80071A74(const char *format, ...);

FieldActor *func_80192560(int type_id, int sub_id, s16 *position) {
    FieldActor *actor;

    actor = D_8009D20C;
    while (actor != 0) {
        if ((actor != ((FieldActor *)D_8009D254)) && (*(char **)actor != 0) &&
            (*(int *)(*(char **)actor + 0x10) > 0) &&
            (actor->sub_id == type_id) && (actor->type_id == sub_id)) {
            func_800CE870((char *)actor, 0, position);
            return actor;
        }
        actor = actor->next;
    }

    func_80071A74(D_8018F1CC, type_id, sub_id);
    return 0;
}
