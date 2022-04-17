#include "bsp_beep.h"

uint16_t Mu_Fre[8] = {
    520,585,650,693,780,867,975,1040
};

static void GPIO_Config(void);
static void TIM_Config(void);
static void PWM_Ch_Config(void);

void BSP_Beep_Init(void)
{
    GPIO_Config();
    TIM_Config();
    PWM_Ch_Config();
    //TIM_Cmd(BEEP_TIM,ENABLE);
}

/*******************************************************************
 * 功能:启动蜂鸣器
 * 参数:鸣叫频率
 * 返回值:无
 * 2022/3  庞碧璋
 *******************************************************************/
void Beep_ON(uint16_t fre)
{
    uint16_t arr;
    arr = 1000000/fre;        //计算计数值 1us
    BEEP_TIM->ARR = arr;
    BEEP_TIM->CCR4 = arr/2;
    BEEP_TIM->CR1 |= TIM_CR1_CEN;    //开启定时器
}

void PWM_Ch_Config(void)
{
    TIM_OCInitTypeDef   TIM_OCInitStruct;

    TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCIdleState_Reset;      //互补通道空闲电平
    TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_Low;         //互补通道有效电平
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;          //有效电平
    TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Disable;   //不开启互补通道
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse = 1 - 1;

    TIM_OC4Init(BEEP_TIM,&TIM_OCInitStruct);
    TIM_OC4PreloadConfig(BEEP_TIM,TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(BEEP_TIM,ENABLE);
}

void TIM_Config(void)
{
    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

    TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_InitStruct.TIM_Period = 1000 - 1;
    TIM_InitStruct.TIM_Prescaler = 72 - 1;
    TIM_InitStruct.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(BEEP_TIM,&TIM_InitStruct);
}

void GPIO_Config(void)
{
    GPIO_InitTypeDef    GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;

    GPIO_Init(GPIOA,&GPIO_InitStruct);
}
