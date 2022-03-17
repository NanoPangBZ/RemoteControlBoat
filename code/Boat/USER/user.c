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
extern TaskHandle_t	ER_TaskHandle[4];

//队列 信号
extern SemaphoreHandle_t nRF24_ISRFlag;         //nRF外部中断标志
extern SemaphoreHandle_t nRF24_RecieveFlag;     //nRF接收中断标志
extern QueueHandle_t     nRF24_SendResult;      //nRF发送结果队列(长度1)
extern SemaphoreHandle_t mpuDat_occFlag;		//mpu数据占用标志(互斥信号量)
extern SemaphoreHandle_t sysStatus_occFlag;     //系统状态变量占用标志(互斥信号量)
extern QueueHandle_t	 ER_CmdQueue[4];

//全局变量
extern float mpu_data[3];               //姿态 -> mpuDat_occFlag保护
extern sysStatus_Type sysStatus;       //系统状态 -> sysStatus_occFlag保护

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
        //test
        OLED12864_Show_Num(4,0,PWM_Read(11),1);
        //
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
        #if 0   //开关OLED
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
        #endif
        ERctr_Type  ctr;
        ctr.type = 1;
        if(Key_Read(0) == Key_Press)
        {
            ctr.dat = 1400;
            xQueueSend(ER_CmdQueue[3],&ctr,0);
        }else
        if(Key_Read(1) == Key_Press)
        {
            ctr.dat = 1650;
            xQueueSend(ER_CmdQueue[3],&ctr,0);
        }else
        if(Key_Read(2) == Key_Press)
        {
            ctr.dat = 1150;
            xQueueSend(ER_CmdQueue[3],&ctr,0);
        }else
        if(Key_Read(3) == Key_Press)
        {
            ctr.dat = 1900;
            xQueueSend(ER_CmdQueue[3],&ctr,0);
        }
        vTaskDelayUntil(&time,40/portTICK_PERIOD_MS);
    }
}

#if 0
void Motor_Task(void*ptr)
{

}
#endif

//电调任务 可重入
void ER_Task(void*ptr)
{
    ER_Type ERT;
    ERctr_Type ctr;
    TickType_t  time;
    uint16_t target_width;
    uint16_t width;
    ERT = *(ER_Type*)ptr;
    target_width = 1400;
    time = xTaskGetTickCount();
    while(1)
    {
        //查看是否有新的指令
        while(xQueueReceive(*ERT.recieveCmd,&ctr,0) == pdTRUE)
        {
            switch (ctr.type)
            {
            case 1: target_width = ctr.dat; break;
            case 2: ERT.MaxInc = ctr.dat; break;
            case 3: ERT.cycle = ctr.dat;break;
            }
            LED_CTR(1,LED_Reserval);
        }
        //更新PWM输出
        width = PWM_Read(ERT.channel);
        if(width > target_width)
        {
            if(width - target_width > ERT.MaxInc)
                width -= ERT.MaxInc;
            else
                width = target_width;
        }else if(width < target_width)
        {
            if(target_width - width > ERT.MaxInc)
                width += ERT.MaxInc;
            else
                width = target_width;
        }
        PWM_Out(ERT.channel,width);
        vTaskDelayUntil(&time,ERT.cycle/portTICK_PERIOD_MS);
    }
}


