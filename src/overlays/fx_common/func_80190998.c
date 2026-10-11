#include "fx_common.h"
#include "func_80190998_externs.h"
/* Sentinel nodes of the three node lists: each head node links back to
 * its tail node, each tail node on to its head. */


void func_80190998(void)
{
    int i, offset;
    char *slot;

    i = 1;
    slot = D_801E4E6C;
    for (; i < 200; i++, slot += 108)
        func_80191580(slot);

    func_80191580(&D_8019C340);
    D_8019C39C = 0;
    D_8019CBC0 = 0;
    for (i = 0; i < 200; i++)
        D_8019C830[i] = i + 1;

    i = 0;
    {
        FxCommonNode *source = D_801EA5E8;
        FxCommonNode *dest = D_8019CC58;

        for (; i < 3;) {
            D_8019CC58[i].previous = source;
            source++;
            D_801EA5E8[i].next = dest;
            dest++;
            D_801EA5E8[i].previous = 0;
            D_8019CC58[i].next = 0;
            i++;
        }
    }

    {
        short empty = -1;

        offset = 0;
        for (; offset < 800; offset += 4) {
            *(short *)((char *)D_801E4A88 + offset) = empty;
            *(short *)((char *)D_801E4A88 + offset + 2) = empty;
        }
    }

    D_8019C9D0 = 200;
    D_801E4DA8 = -1;
    D_801E4DAA = 201;
    D_801E4DAC = 200;
    D_801E4DAE = -1;
}
