#ifndef SNAKE_H
#define SNAKE_H

#include <stdbool.h>
#include <stdint.h>
#include "display.h"

#define SNAKE_COLS 16u
#define SNAKE_ROWS 8u
#define SNAKE_MAX_CELLS (SNAKE_COLS * SNAKE_ROWS)

typedef enum { SNAKE_UP, SNAKE_RIGHT, SNAKE_DOWN, SNAKE_LEFT } snake_dir_t;
typedef struct {
    uint8_t x[SNAKE_MAX_CELLS];
    uint8_t y[SNAKE_MAX_CELLS];
    uint8_t food_x;
    uint8_t food_y;
    uint8_t length;
    snake_dir_t direction;
    uint32_t rng;
    uint16_t score;
    bool dead;
    bool won;
} snake_t;

void snake_init(snake_t *game, uint32_t seed);
void snake_turn(snake_t *game, snake_dir_t direction);
void snake_step(snake_t *game);
void snake_render(const snake_t *game, display_t *display);

#endif
