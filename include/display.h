#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>

#define DISPLAY_WIDTH 64u
#define DISPLAY_HEIGHT 32u
#define DISPLAY_PAGES 4u
#define DISPLAY_BYTES (DISPLAY_WIDTH * DISPLAY_PAGES)

/* Virtual display RAM: one bit per pixel, eight vertical pixels per byte. */
typedef struct {
    uint8_t gram[DISPLAY_BYTES];
    uint32_t page_writes;
    uint32_t bytes_written;
} display_model_t;

typedef struct {
    uint8_t framebuffer[DISPLAY_BYTES];
    uint8_t dirty_pages;
    display_model_t *device;
} display_t;

void display_init(display_t *display, display_model_t *device);
void display_clear(display_t *display);
void display_pixel(display_t *display, int x, int y, bool on);
bool display_get_pixel(const display_t *display, int x, int y);
void display_fill_rect(display_t *display, int x, int y, int w, int h, bool on);
/* Each sprite byte is one horizontal row; high bit is leftmost pixel. */
void display_blit8(display_t *display, int x, int y, const uint8_t sprite[8]);
void display_flush(display_t *display);

#endif
