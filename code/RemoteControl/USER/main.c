#include "stm32f10x.h"
#include "self_stm32f10x.h"

#include ".\BSP\bsp_usart.h"
#include ".\BSP\bsp_spi.h"

#include ".\HARDWARE\nrf24l01.h"
#include ".\HARDWARE\usart_hmi.h"

#include "FreeRTOS.h"
#include "task.h"

#pragma	diag_suppress	870	//屏蔽汉字警告

int main(void)
{
	//中断优先级分组 4位主优先级 0位子优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	//关闭JTAG调试接口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
	while(1);
}

/******************************************************************/

extern void xPortSysTickHandler(void);		//声明port.c中的中断接口函数
void SysTick_Handler(void)
{
  if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)   //判断系统是否已经启动
    {
        xPortSysTickHandler();		//系统心跳函数
    }
}
