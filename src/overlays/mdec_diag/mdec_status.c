#include "pe1/mdec_diag.h"

int func_8010C430(void) {
    return *D_8010DB50;
}

int func_8010C448(char *message) {
    printf(D_8010BD38, message);
    *D_8010DB50 = 0x80000000;
    *D_8010DB24 = 0;
    *D_8010DB30 = 0;
    *D_8010DB30;
    *D_8010DB50 = 0x60000000;
    return 0;
}
