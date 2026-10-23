#ifndef BUZZER_H
#define BUZZER_H

#include <stdbool.h>
#include <stdint.h>

#define BUZZER_SAMPLE_HZ 8000u

/* Virtual digital output pin driven by a fixed-rate software timer ISR. */
typedef struct { uint8_t level; uint32_t transitions; } speaker_model_t;
typedef struct {
    speaker_model_t *speaker;
    uint32_t phase;
    uint16_t frequency_hz;
    uint32_t ticks_left;
} buzzer_t;

void buzzer_init(buzzer_t *buzzer, speaker_model_t *speaker);
bool buzzer_play(buzzer_t *buzzer, uint16_t frequency_hz, uint16_t duration_ms);
void buzzer_tick_125us_isr(buzzer_t *buzzer);
void buzzer_stop(buzzer_t *buzzer);

#endif
