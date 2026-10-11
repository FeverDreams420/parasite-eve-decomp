#include "pe1/psyq_bios.h"
#include "pe1/sys.h"

void Sys_ReportError(char *message, char *detail, int value, unsigned char severity) {
    int index = severity & 0xFF;

    printf(D_80010EB0, D_80091A2C[index], message);
    printf(D_80010EC8, detail, value);

    if (index == 1) {
        while (1) {
        }
    }
}
