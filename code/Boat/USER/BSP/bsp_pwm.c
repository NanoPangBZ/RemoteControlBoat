#include "bsp_pwm.h"

static TIM_TypeDef* Target_TIM[3] = {TIM1,TIM4,TIM8};

static __IO uint16_t* Target_CCR[12] = {
    &TIM1->CCR1,&TIM1->CCR2,&TIM1->CCR3,&TIM1->CCR4,
    &TIM4->CCR1,&TIM4->CCR2,&TIM4->CCR3,&TIM4->CCR4,
    &TIM8->CCR1,&TIM8->CCR2,&TIM8->CCR3,&TIM8->CCR4,
};

static void PWM_GPIO_Init(void);
static void PWM_TIM_Init(void);
static void PWM_Ch_Init(void);

void BSP_PWM_Init(void)
{
    PWM_GPIO_Init();
    PWM_TIM_Init();
    PWM_Ch_Init();
    //开启1 4 8 定时器
    for(uint8_t temp = 0;temp<3;temp++)
        TIM_Cmd(Target_TIM[temp],ENABLE);
    //开启TIM 1 8高级定时器的PWM输出
    TIM_CtrlPWMOutputs(TIM1,ENABLE);
    TIM_CtrlPWMOutputs(TIM8,ENABLE);
}

void PWM_GPIO_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);

    GPIO_PinRemapConfig(GPIO_Remap_TIM4,ENABLE);
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM1,ENABLE);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    //TIM1
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_Init(GPIOE,&GPIO_InitStruct);
    //TIM4
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD,&GPIO_InitStruct);
    //TIM8
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_Init(GPIOC,&GPIO_InitStruct);
}

void PWM_TIM_Init(void)
{
    TIM_TimeBaseInitTypeDef TIMBase_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);

    //TIM4 TIM8 50Hz -> 电调 舵机
    TIMBase_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIMBase_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIMBase_InitStruct.TIM_Period = 20000 - 1;
    TIMBase_InitStruct.TIM_Prescaler = 72 -1 ;
    TIMBase_InitStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4,&TIMBase_InitStruct);
    TIM_TimeBaseInit(TIM8,&TIMBase_InitStruct);

    //TIM1 10kHz -> 直流减速电机 cycle:100us src:72Mhz arr:7200
    TIMBase_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIMBase_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIMBase_InitStruct.TIM_Period = 7200 - 1;
    TIMBase_InitStruct.TIM_Prescaler = 1 - 1 ;
    TIMBase_InitStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1,&TIMBase_InitStruct);
}

void PWM_Ch_Init(void)
{
    TIM_OCInitTypeDef   TIM_OCInitStruct;

    TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCIdleState_Reset;      //互补通道空闲电平
    TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_Low;         //互补通道有效电平
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;          //有效电平
    TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Disable;   //不开启互补通道
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;

    //TIM1 直流减速电机
    TIM_OCInitStruct.TIM_Pulse = 3600 - 1;
    TIM_OC1Init(TIM1,&TIM_OCInitStruct);
    TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);
    TIM_OC2Init(TIM1,&TIM_OCInitStruct);
    TIM_OC2PreloadConfig(TIM1,TIM_OCPreload_Enable);
    TIM_OC3Init(TIM1,&TIM_OCInitStruct);
    TIM_OC3PreloadConfig(TIM1,TIM_OCPreload_Enable);
    TIM_OC4Init(TIM1,&TIM_OCInitStruct);
    TIM_OC4PreloadConfig(TIM1,TIM_OCPreload_Enable);

    //TIM4 电调
    TIM_OCInitStruct.TIM_Pulse = 1400 - 1;   //中位油门
    TIM_OC1Init(TIM4,&TIM_OCInitStruct);
    TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable);
    TIM_OC2Init(TIM4,&TIM_OCInitStruct);
    TIM_OC2PreloadConfig(TIM4,TIM_OCPreload_Enable);
    TIM_OC3Init(TIM4,&TIM_OCInitStruct);
    TIM_OC3PreloadConfig(TIM4,TIM_OCPreload_Enable);
    TIM_OC4Init(TIM4,&TIM_OCInitStruct);
    TIM_OC4PreloadConfig(TIM4,TIM_OCPreload_Enable);

    //TIM8 舵机
    TIM_OCInitStruct.TIM_Pulse = 1400 - 1;  //中位
    TIM_OC1Init(TIM8,&TIM_OCInitStruct);
    TIM_OC1PreloadConfig(TIM8,TIM_OCPreload_Enable);
    TIM_OC2Init(TIM8,&TIM_OCInitStruct);
    TIM_OC2PreloadConfig(TIM8,TIM_OCPreload_Enable);
    TIM_OC3Init(TIM8,&TIM_OCInitStruct);
    TIM_OC3PreloadConfig(TIM8,TIM_OCPreload_Enable);
    TIM_OC4Init(TIM8,&TIM_OCInitStruct);
    TIM_OC4PreloadConfig(TIM8,TIM_OCPreload_Enable);
}

void PWM_Out(uint8_t Channel,uint16_t CCR)
{
    if(Channel < 12)
    *Target_CCR[Channel] = CCR;
}

uint16_t PWM_Read(uint8_t Channel)
{
    if(Channel < 12)
        return *Target_CCR[Channel];
    return 0xffff;
}
