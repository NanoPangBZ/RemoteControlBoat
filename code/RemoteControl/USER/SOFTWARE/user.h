#ifndef _USER_H_
#define _USER_H_

#include "BSP\bsp_adc.h"
#include "BSP\bsp_spi.h"
#include "BSP\bsp_usart.h"

#include "HARDWARE\nrf24l01.h"
#include "HARDWARE\usart_hmi.h"

#include "self_stm32f10x.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

void RemoteControl_Task(void*ptr);
void FreeRTOS_Test_Task(void*ptr);
void nRF24L01_Intterrupt_Task(void*ptr);

#endif //_USER_H_

