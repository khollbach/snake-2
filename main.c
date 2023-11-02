#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdbool.h>

typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;

char _read_val; // needed so the compiler doesn't optimize away the read
#define ADDR(a) ((char*)a)
#define READ(a) (_read_val = *ADDR(a))
#define WRITE(a, x) (*ADDR(a) = x)

enum low_res_color {
    black = 0,
    magenta = 1,
    dark_blue = 2,
    purple = 3,
    dark_green = 4,
    grey_1 = 5,
    medium_blue = 6,
    light_blue = 7,
    brown = 8,
    orange = 9,
    grey_2 = 10,
    pink = 11,
    green = 12,
    yellow = 13,
    aqua = 14,
    white = 15,
};

typedef struct {
    u8 x;
    u8 y;
} player;

int main() {
    player p1, p2;
    u8 key, dx, dy;

    gr(true);
    gr_clear();

    p1.x = 13;
    p1.y = 10;
    p1.x = 26;
    p1.y = 10;
    while (1) {
        draw(p1.x, p1.y, magenta);
        draw(p2.x, p2.y, dark_blue);

        key = cgetc();
        p1_dir(key, &dx, &dy);
        p1.x += dx;
        p1.y += dy;
        p2_dir(key, &dx, &dy);
        p2.x += dx;
        p2.y += dy;
    }
    return 0;
}

// WASD to move in a cardinal direction.
// All other keys return 0.
void p1_dir(u8 key, u8 *dx, u8 *dy) {
    *dx = *dy = 0;
    switch (key) {
    case 87: case 119: // up
        key_y = -1;
        break;
    case 83: case 115: // down
        key_y = 1;
        break;
    case 65: case 97: // left
        key_x = -1;
        break;
    case 68: case 100: // right
        key_x = 1;
        break;
    }
}

// OKL; to move in a cardinal direction.
// All other keys return 0.
void p2_dir(u8 key, u8 *dx, u8 *dy) {
    *dx = *dy = 0;
    switch (key) {
    case 79: case 111: // up
        key_y = -1;
        break;
    case 76: case 108: // down
        key_y = 1;
        break;
    case 75: case 107: // left
        key_x = -1;
        break;
    case 58: case 59: // right
        key_x = 1;
        break;
    }
}

// Toggle graphics mode.
void gr(bool enable) {
    if (enable) {
        WRITE(0xC050, 0); // text mode off
    } else {
        WRITE(0xC051, 0); // text mode on
    }
}

// Write black to all pixels in low-res page 1.
void gr_clear() {
    memset(ADDR(0x400), 0, 0x400);
}

void draw(u8 x, u8 y, u8 color) {
    WRITE(gr_coord_to_addr(x, y), color << 4 | color);
}

// What is the memory address for this low-res pixel?
//
// Note that there's technically a "top-half" and a "bottom-half"
// to each of these "pixels". Each can hold a 4-bit color.
u16 gr_coord_to_addr(u8 x, u8 y) {
    u8 group;
    u16 base, offset;
    assert(x < 40);
    assert(y < 24);

    group = y / 8;
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

    offset = y % 8 * 0x80;

    return base + offset + x;
}
