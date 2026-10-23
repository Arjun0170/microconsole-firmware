#include "snake.h"

#include <string.h>

static uint32_t random_next(snake_t *game) {
    uint32_t x = game->rng;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    game->rng = x;
    return x;
}

static bool occupied(const snake_t *game, uint8_t x, uint8_t y) {
    for (unsigned i = 0; i < game->length; ++i)
        if (game->x[i] == x && game->y[i] == y) return true;
    return false;
}

static void place_food(snake_t *game) {
    if (game->length == SNAKE_MAX_CELLS) {
        game->won = true;
        return;
    }
    unsigned start = random_next(game) % SNAKE_MAX_CELLS;
    for (unsigned n = 0; n < SNAKE_MAX_CELLS; ++n) {
        unsigned cell = (start + n) % SNAKE_MAX_CELLS;
        uint8_t x = (uint8_t)(cell % SNAKE_COLS);
        uint8_t y = (uint8_t)(cell / SNAKE_COLS);
        if (!occupied(game, x, y)) {
            game->food_x = x;
            game->food_y = y;
            return;
        }
    }
}

void snake_init(snake_t *game, uint32_t seed) {
    memset(game, 0, sizeof(*game));
    game->length = 3u;
    game->x[0] = 8u; game->y[0] = 4u;
    game->x[1] = 7u; game->y[1] = 4u;
    game->x[2] = 6u; game->y[2] = 4u;
    game->direction = SNAKE_RIGHT;
    game->rng = seed ? seed : 1u;
    place_food(game);
}

void snake_turn(snake_t *game, snake_dir_t direction) {
    if ((unsigned)direction > SNAKE_LEFT ||
        ((unsigned)direction + 2u) % 4u == (unsigned)game->direction)
        return;
    game->direction = direction;
}

void snake_step(snake_t *game) {
    if (game->dead || game->won) return;
    int nx = game->x[0], ny = game->y[0];
    if (game->direction == SNAKE_UP) ny--;
    if (game->direction == SNAKE_RIGHT) nx++;
    if (game->direction == SNAKE_DOWN) ny++;
    if (game->direction == SNAKE_LEFT) nx--;
    if (nx < 0 || ny < 0 || nx >= (int)SNAKE_COLS || ny >= (int)SNAKE_ROWS) {
        game->dead = true;
        return;
    }
    bool grow = nx == game->food_x && ny == game->food_y;
    unsigned collision_length = game->length - (grow ? 0u : 1u);
    for (unsigned i = 0; i < collision_length; ++i) {
        if (game->x[i] == nx && game->y[i] == ny) {
            game->dead = true;
            return;
        }
    }
    if (grow) { game->length++; game->score++; }
    for (unsigned i = game->length - 1u; i > 0u; --i) {
        game->x[i] = game->x[i - 1u];
        game->y[i] = game->y[i - 1u];
    }
    game->x[0] = (uint8_t)nx;
    game->y[0] = (uint8_t)ny;
    if (grow) place_food(game);
}

void snake_render(const snake_t *game, display_t *display) {
    display_clear(display);
    for (unsigned i = 0; i < game->length; ++i)
        display_fill_rect(display, 4 * game->x[i], 4 * game->y[i], 4, 4, true);
    if (!game->won) {
        int x = 4 * game->food_x, y = 4 * game->food_y;
        display_pixel(display, x + 1, y + 1, true);
        display_pixel(display, x + 2, y + 2, true);
    }
}
