#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "HARDWARE\nrf24l01.h"

extern QueueHandle_t nRF24_ISRFlag;     //声明main.c中的队列
extern void xPortSysTickHandler(void);		//声明port.c中的中断接口函数

void SysTick_Handler(void)
{
    if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)   //判断系统是否已经启动
    {
        xPortSysTickHandler();		//系统心跳函数
    }
}

void EXTI9_5_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;   //用于判断这个发出的信号量是否会引起其他高优先级的任务解除阻塞
    if(EXTI_GetITStatus(NRF24L01_IQR_Line) == SET)
    {
        xSemaphoreGiveFromISR(nRF24_ISRFlag,&xHigherPriorityTaskWoken);
        EXTI_ClearITPendingBit(NRF24L01_IQR_Line);          //挂起中断
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);       //判断是否需要进行上下文切换
    }
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
}
