#ifndef _BSP_LED_H_
#define _BSP_LED_H_

#include "self_stm32f10x.h"

const static Pin LED = {GPIO_Pin_15,GPIOA};

#define LED_ON()    Pin_Reset(LED)
#define LED_OFF()   Pin_Set(LED)
#define LED_Re()    Pin_Reversal(LED)

void BSP_LED_Init(void);

#endif

