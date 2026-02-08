/*
 * signal_gen.c
 *
 *  Created on: 8 лют. 2026 р.
 *      Author: Олександр
 */
#include "signal_gen.h"
#include <math.h>

uint16_t sine_table[SINE_SAMPLES];

void Generete_SineTable(uint16_t amplitude_mw)
{
	// Перераховуємо макс. значення відносно 3.3V (3300 мВ)
	uint32_t max_dac_val = (amplitude_mw * 4095) / 3300;
	uint32_t offset = max_dac_val / 2;

	for(int i = 0; i < SINE_SAMPLES; i ++)
	{
		// Формула: offset + amplitude * sin(2 * pi * i / samples)
		float angle = (2.0f * M_PI * i) / SINE_SAMPLES;
		sine_table[i] = (uint16_t) (offset + (max_dac_val / 2.0f)*sinf(angle));
	}

}

