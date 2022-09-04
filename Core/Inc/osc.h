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
volatile int16_t  currentBufferLength;
volatile uint32_t awdgTR1Part1;
volatile uint32_t awdgTR1Part2;
volatile uint8_t osc_singleTrigger;
volatile uint8_t trigSubCh;
volatile uint8_t fs_changed_flag;
uint16_t osc_autoTrigMax;
double Fs_max;

volatile enum OscStatus {
	paused, running, idle, finished
}oscStatus;

volatile enum OscTrigType {
	trig_norm, trig_auto, trig_none
}oscTrigType;

volatile enum OSCTriggerStates {
	triggerWaitFirstPart, triggerWaitSecondPart, triggerWaitingPretrigger, triggerNotWaiting
}oscTrigState;

volatile uint16_t adcBuffer1[BUFFER_SIZE];
volatile uint16_t adcBuffer2[BUFFER_SIZE];

volatile ADC_HandleTypeDef *triggerADC;

void osc_init();
void osc_sendData();
void osc_beginMeasuring();
void osc_setADCSamplingCycles(double fs);
void osc_prepareAWDGs();
void osc_setTriggerLevel(double value);
void osc_setPretrigger(double value);
void osc_setSamplingFreq(double value);
void osc_settrigch(uint8_t ch);
void osc_setNumCh(uint8_t numChPerADC);
void osc_abort();

#endif /* INC_OSC_H_ */
