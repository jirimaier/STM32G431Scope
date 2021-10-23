/*
 * frequency.c
 *
 *  Created on: 17 Sep 2021
 *      Author: Maier
 */

#include "frequency.h"
#include "config.h"
#include "math.h"

uint8_t frequency_getSettings(volatile uint32_t *presc,volatile uint32_t *arr, double freq, uint64_t maxARR) {
	uint64_t scale = roundf(((float) CPU_clock) / ((float) freq));

	for (uint32_t psc = 1; psc <= _16BIT; psc++) {
		if (scale % psc == 0) {
			if (scale / psc <= maxARR) {
				*presc = psc - 1;
				*arr = scale / psc - 1;

				return 1;
			}
		}
	}

	uint32_t psc = ceil(scale / maxARR);
	if (psc > _16BIT)
		psc = _16BIT;
	uint64_t period = scale / psc;
	if (period > maxARR)
		period = maxARR;
	*arr = period - 1;
	*presc = psc - 1;

	return 0;
}

