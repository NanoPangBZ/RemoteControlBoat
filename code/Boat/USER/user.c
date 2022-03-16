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
extern SemaphoreHandle_t mpuDat_occFlag;		//mpu数据占用标志(互斥信号量)
extern SemaphoreHandle_t sysStatus_occFlag;     //系统状态变量占用标志(互斥信号量)

//全局变量
float mpu_data[3] = {0,0,0};    //姿态 -> mpuDat_occFlag保护
sysStatus_Type sysStatus;       //系统状态 -> sysStatus_occFlag保护

void RTOSCreateTask_Task(void*ptr)
{
    sysStatus.nrf_signal = 0;
    sysStatus.oled_page = 0;

    nRF24_ISRFlag = xSemaphoreCreateBinary();
	nRF24_RecieveFlag = xSemaphoreCreateBinary();
	nRF24_SendResult = xQueueCreate(1,1);
    mpuDat_occFlag = xSemaphoreCreateMutex();
    sysStatus_occFlag = xSemaphoreCreateMutex();

    xTaskCreate(
        ReplyMaster_Task,
        "Reply",
        128,
        (void*)&nrf_maxDelay,
        11,
        &ReplyMaster_TaskHandle
    );

    xTaskCreate(
        MPU_Task,
        "mpu",
        256,
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
        128,
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
    uint8_t MaxWait = *(uint8_t*)ptr / portTICK_RATE_MS;    //换算成心跳周期
    uint8_t*sbuf = nRF24L01_Get_RxBufAddr();    //nrf缓存地址
    uint8_t resualt;        //发射结果接收
    uint8_t signal = 1;     //信号丢失标志
    while(1)
    {
        while(xSemaphoreTake(nRF24_RecieveFlag,MaxWait) == pdFALSE)
        {
            //过长时间没有接收到主机信号
            //紧急停止代码
            //...
            //更新至系统状态
            if(xSemaphoreTake(sysStatus_occFlag,1) == pdTRUE)
            {
                sysStatus.nrf_signal += 1;
                xSemaphoreGive(sysStatus_occFlag);  //释放资源
            }
            signal = 1;
            vTaskSuspend(nRF24L01_Intterrupt_TaskHandle);   //挂起中断服务
            //有可能是本机nrf挂了,重启nrf
            taskENTER_CRITICAL();
            nRF24L01_Restart();
            taskEXIT_CRITICAL();
            vTaskResume(nRF24L01_Intterrupt_TaskHandle);
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
        //判断是否需要系统状态标志
        //能运行到这里说明信号没有丢失
        if(signal == 1)
        {
            if(xSemaphoreTake(sysStatus_occFlag,1) == pdTRUE)
            {
                sysStatus.nrf_signal = 0;
                xSemaphoreGive(sysStatus_occFlag);  //释放资源
                signal = 0;
            }
        }
        LED_CTR(0,LED_Reserval);
    }
}

//nrf24中断处理
void nRF24L01_Intterrupt_Task(void*ptr)
{
    while(1)
    {
        xSemaphoreTake(nRF24_ISRFlag,portMAX_DELAY);
        nRF24L01_InterruptHandle();     //isr处理函数
    }
}

//屏幕刷新
void OLED_Task(void*ptr)
{
    uint8_t Cycle = (1000 / *(uint8_t*)ptr) / portTICK_RATE_MS;     //频率换算成心跳周期
    TickType_t  time = xTaskGetTickCount();
    float gyroscope[3];
    sysStatus_Type temp = {0,0};
    uint8_t sbuf[32];
    while(1)
    {
        time = xTaskGetTickCount();
        if(xSemaphoreTake(sysStatus_occFlag,1) == pdTRUE)
        {
            temp = sysStatus;
            xSemaphoreGive(sysStatus_occFlag);  //释放资源
        }
        switch (temp.oled_page)
        {
        case 0:
            //将陀螺仪数据载入gyroscope 并且更新oled的姿态显示
            if(xSemaphoreTake(mpuDat_occFlag,1) == pdPASS)
            {
                MemCopy((uint8_t*)mpu_data,(uint8_t*)gyroscope,12);
                xSemaphoreGive(mpuDat_occFlag); //释放资源
                OLED12864_Clear_Page(1);
                OLED12864_Clear_Page(2);
                OLED12864_Clear_Page(3);
                sprintf((char*)sbuf,"x:%.1f",gyroscope[0]);
                OLED12864_Show_String(1,0,sbuf,1);
                sprintf((char*)sbuf,"y:%.1f",gyroscope[1]);
                OLED12864_Show_String(2,0,sbuf,1);
                sprintf((char*)sbuf,"z:%.1f",gyroscope[2]);
                OLED12864_Show_String(3,0,sbuf,1);
            }
            OLED12864_Show_Num(7,0,time/portTICK_RATE_MS/1000,1);
            break;
        }
        OLED12864_Clear_Page(0);
        if(temp.nrf_signal != 0)
        {
            sprintf((char*)sbuf,"loss:%d",temp.nrf_signal);
        }else{
            sprintf((char*)sbuf,"connect");
        }
        OLED12864_Show_String(0,0,sbuf,1);
        OLED12864_Refresh();
        vTaskDelayUntil(&time,Cycle);
    }
}

//姿态更新
void MPU_Task(void*ptr)
{
    uint8_t Cycle = (1000 / *(uint8_t*)ptr) / portTICK_RATE_MS; //频率换算成心跳周期
    TickType_t  time = xTaskGetTickCount();
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
        vTaskDelayUntil(&time,Cycle);
    }
}

//按键输入响应
void KeyInput_Task(void*ptr)
{
    TickType_t time = xTaskGetTickCount();
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
        vTaskDelayUntil(&time,40/portTICK_PERIOD_MS);
    }
}

#if 0

//电调控制任务 可重入
//ptr -> uint8_t*
//ptr[0] -> PWM通道号 见bsp_pwm.c中的Target_CCR[]数组
//ptr[1] -> 单周期脉宽增量(单位us)
//ptr[2] -> 任务的频率
void ER_Task(void*ptr)
{
    uint8_t*buf = (uint8_t*)ptr;
    uint8_t channle = buf[0];
    uint8_t inc = buf[1];
    uint8_t cycle = 1000 / buf[2] /portTICK_PERIOD_MS;
    uint16_t target_width = PWM_Read(channle);
    while(1)
    {
        
    }
}
#endif
