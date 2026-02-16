/*
 * signal_gen.c
 *
 *  Created on: 8 лют. 2026 р.
 *      Author: Олександр
 */
#include "signal_gen.h"
#include <math.h>

uint16_t sine_table[SINE_SAMPLES];

void Generete_SineTable(int touch_value)
{
    float max_dac_val = (touch_value * 4095.0f) / 33.0f;

    for(int i = 0; i < SINE_SAMPLES; i++)
    {
        float angle = (2.0f * M_PI * i) / SINE_SAMPLES;
        float sin_val = sinf(angle); // від -1 до +1

        // Пряме масштабування: -1→0, +1→max_dac_val
        float value = (sin_val + 1.0f) * max_dac_val / 2.0f;

        if(value < 0.0f) value = 0.0f;
        if(value > max_dac_val) value = max_dac_val;

        sine_table[i] = (uint16_t)value;
    }
    volatile uint16_t val_0 = sine_table[0];
    volatile uint16_t val_quarter = sine_table[128];
    volatile uint16_t val_half = sine_table[256];
    volatile uint16_t val_3quarter = sine_table[384];

    debug("Val 0 = %u\n", val_0);
    debug("val_quarter = %u\n", val_quarter);
    debug("val_half = %u\n", val_half);
    debug("val_3quarter = %u\n", val_3quarter);
}
