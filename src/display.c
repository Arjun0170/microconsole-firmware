#include "display.h"

#include <string.h>

void display_init(display_t *display, display_model_t *device) {
    memset(display, 0, sizeof(*display));
    display->device = device;
    display->dirty_pages = (uint8_t)((1u << DISPLAY_PAGES) - 1u);
}

void display_clear(display_t *display) {
    for (unsigned page = 0; page < DISPLAY_PAGES; ++page) {
        uint8_t *row = &display->framebuffer[page * DISPLAY_WIDTH];
        for (unsigned x = 0; x < DISPLAY_WIDTH; ++x) {
            if (row[x] != 0u) {
                row[x] = 0u;
                display->dirty_pages |= (uint8_t)(1u << page);
            }
        }
    }
}

void display_pixel(display_t *display, int x, int y, bool on) {
    if (x < 0 || x >= (int)DISPLAY_WIDTH || y < 0 || y >= (int)DISPLAY_HEIGHT)
        return;
    unsigned page = (unsigned)y / 8u;
    unsigned index = page * DISPLAY_WIDTH + (unsigned)x;
    uint8_t mask = (uint8_t)(1u << ((unsigned)y % 8u));
    uint8_t value = display->framebuffer[index];
    uint8_t updated = on ? (uint8_t)(value | mask) : (uint8_t)(value & ~mask);
    if (updated != value) {
        display->framebuffer[index] = updated;
        display->dirty_pages |= (uint8_t)(1u << page);
    }
}

bool display_get_pixel(const display_t *display, int x, int y) {
    if (x < 0 || x >= (int)DISPLAY_WIDTH || y < 0 || y >= (int)DISPLAY_HEIGHT)
        return false;
    unsigned index = ((unsigned)y / 8u) * DISPLAY_WIDTH + (unsigned)x;
    return (display->framebuffer[index] & (1u << ((unsigned)y % 8u))) != 0u;
}

void display_fill_rect(display_t *display, int x, int y, int w, int h, bool on) {
    if (w <= 0 || h <= 0) return;
    int64_t x0 = x < 0 ? 0 : x;
    int64_t y0 = y < 0 ? 0 : y;
    int64_t x1 = (int64_t)x + w;
    int64_t y1 = (int64_t)y + h;
    if (x1 > DISPLAY_WIDTH) x1 = DISPLAY_WIDTH;
    if (y1 > DISPLAY_HEIGHT) y1 = DISPLAY_HEIGHT;
    for (int64_t py = y0; py < y1; ++py)
        for (int64_t px = x0; px < x1; ++px)
            display_pixel(display, (int)px, (int)py, on);
}

void display_blit8(display_t *display, int x, int y, const uint8_t sprite[8]) {
    if (sprite == 0 || x <= -8 || y <= -8 ||
        x >= (int)DISPLAY_WIDTH || y >= (int)DISPLAY_HEIGHT) return;
    for (int row = 0; row < 8; ++row)
        for (int col = 0; col < 8; ++col)
            if (sprite[row] & (0x80u >> col))
                display_pixel(display, x + col, y + row, true);
}

void display_flush(display_t *display) {
    if (display->device == 0) return;
    for (unsigned page = 0; page < DISPLAY_PAGES; ++page) {
        uint8_t bit = (uint8_t)(1u << page);
        if ((display->dirty_pages & bit) == 0u) continue;
        unsigned offset = page * DISPLAY_WIDTH;
        memcpy(&display->device->gram[offset], &display->framebuffer[offset],
               DISPLAY_WIDTH);
        display->device->page_writes++;
        display->device->bytes_written += DISPLAY_WIDTH;
        display->dirty_pages &= (uint8_t)~bit;
    }
}
