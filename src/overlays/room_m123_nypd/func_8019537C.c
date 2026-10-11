#include "func_8019537C_externs.h"

void **func_8019537C(int unused, unsigned int index) {
    if (index >= 2) {
        index = 0;
    }

    switch (index) {
    case 0:
        D_801956B8 = D_80195594;
        break;
    case 1:
        D_801956B8 = D_801955B4;
        break;
    }

    return &D_801956B8;
}
