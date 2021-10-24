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
	page_osc, page_gen
} currentpage;

enum terminalnumberinput {
	input_nothing, input_fs, input_triglvl, input_pwmfreq, input_pwmduty, input_bufferlength
} currentnumberinput;

enum TriggerEdge {
	triggerOnRising, triggerOnFalling
};

volatile struct TerminalSettings {
	double Trigger_lvl;
	double PreTrigger;
	uint8_t TrigCh;
	enum TriggerEdge TriggerEdge;
	double PWM_duty;
	uint8_t PWM_enabled;
	uint16_t BufferLength;
}terminalSettings;

uint8_t terminal_pageupdateneeded, terminal_pagechanged, terminal_triggerlineupdateneeded;
uint32_t terminal_numericinput;
uint32_t terminal_numericinput_decimal;
uint8_t terminal_numericinput_has_decimal;
uint8_t terminal_numericinput_is_negative;
double terminal_inputmax,terminal_inputmin;

char floatToNiceStringBuffer[15];

void terminal_init();
void terminal_draw();
void terminal_command(uint8_t key);
void terminal_update();
void terminal_updateValues();
void terminal_setpage(enum terminalpages page);
void terminal_setnumberinput(enum terminalnumberinput input);
void terminal_finishedNumberinput(uint32_t value);
uint8_t numberOfDigits(uint32_t number);
char *floatToNiceString(double value, uint8_t digits);
char *floatToString(double value, uint8_t length);
void terminal_numberFinished(double value);

#endif /* INC_TERMINAL_H_ */
