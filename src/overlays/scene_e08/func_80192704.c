#include "common.h"
#include "pe1/scene_e08_ring_effect.h"
void func_80192704(s32 arg0, s16 *rec, SceneE08RingEffectState *p) {
    u16 *q;
    s16 pad;

    q = (u16 *)p;
    if (p->firstScale < 0x960) {
        p->firstScale += 0x28;
    }
    if (rec[1] >= 0x3D) {
        q[9] += 0x190;
        if ((s16)q[9] >= 0x1449) {
            q[9] = 0x1448;
        }
    }
    if (rec[1] >= 0x6A) {
        ((s8 *)rec)[1] = 2;
    }
}
