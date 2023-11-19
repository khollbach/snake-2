#ifndef TRON_SCREEN_H_
#define TRON_SCREEN_H_

#include <stdbool.h>

#include "../util/int.h"
#include "point.h"

void gr(bool enable);
void mixed(bool enable);
void gr_clear();
void draw_pixel(point p, u8 color);
u16 coord_to_addr(point p);

#endif
