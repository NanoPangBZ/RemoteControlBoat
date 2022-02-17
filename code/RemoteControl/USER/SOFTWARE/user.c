#include "user.h"

extern SemaphoreHandle_t nRF24_ISRFlag;
extern SemaphoreHandle_t nRF24_RecieveFlag;
extern QueueHandle_t     nRF24_SendStatus;

/*******************************************************************
 * 功能:freeRTOS下的nrf24通讯任务
 * 参数:(uint8_t*) 通讯频率 单位(hz)
 * 返回值:
 * 备注:
 * 2022/2/17   庞碧璋
 *******************************************************************/
void RemoteControl_Task(void*ptr)
{
    uint16_t delay_cycle = 1000 / *(uint8_t*)ptr;    //通讯频率计算
    uint8_t sbuffer[32];
    uint8_t temp;
    TickType_t time = xTaskGetTickCount();  //获取当前系统时间
    while(1)
    {
        nRF24L01_Send(sbuffer,32);
        //等待发送结果1/4任务周期 50hz -> 等待5ms
        if(xQueueReceive(nRF24_SendStatus,&temp,delay_cycle/4/portTICK_RATE_MS) == pdFALSE)
        {
            //没有等待到接收结果消息
            //可能是本机nrf24没有进入中断 或者 中断处理函数没有给出消息
            printf("nrf24 wait send result timeout.\r\n");
            if(nRF24L01_Check() == 1)
            {
                printf("nrf24 is err!!\r\n");
            }else
            {
                printf("nrf24 is ok./r/n");
            }
            //无限重发,直至有发送结果消息,中途会堵塞本任务,不影响其它任务
            while(xQueueReceive(nRF24_SendStatus,&temp,1000/portTICK_RATE_MS) == pdFALSE)
            {
                nRF24L01_Send(sbuffer,32);
                printf("nrf24 wait send result timeout.\r\n");
            }
            time = xTaskGetTickCount();  //获取当前系统时间
        }
        if(temp)
        {
            //发送结果处理
        }else
        {
            //发送结果处理
        }
        //等待从机回复(这里等待不是nrf24硬件上的ACk信号,是从机上软件的回复)
        //等待时长 1/2 任务周期
        if(xQueueReceive(nRF24_RecieveFlag,&temp,delay_cycle/2/portTICK_RATE_MS) == pdFALSE)
        {
            //未接收到从机软件回复
        }else
        {
            //处理从机软件回复
        }
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

void User_FeedBack_Task(void*ptr)
{
    while(1)
    {
        vTaskDelay(1000/portTICK_RATE_MS);
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
