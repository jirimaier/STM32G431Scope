/*
 * terminal.h
 *
 *  Created on: Sep 11, 2021
 *      Author: Maier
 */

#ifndef INC_TERMINAL_H_
#define INC_TERMINAL_H_

#include "main.h"

enum terminalpages {
	mainpage, numberinput_pwm_freq
} currentpage;

enum TriggerEdge {
	triggerOnRising, triggerOnFalling
};

volatile struct TerminalSettings {
	uint32_t Trigger_mV;
	uint32_t PreTrigger_percent;
	int8_t Fs_index;
	uint8_t TrigCh;
	enum TriggerEdge TriggerEdge;
	uint8_t PWM_duty;
	uint8_t PWM_enabled;
	int8_t Samples_index;
}terminalSettings;

uint8_t terminal_pageupdateneeded, terminal_pagechanged, terminal_triggerlineupdateneeded;

uint32_t terminal_numericinput;
uint32_t terminal_numericinput_decimal;
uint8_t terminal_numericinput_has_decimal;
uint8_t terminal_numericinput_is_negative;

char floatToNiceStringBuffer[10];

void terminal_init();
void terminal_draw();
void terminal_command(uint8_t key);
void terminal_update();
void terminal_updateValues();
void terminal_setpage(enum terminalpages page);
void terminal_finishedNumberinput(uint32_t value);
uint8_t numberOfDigits(uint32_t number);
void floatToNiceString(double value);

#endif /* INC_TERMINAL_H_ */
