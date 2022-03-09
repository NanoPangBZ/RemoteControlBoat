#include "self_stm32f10x.h"

#include "HARDWARE\NRF24\nrf24l01.h"

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

//队列 信号
extern SemaphoreHandle_t nRF24_ISRFlag;
extern SemaphoreHandle_t nRF24_RecieveFlag;
extern QueueHandle_t     nRF24_SendResult;
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

void EXTI9_5_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;   //用于判断这个发出的信号量是否会引起其他高优先级的任务解除阻塞
    if(EXTI_GetITStatus(NRF24L01_IQR_Line) == SET)
    {
        xSemaphoreGiveFromISR(nRF24_ISRFlag,&xHigherPriorityTaskWoken);
        EXTI_ClearITPendingBit(NRF24L01_IQR_Line);          //挂起中断
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);       //判断是否需要进行上下文切换(任务调度)
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

