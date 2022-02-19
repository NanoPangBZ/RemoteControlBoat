#include "self_stm32f10x.h"

#include "BSP\bsp_usart.h"
#include "BSP\bsp_spi.h"
#include "BSP\bsp_pwm.h"

#include ".\HARDWARE\nrf24l01.h"

uint32_t SysTick_Count = 0;
uint16_t PWM_Width = 0;
uint8_t dir = 0;

#if 1
uint8_t RxFlag = 0;
uint8_t sbuffer[32];
//nRF24L01初始化结构体
static nRF24L01_Cfg nRF24_Cfg;
static uint8_t RxAddr[5] = {0x43,0x16,'R','C',0xFF};	//遥控器地址
static uint8_t TxAddr[5] = {0x43,0x16,'B','T',0xFF};	//船地址
#endif

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	//关闭JATG调试接口,开启SWD调试接口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

	SysTick_Config(5*72000);	//5ms系统滴答定时器中断

	BSP_Usart_Init();
	while(nRF24L01_Init())
	{
		soft_delay_ms(1000);
		printf("nRF24L01 Err\r\n");
	}
	printf("nRF24L01 Pass\r\n");

	//nRF24L01 相关配置
	nRF24_Cfg.Channel = 50;	//2.45GHz 通讯频段
	nRF24_Cfg.retry = 5;	//最大重发次数
	nRF24_Cfg.retry_cycle = 1;	//重发周期
	nRF24_Cfg.Rx_Length = 32;	//结束长度
	MemCopy(TxAddr,nRF24_Cfg.TX_Addr,5);
	MemCopy(RxAddr,nRF24_Cfg.RX_Addr,5);
	nRF24L01_Config(&nRF24_Cfg);	//配置nRF24L01
	nRF24L01_Rx_Mode();

	while(1)
	{
		if(RxFlag)
		{
			nRF24L01_Read_RxSbuffer(sbuffer,32);
			for(uint8_t temp=0;temp<32;temp++)
			{
				printf("%02X ",sbuffer[temp]);
			}
			RxFlag = 0;
		}
		soft_delay_ms(500);
		printf("\r\nRunFlag\r\n");
	}

	#if 0
	BSP_PWM_Init();

	while(1)
	{
		if(dir)
		{
			if(PWM_Width < 10)
				dir = 0;
			PWM_Width--;
		}else
		{
			if(PWM_Width > 18000)
				dir = 1;
			PWM_Width++;
		}
		PWM_Out(0,PWM_Width);
		soft_delay_us(20);
	}
	#endif
}

void nRF24L01_NoACK_ISR(void)
{
	printf("NoACK_ISR Run\r\n");
}

void nRF24L01_Tx_ISR(void)
{
	printf("Tx_ISR Run\r\n");
}

void nRF24L01_Rx_ISR(void)
{
	printf("Rx_ISR Run\r\n");
	//RxFlag = 1;
}


void SysTick_Handler(void)
{
	SysTick_Count++;
}

/***********************
BSP_Usart_Init();
	while(nRF24L01_Init())
	{
		soft_delay_ms(1000);
		printf("nRF24L01 Err\r\n");
	}
	printf("nRF24L01 Pass\r\n");

	//nRF24L01 相关配置
	nRF24_Cfg.Channel = 50;	//2.45GHz 通讯频段
	nRF24_Cfg.retry = 5;	//最大重发次数
	nRF24_Cfg.retry_cycle = 1;	//重发周期
	nRF24_Cfg.Rx_Length = 32;	//结束长度
	MemCopy(TxAddr,nRF24_Cfg.TX_Addr,5);
	MemCopy(RxAddr,nRF24_Cfg.RX_Addr,5);
	nRF24L01_Config(&nRF24_Cfg);	//配置nRF24L01
	nRF24L01_Rx_Mode();

	while(1)
	{
		if(RxFlag)
		{
			nRF24L01_Read_RxSbuffer(sbuffer,32);
			for(uint8_t temp=0;temp<32;temp++)
			{
				printf("%02X ",sbuffer[temp]);
			}
			RxFlag = 0;
		}
		soft_delay_ms(500);
		printf("\r\nRunFlag\r\n");
	}
***************************/
