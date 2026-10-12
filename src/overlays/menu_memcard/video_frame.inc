#ifndef MENUCARD_UPDATE_VIDEO_FRAME_NAME
#define MENUCARD_UPDATE_VIDEO_FRAME_NAME MenuCard_UpdateVideoFrame
#define MENUCARD_VIDEO_ENTRY D_801D11AC
#define MENUCARD_VIDEO_LAST_TIME D_801D11B0
#define MENUCARD_VIDEO_FINISHED D_801D0DBD
#define MENUCARD_VIDEO_SIZE D_801D0DE0
#endif

void *MENUCARD_UPDATE_VIDEO_FRAME_NAME(VideoDisplay *display) {
    struct {
        void *data;
        VideoFrame *frame;
        VideoRect clear;
    } local;
    s32 retries;
    s32 end;
    s32 remaining;
    s32 status;
    void *data;
    /* Retail never reloads $a0 after the event-sync call below: the frame
       pointer argument lives in $a0 across it, which only a $4 variable
       reproduces. */
    register void **dataPtr asm("$4");

    retries = 2000;
    status = func_8007F72C();
    dataPtr = &local.data;
    if (status == 1) {
        status = func_8007F7A8();
        dataPtr = &local.data;
        if (status != D_800B0DD4) {
            func_800719E4(1);
        }
    }
    while (func_8007C484(dataPtr, &local.frame) != 0) {
        dataPtr = &local.data;
        if (--retries == 0) {
            return 0;
        }
    }
    end = MENUCARD_VIDEO_ENTRY->end;
    if (local.frame->time >= (end - 16)) {
        s32 position = local.frame->time + 16;

        remaining = position - end;
        remaining = 14 - remaining;
        if (remaining < 0) {
            remaining = 0;
        }
        func_800870F0(D_800B0DBE * remaining / 14);
    }
    if (local.frame->time < MENUCARD_VIDEO_LAST_TIME || local.frame->time >= MENUCARD_VIDEO_ENTRY->end) {
        MENUCARD_VIDEO_FINISHED = 1;
    }
    MENUCARD_VIDEO_LAST_TIME = local.frame->time;
    if (MENUCARD_VIDEO_SIZE[0] != local.frame->width || MENUCARD_VIDEO_SIZE[1] != local.frame->height) {
        if (D_800B0DBB) {
            local.clear.x = 0;
            local.clear.y = 0;
            local.clear.w = 480;
            local.clear.h = 480;
        } else {
            local.clear.x = 0;
            local.clear.y = 0;
            local.clear.w = 320;
            local.clear.h = 480;
        }
        ClearImage(&local.clear, 0, 0, 0);
        MENUCARD_VIDEO_SIZE[0] = local.frame->width;
        MENUCARD_VIDEO_SIZE[1] = local.frame->height;
    }
    if (D_800B0DBB) {
        display->regions[0].w = display->regions[1].w = MENUCARD_VIDEO_SIZE[0] * 3 / 2;
    } else {
        display->regions[0].w = display->regions[1].w = MENUCARD_VIDEO_SIZE[0];
    }
    data = local.data;
    display->regions[0].h = display->regions[1].h = MENUCARD_VIDEO_SIZE[1];
    display->rect.h = MENUCARD_VIDEO_SIZE[1];
    return data;
}

#undef MENUCARD_VIDEO_SIZE
#undef MENUCARD_VIDEO_FINISHED
#undef MENUCARD_VIDEO_LAST_TIME
#undef MENUCARD_VIDEO_ENTRY
#undef MENUCARD_UPDATE_VIDEO_FRAME_NAME
