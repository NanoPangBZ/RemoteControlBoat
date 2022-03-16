#include "self_stm32f10x.h"

#include "BSP\bsp_usart.h"
#include "BSP\bsp_led.h"

#include "HARDWARE\NRF24\nrf24l01.h"

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

//任务句柄
extern TaskHandle_t     nRF24L01_Intterrupt_TaskHandle;
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
    static BaseType_t xHigherPriorityTaskWoken = pdFALSE;   //用于判断这个发出的信号量是否会引起其他高优先级的任务解除阻塞
    if(EXTI_GetITStatus(NRF24L01_IQR_Line) == SET)
    {
        if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED && nRF24L01_Intterrupt_TaskHandle != NULL)
        {
            xSemaphoreGiveFromISR(nRF24_ISRFlag,&xHigherPriorityTaskWoken);   
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);       //判断是否需要进行上下文切换(任务调度)
        }else
        {
            nRF24L01_Write_Reg(0x07,0xE0);
            nRF24L01_Send_Cmd(0xE1);
            nRF24L01_Send_Cmd(0xE2);
        }
        EXTI_ClearITPendingBit(NRF24L01_IQR_Line);//挂起中断
    }
}

/******************************串口相关ISR**************************************/
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
    {
        //是否使用串口传透
        #if 1
            Rx_SbufferInput(1,USART_ReceiveData(USART1));
        #else
            while(Usart_BusyCheck(2));
            USART_SendData(USART2,USART_ReceiveData(USART1));
            while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);
        #endif
        USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    }
}


void USART3_IRQHandler(void)
{
    if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET)
    {
        //是否使用串口传透
        #if 1
            Rx_SbufferInput(2,USART_ReceiveData(USART3));
        #else
            while(Usart_BusyCheck(1));
            USART_SendData(USART1,USART_ReceiveData(USART2));
            while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
        #endif
        USART_ClearITPendingBit(USART3,USART_IT_RXNE);
    }
}

void DMA1_Channel4_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC4) == SET)
    {
        Tx_Flag_Clear(1);
        DMA1_Channel4->CCR &= (uint16_t)(~DMA_CCR1_EN);
        DMA_ClearITPendingBit(DMA1_IT_TC4);
    }
}

void DMA1_Channel3_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC3) == SET)
    {
        Tx_Flag_Clear(2);
        DMA1_Channel3->CCR &= (uint16_t)(~DMA_CCR1_EN);
        DMA_ClearITPendingBit(DMA1_IT_TC3);
    }
}

//下面三个isr并不是真的isr,因为由RTOS的中断处理任务调用!
//并不是在硬件的中断服务函数中运行,使用不需要使用RTOS的
//FromISR API函数

static uint8_t send_result;

void nRF24L01_NoACK_ISR(void)
{
    send_result = 0;
    xQueueSend(nRF24_SendResult,&send_result,0);
}

void nRF24L01_Tx_ISR(void)
{
    send_result = 1;
    xQueueSend(nRF24_SendResult,&send_result,0);
}

void nRF24L01_Rx_ISR(void)
{
    xSemaphoreGive(nRF24_RecieveFlag);
}

