#ifndef _BSP_KEY_H_
#define _BSP_KEY_H_

#include "self_stm32f10x.h"

#define KEY_GPIO_CLK_Init()     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); \
                                RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE)

const static Pin Key_Pin[8] = 
{
    {GPIO_Pin_11,GPIOA} , {GPIO_Pin_10,GPIOA},
    {GPIO_Pin_3,GPIOB} , {GPIO_Pin_4,GPIOB},
    {GPIO_Pin_5,GPIOB} , {GPIO_Pin_6,GPIOB},
    {GPIO_Pin_7,GPIOB} , {GPIO_Pin_8,GPIOB}
};

void BSP_Key_Init(void);

#endif

