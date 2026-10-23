#include "console.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static void display_tests(void) {
    display_model_t model = {0};
    display_t display;
    display_init(&display, &model);
    display_pixel(&display, 0, 0, true);
    display_pixel(&display, 63, 31, true);
    display_pixel(&display, -1, 2, true);
    display_fill_rect(&display, -100, -100, 101, 101, true);
    assert(display_get_pixel(&display, 0, 0));
    assert(display_get_pixel(&display, 63, 31));
    assert(!display_get_pixel(&display, 64, 31));
    display_flush(&display);
    assert(model.page_writes == 2u && model.bytes_written == 128u);
    assert(model.gram[0] == 1u && model.gram[255] == 0x80u);
    display_flush(&display);
    assert(model.page_writes == 2u);
    uint8_t sprite[8];
    memset(sprite, 0xFF, sizeof(sprite));
    display_blit8(&display, 60, 29, sprite);
    assert(display_get_pixel(&display, 63, 31));
    assert(!display_get_pixel(&display, 59, 31));
    display_clear(&display);
    assert(!display_get_pixel(&display, 63, 31));
}

static void button_tests(void) {
    button_gpio_t gpio = {.input = 0x0Fu};
    buttons_t buttons;
    buttons_init(&buttons, &gpio);
    gpio.input &= (uint8_t)~BUTTON_UP;
    for (unsigned i = 0; i < 4u; ++i) buttons_poll_1ms(&buttons);
    gpio.input |= BUTTON_UP;
    for (unsigned i = 0; i < 4u; ++i) buttons_poll_1ms(&buttons);
    assert(buttons_take_presses(&buttons) == 0u);
    gpio.input &= (uint8_t)~BUTTON_UP;
    for (unsigned i = 0; i < 5u; ++i) buttons_poll_1ms(&buttons);
    assert(buttons_take_presses(&buttons) == BUTTON_UP);
    assert(buttons_take_presses(&buttons) == 0u);
    for (unsigned i = 0; i < 50u; ++i) buttons_poll_1ms(&buttons);
    assert(buttons_take_presses(&buttons) == 0u);
    gpio.input |= BUTTON_UP;
    for (unsigned i = 0; i < 5u; ++i) buttons_poll_1ms(&buttons);
    assert(buttons.stable_pressed == 0u);
}

static void game_tests(void) {
    snake_t game;
    snake_init(&game, 1u);
    snake_turn(&game, SNAKE_LEFT); /* reverse command rejected */
    assert(game.direction == SNAKE_RIGHT);
    game.food_x = 9u; game.food_y = 4u;
    snake_step(&game);
    assert(game.length == 4u && game.score == 1u && game.x[0] == 9u);
    snake_turn(&game, SNAKE_DOWN);
    snake_step(&game);
    assert(game.y[0] == 5u);
    game.x[0] = 2u; game.y[0] = 2u;
    game.x[1] = 1u; game.y[1] = 2u;
    game.direction = SNAKE_LEFT;
    game.food_x = 15u; game.food_y = 7u;
    snake_step(&game);
    assert(game.dead);
}

static void console_tests(void) {
    display_model_t model = {0};
    console_t console;
    console_init(&console, &model, 9u);
    assert(model.page_writes > 0u);
    uint8_t initial_x = console.game.x[0];
    for (unsigned i = 0; i < 100u; ++i) {
        console_tick_isr(&console);
        console_run_once(&console);
    }
    assert(console.game.x[0] == initial_x + 1u);
    for (unsigned i = 0; i < 400u; ++i) console_tick_isr(&console);
    console_run_once(&console);
    assert(console.skipped_game_ticks == 3u);
}

static void buzzer_tests(void) {
    speaker_model_t speaker = {0};
    buzzer_t buzzer;
    buzzer_init(&buzzer, &speaker);
    assert(!buzzer_play(&buzzer, 5000u, 10u));
    assert(buzzer_play(&buzzer, 1000u, 10u));
    for (unsigned i = 0; i < 80u; ++i) buzzer_tick_125us_isr(&buzzer);
    assert(buzzer.ticks_left == 0u && speaker.level == 0u);
    assert(speaker.transitions >= 18u && speaker.transitions <= 22u);
}

int main(void) {
    display_tests();
    button_tests();
    game_tests();
    console_tests();
    buzzer_tests();
    puts("microconsole tests passed");
    return 0;
}
