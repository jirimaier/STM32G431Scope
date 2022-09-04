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
	terminalSettings.Trigger_lvl = (VREF_HIGH + VREF_LOW) / 2.0;
	terminalSettings.PreTrigger = 0.50;
	terminalSettings.TrigCh = 1;
	terminalSettings.TriggerEdge = triggerOnRising;
	terminalSettings.PWM_duty = 0.5;
	terminalSettings.BufferLength = 4096;
	terminalSettings.PWM_enabled = 1;
	terminalSettings.NumChPerADC = 1;

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
	uint8_t len = sprintf(txBuffer, "$$Stermsize:nb;");
	com_transmit(txBuffer, len);
	if (currentpage == page_osc)
		com_transmit(terminal_pageframe_osc, sizeof(terminal_pageframe_osc));

	if (currentpage == page_gen)
		com_transmit(terminal_pageframe_gen, sizeof(terminal_pageframe_gen));

	if (currentnumberinput == input_fs) {
		com_transmit(terminal_keypad_positive, sizeof(terminal_keypad_positive));
		com_transmit(terminal_keypad_freqsettings, sizeof(terminal_keypad_freqsettings));
		uint8_t len = sprintf(txBuffer, "$$T\e[0m\e[1m\e[48;5;4m\e[9;14H<");
		com_transmit(txBuffer, len);
	}

	if (currentnumberinput == input_pwmfreq) {
		com_transmit(terminal_keypad_positive, sizeof(terminal_keypad_positive));
		com_transmit(terminal_keypad_freqsettings, sizeof(terminal_keypad_freqsettings));
		uint8_t len = sprintf(txBuffer, "$$T\e[0m\e[1m\e[48;5;4m\e[4;14H<");
		com_transmit(txBuffer, len);
	}

	if (currentnumberinput == input_bufferlength) {
		com_transmit(terminal_keypad_uint, sizeof(terminal_keypad_uint));
		com_transmit(terminal_keypad_buffersettings, sizeof(terminal_keypad_buffersettings));
		uint8_t len = sprintf(txBuffer, "$$T\e[0m\e[1m\e[48;5;4m\e[7;14H<");
		com_transmit(txBuffer, len);
	}

	if (currentnumberinput == input_pretrig) {
		com_transmit(terminal_keypad_uint, sizeof(terminal_keypad_uint));
		com_transmit(terminal_keypad_pretrig, sizeof(terminal_keypad_pretrig));
		uint8_t len = sprintf(txBuffer, "$$T\e[0m\e[1m\e[48;5;4m\e[7;8H<");
		com_transmit(txBuffer, len);
	}

	if (currentnumberinput == input_pwmduty) {
		com_transmit(terminal_keypad_positive, sizeof(terminal_keypad_positive));
		uint8_t len = sprintf(txBuffer, "$$T\e[0m\e[1m\e[48;5;4m\e[5;14H<\e[0m\e[48;5;214m\e[20;7H \e[21;7H\e[1m%%");
		com_transmit(txBuffer, len);
	}

	if (currentnumberinput == input_triglvl) {
		com_transmit(
		VREF_LOW < 0 ? terminal_keypad_all : terminal_keypad_positive, sizeof(terminal_keypad_positive));
		uint8_t len = sprintf(txBuffer, "$$T\e[0m\e[1m\e[48;5;4m\e[4;7H<");
		com_transmit(txBuffer, len);
		len = sprintf(txBuffer, "$$T\e[0m\e[48;5;214m\e[20;7H \e[1m\e[21;7Hm\e[22;7Hu\e[0mV\e[20;8HV\e[21;8HV");
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
			len = sprintf(txBuffer, "$$Strigpos:%f;trigch:%d;", terminalSettings.Trigger_lvl, terminalSettings.TrigCh);
			com_transmit(txBuffer, len);
			terminal_triggerlineupdateneeded = 0;
		}

		if (terminalSettings.TriggerEdge == triggerOnRising) {
			len = sprintf(txBuffer, "$$T\e[4;14H\e[48;5;214m\e[37;1m/\e[0m");
			com_transmit(txBuffer, len);
		}

		if (terminalSettings.TriggerEdge == triggerOnFalling) {
			len = sprintf(txBuffer, "$$T\e[4;14H\e[48;5;214m\e[37;1m\\\e[0m");
			com_transmit(txBuffer, len);
		}

		if (oscStatus == paused) {
			len = sprintf(txBuffer, "$$T\e[0m\e[2;2H\e[47;1m\e[37;1mR\e[2;8H\e[48;5;214m\e[38;5;214mP");
			com_transmit(txBuffer, len);

		} else {
			len = sprintf(txBuffer, "$$T\e[0m\e[2;2H\e[48;5;214m\e[38;5;214mR\e[2;8H\e[47;1m\e[37;1mP");
			com_transmit(txBuffer, len);
		}

		if (oscTrigType == trig_auto) {
			len = sprintf(txBuffer, "$$T\e[0m\e[5;11HAuto");
			com_transmit(txBuffer, len);
		}

		if (oscTrigType == trig_norm) {
			len = sprintf(txBuffer, "$$T\e[0m\e[5;11HNorm");
			com_transmit(txBuffer, len);
		}

		if (oscTrigType == trig_none) {
			len = sprintf(txBuffer, "$$T\e[0m\e[5;11HNone");
			com_transmit(txBuffer, len);
		}

		if (terminalSettings.NumChPerADC == 2) {
			len = sprintf(txBuffer, "$$T\e[0m\e[12;1HCh:  1  2 3  4\r\nPin: 6 11 7 12");
			com_transmit(txBuffer, len);
		}
		if (terminalSettings.NumChPerADC == 1) {
			len = sprintf(txBuffer, "$$T\e[0m\e[12;1HCh:  1  2     \r\nPin: 6 11     ");
			com_transmit(txBuffer, len);
		}

		len = sprintf(txBuffer, "$$T\e[0m\e[9;1H%-10sHz", floatToNiceString(
		CPU_clock / ((timer_adc->Instance->PSC + 1) * (timer_adc->Instance->ARR + 1)), 7));
		com_transmit(txBuffer, len);

		len = sprintf(txBuffer, "$$T\e[0m\e[5;1HCh%d", terminalSettings.TrigCh);
		com_transmit(txBuffer, len);

		len = sprintf(txBuffer, "$$T\e[0m\e[7;9H%5d", terminalSettings.BufferLength);
		com_transmit(txBuffer, len);

		int pretrig_percent = terminalSettings.PreTrigger * 100;
		if(pretrig_percent == 100)
			len = sprintf(txBuffer, "$$T\e[0m\e[7;5H100", pretrig_percent);
		else
			len = sprintf(txBuffer, "$$T\e[0m\e[7;5H%2d%%", pretrig_percent);
		com_transmit(txBuffer, len);

	} else if (currentpage == page_gen) {
		len = sprintf(txBuffer, "$$T\e[0m\e[4;1H%-10sHz", floatToNiceString(
		CPU_clock / ((timer_pwm->Instance->PSC + 1) * (timer_pwm->Instance->ARR + 1)), 7));
		com_transmit(txBuffer, len);

		len = sprintf(txBuffer, "$$T\e[0m\e[5;1HDuty: %.2f%%", ((float) __HAL_TIM_GET_COMPARE(timer_pwm, PWM_TIMER_CHANNEL)) / ((float) timer_pwm->Instance->ARR) * 100.0);
		com_transmit(txBuffer, len);
	}

	if (currentnumberinput != input_nothing) {
		if (terminal_numericinput == 0 && terminal_numericinput_has_decimal == 0) {
			len = sprintf(txBuffer, "$$T\e[0m\e[7m\e[20;1H_");
			com_transmit(txBuffer, len);
		} else {
			if (terminal_numericinput_has_decimal == 0) {
				len = sprintf(txBuffer, "$$T\e[0m\e[7m\e[20;1H%lu_", terminal_numericinput);
			} else {
				if (terminal_numericinput_decimal == 0)
					len = sprintf(txBuffer, "$$T\e[0m\e[7m\e[20;1H%lu._", terminal_numericinput);
				else
					len = sprintf(txBuffer, "$$T\e[0m\e[7m\e[20;1H%lu.%lu_", terminal_numericinput, terminal_numericinput_decimal);
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

		if (key == 'f')
			terminal_setnumberinput(input_fs);

		if (key == 't')
			terminal_setnumberinput(input_triglvl);

		if (key == 'b')
			terminal_setnumberinput(input_bufferlength);

		if (key == 'p')
			terminal_setnumberinput(input_pretrig);

		if (key == 'c')
			terminalSettings.BufferLength = 1;

		if (key == '>')
			terminal_setpage(page_gen);

		if (key == 'R') {
			osc_abort();
			oscStatus = idle;
			osc_singleTrigger = 0;
		}

		if (key == 'P') {
			osc_abort();
			oscStatus = paused;
			osc_singleTrigger = 0;
		}

		if (key == 'z') {
			terminalSettings.NumChPerADC = (terminalSettings.NumChPerADC % 2) + 1;
			osc_setNumCh(terminalSettings.NumChPerADC);
		}

		if (key == 's') {
			terminalSettings.TrigCh = (terminalSettings.TrigCh % (2 * terminalSettings.NumChPerADC)) + 1;
			terminal_triggerlineupdateneeded = 1;
		}

		if (key == 'S') {
			osc_abort();
			osc_singleTrigger = 1;
			oscTrigType = trig_norm;
		}

		if (key == 'T') {
			osc_abort();
			if (oscTrigType == trig_auto)
				oscTrigType = trig_norm;
			else if (oscTrigType == trig_norm)
				oscTrigType = trig_none;
			else if (oscTrigType == trig_none)
				oscTrigType = trig_auto;
		}
	}

	else if (currentpage == page_gen) {

		if (key == '>')
			terminal_setpage(page_osc);

		if (key == 'P')
			terminal_setnumberinput(input_pwmfreq);

		if (key == 'D')
			terminal_setnumberinput(input_pwmduty);
	}

	if (currentnumberinput == input_fs || currentnumberinput == input_pwmfreq) {
		if (key == ' ') {
			double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
			terminal_numberFinished(value);
			terminal_numericinput = 0;
		} else if (key == 'k') {
			double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
			value *= 1000.0;
			terminal_numberFinished(value);
			terminal_numericinput = 0;
		} else if (key == 'M') {
			double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
			value *= 1000000.0;
			terminal_numberFinished(value);
			terminal_numericinput = 0;
		} else if (key == 'n') {
			double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
			terminal_numberFinished(1.0 / value);
			terminal_numericinput = 0;
		} else if (key == 'm') {
			double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
			value *= 0.001;
			terminal_numberFinished(1.0 / value);
			terminal_numericinput = 0;
		} else if (key == 'u') {
			double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
			value *= 0.000001;
			terminal_numberFinished(1.0 / value);
			terminal_numericinput = 0;
		}
	}

	if (currentnumberinput == input_triglvl) {
		if (key == ' ') {
			double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
			terminal_numberFinished(value);
			terminal_numericinput = 0;
		} else if (key == 'm') {
			double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
			value *= 0.001;
			terminal_numberFinished(value);
			terminal_numericinput = 0;
		} else if (key == 'u') {
			double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
			value *= 0.000001;
			terminal_numberFinished(value);
			terminal_numericinput = 0;
		}
	}

	if (currentnumberinput == input_pwmduty) {
		if (key == ' ') {
			double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
			terminal_numberFinished(value);
			terminal_numericinput = 0;
		} else if (key == '%') {
			double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
			terminal_numberFinished(value / 100.0);
			terminal_numericinput = 0;
		}
	}

	if (currentnumberinput == input_bufferlength) {
		if (key == ' ') {
			double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
			terminal_numberFinished(value);
			terminal_numericinput = 0;
		}

		if (key == 'A') {
			double value = 512;
			terminal_numberFinished(value);
			terminal_numericinput = 0;
		}
		if (key == 'B') {
			double value = 1024;
			terminal_numberFinished(value);
			terminal_numericinput = 0;
		}
		if (key == 'C') {
			double value = 2048;
			terminal_numberFinished(value);
			terminal_numericinput = 0;
		}
		if (key == 'D') {
			double value = 4096;
			terminal_numberFinished(value);
			terminal_numericinput = 0;
		}
		if (key == 'M') {
			double value = terminal_inputmax;
			terminal_numberFinished(value);
			terminal_numericinput = 0;
		}

	}

	if (currentnumberinput == input_pretrig) {
		if (key == ' ') {
			double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
			terminal_numberFinished(value);
			terminal_numericinput = 0;
		}

		if (key == 'A') {
			double value = 0;
			terminal_numberFinished(value);
			terminal_numericinput = 0;
		}
		if (key == 'B') {
			double value = 50;
			terminal_numberFinished(value);
			terminal_numericinput = 0;
		}
		if (key == 'C') {
			double value = 100;
			terminal_numberFinished(value);
			terminal_numericinput = 0;

		}
	}

	if (key >= '0' && key <= '9') {
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
		terminal_inputmax = Fs_max / terminalSettings.NumChPerADC;
		terminal_inputmin = 0;
	}

	if (input == input_pwmfreq) {
		terminal_inputmax = MAX_PWM_FREQ;
		terminal_inputmin = 0;
	}

	if (input == input_bufferlength) {
		terminal_inputmax = BUFFER_SIZE / terminalSettings.NumChPerADC;
		terminal_inputmin = 1;
	}

	if (input == input_pretrig) {
		terminal_inputmax = 100;
		terminal_inputmin = 0;
	}

	if (input == input_pwmduty) {
		terminal_inputmax = 1;
		terminal_inputmin = 0;
	}

	terminal_pagechanged = 1;
}

void terminal_numberFinished(double value) {
	if (value > terminal_inputmax)
		value = terminal_inputmax;
	if (value < terminal_inputmin)
		value = terminal_inputmin;

	if (currentnumberinput == input_pwmfreq && value != 0)
		pwm_setFreq(value);
	if (currentnumberinput == input_fs && value != 0)
		osc_setSamplingFreq(value);
	if (currentnumberinput == input_triglvl) {
		terminalSettings.Trigger_lvl = value;
		terminal_triggerlineupdateneeded = 1;
	}
	if (currentnumberinput == input_pwmduty)
		pwm_setDuty(value);
	if (currentnumberinput == input_bufferlength)
		terminalSettings.BufferLength = value;
	if (currentnumberinput == input_pretrig)
		terminalSettings.PreTrigger = ((float) value) / 100.0;
	terminal_setnumberinput(input_nothing);
}

