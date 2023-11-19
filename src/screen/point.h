#ifndef TRON_POINT_H_
#define TRON_POINT_H_

#include <stdbool.h>

#include "../util/int.h"

typedef struct {
    i8 x;
    i8 y;
} point;

typedef struct {
    point top_left;
    point bot_right;
} rect;

extern const point zero;
extern const point dims;

point plus(point p1, point p2);
point minus(point p1, point p2);
bool eq(point p1, point p2);
bool nonzero(point p);
bool rect_contains(rect r, point p);

#endif
