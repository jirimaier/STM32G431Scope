/*
 * terminalpages.h
 *
 *  Created on: Sep 18, 2021
 *      Author: Maier
 */

#ifndef INC_TERMINALPAGES_H_
#define INC_TERMINALPAGES_H_

const char terminal_pageframe_osc[] = { "$$Snoclickclr:44,40,43.1;$$T\e[2J\e[48;5;4m\e[1mOSCILLOSCOPE \e[48;5;214m>\r\n\e[2;1H\e[0m  Run   Pause\e[3;1H\e[0m\e[43;1m\e[38;5;4m\e[1m   Trigger    \e[0m\e[4;9HEdge:\r\n\e[6;1H\e[0m\e[43;1m\e[38;5;4m\e[1m   Samples    \r\n\e[8;1H\e[0m\e[43;1m\e[38;5;4m\e[1m  Frequency   \e[10;1H\e[0m\e[43;1m\e[38;5;4m\e[1m  Channels    \e[0m\e[48;5;214m\e[38;5;214m\e[9;14Hf\e[4;7Ht\e[5;4Hs\e[7;14Hb\e[5;10HT\e[11;1Hz\e[16;1HS\e[0mSingleTrigger\e[15;1H\e[0m\e[43;1m\e[38;5;4m\e[1mMore functions\e[0m\e[11;2HSwitch 2/4 ch\e[0m\e[7;1HPre:   \e[38;5;214m\e[48;5;214mp" };

const char terminal_pageframe_gen[] = { "$$T\e[2J\e[48;5;4m\e[1mGENERATOR    \e[48;5;214m>\e[3;1H\e[43;1m\e[34m\e[1m  PWM (pin 8) \e[4;14H\e[0m\e[48;5;214m\e[38;5;214mP\e[5;14HD" };

const char terminal_keypad_all[] = { "$$T\e[0m\e[20;1H\e[1m\e[48;5;15m\e[30m            \e[37;1m\e[48;5;214m<X\r\n\e[21;2H123\e[22;2H456\e[23;2H789\e[24;4H\e[1K\e[24;2H.0\xc2" "\xb1" "" };

const char terminal_keypad_int[] = { "$$T\e[0m\e[20;1H\e[1m\e[48;5;15m\e[30m            \e[37;1m\e[48;5;214m<X\r\n\e[21;2H123\e[22;2H456\e[23;2H789\e[24;4H\e[1K\e[24;2H\e[24;3H0\xc2" "\xb1" "" };

const char terminal_keypad_uint[] = { "$$T\e[0m\e[20;1H\e[1m\e[48;5;15m\e[30m            \e[37;1m\e[48;5;214m<X\r\n\e[21;2H123\e[22;2H456\e[23;2H789\e[24;4H\e[1K\e[24;2H\e[24;3H0" };

const char terminal_keypad_positive[] = { "$$T\e[0m\e[20;1H\e[1m\e[48;5;15m\e[30m            \e[37;1m\e[48;5;214m<X\r\n\e[21;2H123\e[22;2H456\e[23;2H789\e[24;4H\e[1K\e[24;2H.0" };

const char terminal_keypad_freqsettings[] = { "$$T\e[0m\e[24;12H\e[48;5;214m\e[1mu\e[23;12Hm\e[22;12H\e[38;5;214mn\e[0ms\e[23;13Hs\e[24;13Hs\e[22;7H\e[48;5;214m\e[1m \e[23;7Hk\e[24;7HM\e[0mHz\e[23;8HHz\e[22;8HHz" };

const char terminal_keypad_buffersettings[] = { "$$T\e[21;10H\e[48;5;214m\e[38;5;214mA\e[22;10HB\e[23;10HC\e[24;10HD\e[22;6H \e[24;6H\e[37;1m\e[1mM\e[24;11H\e[0m4096\e[23;11H2048\e[22;11H1024\e[21;11H 512\e[22;7HOK\e[24;7Hax" };

const char terminal_keypad_pretrig[] = { "$$T\e[21;10H\e[48;5;214m\e[38;5;214mA\e[22;10HB\e[23;10HC\e[22;6H \e[37;1m\e[1m\e[0m\e[23;11H100%\e[22;11H 50%\e[21;11H  0%\e[22;7HOK" };

#endif /* INC_TERMINALPAGES_H_ */
