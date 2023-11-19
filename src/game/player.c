#include "player.h"
#include "../screen/point.h"
#include "../screen/screen.h"

void draw(player *p) {
    draw_pixel(p->pos, p->color);
}

void move(player *p) {
    p->pos = plus(p->pos, p->dir);
}

void move_backwards(player *p) {
    p->pos = minus(p->pos, p->dir);
}
