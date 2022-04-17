#ifndef _USER_H_
#define _USER_H_

#include "BSP\bsp_adc.h"
#include "BSP\bsp_spi.h"
#include "BSP\bsp_usart.h"

#include "HARDWARE\nrf24l01.h"
#include "HARDWARE\usart_hmi.h"

#include "BSP\bsp_usart.h"
#include "BSP\bsp_spi.h"
#include "BSP\bsp_adc.h"

#include "HARDWARE\usart_hmi.h"
#include "HARDWARE\nrf24l01.h"
#include "HARDWARE\rocker.h"

#include "SOFTWARE\vofa_p.h"

#include "self_portable\self_portable.h"    //通讯协议

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

typedef struct
{
    uint16_t SendCount;
    uint16_t SendAck_Count;
    uint16_t SendNoAck_Count;
    uint16_t Slave_AckCoount;
    uint16_t Slave_NoAckCount;
}nrfCount_Type;

typedef struct
{
    uint8_t sign;                   //nrf信号 0:正常 1:丢失
    RemoteControl_Type  readySend;  //准备发送的数据
    BoatReply_Type      recieve;    //从船只接收到的数据
}sysStatus_Type;

void Main_Task(void*ptr);

void RemoteControl_Task(void*ptr);          //nrf24l01控制
void nRF24L01_Intterrupt_Task(void*ptr);
void User_FeedBack_Task(void*ptr);
void Rocker_Task(void*ptr);
void HMI_Task(void*ptr);

#endif //_USER_H_

