#include <string.h>
#include <assert.h>

#include "screen.h"
#include "../util/memory.h"

// Toggle graphics mode.
void gr(bool enable) {
    if (enable) {
        WRITE(0xC050, 0); // text mode off
    } else {
        WRITE(0xC051, 0); // text mode on
    }
}

// Toggle mixed graphics and text.
void mixed(bool enable) {
    if (enable) {
        WRITE(0xC053, 0);
    } else {
        WRITE(0xC052, 0);
    }
}

// Write black to all pixels in low-res page 1.
void gr_clear() {
    memset((void*) 0x400, 0, 0x400);
}

void draw_pixel(point p, u8 color) {
    WRITE(coord_to_addr(p), color << 4 | color);
}

// What is the memory address for this low-res pixel?
//
// Note that there's technically a "top-half" and a "bottom-half"
// to each of these "pixels". Each can hold a 4-bit color.
u16 coord_to_addr(point p) {
    u8 group;
    u16 base, offset;
    assert(p.x < 40);
    assert(p.y < 24);

    group = p.y / 8;
    switch (group) {
    case 0:
        base = 0x400;
        break;
    case 1:
        base = 0x428;
        break;
    case 2:
        base = 0x450;
        break;
    }

    offset = p.y % 8 * 0x80;

    return base + offset + p.x;
}
