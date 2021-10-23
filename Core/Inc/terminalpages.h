/*
 * terminalpages.h
 *
 *  Created on: Sep 18, 2021
 *      Author: Maier
 */

#ifndef INC_TERMINALPAGES_H_
#define INC_TERMINALPAGES_H_

const char terminal_pageframe_osc[] = {
		"$$T\e[2J\e[48;5;4m\e[1mOSCILLOSCOPE \e[48;5;214m>\r\n\e[2;1H\e[0m  Run   Pause\e[3;1H\e[0m\e[43;1m\e[38;5;4m\e[1m   Trigger    \e[0m\e[5;3HAuto  Single\r\n\e[6;1H\e[0m\e[43;1m\e[38;5;4m\e[1m   Samples    \e[7;11H\e[0mRoll\r\n\e[8;1H\e[0m\e[43;1m\e[38;5;4m\e[1m  Frequency   \r\n\e[10;1H\e[0m\e[43;1m\e[38;5;4m\e[1m  Channels    \e[0m\e[48;5;214m\e[38;5;214m\e[9;14Hf\e[4;7Ht\e[4;14Hs"
};

const char terminal_pageframe_gen[] = {
		"$$T\e[2J\e[48;5;4m\e[1mGENERATOR    \e[48;5;214m>\r\n"
};

const char terminal_keypad_all[] = {
		"$$T\e[0m\e[18;1H\e[1m\e[47;1m\e[30m            \e[37;1m\e[48;5;214m<X\r\n\e[19;2H123\e[20;2H456\e[21;2H789\e[22;4H\e[1K\e[22;2H.0\xc2""\xb1"""
};

const char terminal_keypad_int[] = {
		"$$T\e[0m\e[18;1H\e[1m\e[47;1m\e[30m            \e[37;1m\e[48;5;214m<X\r\n\e[19;2H123\e[20;2H456\e[21;2H789\e[22;4H\e[1K\e[22;2H\e[22;3H0\xc2""\xb1"""
};

const char terminal_keypad_uint[] = {
		"$$T\e[0m\e[18;1H\e[1m\e[47;1m\e[30m            \e[37;1m\e[48;5;214m<X\r\n\e[19;2H123\e[20;2H456\e[21;2H789\e[22;4H\e[1K\e[22;2H\e[22;3H0"
};

const char terminal_keypad_positive[] = {
		"$$T\e[0m\e[18;1H\e[1m\e[47;1m\e[30m            \e[37;1m\e[48;5;214m<X\r\n\e[19;2H123\e[20;2H456\e[21;2H789\e[22;4H\e[1K\e[22;2H.0"
};

#endif /* INC_TERMINALPAGES_H_ */
