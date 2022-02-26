#include "user.h"

extern nRF24L01_Cfg nRF24_Cfg;

extern SemaphoreHandle_t nRF24_ISRFlag;
extern SemaphoreHandle_t nRF24_RecieveFlag;
extern QueueHandle_t     nRF24_SendResult;

uint16_t SendCount = 0;
uint16_t SendAck_Count = 0;
uint16_t SendNoAck_Count = 0;
uint16_t Slave_AckCoount = 0;
uint16_t Slave_NoAckCount = 0;

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
    uint16_t delay_cycle = 1000 / *(uint8_t*)ptr;    //通讯频率计算
    uint8_t sbuffer[32];
    uint8_t temp;
    uint8_t*nrf_recieve = nRF24L01_Get_RxBufAddr();
    TickType_t time = xTaskGetTickCount();  //获取当前系统时间
    while(1)
    {
        nRF24L01_Send(sbuffer,32);
        SendCount++;
        //等待nrf24中断(发送完成中断 或 未应答中断)
        while(xQueueReceive(nRF24_SendResult,&temp,delay_cycle/4/portTICK_RATE_MS) == pdFALSE)
        {
            //等待中断超时
            //可能是本机nrf24没有进入中断 或者 中断处理函数没有给出消息
            printMsg("nrf24 wait send result timeout.\r\n");
            vTaskDelay(500/portTICK_RATE_MS);
            //检查硬件是否正常
            while(nRF24L01_Check() == 1)
            {
                printMsg("nrf24 is err!!\r\n");
                vTaskDelay(500/portTICK_RATE_MS);
            }
            //硬件故障排除,重新发送
            printMsg("nrf24 is ok.\r\n");
            nRF24L01_Config(&nRF24_Cfg);
            printMsg("refresh nrf24 config.\r\n");
            vTaskDelay(500/portTICK_RATE_MS);
            printMsg("Resend.\r\n");
            nRF24L01_Send(sbuffer,32);
            time = xTaskGetTickCount();  //获取当前系统时间
        }
        //nRF24L01_Rx_Mode();     //发送中断处理函数会使nrf24自动进入接收模式   
        if(temp)
        {
            //接收到硬件ACK 说明从机的nrf24已经接收到
            SendAck_Count++;
            //等待从机回复(这里等待不是nrf24硬件上的ACk信号,是从机上软件的回复)
            //等待时长 1/2 任务周期
            if(xSemaphoreTake(nRF24_RecieveFlag,delay_cycle/2/portTICK_RATE_MS) == pdFALSE)
            {
                //未接收到从机软件回复
                Slave_NoAckCount++;
            }else
            {
                //处理从机软件回复
                Slave_AckCoount++;
            }
        }else
        {
            //没有接收到硬件ACK 说明从机没有接收到数据
            SendNoAck_Count++;
        }
        xTaskDelayUntil(&time,delay_cycle/portTICK_RATE_MS); 
    }
}

#if 0
void RemoteControl_Reply_Task(void*ptr)
{
    uint8_t sbuffer[32];
    uint8_t temp;
    uint16_t wait_time = *(uint16_t*)ptr ;
    while(1)
    {
        nRF24L01_Rx_Mode();
        while(xSemaphoreTake(nRF24_RecieveFlag,wait_time/portTICK_RATE_MS) == pdFALSE)    //等待接收结果
        {
            //该周期没有接收到来自遥控器的数据
        }
        //软件应答准备
        //...
        nRF24L01_Send(sbuffer,32);      //发送
        //等待nrf24中断(发送完成中断 或 未应答中断)
        while(xQueueReceive(nRF24_SendResult,&temp,delay_cycle/4/portTICK_RATE_MS) == pdFALSE)
        {
            //等待中断超时
            //可能是本机nrf24没有进入中断 或者 中断处理函数没有给出消息
            printMsg("nrf24 wait send result timeout.\r\n");
            //检查硬件是否正常
            while(nRF24L01_Check() == 1)
            {
                printMsg("nrf24 is err!!\r\n");
                vTaskDelay(500/portTICK_RATE_MS);
            }
            //硬件故障排除,重新发送
            printMsg("nrf24 is ok.\r\n");
            nRF24L01_Config(&nRF24_Cfg);
            printMsg("refresh nrf24 config.\r\n");
            vTaskDelay(500/portTICK_RATE_MS);
            printMsg("Resend.\r\n");
            nRF24L01_Send(sbuffer,32);
        }
    }
}
#endif

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

//任务句柄
extern TaskHandle_t RemoteControl_TaskHandle ;
extern TaskHandle_t nRF24L01_Intterrupt_TaskHandle ;
extern TaskHandle_t User_FeedBack_TaskHandle ;

/*******************************************************************
 * 功能:通过串口定时打印运行状态
 * 参数:NULL
 * 返回值:无
 * 2022/2/17   庞碧璋
 *******************************************************************/
void User_FeedBack_Task(void*ptr)
{
    while(1)
    {
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
