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

#define CPU_clock 170 * 1000000

#define BUFFER_SIZE 8192
#define TRIGGER_HISTERESIS 200
#define TX_BUFFER_SIZE 100

// Rate of LED blinking when waiting for connection: n-times slowet than connection check
#define LED_BLINK_NOT_CONNECTED 2

#define DEFAULT_PWM_FREQ 10000.0

static TIM_HandleTypeDef *timer_pwm = &htim2;
#define PWM_TIMER_CHANNEL  TIM_CHANNEL_4
#define MAX_PWM_FREQ 20000000.0
#define PWM_PIN 8
#define ARR_SIZE _32BIT

static const uint32_t samplingFreqsPresc[11] = { 10, 10, 10, 10, 20, 5, 5, 2, 2, 2, 1 };
static const uint32_t samplingFreqsPeriod[11] = { 17000, 8500, 3400, 1700, 850, 680, 340, 425, 170, 85, 85 };
static const uint8_t triggerCorrections1[11] = { 2, 2, 2, 2, 2, 2, 2, 3, 5, 7, 12 };
static const uint8_t triggerCorrections2[11] = { 2, 2, 2, 2, 2, 2, 2, 3, 5, 7, 12 };
static const uint32_t samplingTimes[11] = { ADC_SAMPLETIME_640CYCLES_5, ADC_SAMPLETIME_640CYCLES_5, ADC_SAMPLETIME_640CYCLES_5,
ADC_SAMPLETIME_640CYCLES_5, ADC_SAMPLETIME_640CYCLES_5, ADC_SAMPLETIME_640CYCLES_5,
ADC_SAMPLETIME_247CYCLES_5, ADC_SAMPLETIME_92CYCLES_5, ADC_SAMPLETIME_47CYCLES_5,
ADC_SAMPLETIME_24CYCLES_5, ADC_SAMPLETIME_6CYCLES_5 };

static const char *cycles[11] = { "640.5", "640.5", "640.5", "640.5", "640.5", "640.5", "247.5", "92.5", "47.5", "24.5", "6.5" };
static const char *in_imp[11] = { "600k", "600k", "600k", "600k", "600k", "600k", "230k", "87k", "44k", "23k", "6100" };

static const uint16_t bufferLengths[5] = { 256, 512, 1024, 2048, 4096 };

#define _16BIT 65536
#define _32BIT 4294967296

#endif /* INC_CONFIG_H_ */
