#include <assert.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdbool.h>

#include "../util/int.h"
#include "../util/memory.h"
#include "../screen/color.h"
#include "../screen/point.h"
#include "../screen/screen.h"
#include "player.h"
#include "keyboard.h"
#include "start_screen.h"

void play_game(u8 p1_color, u8 p2_color);
bool check_gameover(player *p1, player *p2);
void draw_walls();
bool in_bounds(point p);

// Returns never.
void start() {
    play_game(magenta, dark_blue);
}

// Automatically restarts after each game ends.
// Player color is remembered between games.
//
// Returns never.
void play_game(u8 p1_start_color, u8 p2_start_color) {
    player p1, p2;
    u16 i;
    u8 key;
    point dir;
    point p1_input, p2_input;

    p1.pos.x = dims.x / 3;
    p1.pos.y = dims.y / 2;
    p1.color = p1_start_color;
    p1.dir = zero;

    p2.pos.x = dims.x * 2 / 3;
    p2.pos.y = dims.y / 2;
    p2.color = p2_start_color;
    p2.dir = zero;

    start_screen(&p1, &p2);

    // Erase instructions, and use the full screen for the arena.
    mixed(false);
    gr_clear();
    draw_walls();

    // Re-draw initial positions.
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
            // Restart.
            //
            // cc65 probably doesn't do tail-call elimination, so this will
            // overflow the stack after a couple hundred games.
            play_game(p1.color, p2.color);
        }
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
    u8 i;
    u16 time;

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
            for (time = 0; time < 1000; time++);
        }
    }

    return gameover;
}

// Draw walls around the edge of the arena.
void draw_walls() {
    // A naive for-loop was too slow (maybe we're supposed to turn on optimizations
    // in the compiler?) so we're using these library routines instead.
    //
    // It's plenty fast, but it's using text-mode glyphs to draw the lines,
    // e.g.  "-------------" is a horizontal line, so it looks kinda glitchy in
    // gr mode. But honestly, it's growing on me, so we'll keep it for now.
    chlinexy(0, 0, dims.x);
    chlinexy(0, dims.y - 1, dims.x);
    cvlinexy(0, 0, dims.y);
    cvlinexy(dims.x - 1, 0, dims.y);
}

bool in_bounds(point p) {
    point one_one = {1, 1};
    rect bounds;

    // off-by-ones to account for walls around the edge of the area
    bounds.top_left = one_one;
    bounds.bot_right = minus(dims, one_one);
    return rect_contains(bounds, p);
}
