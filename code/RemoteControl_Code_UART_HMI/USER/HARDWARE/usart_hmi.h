#ifndef _USART_HMI_H_
#define _USART_HMI_H_

#include "stm32f10x.h"
#include "usart_hmi.h"
#include "string.h"
#include "usart_hmi_cmd.h"
#include ".\BSP\bsp_usart.h"

#define port_HMI_Send(dat,len)     Usart_Send(2,dat,len)

void Usart_HMI_Send(char*str);

#endif


