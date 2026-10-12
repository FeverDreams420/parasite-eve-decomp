/* GCC_VERSION: 2.8.1 */
/* Psy-Q LIBCD SYS.OBJ, from the object start: CdStatus, CdMode, CdLastCom,
 * CdLastPos and CdReset. The rest of SYS.OBJ is in sys.c and CdIntToPos.c
 * (see docs/CODE_ORGANIZATION.md, "PsyQ objects split over several units"). */
#include "pe1/psyq_cd.h"

int CdStatus(void) {
    return *(u8 *)&D_8009AFC4;
}

int CdMode(void) {
    return g_CdMode;
}

int CdLastCom(void) {
    return g_CdLastCom;
}

CdlLOC *CdLastPos(void) {
    return &g_CdLastPos;
}

void CD_initintr(void);
int CD_init(void);
int CD_initvol(void);

int CdReset(int arg0) {
    int ret;

    if (arg0 == 2) {
        CD_initintr();
        return 1;
    }

    if (CD_init() != 0) {
        return 0;
    }

    if (arg0 == 1) {
        ret = CD_initvol();
        if (ret != 0) {
            return 0;
        }
    }

    return 1;
}
