#include "user.h"
#include "self_stm32f10x.h"
#include <stdio.h>

//其它参数
extern nRF24L01_Cfg nRF24_Cfg;

//任务参数
extern uint8_t oled_fre;		//OLED刷新频率
extern uint8_t nrf_maxDelay;	//nrf最大等待接收时长
extern uint8_t mpu_fre;         //mpu姿态更新频率

//任务句柄
extern TaskHandle_t ReplyMaster_TaskHandle;
extern TaskHandle_t OLED_TaskHandle;
extern TaskHandle_t nRF24L01_Intterrupt_TaskHandle;
extern TaskHandle_t MPU_TaskHandle;

//队列 信号
extern SemaphoreHandle_t nRF24_ISRFlag;
extern SemaphoreHandle_t nRF24_RecieveFlag;
extern QueueHandle_t     nRF24_SendResult;
extern SemaphoreHandle_t USART_RecieveFlag;

//全局变量
float Gyrocope[3] = {0,0,0};    //姿态

//使用串口打印消息(带当前任务的名字,方便调试)
#define printMsg(str)   printf("%s:%s",pcTaskGetName(NULL),str)

void RTOSCreateTask_Task(void*ptr)
{
    nRF24_ISRFlag = xSemaphoreCreateBinary();
	nRF24_RecieveFlag = xSemaphoreCreateBinary();
	nRF24_SendResult = xQueueCreate(1,1);
    USART_RecieveFlag = xSemaphoreCreateBinary();

    xTaskCreate(
        ReplyMaster_Task,
        "Reply",
        144,
        (void*)&nrf_maxDelay,
        12,
        &ReplyMaster_TaskHandle
    );

    xTaskCreate(
        MPU_Task,
        "mpu",
        512,
        (void*)&mpu_fre,
        11,
        &MPU_TaskHandle
    );

    xTaskCreate(
        OLED_Task,
        "oled",
        128,
        (void*)&oled_fre,
        10,
        &OLED_TaskHandle
    );

    xTaskCreate(
        nRF24L01_Intterrupt_Task,
        "nrf interrupt",
        64,
        NULL,
        15,
        &nRF24L01_Intterrupt_TaskHandle
    );

    vTaskDelete(NULL);
}

//回复主机
void ReplyMaster_Task(void*ptr)
{
    uint8_t MaxWait = *(uint8_t*)ptr / portTICK_RATE_MS;
    uint8_t*sbuf = nRF24L01_Get_RxBufAddr();
    while(1)
    {
        while(xSemaphoreTake(nRF24_RecieveFlag,MaxWait) == pdFALSE)
        {
            //过长时间没有接收到主机信号
            //...
            OLED12864_Clear_Page(0);
            OLED12864_Show_String(0,0,"Signal Loss",1);
            nRF24L01_Init();
            nRF24L01_Config(&nRF24_Cfg);
            nRF24L01_Rx_Mode();
        }
        OLED12864_Show_String(0,0,"Signal Right",1);
        //处理主机发送的数据
        //..
        //反馈回主机
        MemCopy((uint8_t*)Gyrocope,sbuf,12);
        nRF24L01_Send(sbuf,32);
        while(xQueueReceive(nRF24_SendResult,sbuf,MaxWait) == pdFALSE)
        {
            //nrf发送失败,硬件检查
            while(nRF24L01_Check())
            {
                vTaskDelay(500/portTICK_RATE_MS);
            }
            nRF24L01_Config(&nRF24_Cfg);
            nRF24L01_Send(sbuf,32);
        }
        LED_CTR(0,LED_Reserval);
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
        time = xTaskGetTickCount();
        OLED12864_Show_Num(7,0,time/portTICK_RATE_MS/100,1);
        OLED12864_Refresh();
        vTaskDelayUntil(&time,Cycle);
    }
}

//姿态更新
void MPU_Task(void*ptr)
{
    uint8_t Cycle = (1000 / *(uint8_t*)ptr) / portTICK_RATE_MS;
    TickType_t  time = xTaskGetTickCount();
    uint8_t sbuf[32];
    while(1)
    {
        mpu_dmp_get_data(&Gyrocope[0],&Gyrocope[1],&Gyrocope[2]);
        for(uint8_t temp=0;temp<3;temp++)
        {
            OLED12864_Clear_Page(1+temp);
            sprintf((char*)sbuf,"%.1f",Gyrocope[temp]);
            OLED12864_Show_String(1+temp,0,sbuf,1);
        }
        vTaskDelayUntil(&time,Cycle);
    }
}

void User_FeedBack_Task(void*ptr)
{
    while(1)
    {
        //printf("");
    }
}
