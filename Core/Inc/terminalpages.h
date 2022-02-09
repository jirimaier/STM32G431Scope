/*
 * terminalpages.h
 *
 *  Created on: Sep 18, 2021
 *      Author: Maier
 */

#ifndef INC_TERMINALPAGES_H_
#define INC_TERMINALPAGES_H_

const char terminal_pageframe_osc[] = {
		"$$T\e[2J\e[48;5;4m\e[1mOSCILLOSCOPE \e[48;5;214m>\r\n\e[2;1H\e[0m  Run   Pause\e[3;1H\e[0m\e[43;1m\e[38;5;4m\e[1m   Trigger    \e[0m\e[4;9HEdge:\r\n\e[6;1H\e[0m\e[43;1m\e[38;5;4m\e[1m   Samples    \e[7;11H\e[0mRoll\r\n\e[8;1H\e[0m\e[43;1m\e[38;5;4m\e[1m  Frequency   \r\n\e[10;1H\e[0m\e[43;1m\e[38;5;4m\e[1m  Channels    \e[0m\e[48;5;214m\e[38;5;214m\e[9;14Hf\e[4;7Ht\e[5;4Hs\e[7;6Hb\e[7;10Hc\e[5;10HT\e[0m\e[11;2H2\e[12;2H4\e[11;8HCh Pin\e[12;9H1  6\e[13;9H2 11\e[14;9H3  7\e[15;9H4 12\e[2J\e[48;5;4m\e[1mOSCILLOSCOPE \e[48;5;214m>\r\n\e[2;1H\e[0m  Run   Pause\e[3;1H\e[0m\e[43;1m\e[38;5;4m\e[1m   Trigger    \e[0m\e[4;9HEdge:\r\n\e[6;1H\e[0m\e[43;1m\e[38;5;4m\e[1m   Samples    \e[7;11H\e[0mRoll\r\n\e[8;1H\e[0m\e[43;1m\e[38;5;4m\e[1m  Frequency   \r\n\e[10;1H\e[0m\e[43;1m\e[38;5;4m\e[1m  Channels    \e[0m\e[48;5;214m\e[38;5;214m\e[9;14Hf\e[4;7Ht\e[5;4Hs\e[7;6Hb\e[7;10Hc\e[5;10HT\e[11;1Hz\e[16;1HS\e[0mSingleTrigger\e[15;1H\e[0m\e[43;1m\e[38;5;4m\e[1mMore functions\e[0m\e[11;2HSwitch 2/4 ch"
};

const char terminal_pageframe_gen[] = {
		"$$T\e[2J\e[48;5;4m\e[1mGENERATOR    \e[48;5;214m>\e[3;1H\e[43;1m\e[34m\e[1m  PWM (pin 8) \e[4;14H\e[0m\e[48;5;214m\e[38;5;214mP\e[5;14HD"
};

const char terminal_keypad_all[] = {
		"$$T\e[0m\e[18;1H\e[1m\e[48;5;15m\e[30m            \e[37;1m\e[48;5;214m<X\r\n\e[19;2H123\e[20;2H456\e[21;2H789\e[22;4H\e[1K\e[22;2H.0\xc2""\xb1"""
};

const char terminal_keypad_int[] = {
		"$$T\e[0m\e[18;1H\e[1m\e[48;5;15m\e[30m            \e[37;1m\e[48;5;214m<X\r\n\e[19;2H123\e[20;2H456\e[21;2H789\e[22;4H\e[1K\e[22;2H\e[22;3H0\xc2""\xb1"""
};

const char terminal_keypad_uint[] = {
		"$$T\e[0m\e[18;1H\e[1m\e[48;5;15m\e[30m            \e[37;1m\e[48;5;214m<X\r\n\e[19;2H123\e[20;2H456\e[21;2H789\e[22;4H\e[1K\e[22;2H\e[22;3H0"
};

const char terminal_keypad_positive[] = {
		"$$T\e[0m\e[18;1H\e[1m\e[48;5;15m\e[30m            \e[37;1m\e[48;5;214m<X\r\n\e[19;2H123\e[20;2H456\e[21;2H789\e[22;4H\e[1K\e[22;2H.0"
};

const char terminal_keypad_freqsettings[] = {
		"$$T\e[0m\e[22;12H\e[48;5;214m\e[1mu\e[21;12Hm\e[20;12H\e[38;5;214mn\e[0ms\e[21;13Hs\e[22;13Hs\e[20;7H\e[48;5;214m\e[1m \e[21;7Hk\e[22;7HM\e[0mHz\e[21;8HHz\e[20;8HHz"
};

const char terminal_keypad_buffersettings[] = {
		"$$T\e[19;10H\e[48;5;214m\e[38;5;214mA\e[20;10HB\e[21;10HC\e[22;10HD\e[20;6H \e[22;6H\e[37;1m\e[1mM\e[22;11H\e[0m4096\e[21;11H2048\e[20;11H1024\e[19;11H 512\e[20;7HOK\e[22;7Hax"
};

#endif /* INC_TERMINALPAGES_H_ */
