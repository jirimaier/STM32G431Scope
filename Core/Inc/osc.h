/*
 * osc.h
 *
 *  Created on: 12 Sep 2021
 *      Author: Maier
 */

#ifndef INC_OSC_H_
#define INC_OSC_H_

#include "main.h"
#include "config.h"

volatile uint16_t postTriggerSamples;
volatile uint8_t pretriggerRequiresFullBuffer;
volatile uint8_t triggerCorrection;
volatile int8_t usedSamples_index;
volatile uint32_t awdgTR1Part1;
volatile uint32_t awdgTR1Part2;
volatile uint8_t osc_triggeredNormaly;

volatile enum OscStatus {
	paused, running, idle, finished
}oscStatus;

volatile enum OscTrigType {
	trig_norm, trig_auto, trig_single, trig_none
}oscTrigType;

volatile enum OSCTriggerStates {
	triggerWaitFirstPart, triggerWaitSecondPart, triggerWaitingPretrigger, triggerNotWaiting
}oscTrigState;

//uint16_t oscbuffer[BUFFER_SIZE];
volatile uint16_t adcBuffer1[4096];
volatile uint16_t adcBuffer2[4096];

volatile ADC_HandleTypeDef *triggerADC;

void osc_init();
void osc_sendData();
void osc_beginMeasuring();
void osc_setADCSamplingCycles();
void osc_prepareAWDGs();
void osc_setTriggerLevel(uint32_t mV);
void osc_setPretrigger(uint8_t percent);
void osc_setSamplingFreq(uint32_t index);
void osc_settrigch(uint8_t ch);

#endif /* INC_OSC_H_ */
