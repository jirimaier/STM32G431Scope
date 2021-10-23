/*
 * terminal.c
 *
 *  Created on: Sep 11, 2021
 *      Author: Maier
 */

#include "terminal.h"
#include "config.h"
#include "main.h"
#include "communication.h"
#include "pwm.h"
#include "osc.h"
#include "math.h"
#include "terminalpages.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void terminal_init() {
	terminalSettings.Trigger_lvl = 1.5;
	terminalSettings.PreTrigger = 0.50;
	terminalSettings.TrigCh = 1;
	terminalSettings.TriggerEdge = triggerOnRising;
	terminalSettings.PWM_duty = 50;
	terminalSettings.BufferLength = 4096;
	terminalSettings.PWM_enabled = 1;

	terminal_triggerlineupdateneeded = 1;
	terminal_pagechanged = 0;
	terminal_pageupdateneeded = 0;
	terminal_numericinput = 0;
	terminal_numericinput_decimal = 0;
	terminal_numericinput_has_decimal = 0;
	terminal_numericinput_is_negative = 0;
}

void terminal_setpage(enum terminalpages page) {
	terminal_numericinput = 0;
	terminal_numericinput_decimal = 0;
	terminal_numericinput_has_decimal = 0;
	terminal_numericinput_is_negative = 0;
	currentpage = page;
	terminal_pagechanged = 1;

	if (page == page_osc)
		terminal_setnumberinput(input_fs);
	if (page == page_gen)
		terminal_setnumberinput(input_pwmfreq);
}

uint8_t numberOfDigits(uint32_t number) {
	uint8_t value = 1;
	while ((number /= 10) != 0)
		value++;

	return value;
}

void terminal_draw() {
	if (currentpage == page_osc)
			com_transmit(terminal_pageframe_osc, sizeof(terminal_pageframe_osc));

	if (currentpage == page_gen)
			com_transmit(terminal_pageframe_gen, sizeof(terminal_pageframe_gen));

	if (currentnumberinput == input_fs) {
		com_transmit(terminal_keypad_positive, sizeof(terminal_keypad_positive));
		uint8_t len = sprintf(txBuffer, "$$T\e[0m\e[1m\e[48;5;4m\e[9;14H<");
		com_transmit(txBuffer, len);
	}

	if (currentnumberinput == input_pwmfreq) {
		com_transmit(terminal_keypad_positive, sizeof(terminal_keypad_positive));
		//uint8_t len = sprintf(txBuffer, "$$T\e[0m\e[1m\e[48;5;4m\e[9;14H<", floatToString(terminalSettings.Trigger_lvl, 4));
		//com_transmit(txBuffer, len);
	}

	if (currentnumberinput == input_triglvl) {
		com_transmit(VREF_LOW < 0 ? terminal_keypad_all : terminal_keypad_positive, sizeof(terminal_keypad_positive));
		uint8_t len = sprintf(txBuffer, "$$T\e[0m\e[1m\e[48;5;4m\e[4;7H<");
		com_transmit(txBuffer, len);
	}

	terminal_updateValues();
}

void terminal_updateValues() {
	uint16_t len;

	if (currentpage == page_osc) {
		len = sprintf(txBuffer, "$$T\e[4;1H\e[0m%-5sV", floatToString(terminalSettings.Trigger_lvl, 4));
		com_transmit(txBuffer, len);

		if (terminal_triggerlineupdateneeded) {
			len = sprintf(txBuffer, "$$Strigpos:%f;", terminalSettings.Trigger_lvl);
			com_transmit(txBuffer, len);
			terminal_triggerlineupdateneeded = 0;
		}

		if (terminalSettings.TriggerEdge == triggerOnRising) {
			len = sprintf(txBuffer, "$$T\e[4;9H\e[48;5;214m\e[37;1m/\e[0m");
			com_transmit(txBuffer, len);
		}

		if (terminalSettings.TriggerEdge == triggerOnFalling) {
			len = sprintf(txBuffer, "$$T\e[4;9H\e[48;5;214m\e[37;1m\\\e[0m");
			com_transmit(txBuffer, len);
		}

		len = sprintf(txBuffer, "$$T\e[9;1H%-10sHz", floatToNiceString(CPU_clock / ((timer_adc->Instance->PSC + 1) * (timer_adc->Instance->ARR + 1)), 7));
		com_transmit(txBuffer, len);

	}

	if (currentnumberinput != input_nothing) {
		if (terminal_numericinput == 0 && terminal_numericinput_has_decimal == 0) {
			len = sprintf(txBuffer, "$$T\e[0m\e[7m\e[18;1H_");
			com_transmit(txBuffer, len);
		} else {
			if (terminal_numericinput_has_decimal == 0) {
				len = sprintf(txBuffer, "$$T\e[0m\e[7m\e[18;1H%lu_", terminal_numericinput);
			} else {
				if (terminal_numericinput_decimal == 0)
					len = sprintf(txBuffer, "$$T\e[0m\e[7m\e[18;1H%lu._", terminal_numericinput);
				else
					len = sprintf(txBuffer, "$$T\e[0m\e[7m\e[18;1H%lu.%lu_", terminal_numericinput, terminal_numericinput_decimal);
			}
		}
		if (len > 12 + 18)
			len = 12 + 18;
		for (; len < 12 + 18; len++)
			txBuffer[len] = ' ';
		com_transmit(txBuffer, len);
	}
}

void terminal_update() {
	if (terminal_pagechanged) {
		terminal_draw();
		terminal_pagechanged = 0;
	} else if (terminal_pageupdateneeded) {
		terminal_updateValues();
		terminal_pageupdateneeded = 0;
	}
}

void terminal_command(uint8_t key) {
	terminal_pageupdateneeded = 1;

	if (currentpage == page_osc) {
		if (key == '/' || key == '\\') {
			if (terminalSettings.TriggerEdge == triggerOnFalling)
				terminalSettings.TriggerEdge = triggerOnRising;
			else
				terminalSettings.TriggerEdge = triggerOnFalling;
		}

		if (key == 'f') {
			terminal_setnumberinput(input_fs);
		}

		if (key == 't') {
			terminal_setnumberinput(input_triglvl);
		}

		if(key == '>'){
			terminal_setpage(page_gen);
		}
	}

	else if (currentpage == page_gen) {

		if(key == '>'){
					terminal_setpage(page_osc);
				}
	}

	if (key == ' ') {
		double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
		value *= 1;
		terminal_numberFinished(value);
		terminal_numericinput = 0;
	} else if (key == 'k') {
		double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
		value *= 1000;
		terminal_numberFinished(value);
		terminal_numericinput = 0;
	} else if (key == 'M') {
		double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
		value *= 1000000;
		terminal_numberFinished(value);
		terminal_numericinput = 0;
	} else if (key == 's') {
		double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
		value = 1 / value;
		terminal_numberFinished(value);
		terminal_numericinput = 0;
	} else if (key == 'm') {
		double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
		value = 1000 / value;
		terminal_numberFinished(value);
		terminal_numericinput = 0;
	} else if (key == 'u') {
		double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
		value = 1000000 / value;
		terminal_numberFinished(value);
		terminal_numericinput = 0;
	}

	else if (key >= '0' && key <= '9') {
		if (key == '0' && terminal_numericinput == 0)
			terminal_numericinput_has_decimal = 1;
		uint32_t *value = terminal_numericinput_has_decimal ? &terminal_numericinput_decimal : &terminal_numericinput;
		if (*value == 0)
			*value = key - '0';
		else {
			*value *= 10;
			*value += key - '0';
		}
	}

	else if (key == 'X') {
		terminal_setnumberinput(input_nothing);
	}

	else if (key == '.') {
		terminal_numericinput_has_decimal = 1;
	}

	else if (key == '<') {
		if (terminal_numericinput_has_decimal) {
			if (terminal_numericinput_decimal == 0)
				terminal_numericinput_has_decimal = 0;
			terminal_numericinput_decimal /= 10;
		} else {
			terminal_numericinput /= 10;
		}
	}
}

char* floatToNiceString(double value, uint8_t digits) {
	char unit = 0;
	if (value >= 1E6) {
		unit = 'M';
		value /= 1E6;
	} else if (value >= 1E3) {
		unit = 'k';
		value /= 1E3;
	} else if (value >= 1) {
		unit = 0;
	} else if (value >= 1E-3) {
		unit = 'm';
		value /= 1E-3;
	} else if (value >= 1E-6) {
		unit = 'u';
		value /= 1E-6;
	}
	uint8_t wholePartLength = numberOfDigits((uint32_t) round(value));
	if (sprintf(floatToNiceStringBuffer, "%.*f %c", digits - wholePartLength, value, unit) >= 15)
		Error_Handler();

	return floatToNiceStringBuffer;
}

char* floatToString(double value, uint8_t digits) {
	uint8_t wholePartLength = numberOfDigits((uint32_t) round(value));
	if (sprintf(floatToNiceStringBuffer, "%.*f", digits - wholePartLength, value) >= 15)
		Error_Handler();

	return floatToNiceStringBuffer;
}

void terminal_setnumberinput(enum terminalnumberinput input) {
	currentnumberinput = input;

	terminal_numericinput = 0;
	terminal_numericinput_decimal = 0;
	terminal_numericinput_has_decimal = 0;
	terminal_numericinput_is_negative = 0;

	if (input == input_fs) {
		terminal_inputmax = MAX_Fs;
		terminal_inputmin = 0;
	}

	if (input == input_pwmfreq) {
		terminal_inputmax = MAX_PWM_FREQ;
		terminal_inputmin = 0;
	}

	if (input == input_triglvl) {
		terminal_inputmax = VREF_HIGH;
		terminal_inputmin = VREF_LOW;
	}

	terminal_pagechanged = 1;
}

void terminal_numberFinished(double value) {
	if (value > terminal_inputmax)
		value = terminal_inputmax;
	if (value < terminal_inputmin)
		value = terminal_inputmin;

	if (currentnumberinput == input_pwmfreq && value !=0)
		pwm_setFreq(value);
	if (currentnumberinput == input_fs && value != 0)
		osc_setSamplingFreq(value);
	if (currentnumberinput == input_triglvl){
		terminalSettings.Trigger_lvl = value;
		terminal_triggerlineupdateneeded=1;
	}
	terminal_setnumberinput(input_nothing);
}

