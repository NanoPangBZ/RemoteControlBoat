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
void nRF24L01_Recieve_Debug(void);
void nRF24L01_Send_Debug(void);

int main(void)
{
	//中断优先级分组 4位主优先级 0位子优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	//关闭JTAG调试接口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

	BSP_Usart_Init();
	if(nRF24L01_Init())
		printf("nRF24L01 初始化错误\r\n");
	else
		printf("nRF24L01 初始化正常\r\n");
	
	nRF24L01_Recieve_Debug();
	
	while(1);
}

void nRF24L01_Recieve_Debug(void)
{
	nRF24L01_Cfg	Cfg;

	Cfg.Channel = 0;
	Cfg.retry = 5;
	Cfg.retry_cycle = 1;
	Cfg.Rx_Length = 32;
	MemCopy("BOAT",Cfg.TX_Addr,5);	//发射的地址
	MemCopy("USER",Cfg.RX_Addr,5);	//监听的地址

	nRF24L01_Config(&Cfg);

}

void nRF24L01_Send_Debug(void)
{
	nRF24L01_Cfg	Cfg;

	uint8_t SendSbuffer[32];

	for(uint8_t temp=0;temp<32;temp++)
		SendSbuffer[temp] = temp;

	Cfg.Channel = 0;
	Cfg.retry = 5;
	Cfg.retry_cycle = 1;
	Cfg.Rx_Length = 32;
	MemCopy("BOAT",Cfg.RX_Addr,5);	//监听的地址
	MemCopy("USER",Cfg.TX_Addr,5);	//发射的地址

	nRF24L01_Config(&Cfg);

	while(1)
	{
		soft_delay_ms(1000);
		nRF24L01_Send(SendSbuffer,32);
	}
}

void nRF24L01_Recieve_Handle(void)
{
	printf("进入接收中断服务函数\r\n");
	//printf("接收到的数据个数:%d\r\n",);
}

void nRF24L01_Send_Handle(void)
{}
void nRF24L01_NoACK_Handle(void)
{}

/***************************测试代码****************************************/

void SysTick_Handler(void)
{ 
	SysTick_Count++;
}
