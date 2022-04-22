#ifndef _MAIN_H_
#define _MAIN_H_

#include "user.h"

//nRF24L01配置信息
extern nRF24L01_Cfg nRF24_Cfg;

//任务句柄
extern  TaskHandle_t RTOS_CreatTask_TaskHandle;
extern  TaskHandle_t Main_TaskHandle;
extern  TaskHandle_t RemoteControl_TaskHandle;
extern  TaskHandle_t nRF24L01_Intterrupt_TaskHandle;
extern  TaskHandle_t User_FeedBack_TaskHandle;
extern  TaskHandle_t Rocker_TaskHandle;
extern  TaskHandle_t HMI_TaskHandle;

//队列句柄
extern SemaphoreHandle_t	nRF24_ISRFlag;		//nrf24硬件中断标志
extern SemaphoreHandle_t	nRF24_RecieveFlag;	//nrf24接收标志(数据已经进入单片机,等待处理)
extern QueueHandle_t		nRF24_SendResult;	//nrf24发送结果
extern SemaphoreHandle_t	boatGyroscope_occFlag;		//船只姿态数据占用标志(互斥信号量)

//全局变量
extern float BoatGyroscope[3];			//船只返回的姿态 boatGyroscope_occFlag保护
extern uint8_t rockerInput[4];			//摇杆输入
extern uint8_t nrf_signal;

#endif

