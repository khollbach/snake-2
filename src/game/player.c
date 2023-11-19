#include "player.h"
#include "../screen/point.h"
#include "../screen/screen.h"

void init_positions(player *p1, player *p2) {
    p1->pos.x = dims.x / 3;
    p1->pos.y = dims.y / 2;

    p2->pos.x = dims.x * 2 / 3;
    p2->pos.y = dims.y / 2;
}

void draw(player *p) {
    draw_pixel(p->pos, p->color);
}

void move(player *p) {
    p->pos = plus(p->pos, p->dir);
}

void move_backwards(player *p) {
    p->pos = minus(p->pos, p->dir);
}
