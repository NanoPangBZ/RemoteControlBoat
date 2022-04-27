#include "bsp_timer.h"

static uint16_t overflow_count;     //定时器溢出次数

/*******************************************************************
 * 功能:初始化计时定时器
 * 参数:无
 * 返回值:无
 * 备注:不会启动定时器
 * 2022/3/7   庞碧璋
 *******************************************************************/
void BSP_Timer_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    NVIC_InitTypeDef    NVIC_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

    TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;    //不分频
    TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
    TIM_InitStruct.TIM_Period = 0xfffe;
    TIM_InitStruct.TIM_Prescaler = 72 - 1;
    TIM_InitStruct.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(timingTimer,&TIM_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 15;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

    NVIC_Init(&NVIC_InitStruct);
}

/*******************************************************************
 * 功能:开始计时
 * 参数:无
 * 返回值:无
 * 备注:记得调用stopTiming()
 * 2022/3/7   庞碧璋
 *******************************************************************/
void startTiming(void)
{
    timingTimer->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));   //关闭定时器
    overflow_count = 0;
    timingTimer->CNT = 0;
    timingTimer->CR1 |= TIM_CR1_CEN;        //开启定时器
}

/*******************************************************************
 * 功能:结束计时
 * 参数:无
 * 返回值:距离上次调用startTiming()的时间 单位(us)
 * 2022/3/7   庞碧璋
 *******************************************************************/
uint32_t stopTiming(void)
{
    timingTimer->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));   //关闭定时器
    if(overflow_count)
    {
        return (uint32_t)overflow_count*65535 + timingTimer->CNT;
    }else
    {
        return (uint32_t)timingTimer->CNT;
    }
}



void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(timingTimer,TIM_IT_Update) == SET)
    {
        overflow_count++;
        TIM_ClearITPendingBit(timingTimer,TIM_IT_Update);
    }
}

