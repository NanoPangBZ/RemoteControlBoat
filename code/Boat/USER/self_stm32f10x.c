#include "self_stm32f10x.h"

/**************************************************************************
 * 功能:翻转IO
 * 参数:Pin->目标IO
 * 返回值:无
 * 备注:需要先初始化目标IO为Output模式
 * 2021/7/16   庞碧璋
 ***********************************************************************/
void Pin_Reversal(Pin pin)
{
    if(pin.GPIO->ODR & pin.Pin )
        pin.GPIO->ODR &= ~pin.Pin;
    else
        pin.GPIO->ODR |= pin.Pin;
}

/**************************************************************************
 * 功能:软件延时 ms级别
 * 参数:要延时的时长(ms)
 * 返回值:无
 * 备注:适用于系统频率72MHz
 * 2021/7/16   庞碧璋
 ***********************************************************************/
void soft_delay_ms(uint16_t ms)
{
    while(ms!=0)
    {
        soft_delay_us(1000);
        ms--;
    }
}

/**************************************************************************
 * 功能:软件延时 us级别
 * 参数:要延时的时长(us)
 * 返回值:无
 * 备注:适用于系统频率72MHz
 * 2021/7/16   庞碧璋
 ***********************************************************************/
void soft_delay_us(uint16_t us)
{
  	volatile uint16_t i = 0;
	while(us--)
	{
	    i = 2;
	    while(i--);
	}
}

/**************************************************************************
 * 功能:内存拷贝
 * 参数:
 * content 要拷贝的数据
 * buf 缓存
 * len 长度
 * 返回值:无
 * 2021/7/16   庞碧璋
 ***********************************************************************/
void MemCopy(const uint8_t*content,uint8_t*buf,uint8_t len)
{
    if(content != buf)  //防止重复
    for(uint8_t temp=0;temp<len;temp++)
        buf[temp] = content[temp];
}

/****************************************************************************/
#if 0

/**************************************************************************
 * 功能:使用cortex m3 的 SysTick定时器开始计时
 * 参数:无
 * 返回值:无
 * 备注:
 * 如果使用了freeRTOS而且系统已经运行的话直接返回
 * 最大计时时长233ms
 * 这个函数主要用于测量代码执行的时长
 * 2021/7/16   庞碧璋
 ***********************************************************************/
void startDebugTiming(void)
{
    #ifdef INC_FREERTOS_H
    //freeRTOS运行后禁止硬件Debug计时
    if(xTaskGetSchedulerState()==taskSCHEDULER_NOT_STARTED)
    {
    #endif //#ifdef INC_FREERTOS_H

    SysTick->LOAD  = (uint32_t)(0xfff - 1UL);                         /* set reload register */
    NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); /* set Priority for Systick Interrupt */
    SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk; 
                     
    #ifdef INC_FREERTOS_H
    }
    #endif //#ifdef INC_FREERTOS_H
}

/**************************************************************************
 * 功能:结束SysTick计时,并且返回计时时长(us)
 * 参数:无
 * 返回值:距离上次调用startDebugTiming()的时长(单位:us)
 * 备注:
 *  如果使用了freeRTOS而且系统已经运行的话直接返回
 * 2021/7/16   庞碧璋
 ***********************************************************************/
uint16_t endDebugTiming(void)
{
    #ifdef INC_FREERTOS_H
    //freeRTOS运行后禁止硬件Debug计时
    if(xTaskGetSchedulerState()==taskSCHEDULER_NOT_STARTED)
    {
    #endif //#ifdef INC_FREERTOS_H

    uint32_t temp;

    //关闭SysTick定时器
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    temp = SysTick->VAL;
    temp = temp/(SystemCoreClock/1000);  //us
    return (uint16_t)temp;

    #ifdef INC_FREERTOS_H
    }
    return 0;
    #endif //#ifdef INC_FREERTOS_H
}

#ifdef INC_FREERTOS_H
void SysTick_Handler(void)
{
    //判断是否装载了freeRTOS系统
    #ifdef INC_FREERTOS_H
    if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED) //如果freeRTOS已经运行
    {
        xPortSysTickHandler();
    }
    #endif  //_freeRTOS_
}
#endif

#endif
