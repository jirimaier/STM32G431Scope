/*
 * pwm.h
 *
 *  Created on: 17 Sep 2021
 *      Author: Maier
 */

#include "main.h"

#ifndef INC_PWM_H_
#define INC_PWM_H_

void pwm_init();
double pwm_getFreq();
void pwm_startStop(uint8_t enabled);
void pwm_setFreq(double value);
void pwm_setDuty(double value);

#endif /* INC_PWM_H_ */
