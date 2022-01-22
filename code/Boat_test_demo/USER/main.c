#include "self_stm32f10x.h"

#include ".\BSP\bsp_usart.h"
#include ".\BSP\bsp_spi.h"

#include ".\HARDWARE\nrf24l01.h"

#include "nrf24_debug.h"

uint32_t SysTick_Count = 0;

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	//关闭JATG调试接口,开启SWD调试接口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

	SysTick_Config(5*72000);	//5ms系统滴答定时器中断

	BSP_Usart_Init();

	nRF24L01_UserBoard_Test();

	while(1);

}

void SysTick_Handler(void)
{
	SysTick_Count++;
}
