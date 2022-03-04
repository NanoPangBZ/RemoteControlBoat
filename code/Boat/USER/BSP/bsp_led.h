#ifndef _BSP_LED_H_
#define _BSP_LED_H_

#include "self_stm32f10x.h"

static Pin LED_Pin[1] = {
    {GPIO_Pin_1,GPIOA}
};

#define LED_ON          0
#define LED_OFF         1
#define LED_Reserval    2

void BSP_LED_Init(void);
void LED_CRT(uint8_t num,uint8_t status);

#endif

