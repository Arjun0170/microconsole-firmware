#include "console.h"

#include <stdio.h>
#include <string.h>

static bool pixel(const display_model_t *model, unsigned x, unsigned y) {
    return (model->gram[(y / 8u) * DISPLAY_WIDTH + x] & (1u << (y % 8u))) != 0u;
}

static void snapshot(const console_t *console, const display_model_t *model) {
    printf("\n%lu ms  score=%u  display_bytes=%lu  skipped_ticks=%lu\n",
           (unsigned long)console->milliseconds, console->game.score,
           (unsigned long)model->bytes_written,
           (unsigned long)console->skipped_game_ticks);
    for (unsigned y = 0; y < SNAKE_ROWS; ++y) {
        for (unsigned x = 0; x < SNAKE_COLS; ++x) {
            char c = pixel(model, x * 4u + 1u, y * 4u + 1u) ? '#' : ' ';
            if (x == console->game.x[0] && y == console->game.y[0]) c = '@';
            else if (!console->game.won && x == console->game.food_x &&
                     y == console->game.food_y) c = '*';
            putchar(c);
        }
        putchar('\n');
    }
}

int main(void) {
    display_model_t model = {0};
    console_t console;
    console_init(&console, &model, 0x12345678u);
    /* Place the first food on the scripted path to show the growth and tone. */
    console.game.food_x = 9u;
    console.game.food_y = 4u;
    snake_render(&console.game, &console.display);
    display_flush(&console.display);
    for (unsigned ms = 1; ms <= 2500u; ++ms) {
        /* Virtual pull-up inputs; hold each scripted press for 10 ms. */
        console.gpio.input = 0x0Fu;
        if (ms >= 490u && ms < 500u) console.gpio.input &= (uint8_t)~BUTTON_DOWN;
        if (ms >= 790u && ms < 800u) console.gpio.input &= (uint8_t)~BUTTON_LEFT;
        if (ms >= 1890u && ms < 1900u) console.gpio.input &= (uint8_t)~BUTTON_UP;
        if (ms >= 2090u && ms < 2100u) console.gpio.input &= (uint8_t)~BUTTON_RIGHT;
        console_tick_isr(&console);
        console_run_once(&console);
        for (unsigned sample = 0; sample < 8u; ++sample)
            console_audio_isr(&console);
        if (ms % 500u == 0u) snapshot(&console, &model);
    }
    printf("\nfinal: score=%u dead=%u page_writes=%lu audio_edges=%lu RAM=%zu bytes\n",
           console.game.score, console.game.dead ? 1u : 0u,
           (unsigned long)model.page_writes,
           (unsigned long)console.speaker.transitions, sizeof(console));
    return 0;
}
