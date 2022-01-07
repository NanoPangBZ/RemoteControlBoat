#ifndef _USART_HMI_H_
#define _USART_HMI_H_

#include "self_stm32f10x.h"
#include "usart_hmi.h"
#include <string.h>
#include ".\BSP\bsp_usart.h"
#define YM  0   //油门
#define SC  1   //速差

#define port_HMI_Send(dat,len)     Usart_Send(1,dat,len)

//USART_HMI支持+=操作
static uint8_t HMI_Sbuffer[64];

uint8_t Usart_HMI_Send(uint8_t*str);
uint8_t Usart_HMI_MsgBox(uint8_t*msg);
uint8_t Usart_HMI_GetFlow(uint8_t j);

#endif


