/*
 * communication.c
 *
 *  Created on: Sep 11, 2021
 *      Author: Maier
 */

#include "communication.h"
#include "config.h"
#include "terminal.h"
#include "tim.h"
#include <stdio.h>

//char txBuffer[TX_BUFFER_SIZE];

void com_init() {
	connectionStatus = disconnected;
	HAL_TIM_Base_Start_IT(&htim6);
	compendingconnectiontest = 0;
	com_lastACKreceived = 1;
	lastacknumber = 1;
}

void com_transmit(const char *buf, uint16_t len) {
	CDC_Transmit_FS((uint8_t*) buf, len);
}

void com_receive(uint8_t *buf, uint32_t *len) {
	for (uint32_t i = 0; i < *len; i++) {
		if (buf[i] == 0) {
			if (compendingconnectiontest > 0)
				compendingconnectiontest--;
			if (connectionStatus == disconnected) {
				connectionStatus = newlyconnected;
				compendingconnectiontest = 0;
				com_lastACKreceived = 1;
			}
		} else if (buf[i] == lastacknumber) {
			com_lastACKreceived = 1;
		} else {
			terminal_command(buf[i]);
		}
	}
}

void com_requestack() {
	com_lastACKreceived = 0;
	lastacknumber++;
	if (lastacknumber >= 10)
		lastacknumber = 1;
	uint32_t len = sprintf(txBuffer, "$$E%c", lastacknumber);
	com_transmit(txBuffer, len);
}

void com_test() {
	uint32_t len = sprintf(txBuffer, "$$E%c", 0);
	com_transmit(txBuffer, len);
	compendingconnectiontest++;

	if ((compendingconnectiontest > 5) && connectionStatus == connected) {
		uint32_t len = sprintf(txBuffer, "$$WMissed %d ACKs, suspecting client disconnected!", compendingconnectiontest);
		CDC_Transmit_FS((uint8_t*) txBuffer, len);
		connectionStatus = disconnected;
	}
}

void com_print(const char *str) {
	uint32_t len = sprintf(txBuffer, "%s", str);
	com_transmit(txBuffer, len);
}

uint8_t com_status() {
	if (connectionStatus == newlyconnected) {
		connectionStatus = connected;
		return newlyconnected;
	}
	return connectionStatus;
}
