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
    i8 x,
    i8 y,
} point;

const point zero = {0, 0};

typedef struct {
    point pos;
    point dir;
    u8 color;
} player;

int main() {
    player p1, p2;
    u8 key;
    point dir;

    gr(true);
    gr_clear();

    p1.pos = {13, 10};
    p1.color = magenta;

    p2.pos = {26, 10};
    p2.color = dark_blue;

    while (1) {
        draw(&p1);
        draw(&p2);

        for (i = 0; i < 1000; i++) {
            key = try_getc();

            // Note that we always run the loop body, so that each "tick" takes
            // a similar amount of time, regardless of whether a key is pressed.

            dir = p1_dir(key);
            if (dir != zero) p1.dir = dir;
            dir = p2_dir(key);
            if (dir != zero) p2.dir = dir;
        }

        move(&p1);
        move(&p2);

        // Restart on wall collide.
        if (!in_bounds(p1.pos)) {
            return main();
        }
        if (!in_bounds(p2.pos)) {
            return main();
        }
    }
    return 0;
}

bool in_bounds(point p) {
    i8 x_dim = 40;
    i8 y_dim = 20; // todo: update to 24 and use fullscreen mode?
    return 0 <= p.x && p.x < x_max && 0 <= p.y && p.y < y_max;
}

// Non-blocking keyboard input.
//
// Return 0 if there's no keypress in the input buffer.
u8 try_getc() {
    if (READ(0xC000) & 0x80) {
        return cgetc();
    } else {
        return 0;
    }
}

void draw(player *p) {
    draw_pixel(p.pos, p.color);
}

void move(player *p) {
    p.pos.x += p.dir.x;
    p.pos.y += p.dir.y;
}

// WASD to move in a cardinal direction.
// All other keys return 0.
point p1_dir(u8 key) {
    switch (key) {
    case 87: case 119: // up
        return {0, -1};
    case 83: case 115: // down
        return {0, 1};
    case 65: case 97: // left
        return {-1, 0};
    case 68: case 100: // right
        return {1, 0};
    default:
        return {0, 0};
    }
}

// OKL; to move in a cardinal direction.
// All other keys return 0.
point p2_dir(u8 key) {
    switch (key) {
    case 79: case 111: // up
        return {0, -1};
    case 76: case 108: // down
        return {0, 1};
    case 75: case 107: // left
        return {-1, 0};
    case 58: case 59: // right
        return {1, 0};
    default:
        return {0, 0};
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

void draw_pixel(u8 x, u8 y, u8 color) {
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
