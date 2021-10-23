/*
 * pwm.c
 *
 *  Created on: 17 Sep 2021
 *      Author: Maier
 */

#include "pwm.h"
#include "main.h"
#include "config.h"
#include "terminal.h"
#include "frequency.h"

void pwm_init(){
	pwm_setFreq(DEFAULT_PWM_FREQ);
	pwm_setDuty(terminalSettings.PWM_duty);
	pwm_startStop(terminalSettings.PWM_enabled);
}

void pwm_startStop(uint8_t enabled)
{
	if(enabled)
		HAL_TIM_PWM_Start(timer_pwm, PWM_TIMER_CHANNEL);
	else
		HAL_TIM_PWM_Stop(timer_pwm, PWM_TIMER_CHANNEL);
}

double pwm_getFreq() {
	return ((double)CPU_clock / (((double)timer_pwm->Instance->ARR + 1.0) * ((double)timer_pwm->Instance->PSC + 1.0)));
}

void pwm_setFreq(double value) {
	frequency_getSettings(&(timer_pwm->Instance->PSC),&(timer_pwm->Instance->ARR),value,PWM_TIMER_MAX_ARR);
	pwm_setDuty(terminalSettings.PWM_duty);

}

void pwm_setDuty(uint32_t value) {
	uint64_t CCR = timer_pwm->Instance->ARR;
	CCR*=value;
	CCR/=100;
	__HAL_TIM_SET_COMPARE(timer_pwm,PWM_TIMER_CHANNEL,(uint32_t)CCR);
}
