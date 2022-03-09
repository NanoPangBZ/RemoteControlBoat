#include "user.h"
#include "self_stm32f10x.h"
#include <stdio.h>

//队列 信号
extern SemaphoreHandle_t nRF24_ISRFlag;
extern SemaphoreHandle_t nRF24_RecieveFlag;
extern QueueHandle_t     nRF24_SendResult;

//全局变量
float Gyrocope[3] = {0,0,0};

//使用串口打印消息(带当前任务的名字,方便调试)
#define printMsg(str)   printf("%s:%s",pcTaskGetName(NULL),str)

void RTOSCreateTask_Task(void*ptr)
{
    nRF24_ISRFlag = xSemaphoreCreateBinary();
	nRF24_RecieveFlag = xSemaphoreCreateBinary();
	nRF24_SendResult = xQueueCreate(1,1);
    vTaskDelete(NULL);
}

//回复主机
void ReplyMaster_Task(void*ptr)
{
    uint8_t MaxWait = *(uint8_t*)ptr / portTICK_RATE_MS;
    uint8_t sbuf[32];
    while(1)
    {
        if(xSemaphoreTake(nRF24_RecieveFlag,MaxWait) == pdFALSE)
        {
            printMsg("nrf loss Sigh\r\n");
        }else
        {
            nRF24L01_Read_RxSbuffer(sbuf,32);
            nRF24L01_Send(sbuf,32);
            xQueueReceive(nRF24_SendResult,sbuf,MaxWait);
            if(sbuf[0] == 1);
        }
    }
}

//nrf24中断处理
void nRF24L01_Intterrupt_Task(void*ptr)
{
    while(1)
    {
        xSemaphoreTake(nRF24_ISRFlag,portMAX_DELAY);    //无限期等待
        nRF24L01_InterruptHandle();     //isr处理函数
    }
}

//屏幕刷新
void OLED_Task(void*ptr)
{
    uint8_t Cycle = (1000 / *(uint8_t*)ptr) / portTICK_RATE_MS;
    TickType_t  time = xTaskGetTickCount();
    while(1)
    {
        OLED12864_Refresh();
        vTaskDelayUntil(&time,Cycle);
    }
}

//姿态更新
void Gyroscope_Task(void*ptr)
{
    uint8_t Cycle = (1000 / *(uint8_t*)ptr) / portTICK_RATE_MS;
    TickType_t  time = xTaskGetTickCount();
    while(1)
    {
        mpu_dmp_get_data(&Gyrocope[0],&Gyrocope[1],&Gyrocope[2]);
        vTaskDelayUntil(&time,Cycle);
    }
}

