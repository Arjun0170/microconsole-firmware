#include "console.h"

#include <string.h>

void console_init(console_t *console, display_model_t *device, uint32_t seed) {
    memset(console, 0, sizeof(*console));
    console->gpio.input = 0x0Fu;
    buttons_init(&console->buttons, &console->gpio);
    buzzer_init(&console->buzzer, &console->speaker);
    display_init(&console->display, device);
    snake_init(&console->game, seed);
    snake_render(&console->game, &console->display);
    display_flush(&console->display);
    console->next_game_tick = 100u;
}

void console_tick_isr(console_t *console) { console->milliseconds++; }
void console_audio_isr(console_t *console) { buzzer_tick_125us_isr(&console->buzzer); }

void console_run_once(console_t *console) {
    buttons_poll_1ms(&console->buttons);
    uint8_t presses = buttons_take_presses(&console->buttons);
    if (presses & BUTTON_UP) snake_turn(&console->game, SNAKE_UP);
    else if (presses & BUTTON_RIGHT) snake_turn(&console->game, SNAKE_RIGHT);
    else if (presses & BUTTON_DOWN) snake_turn(&console->game, SNAKE_DOWN);
    else if (presses & BUTTON_LEFT) snake_turn(&console->game, SNAKE_LEFT);
    uint32_t now = console->milliseconds;
    if ((int32_t)(now - console->next_game_tick) >= 0) {
        uint32_t releases = (now - console->next_game_tick) / 100u + 1u;
        console->skipped_game_ticks += releases - 1u;
        console->next_game_tick += releases * 100u;
        uint16_t prior_score = console->game.score;
        bool prior_dead = console->game.dead;
        snake_step(&console->game);
        if (console->game.score != prior_score)
            (void)buzzer_play(&console->buzzer, 880u, 80u);
        else if (!prior_dead && console->game.dead)
            (void)buzzer_play(&console->buzzer, 220u, 250u);
        snake_render(&console->game, &console->display);
        display_flush(&console->display);
    }
}
