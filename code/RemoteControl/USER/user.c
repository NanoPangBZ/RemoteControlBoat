#include "user.h"
#include "main.h"

/*******************************************************************
 * 功能:freeRTOS下的nrf24通讯任务
 * 参数:(uint8_t*) 通讯频率 单位(hz)
 * 返回值:
 * 备注:
 * 2022/2/17   庞碧璋
 *******************************************************************/
uint16_t SendCount = 0;
uint16_t AckCount = 0;
void RemoteControl_Task(void*ptr)
{
    uint16_t delay_cycle = (1000 / *(uint8_t*)ptr) / portTICK_RATE_MS;    //通讯频率计算
    uint8_t*sbuffer = nRF24L01_Get_RxBufAddr();
    RemoteControl_Type  send;
    uint8_t sendResault = 0;
    uint8_t statis_sendCount = 0;   //发送统计
    uint8_t statis_sfAckCount = 0;  //从机软件回复统计
    TickType_t time = xTaskGetTickCount();  //获取当前系统时间
    while(1)
    {
        send.cmd = 1;
        MemCopy(rockerInput,send.rocker,4);     //将摇杆值载入发送
        nRF24L01_Send((uint8_t*)&send,32);      //发送
        //等待nrf24中断(发送完成中断 或 未应答中断)
        while(xQueueReceive(nRF24_SendResult,&sendResault,delay_cycle) == pdFALSE)
        {
            //可能是nrf出现未知错误
            vTaskSuspend(nRF24L01_Intterrupt_TaskHandle);   //挂起中断任务
            nRF24L01_Write_Reg(0x07,0xE0);    //清除nrf24所有中断
            EXTI_ClearITPendingBit(NRF24L01_IQR_Line);          //挂起外部中断
            nRF24L01_Init();                   //重新初始化nrf
            nRF24L01_Config(&nRF24_Cfg);
            vTaskResume(nRF24L01_Intterrupt_TaskHandle);    //解挂
            nRF24L01_Send((uint8_t*)&send,32);    //重新发送
            time = xTaskGetTickCount();  //重新获取当前系统时间
            statis_sendCount = statis_sfAckCount = 0;
            nrf_signal = 0;
        }
        statis_sendCount++;
        SendCount++;
        //nRF24L01_Rx_Mode();     //发送中断处理函数会使nrf24自动进入接收模式   
        if(sendResault)
        {
            //接收到硬件ACK 说明从机的nrf24已经接收到
            //等待从机回复(这里等待不是nrf24硬件上的ACk信号,是从机上软件的回复)
            //等待时长 1/2 任务周期
            if(xSemaphoreTake(nRF24_RecieveFlag,delay_cycle) == pdFALSE)
            {
                //未接收到从机软件回复
            }else
            {
                AckCount++;
                statis_sfAckCount++;
                //处理从机软件回复
                if(xSemaphoreTake(boatGyroscope_occFlag,2) == pdPASS)
                {
                    MemCopy((uint8_t*)sbuffer,(uint8_t*)BoatGyroscope,12);
                    xSemaphoreGive(boatGyroscope_occFlag);
                }
            }
        }else
        {
            //没有接收到硬件ACK 说明从机没有接收到数据
        }
        if(statis_sendCount == 50)
        {
            nrf_signal = statis_sfAckCount;
            statis_sendCount = statis_sfAckCount = 0;
        }
        xTaskDelayUntil(&time,delay_cycle); 
    }
}

/*******************************************************************
 * 功能:摇杆输入
 * 参数:(uint8_t*) -> 任务频率
 * 返回值:无
 * 2022/2/17   庞碧璋
 *******************************************************************/
void Rocker_Task(void*ptr)
{
    uint8_t cycle = (1000 / *(uint8_t*)ptr) / portTICK_PERIOD_MS;   //频率换算周期
    TickType_t  time = xTaskGetTickCount();
    while(1)
    {
        for(uint8_t temp=0;temp<4;temp++)
            rockerInput[temp] = Rocker_UnsignedGet(&rockers[temp]);
        vTaskDelayUntil(&time,cycle);
    }
}

/*******************************************************************
 * 功能:freeRTOS下的nrf24中断处理函数
 * 参数:NULL
 * 返回值:无
 * 备注:在isr外处理nrf24的中断,需要isr发出nRF24_ISRFlag信号
 * 2022/2/17   庞碧璋
 *******************************************************************/
void nRF24L01_Intterrupt_Task(void*ptr)
{
    while(1)
    {
        xSemaphoreTake(nRF24_ISRFlag,portMAX_DELAY);    //无限期等待
        nRF24L01_InterruptHandle();     //isr处理函数
    }
}

/*******************************************************************
 * 功能:通过串口定时打印运行状态
 * 参数:NULL
 * 返回值:无
 * 2022/2/17   庞碧璋
 *******************************************************************/
void User_FeedBack_Task(void*ptr)
{
    TickType_t time = xTaskGetTickCount();
    while(1)
    {
        vTaskDelayUntil(&time,20/portTICK_PERIOD_MS);   //50Hz
        float sbuf[3];
        //姿态反馈
        if(xSemaphoreTake(boatGyroscope_occFlag,5) == pdPASS)
        {
            MemCopy((uint8_t*)BoatGyroscope,(uint8_t*)sbuf,12);
            xSemaphoreGive(boatGyroscope_occFlag);
        }
        for(uint8_t temp=0;temp<3;temp++)
            Vofa_Input(BoatGyroscope[temp],temp);
        
        #if 1
        Vofa_Input((float)rockerInput[0],3);
        Vofa_Input((float)rockerInput[1],4);
        Vofa_Input((float)rockerInput[2],5);
        Vofa_Input((float)rockerInput[3],6);
        Vofa_Input((float)SendCount,7);
        Vofa_Input((float)AckCount,8);
        #endif
        Vofa_Send();
    }
}

/*******************************************************************
 * 功能:刷新串口屏显示
 * 参数:(uint8_t*) -> 刷新频率
 * 返回值:无
 ******************************************************************/
void HMI_Task(void*ptr)
{
    TickType_t time = xTaskGetTickCount();
    uint16_t cycle = 1000 / *(uint8_t*)ptr / portTICK_PERIOD_MS;
    while(1)
    {
        //更新油门显示
        HMI_SetNum((short)rockerInput[0] - 50,0);
        HMI_SetNum((short)rockerInput[1] - 50,1);
        HMI_SetNum((short)(nrf_signal)*2,2);
        for(uint8_t temp=0;temp<3;temp++)
            HMI_SetFloat(BoatGyroscope[temp],temp);
        //处理串口屏返回
        vTaskDelayUntil(&time,cycle);
    }
}

