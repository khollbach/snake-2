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
    i8 x;
    i8 y;
} point;

const point zero = {0, 0};

typedef struct {
    point pos;
    point dir;
    u8 color;
} player;

bool check_gameover(player *p1, player *p2);
bool in_bounds(point p);
u8 try_getc();
void draw(player *p);
void move(player *p);
void move_backwards(player *p);
bool eq(point p1, point p2);
bool nonzero(point p);
point p1_dir(u8 key);
point p2_dir(u8 key);
void gr(bool enable);
void mixed(bool enable);
void gr_clear();
void draw_pixel(point p, u8 color);
u16 coord_to_addr(point p);

int main() {
    player p1, p2;
    u16 i;
    u8 key;
    point dir;

    gr(true);
    mixed(false);
    gr_clear();

    p1.pos.x = 13;
    p1.pos.y = 10;
    p1.color = magenta;
    p1.dir = zero;

    p2.pos.x = 26;
    p2.pos.y = 10;
    p2.color = dark_blue;
    p2.dir = zero;

    // Draw initial positions.
    draw(&p1);
    draw(&p2);

    // Wait until both player input an initial direction.
    // Then move them each one step.
    while (1) {
        key = cgetc();
        dir = p1_dir(key);
        if (nonzero(dir)) p1.dir = dir;
        dir = p2_dir(key);
        if (nonzero(dir)) p2.dir = dir;

        if (nonzero(p1.dir) && nonzero(p2.dir)) {
            move(&p1);
            move(&p2);
            break;
        }
    }

    // Game loop.
    while (1) {
        draw(&p1);
        draw(&p2);

        for (i = 0; i < 20; i++) {
            key = try_getc();

            // Note that we always run the loop body, so that each "tick" takes
            // a similar amount of time, regardless of whether a key is pressed.

            dir = p1_dir(key);
            if (nonzero(dir)) p1.dir = dir;
            dir = p2_dir(key);
            if (nonzero(dir)) p2.dir = dir;
        }

        move(&p1);
        move(&p2);

        if (check_gameover(&p1, &p2)) {
            return main(); // restart
        }
    }

    return 0; // unreachable
}

// Check for collisions with a wall or a snake's "tail".
//
// If collision is detected, do a "death" animation for the losing player(s),
// and return true.
//
// If no collision, return false.
bool check_gameover(player *p1, player *p2) {
    bool p1_loss, p2_loss, gameover;
    u16 i, j;

    p1_loss = !in_bounds(p1->pos) || READ(coord_to_addr(p1->pos));
    p2_loss = !in_bounds(p2->pos) || READ(coord_to_addr(p2->pos));
    gameover = p1_loss || p2_loss;

    // Death animation: blink the snake's head for the losing player(s).
    if (gameover) {
        if (p1_loss) move_backwards(p1);
        if (p2_loss) move_backwards(p2);
        for (i = 0; i < 8; i++) {
            if (p1_loss) draw_pixel(p1->pos, i % 2 ? p1->color : black);
            if (p2_loss) draw_pixel(p2->pos, i % 2 ? p2->color : black);
            for (j = 0; j < 1000; j++); // timing
        }
    }

    return gameover;
}

bool in_bounds(point p) {
    i8 x_dim = 40;
    i8 y_dim = 24;
    return 0 <= p.x && p.x < x_dim && 0 <= p.y && p.y < y_dim;
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
    draw_pixel(p->pos, p->color);
}

void move(player *p) {
    p->pos.x += p->dir.x;
    p->pos.y += p->dir.y;
}

void move_backwards(player *p) {
    p->pos.x -= p->dir.x;
    p->pos.y -= p->dir.y;
}

bool eq(point p1, point p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

bool nonzero(point p) {
    return !eq(p, zero);
}

// WASD to move in a cardinal direction.
// All other keys return 0.
point p1_dir(u8 key) {
    point p;
    p.x = p.y = 0;
    switch (key) {
    case 87: case 119: // up
        p.y = -1;
        break;
    case 83: case 115: // down
        p.y = 1;
        break;
    case 65: case 97: // left
        p.x = -1;
        break;
    case 68: case 100: // right
        p.x = 1;
        break;
    }
    return p;
}

// OKL; to move in a cardinal direction.
// All other keys return 0.
point p2_dir(u8 key) {
    point p;
    p.x = p.y = 0;
    switch (key) {
    case 79: case 111: // up
        p.y = -1;
        break;
    case 76: case 108: // down
        p.y = 1;
        break;
    case 75: case 107: // left
        p.x = -1;
        break;
    case 58: case 59: // right
        p.x = 1;
        break;
    }
    return p;
}

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
    memset(ADDR(0x400), 0, 0x400);
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
