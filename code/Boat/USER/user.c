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
extern TaskHandle_t KeyInput_TaskHandle;

//队列 信号
extern SemaphoreHandle_t nRF24_ISRFlag;         //nRF外部中断标志
extern SemaphoreHandle_t nRF24_RecieveFlag;     //nRF接收中断标志
extern QueueHandle_t     nRF24_SendResult;      //nRF发送结果队列(长度1)
extern SemaphoreHandle_t USART_RecieveFlag;     //串口有未处理数据标志位
extern SemaphoreHandle_t mpuDat_occFlag;		//mpu数据占用标志(互斥信号量)

//全局变量
float mpu_data[3] = {0,0,0};    //姿态
uint8_t oled_page = 0;          //表示当前oled应该显示的页面

void RTOSCreateTask_Task(void*ptr)
{
    nRF24_ISRFlag = xSemaphoreCreateBinary();
	nRF24_RecieveFlag = xSemaphoreCreateBinary();
	nRF24_SendResult = xQueueCreate(1,1);
    USART_RecieveFlag = xSemaphoreCreateBinary();
    mpuDat_occFlag = xSemaphoreCreateMutex();

    xTaskCreate(
        ReplyMaster_Task,
        "Reply",
        256,
        (void*)&nrf_maxDelay,
        11,
        &ReplyMaster_TaskHandle
    );

    xTaskCreate(
        MPU_Task,
        "mpu",
        512,
        (void*)&mpu_fre,
        10,
        &MPU_TaskHandle
    );

    xTaskCreate(
        OLED_Task,
        "oled",
        256,
        (void*)&oled_fre,
        9,
        &OLED_TaskHandle
    );

    xTaskCreate(
        nRF24L01_Intterrupt_Task,
        "nrf interrupt",
        64,
        NULL,
        14,
        &nRF24L01_Intterrupt_TaskHandle
    );

    xTaskCreate(
        KeyInput_Task,
        "key",
        64,
        NULL,
        9,
        &KeyInput_TaskHandle
    );

    vTaskDelete(NULL);
}

//回复主机
void ReplyMaster_Task(void*ptr)
{
    uint8_t MaxWait = *(uint8_t*)ptr / portTICK_RATE_MS;    //频率换算成心跳周期
    uint8_t*sbuf = nRF24L01_Get_RxBufAddr();    //nrf缓存地址
    uint8_t resualt;        //发射结果接收
    uint8_t timeout = 0;    //信号丢失计数
    uint8_t signal = 1;     //信号丢失标志
    while(1)
    {
        while(xSemaphoreTake(nRF24_RecieveFlag,MaxWait) == pdFALSE)
        {
            //过长时间没有接收到主机信号
            //紧急停止代码
            //...
            //使用oled反馈情况,统计信号丢失情况
            if(signal == 0)
            {
                timeout = 0;
                signal = 1;
                OLED12864_Clear_Page(0);
                OLED12864_Show_String(0,0,"Signal Loss",1);
            }
            timeout++;
            OLED12864_Show_Num(0,67,timeout,1);
            //有可能是本机nrf挂了,重启nrf
            taskENTER_CRITICAL();
            nRF24L01_Restart();
            taskEXIT_CRITICAL();

        }
        //是否需要更新oled显示的连接情况
        if(signal == 1)
        {
            signal = 0;
            OLED12864_Clear_Page(0);
            OLED12864_Show_String(0,0,"Signal Right",1);
        }
        //处理主机发送的数据
        //..
        //回复主机
        if(xSemaphoreTake(mpuDat_occFlag,1) == pdPASS)
        {
            MemCopy((uint8_t*)mpu_data,sbuf,12);
            xSemaphoreGive(mpuDat_occFlag);     //释放资源
        }
        nRF24L01_Send(sbuf,32);
        xQueueReceive(nRF24_SendResult,&resualt,MaxWait);   //等待发送结果
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
    uint8_t Cycle = (1000 / *(uint8_t*)ptr) / portTICK_RATE_MS;     //频率换算成心跳周期
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
    uint8_t Cycle = (1000 / *(uint8_t*)ptr) / portTICK_RATE_MS; //频率换算成心跳周期
    TickType_t  time = xTaskGetTickCount();
    uint8_t sbuf[32];
    float fsbuf[3];
    while(1)
    {
        //进入临界区,防止软件iic时序错误
        //taskENTER_CRITICAL();
        mpu_dmp_get_data(&fsbuf[0],&fsbuf[1],&fsbuf[2]);
        //taskEXIT_CRITICAL();
        //更新陀螺仪数据
        if(xSemaphoreTake(mpuDat_occFlag,1) == pdPASS)
        {
            MemCopy((uint8_t*)fsbuf,(uint8_t*)mpu_data,12);
            xSemaphoreGive(mpuDat_occFlag); //释放资源
        }
        for(uint8_t temp=0;temp<3;temp++)
        {
            OLED12864_Clear_Page(1+temp);
            sprintf((char*)sbuf,"%.1f",fsbuf[temp]);
            OLED12864_Show_String(1+temp,0,sbuf,1);
        }
        vTaskDelayUntil(&time,Cycle);
    }
}

void KeyInput_Task(void*ptr)
{
    TickType_t time = xTaskGetTickCount();
    static uint8_t oled_status = 1;
    while(1)
    {
        if(Key_Read(0) == Key_Press)
        {
            vTaskSuspend(OLED_TaskHandle);
            OLED12864_Clear();
            OLED12864_Refresh();
        }
        if(Key_Read(1) == Key_Press)
        {
            vTaskResume(OLED_TaskHandle);
        }
        vTaskDelayUntil(&time,20/portTICK_PERIOD_MS);
    }
}
