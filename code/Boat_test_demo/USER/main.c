#include "self_stm32f10x.h"

#include ".\BSP\bsp_usart.h"
#include ".\BSP\bsp_spi.h"

static uint32_t SysCount = 0;

int main(void)
{
	//关闭JATG调试接口,开启SWD调试接口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

	SysTick_Config(5*72000);	//5ms系统滴答定时器中断

	BSP_Usart_Init();	//串口初始化

	while(1)
	{
		if(SysCount %200 == 0)
			printf("HelloWorld!\r\n");
	}
}

void SysTick_Handler(void)
{
	SysCount++;
}
