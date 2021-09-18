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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void terminal_init() {
	terminalSettings.Trigger_mV = 1500;
	terminalSettings.PreTrigger_percent = 50;
	terminalSettings.Fs_index = 7;
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
}

uint8_t numberOfDigits(uint32_t number) {
	uint8_t value = 1;
	while ((number /= 10) != 0)
		value++;

	return value;
}

void terminal_draw() {
	uint16_t len;

	if (currentpage == mainpage) {
		len = sprintf(txBuffer, "$$T\e[2JTrigger\e[2;2HCh1\e[3;2HCh2\e[2;9HV\e[3;6H\e[1m\e[48;5;214m-\e[3;7H+\e[0m");
		com_transmit(txBuffer, len);
		len = sprintf(txBuffer, "\e[0m\e[5;1HPretrigger\e[6;2H\e[1m\e[48;5;214m<\e[6;6H>\e[0m");
		com_transmit(txBuffer, len);
		len = sprintf(txBuffer, "\e[0m\e[8;1HSampling\e[9;4H1 kHz\e[10;4H2 kHz\e[11;4H5 kHz\e[12;3H10 kHz\e[13;3H20 kHz");
		com_transmit(txBuffer, len);
		len = sprintf(txBuffer, "\e[14;3H50 kHz\e[15;2H100 kHz\e[16;2H200 kHz\e[17;2H500 kHz\e[18;4H1 MHz\e[19;4H2 MHz");
		com_transmit(txBuffer, len);
		len = sprintf(txBuffer, "\e[2;12H\e[0m\e[48;5;214m\e[1mRPS");
		com_transmit(txBuffer, len);
		len = sprintf(txBuffer, "\e[0m\e[22;1HSamples\e[23;4H256\e[24;4H512\e[25;3H1024\e[26;3H2048\e[27;3H4096");
		com_transmit(txBuffer, len);

		len = sprintf(txBuffer, "\e[0m\e[30;1HChannels\e[31;3HCh1 (6)\e[32;3HCh2 (11)");
		com_transmit(txBuffer, len);
		len = sprintf(txBuffer, "\e[34;1HGenerators\e[35;3HPWM (%d)\e[36;14H\e[38;5;214m\e[48;5;214mp",PWM_PIN);
		com_transmit(txBuffer, len);
		len = sprintf(txBuffer, "\e[37;3H\e[0mDuty\e[37;11H%%\e[38;5;214m\e[48;5;214m\e[37;8Hl\e[37;12Hh");
		com_transmit(txBuffer, len);
		len = sprintf(txBuffer, "\e[40;1H\e[0mSampling\e[43;1HMax impedance");
		com_transmit(txBuffer, len);

		len = sprintf(txBuffer, "$$Snoclickclr:40,41.1,48.5.34;");
		com_transmit(txBuffer, len);

	} else if (currentpage == numberinput_pwm_freq) {
		len = sprintf(txBuffer, "$$T\e[2J_\e[1m\e[3;2H\e[48;5;214m123\e[4;2H456\e[5;2H789\e[6;2H.\e[6;3H0\e[1;12H<\e[1;14HX");
		com_transmit(txBuffer, len);
		len = sprintf(txBuffer, "\e[3;7H\e[1m \e[4;7Hk\e[5;7HM\e[3;8H\e[0mHz\e[4;8HHz\e[5;8HHz");
		com_transmit(txBuffer, len);
		len = sprintf(txBuffer, "\e[0m\e[1m\e[5;12H\e[48;5;214mu\e[4;12Hm\e[38;5;214m\e[3;12Hs\e[0ms\e[4;13Hs\e[5;13Hs");
		com_transmit(txBuffer, len);
	}

	terminal_updateValues();
}

void terminal_updateValues() {
	uint16_t len;

	if (currentpage == mainpage) {
		if (terminalSettings.TrigCh == 1) {
			len = sprintf(txBuffer, "$$T\e[2;1H\e[48;5;214m\e[38;5;214mA\e[3;1H\e[38;5;254m\e[48;5;254mB\e[0m");
			com_transmit(txBuffer, len);
			len = sprintf(txBuffer, "$$Strigch:1;");
			com_transmit(txBuffer, len);
		}
		if (terminalSettings.TrigCh == 2) {
			len = sprintf(txBuffer, "$$T\e[3;1H\e[48;5;214m\e[38;5;214mB\e[2;1H\e[38;5;254m\e[48;5;254mA\e[0m");
			com_transmit(txBuffer, len);
			len = sprintf(txBuffer, "$$Strigch:2;");
			com_transmit(txBuffer, len);
		}

		len = sprintf(txBuffer, "$$T\e[2;6H\e[0m%lu.%lu\e[0m", terminalSettings.Trigger_mV / 1000, (terminalSettings.Trigger_mV % 1000) / 100);
		com_transmit(txBuffer, len);

		if (terminal_triggerlineupdateneeded) {
			len = sprintf(txBuffer, "$$Strigpos:%lu.%lu;", terminalSettings.Trigger_mV / 1000, (terminalSettings.Trigger_mV % 1000) / 100);
			com_transmit(txBuffer, len);
			terminal_triggerlineupdateneeded = 0;
		}

		if (terminalSettings.TriggerEdge == triggerOnRising) {
			len = sprintf(txBuffer, "$$T\e[3;9H\e[48;5;214m\e[37;1m/\e[0m");
			com_transmit(txBuffer, len);
		}
		if (terminalSettings.TriggerEdge == triggerOnFalling) {
			len = sprintf(txBuffer, "$$T\e[3;9H\e[48;5;214m\e[37;1m\\\e[0m");
			com_transmit(txBuffer, len);
		}

		if (terminalSettings.PreTrigger_percent == 0) {
			len = sprintf(txBuffer, "$$T\e[6;3H\e[0mOFF");
			com_transmit(txBuffer, len);
		} else if (terminalSettings.PreTrigger_percent == 100) {
			len = sprintf(txBuffer, "$$T\e[6;3H\e[0mALL");
			com_transmit(txBuffer, len);
		} else {
			len = sprintf(txBuffer, "$$T\e[6;3H\e[0m%lu%%", terminalSettings.PreTrigger_percent);
			com_transmit(txBuffer, len);
		}

		len = sprintf(txBuffer, "$$T\e[9;1H\e[38;5;254m\e[48;5;254m0\e[10;1H1\e[11;1H2\e[12;1H3");
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
		com_transmit(txBuffer, len);

		/*if (terminalSettings.status == singleTrigger) {
		 len = sprintf(txBuffer, "$$T\e[0m\e[1;9H\e[41;1m WAIT ");
		 com_transmit(txBuffer, len);
		 }

		 if (terminalSettings.status == paused) {
		 len = sprintf(txBuffer, "$$T\e[0m\e[1;9H\e[41;1mPAUSED");
		 com_transmit(txBuffer, len);
		 }*/

		if (terminalSettings.PWM_enabled) {
			len = sprintf(txBuffer, "$$T\e[0m\e[48;5;214m\e[38;5;214m\e[35;1HG");
			com_transmit(txBuffer, len);
		} else {
			len = sprintf(txBuffer, "$$T\e[48;5;254m\e[38;5;254m\e[35;1HG");
			com_transmit(txBuffer, len);
		}

		double freq = pwm_getFreq();
		floatToNiceString(freq);
		len = sprintf(txBuffer, "\e[0m\e[36;3H%sHz", floatToNiceStringBuffer);
		com_transmit(txBuffer, len);

		len = sprintf(txBuffer, "\e[0m\e[37;9H%2d", terminalSettings.PWM_duty);
		com_transmit(txBuffer, len);
	}

	else if (currentpage == numberinput_pwm_freq) {
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
	if (currentpage == mainpage) {
		if (key == '+') {
			if (terminalSettings.Trigger_mV < 3000) {
				terminalSettings.Trigger_mV += 500;
			}
			terminal_triggerlineupdateneeded = 1;
		}

		else if (key == '-') {
			if (terminalSettings.Trigger_mV > 500) {
				terminalSettings.Trigger_mV -= 500;
			}
			terminal_triggerlineupdateneeded = 1;
		}

		else if (key == '>') {
			if (terminalSettings.PreTrigger_percent < 90) {
				terminalSettings.PreTrigger_percent += 10;
			}
			terminal_triggerlineupdateneeded = 1;
		}

		else if (key == '<') {
			if (terminalSettings.PreTrigger_percent > 0) {
				terminalSettings.PreTrigger_percent -= 10;
			}
			terminal_triggerlineupdateneeded = 1;
		}

		else if (key == '/') {
			terminalSettings.TriggerEdge = triggerOnFalling;
			terminal_triggerlineupdateneeded = 1;
		}

		else if (key == '\\') {
			terminalSettings.TriggerEdge = triggerOnRising;
			terminal_triggerlineupdateneeded = 1;
		}

		else if (key == 'A') {
			terminalSettings.TrigCh = 1;
			terminal_triggerlineupdateneeded = 1;
		}

		else if (key == 'B') {
			terminalSettings.TrigCh = 2;
			terminal_triggerlineupdateneeded = 1;
		}

		else if (key == 'R') {
			//terminalSettings.status = running;
		}

		else if (key == 'P') {
			//status = paused;
		}

		else if (key == 'S') {
			//status = singleTrigger;
		}

		else if (key >= '0' && key < ';') {
			terminalSettings.Fs_index = key - '0';
		}

		else if (key >= 'a' && key < 'f') {
			terminalSettings.Samples_index = key - 'a';
		}

		else if (key >= 'w' && key <= 'x') {
			//chActive[key- 'w'] = !chActive[key- 'w'];
		}

		else if (key == 'G') {
			terminalSettings.PWM_enabled = !terminalSettings.PWM_enabled;
			pwm_startStop(terminalSettings.PWM_enabled);
		}

		else if (key == 'p') {
			terminal_setpage(numberinput_pwm_freq);
		}

		else if (key == 'l') {
			if (terminalSettings.PWM_duty > 5) {
				terminalSettings.PWM_duty -= 5;
				pwm_setDuty(terminalSettings.PWM_duty);
			}
		}

		else if (key == 'h') {
			if (terminalSettings.PWM_duty < 95) {
				terminalSettings.PWM_duty += 5;
				pwm_setDuty(terminalSettings.PWM_duty);
			}
		}
	}

	else if (currentpage == numberinput_pwm_freq) {
		if (key == ' ') {
			double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
			value *= 1;
			if (value > MAX_PWM_FREQ)
				value = MAX_PWM_FREQ;
			pwm_setFreq(value);
			terminal_setpage(mainpage);
		} else if (key == 'k') {
			double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
			value *= 1000;
			if (value > MAX_PWM_FREQ)
				value = MAX_PWM_FREQ;
			pwm_setFreq(value);
			terminal_setpage(mainpage);
		} else if (key == 'M') {
			double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
			value *= 1000000;
			if (value > MAX_PWM_FREQ)
				value = MAX_PWM_FREQ;
			pwm_setFreq(value);
			terminal_setpage(mainpage);
		} else if (key == 's') {
			double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
			value = 1/value;
			if (value > MAX_PWM_FREQ)
				value = MAX_PWM_FREQ;
			pwm_setFreq(value);
			terminal_setpage(mainpage);
		} else if (key == 'm') {
			double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
			value = 1000/value;
			if (value > MAX_PWM_FREQ)
				value = MAX_PWM_FREQ;
			pwm_setFreq(value);
			terminal_setpage(mainpage);
		} else if (key == 'u') {
			double value = terminal_numericinput + terminal_numericinput_decimal / pow(10, numberOfDigits(terminal_numericinput_decimal));
			value = 1000000/value;
			if (value > MAX_PWM_FREQ)
				value = MAX_PWM_FREQ;
			pwm_setFreq(value);
			terminal_setpage(mainpage);
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

			if (terminal_numericinput > MAX_PWM_FREQ)
				terminal_numericinput = MAX_PWM_FREQ;
		}

		else if (key == 'X') {
			terminal_setpage(mainpage);
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
}

void floatToNiceString(double value) {
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
	if (sprintf(floatToNiceStringBuffer, "%.*f %c", 6 - wholePartLength, value, unit) >= 10)
		Error_Handler();

}
