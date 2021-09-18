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

void osc_init() {
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);

	osc_triggeredNormaly = 0;
	pretriggerRequiresFullBuffer = 0;
	oscTrigState = triggerNotWaiting;
	oscStatus = idle;

	oscTrigType = trig_norm;
	htim1.Instance->CNT = 1;
	HAL_TIM_Base_Start_IT(&htim1);
	oscTrigType = trig_auto;
}

void osc_sendData() {
	uint32_t begin1 = bufferLengths[usedSamples_index] - hadc1.DMA_Handle->Instance->CNDTR;
	uint32_t begin2 = bufferLengths[usedSamples_index] - hadc2.DMA_Handle->Instance->CNDTR;

	uint8_t len;

	uint32_t samplingPeriod_ns = (htim3.Init.Period + 1) * (htim3.Init.Prescaler + 1) * 100 / 17;

	len = sprintf(txBuffer, "$$C1,%lue-9,%d,12,0,3.3,%d;u2", samplingPeriod_ns, bufferLengths[usedSamples_index], bufferLengths[usedSamples_index] - postTriggerSamples);
	com_transmit(txBuffer, len);
	com_transmit((char*) &adcBuffer1[begin1], 2 * (bufferLengths[usedSamples_index] - begin1));
	com_transmit((char*) &adcBuffer1[0], 2 * begin1);
	com_transmit(";", 1);

	len = sprintf(txBuffer, "$$C2,%lue-9,%d,12,0,3.3,%d;u2", samplingPeriod_ns, bufferLengths[usedSamples_index], bufferLengths[usedSamples_index] - postTriggerSamples);
	com_transmit(txBuffer, len);
	com_transmit((char*) &adcBuffer2[begin2], 2 * (bufferLengths[usedSamples_index] - begin2));
	com_transmit((char*) &adcBuffer2[0], 2 * begin2);
	com_transmit(";", 1);

	oscStatus = idle;
}

void osc_beginMeasuring() {
	usedSamples_index = terminalSettings.Samples_index;

	osc_prepareAWDGs();
	osc_settrigch(terminalSettings.TrigCh);
	osc_setTriggerLevel(terminalSettings.Trigger_lvl);
	osc_setPretrigger(terminalSettings.PreTrigger);
	//osc_setSamplingFreq(terminalSettings.Fs_index);

	osc_setADCSamplingCycles();

	htim1.Instance->CNT = 4 * bufferLengths[usedSamples_index];

	oscTrigState = triggerWaitingPretrigger;

	if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adcBuffer1, bufferLengths[usedSamples_index]) != HAL_OK)
		Error_Handler();
	if (HAL_ADC_Start_DMA(&hadc2, (uint32_t*) adcBuffer2, bufferLengths[usedSamples_index]) != HAL_OK)
		Error_Handler();

	if (HAL_TIM_Base_Start(&htim3) != HAL_OK)
		Error_Handler();

	oscStatus = running;
}

void osc_prepareAWDGs() {
	ADC_AnalogWDGConfTypeDef AnalogWDGConfig = { 0 };
	AnalogWDGConfig.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
	AnalogWDGConfig.WatchdogMode = ADC_ANALOGWATCHDOG_SINGLE_REG;
	AnalogWDGConfig.Channel = ADC_CHANNEL_2;
	AnalogWDGConfig.ITMode = (triggerADC == &hadc1) ? ENABLE : DISABLE;
	AnalogWDGConfig.HighThreshold = 4095;
	AnalogWDGConfig.LowThreshold = 0;
	AnalogWDGConfig.FilteringConfig = ADC_AWD_FILTERING_NONE;
	if (HAL_ADC_AnalogWDGConfig(&hadc1, &AnalogWDGConfig) != HAL_OK) {
		Error_Handler();
	}

	AnalogWDGConfig.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
	AnalogWDGConfig.WatchdogMode = ADC_ANALOGWATCHDOG_SINGLE_REG;
	AnalogWDGConfig.Channel = ADC_CHANNEL_3;
	AnalogWDGConfig.ITMode = (triggerADC == &hadc2) ? ENABLE : DISABLE;
	AnalogWDGConfig.HighThreshold = 4095;
	AnalogWDGConfig.LowThreshold = 0;
	AnalogWDGConfig.FilteringConfig = ADC_AWD_FILTERING_NONE;
	if (HAL_ADC_AnalogWDGConfig(&hadc2, &AnalogWDGConfig) != HAL_OK) {
		Error_Handler();
	}
}

void osc_setADCSamplingCycles() {
	ADC_ChannelConfTypeDef sConfig = { 0 };
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;

	//sConfig.SamplingTime = samplingTimes[terminalSettings.Fs_index];

	sConfig.Channel = ADC_CHANNEL_2;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}

	sConfig.Channel = ADC_CHANNEL_3;
	if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK) {
		Error_Handler();
	}
}

void osc_setTriggerLevel(double value) {
	uint32_t triggerLevel = ((value+ADC_REF_LOW) / (ADC_REF_HIGH-ADC_REF_LOW))*4096;

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
	postTriggerSamples = (bufferLengths[usedSamples_index] * (1.0 - value));
	pretriggerRequiresFullBuffer = (postTriggerSamples < bufferLengths[usedSamples_index] / 2);
}

void osc_setSamplingFreq(double value) {

}

void osc_settrigch(uint8_t ch) {
	if (ch == 1)
		triggerADC = &hadc1;
	else if (ch == 2)
		triggerADC = &hadc2;
}

