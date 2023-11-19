#include <conio.h>

#include "keyboard.h"
#include "../util/int.h"
#include "../util/memory.h"
#include "../screen/point.h"

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

// Return a non-black color code if the keypress is a number key.
//
// Return black (0) otherwise.
u8 p1_color(u8 keypress) {
    switch (keypress) {
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 6; // (skip grey_1)
    case '6': return 7;
    case '7': return 8;
    case '8': return 9;
    case '9': return 11; // (skip grey_2)
    case '0': return 12;
    case '-': return 13;
    case '=': return 14;
    default: return 0; // (skip white)
    }
}

// Return a non-black color code if the keypress is a symbol key.
//
// Return black (0) otherwise.
u8 p2_color(u8 keypress) {
    switch (keypress) {
    case '!': return 1;
    case '@': return 2;
    case '#': return 3;
    case '$': return 4;
    case '%': return 6; // (skip grey_1)
    case '^': return 7;
    case '&': return 8;
    case '*': return 9;
    case '(': return 11; // (skip grey_2)
    case ')': return 12;
    case '_': return 13;
    case '+': return 14;
    default: return 0; // (skip white)
    }
}

// WASD to move in a cardinal direction.
// All other keys return 0.
point p1_dir(u8 key) {
    point p;
    p.x = p.y = 0;
    switch (key) {
    case 'W': case 'w': // up
        p.y = -1;
        break;
    case 'A': case 'a': // left
        p.x = -1;
        break;
    case 'S': case 's': // down
        p.y = 1;
        break;
    case 'D': case 'd': // right
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
    case 'I': case 'i': // up
        p.y = -1;
        break;
    case 'J': case 'j': // left
        p.x = -1;
        break;
    case 'K': case 'k': // down
        p.y = 1;
        break;
    case 'L': case 'l': // right
        p.x = 1;
        break;
    }
    return p;
}
