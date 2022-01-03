#ifndef _BSP_ADC_H_
#define _BSP_ADC_H_

#include "self_stm32f10x.h"

#define ADC_GPIO_CLK()  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); \
                        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE)
#define ADC_CLK()       RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE)

//硬件对应引脚编号
#define ROCK_RX 0
#define ROCK_RY 1
#define ROCK_LX 2
#define ROCK_LY 3
#define KNOB_L  4
#define KNOB_R  5
#define VBAT_C  6

const static Pin ADC_Pin[7] = {
    {GPIO_Pin_3,GPIOA},{GPIO_Pin_4,GPIOA},
    {GPIO_Pin_1,GPIOA},{GPIO_Pin_2,GPIOA},
    {GPIO_Pin_0,GPIOA},{GPIO_Pin_0,GPIOA},
    {GPIO_Pin_0,GPIOA}
};

const static uint8_t ADC_Channel[7] = 
{
    ADC_Channel_3,ADC_Channel_4,
    ADC_Channel_1,ADC_Channel_2,
    ADC_Channel_0,ADC_Channel_0,
    ADC_Channel_0
};

void BSP_ADC_Init(void);
float ADC_ReadVoltage(uint8_t channel_Num);

#endif

