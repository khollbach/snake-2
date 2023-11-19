#include <assert.h>
#include <stdbool.h>

#include "../util/int.h"
#include "../util/memory.h"
#include "../screen/color.h"
#include "../screen/point.h"
#include "../screen/screen.h"
#include "player.h"
#include "keyboard.h"
#include "start_screen.h"

typedef enum {
    ongoing,
    p1_win,
    p2_win,
    tie,
} gameover;

// The coordinates of the top-left wall tile.
const point arena_top_left = {0, 1};

void play_match(player *p1, player *p2);
void play_game(player *p1, player *p2);
gameover check_gameover(player *p1, player *p2);
void draw_walls();
bool in_bounds(point p);

// Returns never.
void start() {
    player p1, p2;

    p1.color = magenta;
    p2.color = dark_blue;

    while (1) {
        play_match(&p1, &p2);
    }
}

void play_match(player *p1, player *p2) {
    p1->score = 0;
    p2->score = 0;

    while (p1->score < 5 && p2->score < 5) {
        play_game(p1, p2);
    }

    match_end_screen(p1, p2);
}

// Updates the winner's score. (Ties are possible.)
void play_game(player *p1, player *p2) {
    u16 i;
    u8 key;
    point dir;
    point p1_input, p2_input;

    init_positions(p1, p2);
    start_screen(p1, p2);

    // Erase instructions, and use the full screen for the arena.
    mixed(false);
    gr_clear();
    draw_walls();

    // Re-draw initial positions.
    draw(p1);
    draw(p2);

    // Move both players one step before waiting for input again.
    move(p1);
    move(p2);

    // Game loop.
    while (1) {
        draw(p1);
        draw(p2);

        // Wait for one "tick", while polling for player input.
        p1_input = p1->dir;
        p2_input = p2->dir;
        for (i = 0; i < 20; i++) {
            key = try_getc();

            // Note that we always run the loop body, so that each "tick" takes
            // a similar amount of time, regardless of whether a key is pressed.

            // Note: players can't move backwards into themselves.
            dir = p1_dir(key);
            if (nonzero(dir) && nonzero(plus(dir, p1->dir))) p1_input = dir;
            dir = p2_dir(key);
            if (nonzero(dir) && nonzero(plus(dir, p2->dir))) p2_input = dir;
        }
        p1->dir = p1_input;
        p2->dir = p2_input;

        move(p1);
        move(p2);

        switch (check_gameover(p1, p2)) {
        case ongoing:
            continue;
        case p1_win:
            p1->score += 1;
            return;
        case p2_win:
            p2->score += 1;
            return;
        case tie:
            return;
        }
    }
}

// Check for collisions with a wall or a snake's "tail".
//
// If collision is detected, do a "death" animation for the losing player(s).
gameover check_gameover(player *p1, player *p2) {
    bool p1_loss, p2_loss;
    u8 i;
    u16 time;

    p1_loss = !in_bounds(p1->pos) || READ(coord_to_addr(p1->pos));
    p2_loss = !in_bounds(p2->pos) || READ(coord_to_addr(p2->pos));

    // Death animation: blink the snake's head for the losing player(s).
    if (p1_loss || p2_loss) {
        if (p1_loss) move_backwards(p1);
        if (p2_loss) move_backwards(p2);
        for (i = 0; i < 8; i++) {
            if (p1_loss) draw_pixel(p1->pos, i % 2 ? p1->color : black);
            if (p2_loss) draw_pixel(p2->pos, i % 2 ? p2->color : black);
            for (time = 0; time < 1000; time++);
        }
    }

    if (!p1_loss && !p2_loss) {
        return ongoing;
    } else if (p1_loss && p2_loss) {
        return tie;
    } else if (p2_loss) {
        return p1_win;
    } else {
        assert(p1_loss);
        return p2_win;
    }
}

// Draw walls around the edge of the arena.
void draw_walls() {
    rect box;
    box.top_left = arena_top_left;
    box.bot_right = dims;
    draw_box(box);
}

// Is `p` inside the walls?
bool in_bounds(point p) {
    rect outer, inner;

    outer.top_left = arena_top_left;
    outer.bot_right = dims;

    inner.top_left = plus(outer.top_left, one_one);
    inner.bot_right = minus(outer.bot_right, one_one);

    return rect_contains(inner, p);
}
