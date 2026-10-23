#include "buttons.h"

#include <stdbool.h>
#include <string.h>

void buttons_init(buttons_t *buttons, button_gpio_t *gpio) {
    memset(buttons, 0, sizeof(*buttons));
    buttons->gpio = gpio;
}

void buttons_poll_1ms(buttons_t *buttons) {
    uint8_t raw = buttons->gpio->input;
    for (unsigned i = 0; i < 4u; ++i) {
        uint8_t bit = (uint8_t)(1u << i);
        bool pressed = (raw & bit) == 0u;
        if (pressed && buttons->count[i] < BUTTON_DEBOUNCE_MS)
            buttons->count[i]++;
        else if (!pressed && buttons->count[i] > 0u)
            buttons->count[i]--;
        if (buttons->count[i] == BUTTON_DEBOUNCE_MS &&
            (buttons->stable_pressed & bit) == 0u) {
            buttons->stable_pressed |= bit;
            buttons->press_edges |= bit;
        } else if (buttons->count[i] == 0u) {
            buttons->stable_pressed &= (uint8_t)~bit;
        }
    }
}

uint8_t buttons_take_presses(buttons_t *buttons) {
    uint8_t edges = buttons->press_edges;
    buttons->press_edges = 0u;
    return edges;
}
