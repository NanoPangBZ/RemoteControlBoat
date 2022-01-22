#ifndef _NRF24_DEBUG_H_
#define _NRF24_DEBUG_H_

#include ".\BSP\bsp_usart.h"
#include ".\HARDWARE\nrf24l01.h"

static uint8_t sbuffer[32];

const static uint8_t User_BoardAddr[5] = {0x43,0x16,0x66,0x66,0x00};
const static uint8_t Boat_BoardAddr[5] = {0x43,0x16,0x88,0x88,0x00};

void nRF24L01_UserBoard_Test(void);
void nRF24L01_BoatBoard_Test(void);

void nRF24L01_Rx_Handle(void);
void nRF24L01_NoACK_Handle(void);
void nRF24L01_Tx_Handle(void);

#endif
