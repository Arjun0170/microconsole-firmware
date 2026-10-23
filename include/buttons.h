#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>

enum { BUTTON_UP = 1u, BUTTON_RIGHT = 2u, BUTTON_DOWN = 4u, BUTTON_LEFT = 8u };
#define BUTTON_DEBOUNCE_MS 5u

/* Four virtual pull-up GPIO inputs: a zero bit means pressed. */
typedef struct { volatile uint8_t input; } button_gpio_t;

typedef struct {
    button_gpio_t *gpio;
    uint8_t count[4];
    uint8_t stable_pressed;
    uint8_t press_edges;
} buttons_t;

void buttons_init(buttons_t *buttons, button_gpio_t *gpio);
/* Call once per simulated millisecond. */
void buttons_poll_1ms(buttons_t *buttons);
uint8_t buttons_take_presses(buttons_t *buttons);

#endif
