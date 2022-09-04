/*
 * osc.c
 *
 *  Created on: 12 Sep 2021
 *      Author: Maier
 */
#include "osc.h"
#include "main.h"
#include "config.h"
#include "adc.h"
#include "tim.h"
#include "communication.h"
#include "terminal.h"
#include "frequency.h"

void osc_init() {
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);

	HAL_Delay(10);

	Fs_max = ((double)ADC_clock)/(samplingCycles[0]+0.5+12.5);
	pretriggerRequiresFullBuffer = 0;
	oscTrigState = triggerNotWaiting;
	oscStatus = idle;
	osc_singleTrigger = 0;
	trigSubCh = 0;

	osc_setNumCh(terminalSettings.NumChPerADC);

	osc_setSamplingFreq(OSC_DEFAULT_FS);

	oscTrigType = trig_norm;
	htim1.Instance->ARR = 65535;
	htim1.Instance->CNT = 1;
	HAL_TIM_Base_Start_IT(&htim1);
	oscTrigType = trig_auto;
}

void osc_sendData() {
	uint32_t begin1 = currentBufferLength * terminalSettings.NumChPerADC - hadc1.DMA_Handle->Instance->CNDTR;
	uint32_t begin2 = currentBufferLength * terminalSettings.NumChPerADC - hadc2.DMA_Handle->Instance->CNDTR;

	uint8_t len;

	double samplingPeriod = (double) (timer_adc->Instance->PSC + 1) * (double) (timer_adc->Instance->ARR + 1) / (double) CPU_clock;

	if (terminalSettings.NumChPerADC == 2)
		len = sprintf(txBuffer, "$$C1+3,%e,%d,12,%f,%f,%d;u2", samplingPeriod, currentBufferLength * terminalSettings.NumChPerADC, VREF_LOW, VREF_HIGH, currentBufferLength - postTriggerSamples);
	else
		len = sprintf(txBuffer, "$$Sclearch:3;$$C1,%e,%d,12,%f,%f,%d;u2", samplingPeriod, currentBufferLength * terminalSettings.NumChPerADC, VREF_LOW, VREF_HIGH, currentBufferLength - postTriggerSamples);
	com_transmit(txBuffer, len);
	com_transmit((char*) &adcBuffer1[begin1], 2 * (currentBufferLength * terminalSettings.NumChPerADC - begin1));
	com_transmit((char*) &adcBuffer1[0], 2 * begin1);
	com_transmit(";", 1);

	if (terminalSettings.NumChPerADC == 2)
		len = sprintf(txBuffer, "$$C2+4,%e,%d,12,%f,%f,%d;u2", samplingPeriod, currentBufferLength * terminalSettings.NumChPerADC, VREF_LOW, VREF_HIGH, currentBufferLength - postTriggerSamples);
	else
		len = sprintf(txBuffer, "$$Sclearch:4;$$C2,%e,%d,12,%f,%f,%d;u2", samplingPeriod, currentBufferLength * terminalSettings.NumChPerADC, VREF_LOW, VREF_HIGH, currentBufferLength - postTriggerSamples);
	com_transmit(txBuffer, len);
	com_transmit((char*) &adcBuffer2[begin2], 2 * (currentBufferLength * terminalSettings.NumChPerADC - begin2));
	com_transmit((char*) &adcBuffer2[0], 2 * begin2);
	com_transmit(";", 1);
}

void osc_beginMeasuring() {
	currentBufferLength = terminalSettings.BufferLength;
	if (currentBufferLength > BUFFER_SIZE / terminalSettings.NumChPerADC)
		currentBufferLength = BUFFER_SIZE / terminalSettings.NumChPerADC;

	osc_settrigch(terminalSettings.TrigCh);
	osc_setTriggerLevel(terminalSettings.Trigger_lvl);
	osc_setPretrigger(terminalSettings.PreTrigger);
	osc_prepareAWDGs();

	if (fs_changed_flag) {
		fs_changed_flag = 0;
		double fs = ((double) CPU_clock) / ((double) (timer_adc->Instance->PSC + 1) * (double) (timer_adc->Instance->ARR + 1));
		if(fs>Fs_max/terminalSettings.NumChPerADC) {
			osc_setSamplingFreq(Fs_max/terminalSettings.NumChPerADC);
			terminal_pageupdateneeded = 1;
		}

		osc_setADCSamplingCycles(fs);
		if (fs >= 65530.0)
			osc_autoTrigMax = 65535;
		else
			osc_autoTrigMax = fs;
	}

	if (oscTrigType == trig_none || currentBufferLength == 1)
		htim1.Instance->CNT = currentBufferLength + (currentBufferLength >> 3);
	else {

		htim1.Instance->CNT = (osc_autoTrigMax>currentBufferLength*2)?osc_autoTrigMax:(currentBufferLength*2);
	}

	oscTrigState = triggerWaitingPretrigger;

	SET_BIT(hadc2.Instance->CFGR, ADC_CFGR_DMAEN);
	SET_BIT(hadc1.Instance->CFGR, ADC_CFGR_DMAEN);
	if (HAL_DMA_Start_IT(hadc2.DMA_Handle, (uint32_t) &hadc2.Instance->DR, (uint32_t) adcBuffer2, currentBufferLength * terminalSettings.NumChPerADC) != HAL_OK)
		Error_Handler();
	if (HAL_ADCEx_MultiModeStart_DMA(&hadc1, (uint32_t*) adcBuffer1, currentBufferLength * terminalSettings.NumChPerADC) != HAL_OK)
		Error_Handler();

	if (HAL_TIM_Base_Start(&htim3) != HAL_OK)
		Error_Handler();

	oscStatus = running;
}

void osc_prepareAWDGs() {
	ADC_AnalogWDGConfTypeDef AnalogWDGConfig = { 0 };
	AnalogWDGConfig.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
	AnalogWDGConfig.WatchdogMode = ADC_ANALOGWATCHDOG_SINGLE_REG;
	AnalogWDGConfig.Channel = trigSubCh ? ADC_CHANNEL_3 : ADC_CHANNEL_2;
	AnalogWDGConfig.ITMode = (triggerADC == &hadc1) ? ENABLE : DISABLE;
	AnalogWDGConfig.HighThreshold = 4095;
	AnalogWDGConfig.LowThreshold = 0;
	AnalogWDGConfig.FilteringConfig = ADC_AWD_FILTERING_NONE;
	if (HAL_ADC_AnalogWDGConfig(&hadc1, &AnalogWDGConfig) != HAL_OK) {
		Error_Handler();
	}

	AnalogWDGConfig.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
	AnalogWDGConfig.WatchdogMode = ADC_ANALOGWATCHDOG_SINGLE_REG;
	AnalogWDGConfig.Channel = trigSubCh ? ADC_CHANNEL_4 : ADC_CHANNEL_3;
	AnalogWDGConfig.ITMode = (triggerADC == &hadc2) ? ENABLE : DISABLE;
	AnalogWDGConfig.HighThreshold = 4095;
	AnalogWDGConfig.LowThreshold = 0;
	AnalogWDGConfig.FilteringConfig = ADC_AWD_FILTERING_NONE;
	if (HAL_ADC_AnalogWDGConfig(&hadc2, &AnalogWDGConfig) != HAL_OK) {
		Error_Handler();
	}
}

void osc_setADCSamplingCycles(double fs) {
	uint32_t maxCycles = ADC_clock / (fs* terminalSettings.NumChPerADC) - 13;
	int8_t i = sizeof(samplingCycles) / sizeof(samplingCycles[0]) - 1;
	for (; i >= 0; i--) {
		if (samplingCycles[i] <= maxCycles)
			break;
	}
	if (i < 0) {
		uint16_t len = sprintf(txBuffer, "$$WUnable to reach this sampling frequency!");
		com_transmit(txBuffer, len);
		i = 0;
	}

	hadc1.Instance->SMPR1 = 0;
	hadc2.Instance->SMPR1 = 0;
	for (uint8_t ch = 0; ch <= 9; ch++) {
		hadc1.Instance->SMPR1 |= i << (3 * ch);
		hadc2.Instance->SMPR1 |= i << (3 * ch);

	}
	uint16_t len = sprintf(txBuffer, "$$ISamplingCycles: %d.5 (%.3fns)", samplingCycles[i], 1e9 * ((float) samplingCycles[i] + 0.5) / ADC_clock);
	com_transmit(txBuffer, len);

}

void osc_setTriggerLevel(double value) {
	uint32_t triggerLevel = ((value + ADC_REF_LOW) / (ADC_REF_HIGH - ADC_REF_LOW)) * 4096;

	uint32_t levelandhist = terminalSettings.TriggerEdge == triggerOnRising ? triggerLevel - TRIGGER_HISTERESIS : triggerLevel + TRIGGER_HISTERESIS;

	if (terminalSettings.TriggerEdge == triggerOnRising) {
		awdgTR1Part1 = (4095 << 16) | levelandhist;
		awdgTR1Part2 = (triggerLevel << 16) | 0;
	} else {
		awdgTR1Part1 = (levelandhist << 16) | 0;
		awdgTR1Part2 = (4095 << 16) | triggerLevel;
	}
}

void osc_setPretrigger(double value) {
	postTriggerSamples = currentBufferLength * (1.0 - value);
	pretriggerRequiresFullBuffer = value > 0.5;
}

void osc_setSamplingFreq(double value) {
	osc_abort();
	uint32_t psc, arr;
	frequency_getSettings(&psc, &arr, value, ADC_TIMER_MAX_ARR);
	timer_adc->Instance->PSC = psc;
	timer_adc->Instance->ARR = arr;
	timer_adc->Instance->CNT = 0;
	fs_changed_flag = 1;
}

void osc_settrigch(uint8_t ch) {
	if (ch == 1 || ch == 3)
		triggerADC = &hadc1;
	else if (ch == 2 || ch == 4)
		triggerADC = &hadc2;
	trigSubCh = (ch <= 2) ? 0 : 1;
}

void osc_abort() {
	HAL_TIM_Base_Stop(&htim3);
	HAL_ADCEx_MultiModeStop_DMA(&hadc1);
	HAL_DMA_Abort(hadc2.DMA_Handle);
	oscStatus = idle;
}

void osc_setNumCh(uint8_t numChPerADC) {
	osc_abort();
	hadc1.Instance->SQR1 &= ~0b1111;
	hadc1.Instance->SQR1 |= (numChPerADC - 1) & 0b1111;
	hadc2.Instance->SQR1 &= ~0b1111;
	hadc2.Instance->SQR1 |= (numChPerADC - 1) & 0b1111;
	fs_changed_flag = 1;
}

