typedef struct TempPoint {
    int x;
    int y;
} TempPoint;

#include "Save_GetCurrentMapNumber_externs.h"

int Str_EncodeBase32(char *out, unsigned int value);
int Str_ParseMapNumber(char *arg0);

int Save_GetCurrentMapNumber(void) {
    TempPoint point;

    Str_EncodeBase32(&point, g_SceneDispatchToken);
    return Str_ParseMapNumber(&point);
}
