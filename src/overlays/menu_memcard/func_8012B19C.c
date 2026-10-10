#include "pe1/cdrom.h"
#include "pe1/psyq_press.h"
extern unsigned char D_800B0DBA;

void func_800870F0(int arg0);
void StUnSetRing(void);

void func_8012B19C(void) {
    unsigned char *counter;
    int value;

    counter = &D_800B0DBA;
    value = *counter - 1;
    *counter = value;
    func_800870F0(0);
    func_8010C0D8(0);
    StUnSetRing();
    DsControlB(9, 0, 0);
}
