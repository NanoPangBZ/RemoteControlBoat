#include "user.h"

//任务参数
extern nRF24L01_Cfg nRF24_Cfg;
extern uint8_t SendFre;

//任务句柄
extern TaskHandle_t RemoteControl_TaskHandle ;
extern TaskHandle_t nRF24L01_Intterrupt_TaskHandle ;    
extern TaskHandle_t User_FeedBack_TaskHandle ;

//队列 信号
extern SemaphoreHandle_t	nRF24_ISRFlag;		//nrf24硬件中断标志
extern SemaphoreHandle_t	nRF24_RecieveFlag;	//nrf24接收标志(数据已经进入单片机,等待处理)
extern QueueHandle_t		nRF24_SendResult;	//nrf24发送结果
extern SemaphoreHandle_t	boatGyroscope_occFlag;		//船只姿态数据占用标志(互斥信号量)

//全局变量
extern float BoatGyroscope[3];

//使用串口打印消息(带当前任务的名字,方便调试)
#define printMsg(str)   printf("%s:%s",pcTaskGetName(NULL),str)


/*******************************************************************
 * 功能:freeRTOS下的nrf24通讯任务
 * 参数:(uint8_t*) 通讯频率 单位(hz)
 * 返回值:
 * 备注:
 * 2022/2/17   庞碧璋
 *******************************************************************/
void RemoteControl_Task(void*ptr)
{
    uint16_t delay_cycle = (1000 / *(uint8_t*)ptr) / portTICK_RATE_MS;    //通讯频率计算
    uint8_t*sbuffer = nRF24L01_Get_RxBufAddr();
    uint8_t temp;
    TickType_t time = xTaskGetTickCount();  //获取当前系统时间
    while(1)
    {
        nRF24L01_Send(sbuffer,32);
        //等待nrf24中断(发送完成中断 或 未应答中断)
        while(xQueueReceive(nRF24_SendResult,&temp,delay_cycle/4) == pdFALSE)
        {
            nRF24L01_Send(sbuffer,32);
            time = xTaskGetTickCount();  //重新获取当前系统时间
        }
        //nRF24L01_Rx_Mode();     //发送中断处理函数会使nrf24自动进入接收模式   
        if(temp)
        {
            //接收到硬件ACK 说明从机的nrf24已经接收到
            //等待从机回复(这里等待不是nrf24硬件上的ACk信号,是从机上软件的回复)
            //等待时长 1/2 任务周期
            if(xSemaphoreTake(nRF24_RecieveFlag,delay_cycle/2) == pdFALSE)
            {
                //未接收到从机软件回复
            }else
            {
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
        xTaskDelayUntil(&time,delay_cycle); 
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
        #if 0
        printf("******************FeedBack***********************\r\n");
        printf("%s surplusStack:%d\r\n",pcTaskGetName(RemoteControl_TaskHandle),uxTaskGetStackHighWaterMark(RemoteControl_TaskHandle));
        printf("%s surplusStack:%d\r\n",pcTaskGetName(nRF24L01_Intterrupt_TaskHandle),uxTaskGetStackHighWaterMark(nRF24L01_Intterrupt_TaskHandle));
        printf("%s surplusStack:%d\r\n",pcTaskGetName(User_FeedBack_TaskHandle),uxTaskGetStackHighWaterMark(User_FeedBack_TaskHandle));
        printf("SendCount:%d\r\n",SendCount);
        printf("SendAck_Count:%d\r\n",SendAck_Count);
        printf("SendNoAck_Count:%d\r\n",SendNoAck_Count);
        printf("Slave_AckCoount:%d\r\n",Slave_AckCoount);
        printf("Slave_NoAckCount:%d\r\n",Slave_NoAckCount);
        vTaskDelay(1000/portTICK_RATE_MS);
        #endif
        //vTaskDelay(20/portTICK_PERIOD_MS);
        vTaskDelayUntil(&time,20/portTICK_PERIOD_MS);   //50Hz
        if(xSemaphoreTake(boatGyroscope_occFlag,10) == pdPASS)
        {
            for(uint8_t temp=0;temp<3;temp++)
                Vofa_Input(BoatGyroscope[temp],temp);
            xSemaphoreGive(boatGyroscope_occFlag);
        }
        for(uint8_t temp = 0;temp<4;temp++)
        {
            Vofa_Input((float)Rocker_UnsignedGet(&rockers[temp]),temp + 3);
        }
        Vofa_Send();
    }
}


