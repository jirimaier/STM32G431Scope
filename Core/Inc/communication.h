/*
 * communication.h
 *
 *  Created on: Sep 11, 2021
 *      Author: Maier
 */

#ifndef INC_COMMUNICATION_H_
#define INC_COMMUNICATION_H_

#include "config.h"
#include "usb_device.h"

char txBuffer[TX_BUFFER_SIZE];

extern uint8_t CDC_Transmit_FS(uint8_t *Buf, uint16_t Len);

uint8_t compendingconnectiontest;
uint8_t com_lastACKreceived;
uint8_t lastacknumber;

enum ConnectionStatus {
	disconnected, connected, newlyconnected
} connectionStatus;

void com_init();
void com_requestack();
void com_transmit(char *buf, uint16_t len);
void com_receive(uint8_t *buf, uint32_t *len);
uint8_t com_status();
void com_test();
void com_print(const char *str);

#endif /* INC_COMMUNICATION_H_ */
