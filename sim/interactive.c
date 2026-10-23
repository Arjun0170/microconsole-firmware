#define _POSIX_C_SOURCE 200809L
#include "console.h"

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

static struct termios saved_terminal;
static int saved_flags;
static bool terminal_changed;

static void restore_terminal(void) {
    if (!terminal_changed) return;
    (void)tcsetattr(STDIN_FILENO, TCSANOW, &saved_terminal);
    (void)fcntl(STDIN_FILENO, F_SETFL, saved_flags);
    fputs("\033[?25h\n", stdout);
    fflush(stdout);
}

static bool setup_terminal(void) {
    if (!isatty(STDIN_FILENO) || tcgetattr(STDIN_FILENO, &saved_terminal) != 0)
        return false;
    saved_flags = fcntl(STDIN_FILENO, F_GETFL);
    if (saved_flags < 0) return false;
    struct termios raw = saved_terminal;
    raw.c_lflag &= (tcflag_t)~(ICANON | ECHO);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) != 0) return false;
    terminal_changed = true;
    if (fcntl(STDIN_FILENO, F_SETFL, saved_flags | O_NONBLOCK) != 0) {
        restore_terminal();
        return false;
    }
    (void)atexit(restore_terminal);
    fputs("\033[2J\033[?25l", stdout);
    return true;
}

static bool model_pixel(const display_model_t *model, unsigned x, unsigned y) {
    return (model->gram[(y / 8u) * DISPLAY_WIDTH + x] & (1u << (y % 8u))) != 0u;
}

static void draw(const console_t *console, const display_model_t *model) {
    fputs("\033[HMicroconsole  WASD move  R restart  Q quit\n+----------------+\n", stdout);
    for (unsigned y = 0; y < SNAKE_ROWS; ++y) {
        putchar('|');
        for (unsigned x = 0; x < SNAKE_COLS; ++x) {
            char cell = model_pixel(model, x * 4u + 1u, y * 4u + 1u) ? '#' : ' ';
            if (x == console->game.x[0] && y == console->game.y[0]) cell = '@';
            else if (!console->game.won && x == console->game.food_x &&
                     y == console->game.food_y) cell = '*';
            putchar(cell);
        }
        fputs("|\n", stdout);
    }
    printf("+----------------+\nscore=%u  %s            \n",
           console->game.score,
           console->game.won ? "YOU WIN" : console->game.dead ? "GAME OVER (R to restart)" : "running");
    fflush(stdout);
}

int main(void) {
    if (!setup_terminal()) {
        fputs("microconsole_play requires an interactive terminal.\n", stderr);
        return 1;
    }
    display_model_t model = {0};
    console_t console;
    console_init(&console, &model, (uint32_t)time(NULL));
    uint8_t held_button = 0u;
    unsigned hold_ms = 0u;
    bool quit = false;
    draw(&console, &model);
    while (!quit) {
        char key;
        ssize_t count;
        while ((count = read(STDIN_FILENO, &key, 1u)) == 1) {
            if (key == 'q' || key == 'Q') quit = true;
            if (key == 'r' || key == 'R') {
                console_init(&console, &model, (uint32_t)time(NULL));
                held_button = 0u;
                hold_ms = 0u;
                draw(&console, &model);
            }
            if (key == 'w' || key == 'W') held_button = BUTTON_UP;
            if (key == 'd' || key == 'D') held_button = BUTTON_RIGHT;
            if (key == 's' || key == 'S') held_button = BUTTON_DOWN;
            if (key == 'a' || key == 'A') held_button = BUTTON_LEFT;
            if (held_button != 0u && key != 'r' && key != 'R') hold_ms = 8u;
        }
        if (count < 0 && errno != EAGAIN && errno != EWOULDBLOCK) break;
        console.gpio.input = hold_ms ? (uint8_t)(0x0Fu & ~held_button) : 0x0Fu;
        if (hold_ms) hold_ms--;
        console_tick_isr(&console);
        console_run_once(&console);
        for (unsigned i = 0; i < 8u; ++i) console_audio_isr(&console);
        if (console.milliseconds % 100u == 0u) draw(&console, &model);
        struct timespec delay = {.tv_sec = 0, .tv_nsec = 1000000L};
        (void)nanosleep(&delay, NULL);
    }
    return 0;
}
