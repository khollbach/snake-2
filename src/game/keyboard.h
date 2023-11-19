#ifndef TRON_GAME_KEYBOARD_H_
#define TRON_GAME_KEYBOARD_H_

#include "../util/int.h"
#include "../screen/point.h"

u8 try_getc();
u8 p1_color(u8 keypress);
u8 p2_color(u8 keypress);
point p1_dir(u8 key);
point p2_dir(u8 key);

#endif
