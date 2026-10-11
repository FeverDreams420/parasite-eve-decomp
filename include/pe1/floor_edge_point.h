#ifndef PE1_FLOOR_EDGE_POINT_H
#define PE1_FLOOR_EDGE_POINT_H

#include "common.h"

/* Endpoints of the edge the player last crossed onto a ramp, as screen
 * style XY pairs: the GTE normal clip reads each point as one packed word. */
typedef union FloorEdgePoint {
    struct {
        s16 x, z;
    } point;
    u32 packed;
} FloorEdgePoint;
extern FloorEdgePoint D_8009CE0C, D_8009CE10;

#endif /* PE1_FLOOR_EDGE_POINT_H */
