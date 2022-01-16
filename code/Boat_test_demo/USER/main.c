#include "self_stm32f10x.h"

#include ".\BSP\bsp_usart.h"
#include ".\BSP\bsp_spi.h"

#include ".\HARDWARE\nrf24l01.h"

static uint32_t SysCount = 0;
static nRF24L01_Cfg Cfg;

int main(void)
{
	//关闭JATG调试接口,开启SWD调试接口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

	SysTick_Config(5*72000);	//5ms系统滴答定时器中断

	BSP_Usart_Init();	//串口初始化

	nRF24L01_Init();

	Cfg.Channel = 20;
	Cfg.retry = 5;
	Cfg.retry_cycle = 1;
	Cfg.Rx_Length = 32;
	MemCopy("USER",Cfg.RX_Addr,5);
	MemCopy("BOAT",Cfg.TX_Addr,5);

	nRF24L01_Config(&Cfg);

	uint8_t Test[36] = {1,2,3,4,5};

	for(uint8_t temp=0;temp<36;temp++)
		Test[temp] = temp;

	nRF24L01_Send(Test,36);

	while(1)
	{
		soft_delay_ms(500);
		printf("nRF24L01 STATUS REG = 0x%02X\r\n",nRF24L01_Status());
	}
}

void SysTick_Handler(void)
{
	SysCount++;
}
