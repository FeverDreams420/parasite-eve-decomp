#ifndef PE1_FIELDFX_CALLBACK_LIST_H
#define PE1_FIELDFX_CALLBACK_LIST_H

typedef int (*FieldFxCallbackListCallback)(int mode, void *state, int argument);

typedef struct FieldFxCallbackListEntry {
    signed short active;
    signed short age;
    unsigned char state[1];
} FieldFxCallbackListEntry;

typedef struct FieldFxCallbackList {
    int stride;
    int count;
    FieldFxCallbackListCallback callback;
    unsigned char entries[1];
} FieldFxCallbackList;

/* Effect task pools: func_800CE560 lays out a list of count entries of
 * stride bytes (plus the active/age prefix) behind its header and returns
 * the bytes used; func_800CE610 claims the first inactive entry and returns
 * its state, or null when the list is full. */
int func_800CE560(char *out, int stride, int count,
                  FieldFxCallbackListCallback callback);
void *func_800CE610(char *list);

/* Age of the entry whose callback is running, saved and restored around
 * each list walk; callbacks read it as their elapsed frame count. */
extern int D_800E27EC;

int func_800CE5AC(void *arg0, int arg1, int arg2, int arg3, void *arg4);

int func_800CE78C(char *list);

#endif /* PE1_FIELDFX_CALLBACK_LIST_H */
