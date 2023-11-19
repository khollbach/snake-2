#include "point.h"

const point zero = {0, 0};
const point dims = {40, 24};

point plus(point p1, point p2) {
    point p;
    p.x = p1.x + p2.x;
    p.y = p1.y + p2.y;
    return p;
}

point minus(point p1, point p2) {
    point p;
    p.x = p1.x - p2.x;
    p.y = p1.y - p2.y;
    return p;
}

bool eq(point p1, point p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

bool nonzero(point p) {
    return !eq(p, zero);
}

bool rect_contains(rect r, point p) {
    return r.top_left.x <= p.x && p.x <= r.bot_right.x &&
        r.top_left.y <= p.y && p.y < r.bot_right.y;
}
