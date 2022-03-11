#include "self_stm32f10x.h"

#include "BSP\bsp_spi.h"
#include "BSP\bsp_pwm.h"
#include "BSP\bsp_led.h"
#include "BSP\bsp_usart.h"
#include "BSP\bsp_timer.h"

#include "HARDWARE\MPU6050\mpu6050.h"
#include "HARDWARE\MPU6050\eMPL\inv_mpu.h"
#include "HARDWARE\MPU6050\eMPL\inv_mpu_dmp_motion_driver.h"
#include "HARDWARE\NRF24\nrf24l01.h"
#include "HARDWARE\OLED\oled12864.h"

#include "user.h"
#include "SOFTWARE\vofa_p.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#pragma	diag_suppress	870	//屏蔽汉字警告

//nRF24L01初始化结构体
nRF24L01_Cfg nRF24_Cfg;
static uint8_t RxAddr[5] = {0x43,0x16,'R','C',0xFF};	//遥控器地址
static uint8_t TxAddr[5] = {0x43,0x16,'B','T',0xFF};	//船地址

//任务参数
uint8_t oled_fre = 24;		//OLED刷新频率
uint8_t nrf_maxDelay = 200;	//nrf最大等待接收时长
uint8_t mpu_fre = 25;		//mpu更新频率

//任务句柄
TaskHandle_t	RTOSCreateTask_TaskHandle = NULL;
TaskHandle_t	ReplyMaster_TaskHandle = NULL;
TaskHandle_t	OLED_TaskHandle = NULL;
TaskHandle_t	nRF24L01_Intterrupt_TaskHandle = NULL;
TaskHandle_t	MPU_TaskHandle = NULL;

//队列句柄
SemaphoreHandle_t	nRF24_ISRFlag = NULL;		//nrf24硬件中断标志
SemaphoreHandle_t	nRF24_RecieveFlag = NULL;	//nrf24接收标志(数据已经进入单片机,等待处理)
QueueHandle_t		nRF24_SendResult = NULL;	//nrf24发送结果
SemaphoreHandle_t	USART_RecieveFlag = NULL;	//串口有未处理数据标志

int main(void)
{
	uint8_t nrf_err;
	uint8_t mpu_err;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	//关闭JATG调试接口,开启SWD调试接口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

	BSP_LED_Init();
	BSP_Usart_Init();

	//OLED初始化
	OLED12864_Init();
	OLED12864_Show_String(0,0,"hardware init",1);
	OLED12864_Refresh();

	//MPU初始化
	mpu_err = MPU_Init();
	if(mpu_err)
	{
		OLED12864_Show_String(2,0,"mpu err",1);
	}else
	{
		OLED12864_Show_String(2,0,"mpu pass",1);
		if(mpu_dmp_init())	//初始化官方的DMP固件库
			OLED12864_Show_String(3,0,"6050 dmp err",1);
	}

	//nrf24初始化和配置
	nrf_err = nRF24L01_Init();
	if(nrf_err)	//检查硬件
	{
		OLED12864_Show_String(1,0,"nrf err",1);
	}else
	{
		//nRF24L01 相关配置
		nRF24_Cfg.Channel = 50;	//2.45GHz 通讯频段
		nRF24_Cfg.retry = 5;	//最大重发次数
		nRF24_Cfg.retry_cycle = 1;	//重发周期
		nRF24_Cfg.Rx_Length = 32;	//结束长度
		MemCopy(TxAddr,nRF24_Cfg.TX_Addr,5);
		MemCopy(RxAddr,nRF24_Cfg.RX_Addr,5);
		nRF24L01_Config(&nRF24_Cfg);	//配置nRF24L01
		OLED12864_Show_String(1,0,"nrf pass",1);
		nRF24L01_Rx_Mode();
	}
	OLED12864_Refresh();

	//创建RTOS初始化任务
	if(nrf_err == 0 & mpu_err == 0)
	{
		xTaskCreate(
			RTOSCreateTask_Task,
			"init",
			128,
			NULL,
			15,
			&RTOSCreateTask_TaskHandle
		);
	}

	vTaskStartScheduler();

	while(1);
}

