#include "user.h"

void RemoteControl_Task(void*ptr)
{
    uint8_t delay_cycle = 1000 / *(uint8_t*)ptr;
    uint8_t sbuffer[32];
    TickType_t time = xTaskGetTickCount();  //获取当前系统时间
    while(1)
    {
        nRF24L01_Send(sbuffer,32);
        xTaskDelayUntil(&time,delay_cycle/portTICK_RATE_MS);
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
