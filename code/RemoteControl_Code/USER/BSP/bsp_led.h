#ifndef _BSP_LED_H_
#define _BSP_LED_H_

#include "self_stm32f10x.h"

#define LED_CLK_Init()

static const Pin LED_Pin[3] = {
    {GPIO_Pin_0,GPIOA},
    {GPIO_Pin_0,GPIOA},
    {GPIO_Pin_0,GPIOA}
};

void BSP_LED_Init(void);

void LED_ON(uint8_t num);
void LED_OFF(uint8_t num);
void LED_Reversal(uint8_t num);

#endif

