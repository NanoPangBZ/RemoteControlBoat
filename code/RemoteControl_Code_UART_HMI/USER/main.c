#include "stm32f10x.h"
#include "self_stm32f10x.h"
#include <math.h>
#include "vofa_p.h"

#include ".\BSP\bsp_usart.h"
#include ".\BSP\bsp_spi.h"
#include ".\BSP\bsp_adc.h"

#include ".\HARDWARE\nrf24l01.h"
#include ".\HARDWARE\usart_hmi.h"

#include "nrf24_debug.h"

#pragma	diag_suppress	870	//屏蔽汉字警告

uint32_t SysTick_Count = 0;

int main(void)
{
	//中断优先级分组 4位主优先级 0位子优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	//关闭JTAG调试接口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

	BSP_Usart_Init();

	nRF24L01_UserBoard_Test();
	
	while(1);
}


/***************************测试代码****************************************/

void SysTick_Handler(void)
{ 
	SysTick_Count++;
}
