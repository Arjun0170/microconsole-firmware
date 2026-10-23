#include "buzzer.h"

#include <string.h>

static void set_level(buzzer_t *buzzer, uint8_t level) {
    if (buzzer->speaker != 0 && buzzer->speaker->level != level) {
        buzzer->speaker->level = level;
        buzzer->speaker->transitions++;
    }
}

void buzzer_init(buzzer_t *buzzer, speaker_model_t *speaker) {
    memset(buzzer, 0, sizeof(*buzzer));
    buzzer->speaker = speaker;
}

bool buzzer_play(buzzer_t *buzzer, uint16_t frequency_hz, uint16_t duration_ms) {
    if (buzzer == 0 || frequency_hz < 20u || frequency_hz > 4000u ||
        duration_ms == 0u) return false;
    buzzer->phase = 0u;
    buzzer->frequency_hz = frequency_hz;
    buzzer->ticks_left = (uint32_t)duration_ms * 8u;
    return true;
}

void buzzer_tick_125us_isr(buzzer_t *buzzer) {
    if (buzzer->ticks_left == 0u) {
        set_level(buzzer, 0u);
        return;
    }
    buzzer->phase += buzzer->frequency_hz;
    if (buzzer->phase >= BUZZER_SAMPLE_HZ) buzzer->phase -= BUZZER_SAMPLE_HZ;
    set_level(buzzer, buzzer->phase < BUZZER_SAMPLE_HZ / 2u ? 1u : 0u);
    buzzer->ticks_left--;
    if (buzzer->ticks_left == 0u) set_level(buzzer, 0u);
}

void buzzer_stop(buzzer_t *buzzer) {
    buzzer->ticks_left = 0u;
    set_level(buzzer, 0u);
}
