#include "user.h"
#include "self_stm32f10x.h"
#include <stdio.h>

//其它参数
extern nRF24L01_Cfg nRF24_Cfg;

//任务参数
extern uint8_t oled_fre;		//OLED刷新频率
extern uint8_t nrf_maxDelay;	//nrf最大等待接收时长
extern uint8_t gyroscope_fre;   //姿态刷新频率

//任务句柄
extern TaskHandle_t ReplyMaster_TaskHandle;
extern TaskHandle_t OLED_TaskHandle;
extern TaskHandle_t nRF24L01_Intterrupt_TaskHandle;
extern TaskHandle_t Gyroscope_TaskHandle;

//队列 信号
extern SemaphoreHandle_t nRF24_ISRFlag;
extern SemaphoreHandle_t nRF24_RecieveFlag;
extern QueueHandle_t     nRF24_SendResult;
extern SemaphoreHandle_t USART_RecieveFlag;

//全局变量
BoatPack_TypeDef    BoatPack;

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
        14,
        &ReplyMaster_TaskHandle
    );

    xTaskCreate(
        OLED_Task,
        "oled",
        144,
        (void*)&oled_fre,
        11,
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

    xTaskCreate(
        Gyroscope_Task,
        "mpu",
        144,
        &gyroscope_fre,
        12,
        &Gyroscope_TaskHandle
    );

    vTaskDelete(NULL);
}

//回复主机
void ReplyMaster_Task(void*ptr)
{
    uint8_t MaxWait = *(uint8_t*)ptr / portTICK_PERIOD_MS ;
    uint8_t sbuf[32];
    while(1)
    {
        while(xSemaphoreTake(nRF24_RecieveFlag,MaxWait) == pdFALSE)
        {
            //过长时间没有接收到主机信号
            //...
        }
        //处理主机发送的数据
        nRF24L01_Read_RxSbuffer(sbuf,32);
        //反馈回主机
        nRF24L01_Send(sbuf,32);
        while(xQueueReceive(nRF24_SendResult,sbuf,MaxWait) == pdFALSE)
        {
            //nrf发送失败,硬件检查
            while(nRF24L01_Check())
            {
                vTaskDelay(500/portTICK_PERIOD_MS);
            }
            nRF24L01_Config(&nRF24_Cfg);
            nRF24L01_Send(sbuf,32);
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
    uint8_t Cycle = (1000 / *(uint8_t*)ptr) / portTICK_PERIOD_MS ;
    TickType_t  time = xTaskGetTickCount();
    uint8_t sbuf[16];
    while(1)
    {
        //time * ( ( TickType_t ) 1000 / configTICK_RATE_HZ ) / 100
        //time / 50
        OLED12864_Show_Num(7,0,(time*portTICK_PERIOD_MS)/100,1); //单位 0.1s
        sprintf((char*)sbuf,"%.1f",BoatPack.Gyroscope[0]);
        OLED12864_Show_String(3,0,sbuf,1);
        sprintf((char*)sbuf,"%.1f",BoatPack.Gyroscope[1]);
        OLED12864_Show_String(4,0,sbuf,1);
        sprintf((char*)sbuf,"%.1f",BoatPack.Gyroscope[2]);
        OLED12864_Show_String(5,0,sbuf,1);
        OLED12864_Refresh();
        vTaskDelayUntil(&time,Cycle);
    }
}

//姿态更新
void Gyroscope_Task(void*ptr)
{
    uint8_t Cycle = (1000 / *(uint8_t*)ptr) / portTICK_PERIOD_MS ;
    TickType_t  time = xTaskGetTickCount();
    while(1)
    {
        //mpu_dmp_get_data 内部自带进入和退出临界区的代码段
        //无需担心软件i2c被打断
        mpu_dmp_get_data(&BoatPack.Gyroscope[0],&BoatPack.Gyroscope[1],&BoatPack.Gyroscope[2]);
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
