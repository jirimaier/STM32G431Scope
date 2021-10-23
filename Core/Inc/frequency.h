/*
 * frequency.h
 *
 *  Created on: 17 Sep 2021
 *      Author: Maier
 */

#include "main.h"

#ifndef INC_FREQUENCY_H_
#define INC_FREQUENCY_H_

uint8_t frequency_getSettings(volatile uint32_t *presc, volatile uint32_t *arr, double freq, uint64_t maxARR);

#endif /* INC_FREQUENCY_H_ */
