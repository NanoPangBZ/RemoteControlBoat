#include "self_stm32f10x.h"

#include "BSP\bsp_usart.h"
#include "BSP\bsp_spi.h"
#include "BSP\bsp_pwm.h"
#include "BSP\bsp_led.h"

#include "HARDWARE\nrf24l01.h"
#include "HARDWARE\oled12864.h"
#include "SOFTWARE\user.h"

#include "FreeRTOS.h"
#include "task.h"

//nRF24L01初始化结构体
static nRF24L01_Cfg nRF24_Cfg;
static uint8_t RxAddr[5] = {0x43,0x16,'R','C',0xFF};	//遥控器地址
static uint8_t TxAddr[5] = {0x43,0x16,'B','T',0xFF};	//船地址

//任务句柄
TaskHandle_t	Test_TaskHandle = NULL;

int main(void)
{
	uint8_t nrf_err;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	//关闭JATG调试接口,开启SWD调试接口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

	//板级支持包
	BSP_LED_Init();
	BSP_PWM_Init();

	//OLED初始化
	OLED12864_Init();
	OLED12864_Show_String(0,0,"hardware init",1);
	OLED12864_Refresh();
	//nrf24初始化和配置
	nrf_err = nRF24L01_Init();
	if(nrf_err)	//检查硬件
	{
		OLED12864_Show_String(1,0,"nrf err",1);
		LED_CRT(0,LED_ON);
	}else
	{
		//配置nrf24
		nRF24_Cfg.Channel = 50;	//2.45GHz 通讯频段
		nRF24_Cfg.retry = 5;	//最大重发次数
		nRF24_Cfg.retry_cycle = 1;	//重发周期
		nRF24_Cfg.Rx_Length = 32;	//结束长度
		MemCopy(TxAddr,nRF24_Cfg.TX_Addr,5);
		MemCopy(RxAddr,nRF24_Cfg.RX_Addr,5);
		nRF24L01_Config(&nRF24_Cfg);	//配置nRF24L01
		OLED12864_Show_String(1,0,"nrf pass",1);
	}
	OLED12864_Refresh();

	while(1);
#if 0
	//串口初始化
	BSP_Usart_Init();
	//nRF24L01初始化
	if(nRF24L01_Init())
		printf("nRF24L01_Err\r\n");
	else
		printf("nRF24L01_Pass\r\n");
	//nRF24L01 相关配置
	nRF24_Cfg.Channel = 50;	//2.45GHz 通讯频段
	nRF24_Cfg.retry = 5;	//最大重发次数
	nRF24_Cfg.retry_cycle = 1;	//重发周期
	nRF24_Cfg.Rx_Length = 32;	//结束长度
	MemCopy(TxAddr,nRF24_Cfg.TX_Addr,5);
	MemCopy(RxAddr,nRF24_Cfg.RX_Addr,5);
	nRF24L01_Config(&nRF24_Cfg);	//配置nRF24L01

	xTaskCreate(
		Test_Task,
		"Test",
		64,
		NULL,
		15,
		&Test_TaskHandle
	);

	vTaskStartScheduler();	//开启任务调度器

	while(1)
	{
	}
#endif
}
