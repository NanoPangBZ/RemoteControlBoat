#include "FreeRTOS.h"
#include "task.h"

#include "self_stm32f10x.h"

#include <stdio.h>

//声明port.c中的接口函数
extern void xPortSysTickHandler(void);
//freeRTOS系统心跳
void SysTick_Handler(void)
{
    //判断调度器是否正在运行
	if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        xPortSysTickHandler();  //调用RTOS接口函数
    }
}

//以下是nrf24的中断处理函数,不一定要在ISR调用或者运行!
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
}

