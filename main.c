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
const point dims = {40, 24};

typedef struct {
    point pos;
    point dir;
    u8 color;
} player;

void play(u8 p1_color, u8 p2_color);
void print_ready(bool p1, bool p2);
bool check_gameover(player *p1, player *p2);
void draw_walls();
bool in_bounds(point p);
u8 try_getc();
void draw(player *p);
void move(player *p);
void move_backwards(player *p);
point plus(point p1, point p2);
bool eq(point p1, point p2);
bool nonzero(point p);
point p1_dir(u8 key);
point p2_dir(u8 key);
u8 p1_color(u8 keypress);
u8 p2_color(u8 keypress);
void gr(bool enable);
void mixed(bool enable);
void gr_clear();
void draw_pixel(point p, u8 color);
u16 coord_to_addr(point p);

int main() {
    play(magenta, dark_blue);
    assert(false); // unreachable
    return 0;
}

// Automatically restarts after each game ends.
// Player color is remembered between games.
//
// Returns never.
void play(u8 p1_start_color, u8 p2_start_color) {
    player p1, p2;
    u16 i;
    u8 key, color;
    point dir;
    point p1_input, p2_input;

    p1.pos.x = 13;
    p1.pos.y = 10;
    p1.color = p1_start_color;
    p1.dir = zero;

    p2.pos.x = 26;
    p2.pos.y = 10;
    p2.color = p2_start_color;
    p2.dir = zero;

    gr(true);
    gr_clear();

    // Draw initial positions.
    draw(&p1);
    draw(&p2);

    // Show instructions.
    mixed(true);
    gotoxy(0, 20);
    cprintf("   W                               I    ");
    cprintf("  ASD                             JKL   ");
    cprintf("                                        ");
    cprintf("                                        ");

    // Wait until both players input an initial direction.
    while (1) {
        print_ready(nonzero(p1.dir), nonzero(p2.dir));

        key = cgetc();
        dir = p1_dir(key);
        if (nonzero(dir)) p1.dir = dir;
        dir = p2_dir(key);
        if (nonzero(dir)) p2.dir = dir;

        // Also check for color-changes.
        // Player 1 can press keys on the number row to change color.
        // Player 2 can do the same while holding shift.
        color = p1_color(key);
        if (color != 0) {
            p1.color = color;
            draw(&p1);
        }
        color = p2_color(key);
        if (color != 0) {
            p2.color = color;
            draw(&p2);
        }

        if (nonzero(p1.dir) && nonzero(p2.dir)) {
            break;
        }
    }

    // Erase instructions, and use the full screen for the arena.
    mixed(false);
    gr_clear();
    draw_walls();
    draw(&p1);
    draw(&p2);

    // Move both players one step before waiting for input again.
    move(&p1);
    move(&p2);

    // Game loop.
    while (1) {
        draw(&p1);
        draw(&p2);

        // Wait for one "tick", while polling for player input.
        p1_input = p1.dir;
        p2_input = p2.dir;
        for (i = 0; i < 20; i++) {
            key = try_getc();

            // Note that we always run the loop body, so that each "tick" takes
            // a similar amount of time, regardless of whether a key is pressed.

            // Note: players can't move backwards into themselves.
            dir = p1_dir(key);
            if (nonzero(dir) && nonzero(plus(dir, p1.dir))) p1_input = dir;
            dir = p2_dir(key);
            if (nonzero(dir) && nonzero(plus(dir, p2.dir))) p2_input = dir;
        }
        p1.dir = p1_input;
        p2.dir = p2_input;

        move(&p1);
        move(&p2);

        if (check_gameover(&p1, &p2)) {
            // restart
            play(p1.color, p2.color);
            return;
        }
    }

    assert(false); // unreachable
}

void print_ready(bool p1, bool p2) {
    gotoxy(0, 23);
    if (!p1 && !p2) {
        cprintf("PLAYER 1                        PLAYER 2");
    } else if (!p1 && p2) {
        cprintf("PLAYER 1                         READY! ");
    } else if (p1 && !p2) {
        cprintf(" READY !                        PLAYER 2");
    } else {
        assert(p1 && p2);
        cprintf(" READY!                          READY! ");
    }
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

// This is plenty fast, but it's using text-mode glyphs to draw the lines, e.g.
// "-------------" is a horizontal line, so it looks glitchy in gr mode.
void draw_walls_fast() {
    chlinexy(0, 0, dims.x);
    chlinexy(0, dims.y - 1, dims.x);
    cvlinexy(0, 0, dims.y);
    cvlinexy(dims.x - 1, 0, dims.y);
}

// Draw walls around the edge of the arena.
//
// todo: this is really slow. Maybe re-write it in assembly, or find a relevant
// library function?
void draw_walls() {
    draw_walls_fast();
    // i8 x, y;
    // point p;
    // for (x = 0; x < dims.x; x++) {
    //     p.x = x;
    //     p.y = 0;
    //     draw_pixel(p, grey_1);
    //     p.x = x;
    //     p.y = dims.y - 1;
    //     draw_pixel(p, grey_1);
    // }
    // for (y = 0; y < dims.y; y++) {
    //     p.x = 0;
    //     p.y = y;
    //     draw_pixel(p, grey_1);
    //     p.x = dims.x - 1;
    //     p.y = y;
    //     draw_pixel(p, grey_1);
    // }
}

bool in_bounds(point p) {
    // off-by-ones to account for walls around the edge of the area
    return 1 <= p.x && p.x < dims.x - 1 && 1 <= p.y && p.y < dims.y - 1;
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

point plus(point p1, point p2) {
    point p;
    p.x = p1.x + p2.x;
    p.y = p1.y + p2.y;
    return p;
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

// IJKL to move in a cardinal direction.
// All other keys return 0.
point p2_dir(u8 key) {
    point p;
    p.x = p.y = 0;
    switch (key) {
    case 73: case 105: // up
        p.y = -1;
        break;
    case 74: case 106: // left
        p.x = -1;
        break;
    case 75: case 107: // down
        p.y = 1;
        break;
    case 76: case 108: // right
        p.x = 1;
        break;
    }
    return p;
}

// Return a non-black color code if the keypress is any of:
// `1234567890-=
//
// Return black (0) otherwise.
u8 p1_color(u8 keypress) {
    switch (keypress) {
    case 96: return 1;
    case 49: return 2;
    case 50: return 3;
    case 51: return 4;
    case 52: return 6; // (skip grey_1)
    case 53: return 7;
    case 54: return 8;
    case 55: return 9;
    case 56: return 11; // (skip grey_2)
    case 57: return 12;
    case 48: return 13;
    case 45: return 14;
    case 61: return 15;
    default: return 0;
    }
}

// Return a non-black color code if the keypress is any of:
// ~!@#$%^&*()_+
//
// Return black (0) otherwise.
u8 p2_color(u8 keypress) {
    switch (keypress) {
    case 126: return 1;
    case 33: return 2;
    case 64: return 3;
    case 35: return 4;
    case 36: return 6; // (skip grey_1)
    case 37: return 7;
    case 94: return 8;
    case 38: return 9;
    case 42: return 11; // (skip grey_2)
    case 40: return 12;
    case 41: return 13;
    case 95: return 14;
    case 43: return 15;
    default: return 0;
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
