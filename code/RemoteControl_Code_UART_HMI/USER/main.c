#include "stm32f10x.h"
#include "self_stm32f10x.h"
#include <math.h>
#include "vofa_p.h"

#include ".\BSP\bsp_usart.h"
#include ".\BSP\bsp_spi.h"
#include ".\BSP\bsp_adc.h"

#include ".\HARDWARE\nrf24l01.h"
#include ".\HARDWARE\usart_hmi.h"

#pragma	diag_suppress	870	//屏蔽汉字警告

uint32_t SysTick_Count = 0;
uint8_t debug_sbuffer[64];

int main(void)
{
	//中断优先级分组 4位主优先级 0位子优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	//关闭JTAG调试接口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

	//SysTick_Config(5*72000);	//5ms中断

	BSP_Usart_Init();
	nRF24L01_Init();

	nRF24L01_Cfg	Cfg;
	uint8_t test[32];

	Cfg.Channel = 0;
	Cfg.retry = 10;
	Cfg.retry_cycle = 1;
	MemCopy("USER",Cfg.RX_Addr,5);
	MemCopy("Test",Cfg.TX_Addr,5);
	Cfg.Rx_Length = 32;

	nRF24L01_Config(&Cfg);

	for(uint8_t temp=0;temp<32;temp++)
		test[temp] = temp;

	while(1)
	{
		soft_delay_ms(500);
		printf("发送32字节\r\n");
		nRF24L01_Send(test,32);
	}
}

/***************************测试代码****************************************/

void SysTick_Handler(void)
{ 
	SysTick_Count++;
}
