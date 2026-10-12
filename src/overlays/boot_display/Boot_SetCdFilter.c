#include "common.h"
#include "pe1/boot_display.h"
#include "pe1/cdrom.h"

#include "Boot_SetCdFilter_externs.h"

int Boot_SetCdFilter(int code) {
    int result[2];
    u8 command[2];
    command[0] = 1;
    command[1] = code;
    while (func_8007F418(0, 0) != 2) {}
    DsControlB(13, command, result);
    return 0;
}

int func_801257EC(int command, int parameter) {
    int result[2];
    int status;
    int *request = &D_80172CE8;
    func_80080B44(*request, result);
    status = func_8007F0C8((u8)command, result, (u8)parameter, 0, -1);
    D_80172CF0 = *request;
    D_80172CF4++;
    return (u8)status;
}
