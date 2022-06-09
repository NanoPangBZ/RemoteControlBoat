#ifndef _MAIN_H_
#define _MAIN_H_

#include "user.h"

//对外声明main.c中的全局变量

//参数
extern nRF24L01_Cfg nRF24_Cfg;

//任务句柄
extern TaskHandle_t nRF24L01_Intterrupt_TaskHandle;

//队列 信号
extern SemaphoreHandle_t nRF24_ISRFlag;         //nRF外部中断标志
extern SemaphoreHandle_t nRF24_RecieveFlag;     //nRF接收中断标志
extern QueueHandle_t     nRF24_SendResult;      //nRF发送结果队列(长度1)
extern SemaphoreHandle_t mpuDat_occFlag;		//mpu数据占用标志(互斥信号量)
extern SemaphoreHandle_t sysStatus_occFlag;     //系统状态变量占用标志(互斥信号量)
extern QueueHandle_t     Beep_CmdQueue;
extern QueueHandle_t	 ER_CmdQueue[4];
extern QueueHandle_t     DCMotor_CmdQueue[2];
extern QueueHandle_t     STMotor_CmdQueue[4];

//全局变量
extern float mpu_data[3];               //姿态 -> mpuDat_occFlag保护
extern float BatVol;
extern float Depth;
extern sysStatus_Type sysStatus;       //系统状态 -> sysStatus_occFlag保护

#endif

