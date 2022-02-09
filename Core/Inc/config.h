/*
 * config.h
 *
 *  Created on: Sep 11, 2021
 *      Author: Maier
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

#include "main.h"
#include "tim.h"

#define _16BIT 65536
#define _12BIT 4096
#define _32BIT 4294967296

#define CPU_clock (170E6)
#define ADC_clock (CPU_clock/4)

#define BUFFER_SIZE 4096
#define TRIGGER_HISTERESIS 200
#define TX_BUFFER_SIZE 200

// Rate of LED blinking when waiting for connection: n-times slower than connection check
#define LED_BLINK_NOT_CONNECTED 2

#define DEFAULT_PWM_FREQ 1000.0
#define OSC_DEFAULT_FS 100000.0

static TIM_HandleTypeDef *timer_pwm = &htim2;
#define PWM_TIMER_CHANNEL  TIM_CHANNEL_4
#define MAX_PWM_FREQ 20000000.0
#define MAX_Fs 4000000.0
#define PWM_PIN 8
#define PWM_TIMER_MAX_ARR _32BIT

static TIM_HandleTypeDef *timer_adc = &htim3;
#define ADC_RESOLUTION _12BIT
#define ADC_REF_LOW 0.0
#define ADC_REF_HIGH 3.3
#define ADC_TIMER_MAX_ARR _16BIT

#define VREF_LOW 0.0
#define VREF_HIGH 3.3

static const uint16_t bufferLengths[5] = { 256, 512, 1024, 2048, 4096 };

#endif /* INC_CONFIG_H_ */
