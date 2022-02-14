#include "user.h"

extern SemaphoreHandle_t nRF24_ISRFlag;

void RemoteControl_Task(void*ptr)
{
    uint16_t delay_cycle = 1000 / *(uint8_t*)ptr;    //通讯频率计算
    uint8_t sbuffer[32];
    TickType_t time = xTaskGetTickCount();  //获取当前系统时间
    while(1)
    {
        nRF24L01_Send(sbuffer,32);
        printf("Send 32Byte\r\n");
        xTaskDelayUntil(&time,delay_cycle/portTICK_RATE_MS);
    }
}

void nRF24L01_Intterrupt_Task(void*ptr)
{
    while(1)
    {
        xSemaphoreTake(nRF24_ISRFlag,portMAX_DELAY);    //无限期等待
        nRF24L01_InterruptHandle();     //isr处理函数
    }
}

void FreeRTOS_Test_Task(void*ptr)
{
    while(1)
    {
        printf("%s\r\n",(char*)ptr);
        vTaskDelay(2000/portTICK_RATE_MS);
    }
}
