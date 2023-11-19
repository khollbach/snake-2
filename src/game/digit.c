#include <stdbool.h>
#include <assert.h>

#include "digit.h"
#include "../util/int.h"
#include "../screen/point.h"
#include "../screen/color.h"
#include "../screen/screen.h"

#define ZERO {{ \
    {1, 1, 1}, \
    {1, 0, 1}, \
    {1, 0, 1}, \
    {1, 0, 1}, \
    {1, 1, 1}, \
}}

#define ONE {{ \
    {0, 1, 0}, \
    {0, 1, 0}, \
    {0, 1, 0}, \
    {0, 1, 0}, \
    {0, 1, 0}, \
}}

#define TWO {{ \
    {1, 1, 1}, \
    {0, 0, 1}, \
    {1, 1, 1}, \
    {1, 0, 0}, \
    {1, 1, 1}, \
}}

#define THREE {{ \
    {1, 1, 1}, \
    {0, 0, 1}, \
    {1, 1, 1}, \
    {0, 0, 1}, \
    {1, 1, 1}, \
}}

#define FOUR {{ \
    {1, 0, 1}, \
    {1, 0, 1}, \
    {1, 1, 1}, \
    {0, 0, 1}, \
    {0, 0, 1}, \
}}

#define FIVE {{ \
    {1, 1, 1}, \
    {1, 0, 0}, \
    {1, 1, 1}, \
    {0, 0, 1}, \
    {1, 1, 1}, \
}}

#define SIX {{ \
    {1, 1, 1}, \
    {1, 0, 0}, \
    {1, 1, 1}, \
    {1, 0, 1}, \
    {1, 1, 1}, \
}}

#define SEVEN {{ \
    {1, 1, 1}, \
    {0, 0, 1}, \
    {0, 0, 1}, \
    {0, 0, 1}, \
    {0, 0, 1}, \
}}

#define EIGHT {{ \
    {1, 1, 1}, \
    {1, 0, 1}, \
    {1, 1, 1}, \
    {1, 0, 1}, \
    {1, 1, 1}, \
}}

#define NINE {{ \
    {1, 1, 1}, \
    {1, 0, 1}, \
    {1, 1, 1}, \
    {0, 0, 1}, \
    {1, 1, 1}, \
}}

const point digit_dims = {3, 5};

typedef struct {
    bool pixels[5][3];
} digit;

const digit digits[10] = {
    ZERO,
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
};

void display_digit(point top_left, u8 digit) {
    point bot_right, p;
    rect screen;
    u8 dx, dy, color;

    // Bounds checks.
    bot_right = plus(top_left, digit_dims); // exclusive
    screen.top_left = zero; // inclusive
    screen.bot_right = dims; // exclusive
    assert(rect_contains(screen, top_left));
    assert(rect_contains(screen, bot_right));

    for (dy = 0; dy < digit_dims.y; dy++) {
        for (dx = 0; dx < digit_dims.x; dx++) {
            color = digits[digit].pixels[dy][dx] ? white : black;
            p.x = top_left.x + dx;
            p.y = top_left.y + dy;
            draw_pixel(p, color);
        }
    }
}
