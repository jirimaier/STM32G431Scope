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
#include "math.h"
#include "terminalpages.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void terminal_init() {
	terminalSettings.Trigger_lvl = 1.5;
	terminalSettings.PreTrigger = 50;
	terminalSettings.TrigCh = 1;
	terminalSettings.TriggerEdge = triggerOnRising;
	terminalSettings.PWM_duty = 50;
	terminalSettings.Samples_index = 2;
	terminalSettings.PWM_enabled = 1;

	terminal_triggerlineupdateneeded = 0;
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

	if (currentnumberinput == input_fs)
		com_transmit(terminal_keypad_positive, sizeof(terminal_keypad_positive));

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

		/*if (terminalSettings.PreTrigger_percent == 0) {
		 len = sprintf(txBuffer, "$$T\e[6;3H\e[0mOFF");
		 com_transmit(txBuffer, len);
		 } else if (terminalSettings.PreTrigger_percent == 100) {
		 len = sprintf(txBuffer, "$$T\e[6;3H\e[0mALL");
		 com_transmit(txBuffer, len);
		 } else {
		 len = sprintf(txBuffer, "$$T\e[6;3H\e[0m%lu%%",
		 terminalSettings.PreTrigger_percent);
		 com_transmit(txBuffer, len);
		 }*/

		/*len = sprintf(txBuffer, "$$T\e[9;1H\e[38;5;254m\e[48;5;254m0\e[10;1H1\e[11;1H2\e[12;1H3");
		 com_transmit(txBuffer, len);
		 len = sprintf(txBuffer, "\e[13;1H4\e[14;1H5\e[15;1H6\e[16;1H7\e[17;1H8\e[18;1H9\e[19;1H:");
		 com_transmit(txBuffer, len);

		 len = sprintf(txBuffer, "\e[38;5;214m\e[48;5;214m\e[%d;1H%c", 9 + terminalSettings.Fs_index, (char) ('0' + terminalSettings.Fs_index));
		 com_transmit(txBuffer, len);

		 len = sprintf(txBuffer, "\e[41;1H\e[0m%5s cycles", cycles[terminalSettings.Fs_index]);
		 com_transmit(txBuffer, len);

		 len = sprintf(txBuffer, "\e[44;1H\e[0m%4s\xce" "\xa9", in_imp[terminalSettings.Fs_index]);
		 com_transmit(txBuffer, len);

		 len = sprintf(txBuffer, "$$T\e[38;5;254m\e[48;5;254m\e[23;1Ha\e[24;1Hb\e[25;1Hc\e[26;1Hd\e[27;1He");
		 com_transmit(txBuffer, len);

		 len = sprintf(txBuffer, "\e[38;5;214m\e[48;5;214m\e[%d;1H%c", 23 + terminalSettings.Samples_index, (char) ('a' + terminalSettings.Samples_index));
		 com_transmit(txBuffer, len);*/

		/*if (terminalSettings.status == singleTrigger) {
		 len = sprintf(txBuffer, "$$T\e[0m\e[1;9H\e[41;1m WAIT ");
		 com_transmit(txBuffer, len);
		 }

		 if (terminalSettings.status == paused) {
		 len = sprintf(txBuffer, "$$T\e[0m\e[1;9H\e[41;1mPAUSED");
		 com_transmit(txBuffer, len);
		 }*/

		/*if (terminalSettings.PWM_enabled) {
		 len = sprintf(txBuffer, "$$T\e[0m\e[48;5;214m\e[38;5;214m\e[35;1HG");
		 com_transmit(txBuffer, len);
		 } else {
		 len = sprintf(txBuffer, "$$T\e[48;5;254m\e[38;5;254m\e[35;1HG");
		 com_transmit(txBuffer, len);
		 }

		 double freq = pwm_getFreq();
		 ;
		 len = sprintf(txBuffer, "\e[0m\e[36;3H%sHz", floatToNiceString(freq, 6));
		 com_transmit(txBuffer, len);

		 len = sprintf(txBuffer, "\e[0m\e[37;9H%2d", terminalSettings.PWM_duty);
		 com_transmit(txBuffer, len);*/
	}

	/*else if (currentpage == numberinput_pwm_freq) {
	 if (terminal_numericinput == 0 && terminal_numericinput_has_decimal == 0) {
	 len = sprintf(txBuffer, "$$T\e[0m\e[1;1H_");
	 com_transmit(txBuffer, len);
	 } else {
	 if (terminal_numericinput_has_decimal == 0) {
	 len = sprintf(txBuffer, "$$T\e[0m\e[1;1H%lu_", terminal_numericinput);
	 } else {
	 if (terminal_numericinput_decimal == 0)
	 len = sprintf(txBuffer, "$$T\e[0m\e[1;1H%lu._", terminal_numericinput);
	 else
	 len = sprintf(txBuffer, "$$T\e[0m\e[1;1H%lu.%lu_", terminal_numericinput, terminal_numericinput_decimal);
	 }
	 }

	 if (len > 9 + 15)
	 len = 9 + 15;
	 for (; len < 9 + 15; len++)
	 txBuffer[len] = ' ';
	 com_transmit(txBuffer, len);

	 }*/
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
	if (key == '/') {
		terminalSettings.TriggerEdge = triggerOnFalling;
	} else if (key == '\\') {
		terminalSettings.TriggerEdge = triggerOnRising;
	}

	if (key == ' ') {
		double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
		value *= 1;
		if (value > MAX_PWM_FREQ)
			value = MAX_PWM_FREQ;
		pwm_setFreq(value);
		terminal_numericinput = 0;
	} else if (key == 'k') {
		double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
		value *= 1000;
		if (value > MAX_PWM_FREQ)
			value = MAX_PWM_FREQ;
		pwm_setFreq(value);
		terminal_numericinput = 0;
	} else if (key == 'M') {
		double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
		value *= 1000000;
		if (value > MAX_PWM_FREQ)
			value = MAX_PWM_FREQ;
		pwm_setFreq(value);
		terminal_numericinput = 0;
	} else if (key == 's') {
		double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
		value = 1 / value;
		if (value > MAX_PWM_FREQ)
			value = MAX_PWM_FREQ;
		pwm_setFreq(value);
		terminal_numericinput = 0;
	} else if (key == 'm') {
		double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
		value = 1000 / value;
		if (value > MAX_PWM_FREQ)
			value = MAX_PWM_FREQ;
		pwm_setFreq(value);
		terminal_numericinput = 0;
	} else if (key == 'u') {
		double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
		value = 1000000 / value;
		if (value > MAX_PWM_FREQ)
			value = MAX_PWM_FREQ;
		pwm_setFreq(value);
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

		if (terminal_numericinput > terminal_inputmax)
			terminal_numericinput = terminal_inputmax;
		if (terminal_numericinput > terminal_inputmin)
			terminal_numericinput = terminal_inputmin;
	}

	else if (key == 'X') {
		terminal_numericinput = 0;
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
	if (sprintf(floatToNiceStringBuffer, "%.*f %c", digits - wholePartLength, value, unit) >= 10)
		Error_Handler();

	return floatToNiceStringBuffer;
}

char* floatToString(double value, uint8_t digits) {
	uint8_t wholePartLength = numberOfDigits((uint32_t) round(value));
	if (sprintf(floatToNiceStringBuffer, "%.*f", digits - wholePartLength, value) >= 10)
		Error_Handler();

	return floatToNiceStringBuffer;
}

void terminal_setnumberinput(enum terminalnumberinput input) {
	currentnumberinput = input;
	terminal_pagechanged = 1;
}
