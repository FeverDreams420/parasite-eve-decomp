/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* Psy-Q LIBDS DSREADY.OBJ, part 3 of 4: LIBDS_DSREADY_text_3D8, ER_retry. */
#include "pe1/psyq_cd.h"
#include "pe1/psyq_ds.h"
#include "pe1/cdrom.h"
#include "pe1/psyq_ds_queue.h"

#include "dsready_3_externs.h"

void LIBDS_DSREADY_text_3D8(unsigned char arg0, unsigned char *arg1) {
    DsCallback *state;
    int status;
    void *data;
    DsAsyncReadCallback callback;

    state = &D_8009B708;
    data = arg1;
    if (state[0] == 0) {
        return;
    }
    if (state[1] == 0) {
        return;
    }

    status = arg0;
    if (status == 2) {
        if (DsQueueLen() == 0) {
            ER_retry();
        }
        return;
    }

    DsReadyCallback(DS_ASYNC_READ_FIELD(state + 1, savedReadyCallback));
    DsStartCallback(DS_ASYNC_READ_FIELD(state + 1, savedStartCallback));
    callback = DS_ASYNC_READ_FIELD(state + 1, callback);
    state[1] = 0;
    if (callback != 0) {
        callback(status, data, 0);
    }
}

int ER_retry(void) {
    int mode;
    CdlLOC *position;
    int command;
    int argumentMode;
    register int limit asm("$3");

    DsReadyCallback(0);
    g_DsAsyncReadState.nextSector = DsPosToInt(DS_lastpos());
    mode = DS_lastmode() & 0xFF;
    position = DS_lastpos();
    command = func_8007FC44();
    argumentMode = mode;
    limit = -1;
    return DsPacket(argumentMode, position,
        (unsigned char)command, (DslCB)func_8008227C, limit);
}
