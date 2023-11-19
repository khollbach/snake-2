#include <stdbool.h>
#include <conio.h>

#include "start_screen.h"
#include "player.h"
#include "keyboard.h"
#include "../util/int.h"
#include "../screen/screen.h"

void print_instructions();
void print_ready(bool p1, bool p2);
void handle_keypress(u8 key, player *p1, player *p2);
void countdown(player *p1, player *p2);
void wait(u16 time, player *p1, player *p2);

// Display instructions, and wait for both players to be ready.
void start_screen(player *p1, player *p2) {
    bool p1_ready, p2_ready;
    u8 key;

    gr(true);
    mixed(true);

    gr_clear();
    print_instructions();

    // Draw initial positions.
    draw(p1);
    draw(p2);

    // Wait until both players input an initial direction.
    while (1) {
        p1_ready = nonzero(p1->dir);
        p2_ready = nonzero(p2->dir);
        print_ready(p1_ready, p2_ready);
        if (p1_ready && p2_ready) {
            break;
        }

        key = cgetc();
        handle_keypress(key, p1, p2);
    }

    countdown(p1, p2);
}

void print_instructions() {
    gotoxy(0, 20);
    cprintf("   W                               I    ");
    cprintf("  ASD                             JKL   ");
    cprintf("                                        ");
    cprintf("PLAYER 1                        PLAYER 2");
}

void print_ready(bool p1, bool p2) {
    if (p1) {
        gotoxy(7, 21);
        cprintf("(READY!)");
    }
    if (p2) {
        gotoxy(24, 21);
        cprintf("(READY!)");
    }
}

// Update player directions accordingly.
void handle_keypress(u8 key, player *p1, player *p2) {
    point dir;
    u8 color;

    dir = p1_dir(key);
    if (nonzero(dir)) p1->dir = dir;
    dir = p2_dir(key);
    if (nonzero(dir)) p2->dir = dir;

    // Also check for color-changes.
    // Player 1 can press keys on the number row to change color.
    // Player 2 can do the same while holding shift.
    color = p1_color(key);
    if (color != 0) {
        p1->color = color;
        draw(p1);
    }
    color = p2_color(key);
    if (color != 0) {
        p2->color = color;
        draw(p2);
    }
}

// Continue checking for keypresses during the countdown.
void countdown(player *p1, player *p2) {
    gotoxy(18, 20);
    cprintf("3...");
    wait(3000, p1, p2);

    gotoxy(18, 21);
    cprintf("2...");
    wait(3000, p1, p2);

    gotoxy(18, 22);
    cprintf("1...");
    wait(3000, p1, p2);
}

void wait(u16 num_loops, player *p1, player *p2) {
    u16 time;
    u8 key;

    for (time = 0; time < num_loops; time++) {
        if ((key = try_getc())) {
            handle_keypress(key, p1, p2);
        }
    }
}
