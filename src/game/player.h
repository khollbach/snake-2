#ifndef TRON_GAME_PLAYER_H_
#define TRON_GAME_PLAYER_H_

#include "../util/int.h"
#include "../screen/point.h"

typedef struct {
    point pos;
    point dir;
    u8 color;
    u8 score;
} player;

void init_positions(player *p1, player *p2);
void draw(player *p);
void move(player *p);
void move_backwards(player *p);

#endif
