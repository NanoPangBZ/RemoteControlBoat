#include "stm32f10x.h"
#include "self_stm32f10x.h"
#include <math.h>
#include "vofa_p.h"

#include ".\BSP\bsp_usart.h"
#include ".\BSP\bsp_spi.h"
#include ".\BSP\bsp_adc.h"

#include ".\HARDWARE\nrf24l01.h"

#pragma	diag_suppress	870	//屏蔽汉字警告

static uint32_t SysTick_Count = 0;

int main(void)
{
	uint8_t buf[32];
	//中断优先级分组 4位主优先级 0位子优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	//关闭JTAG调试接口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

	SysTick_Config(5*72000);	//5ms中断

	BSP_Usart_Init();
	BSP_ADC_Init();
	if(nRF24L01_Init())
		printf("nRF24L01 Err!!!\r\n");
	else
		printf("nRF24L01 Pass\r\n");

	nRF24L01_Cfg	Cfg;
	Cfg.Channel = 100;
	Cfg.retry = 10;
	Cfg.retry_cycle = 10;
	MemCopy("RxAdd",Cfg.RX_Addr,5);
	MemCopy("TxAdd",Cfg.TX_Addr,5);
	nRF24L01_Config(&Cfg);

	nRF24L01_Send(buf,32);

	while(1)
	{
		soft_delay_ms(100);
		printf("Voltage = %.2f\r\n",ADC_ReadVoltage(0));
	}
}

void SysTick_Handler(void)
{ 
	uint8_t*dat;
	SysTick_Count++;
	dat = Usart_Read(1);
	if(*dat!=0)
		printf("nRF24L01 REGIST 0x%02X = 0x%02X\r\n",*(dat+1),nRF24L01_Read_Reg(*(dat+1)));
	USART_Push(1,1);
}
