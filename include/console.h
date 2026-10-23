#ifndef CONSOLE_H
#define CONSOLE_H

#include "buttons.h"
#include "buzzer.h"
#include "display.h"
#include "snake.h"

typedef struct {
    display_t display;
    buttons_t buttons;
    button_gpio_t gpio;
    speaker_model_t speaker;
    buzzer_t buzzer;
    snake_t game;
    volatile uint32_t milliseconds;
    uint32_t next_game_tick;
    uint32_t skipped_game_ticks;
} console_t;

void console_init(console_t *console, display_model_t *display, uint32_t seed);
void console_tick_isr(console_t *console);
void console_audio_isr(console_t *console);
/* Run once per millisecond after the tick ISR, with virtual GPIO state set. */
void console_run_once(console_t *console);

#endif
