#ifndef PE1_BOOT_H
#define PE1_BOOT_H

void InitSystem(void);
void Boot_VsyncCallback(void);

/* Frame counter Boot_FrameLoop advances once per frame (an array view, so
 * -G8 units still address it absolutely). */
extern unsigned int D_8009CDA4[];

#endif
