#include "common.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_actor.h"
#include "pe1/field_anim_pool.h"
#include "room_m350_shared.h"
extern char D_8019A854;

extern void func_8019A134(void);

s32 func_80199E50(s32 arg0) {
    char *obj;

    if (arg0 != 1) {
        if (arg0 >= 2) {
            return 0;
        }
        if (arg0 != 0) {
            return 0;
        }
        return func_800CE560(D_800F33E0->end, 4, 2,
                             (FieldAnimCallbackListCallback)func_8019A134);
    } else if ((char)D_8019A86E != 0) {
        return 2;
    } else if (D_8019A854 != 0) {
        obj = func_800CE610(D_800F33E0->end);
        if (obj != 0) {
            /* The translation of the owner's eighth matrix. */
            *(s32 **)obj = D_800F32D0->actor->render_object.matrices[7].translation;
            D_8019A854 = 0;
        }
    }

    return 0;
}
