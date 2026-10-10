#include "room_m350_shared.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/gte_types.h"
#include "pe1/field_actor.h"
#include "pe1/player_entity.h"
typedef RoomM350Vector Vector;
typedef RenderMatrix Matrix;
typedef struct { Vector velocity; short x, y, z; unsigned char stopped, pad; } Particle;

typedef RenderMatrix Transform;
typedef FieldActor Instance;
typedef RoomM350Actor Actor;
typedef FieldActor Player;
extern RoomM350Emitter *D_800F33E0;
extern Actor *D_800F32D0;
extern unsigned char D_8019A86E, D_8019A855;
extern short D_8019A86A;
extern int func_80196F2C(int, Particle *);
extern int Math_IntSqrt(int);
extern int Inv_ScrambleGrid(void);
extern int Asset_Find08w(int, int, short, short, short);

int func_80197364(int event)
{
    Matrix matrix;
    Vector vector;
    if (event != 1) {
        if (event < 2) {
            if (event != 0) {
                return 0;
            }
        } else {
        goto done;
        }
        return func_800CE560(D_800F33E0->pool, 16, 4,
                             (FieldAnimCallbackListCallback)func_80196F2C);
    }
update:
    {
        Particle *effect;
        Instance *instance;
        Transform *transform;
        Player *player;
        int dx, dz, dy;
        int angle;
        if (D_8019A86E) return 2;
        if (!D_8019A855) return 0;
        effect = func_800CE610(D_800F33E0->pool);
        if (!effect) return 0;
        instance = (Instance *)D_800F32D0->instance;
        transform = instance->render_object.matrices;
        player = g_PlayerEntity;
        effect->x = transform[7].translation[0];
        effect->y = transform[7].translation[1];
        effect->z = transform[7].translation[2];
        effect->stopped = 0;
        dx = transform[7].translation[0] - player->render_object.model_matrix.translation[0];
        dz = transform[7].translation[2] - player->render_object.model_matrix.translation[2];
        dy = transform[7].translation[1] - player->render_object.model_matrix.translation[1];
        angle = ratan2(dy, Math_IntSqrt(dx * dx + dz * dz));
        angle += (signed char)Inv_ScrambleGrid() / 2;
        angle &= 0xFFF;
        vector.x = angle;
        if (angle < 0xD00) vector.x = 0xD00;
        {
            int offset = (signed char)Inv_ScrambleGrid();
            Vector *v = &vector;
            register Vector *input asm("$4") = v;
            register Matrix *m asm("$5") = &matrix;
            int yaw = (u16)instance->rot_y;
            /* Keep the rotation arguments ready before adding the yaw offset. */
            asm("" : "=r"(offset) : "0"(offset), "r"(input), "r"(m), "r"(yaw));
            offset += 0x800;
            yaw += offset;
            vector.y = yaw;
            vector.z = 0;
            RotMatrixYXZ(input, m);
        }
        vector.x = 0;
        vector.y = 0;
        vector.z = 128;
        ApplyMatrixSV(&matrix, &vector, &effect->velocity);
        {
            Actor *actor = D_800F32D0;
            D_8019A855 = 0;
            /* Publish consumption before loading the sound's position and owner. */
            asm volatile("" : : : "memory");
            {
                int x = effect->x;
                Asset_Find08w(0x60A, ((Instance *)actor->instance)->state->progress,
                    x, effect->y, effect->z);
            }
        }
        D_8019A86A = 8;
    }
done:
    return 0;
}
