#ifndef _USER_H_
#define _USER_H_

#include "BSP\bsp_spi.h"
#include "BSP\bsp_pwm.h"
#include "BSP\bsp_led.h"
#include "BSP\bsp_usart.h"
#include "BSP\bsp_timer.h"
#include "BSP\bsp_key.h"
#include "BSP\bsp_beep.h"

#include "HARDWARE\hardware_def.h"
#include "HARDWARE\MPU6050\mpu6050.h"
#include "HARDWARE\MPU6050\eMPL\inv_mpu.h"
#include "HARDWARE\MPU6050\eMPL\inv_mpu_dmp_motion_driver.h"
#include "HARDWARE\NRF24\nrf24l01.h"
#include "HARDWARE\OLED\oled12864.h"
#include "HARDWARE\MOTOR\a4950.h"
#include "HARDWARE\MOTOR\street_motor.h"
#include "HARDWARE\MOTOR\er.h"

#include "SOFTWARE\vofa_p.h"

#include "self_portable\self_portable.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

/**************************电调相关类型*****************************************/
//电调任务参数
typedef struct
{
    er_Type er;    //电调
    uint8_t max_inc;     //单任务周期脉宽最大增量 -> us单位
    uint8_t cycle;      //任务执行周期 -> ms单位
    QueueHandle_t*queueAddr;   ///队列句柄的地址,用于电调任务接收控制信号
}ER_Type;

//电调控制类型
typedef struct
{
    uint8_t type;   //0:保留 1:目标输出 2:最大增量 3:任务周期
    int dat;        
}ERctr_Type;

/**************************直流电机相关类型*****************************************/
//直流电机任务参数
typedef struct
{
    a4950_Type a4950 ;  //a4950
    uint16_t max_inc ;  //最大增量
    uint8_t cycle;
    QueueHandle_t*queueAddr;   //队列句柄的地址,用于直流电机任务接收控制信号
}DCMotor_Type;

//直流电机控制类型
typedef struct
{
    uint8_t type;   //0:保留 1:目标速度 2:刹车 3:单周期增量 4:基于当前目标增加
    int dat;
}DCMotorCtr_Type;

/**************************舵机相关类型*****************************************/
//舵机任务参数
typedef struct
{
    streetMotor_Type streetMotor; //舵机 见hardware_def.h
    uint8_t cycle;              //任务执行周期 -> ms单位
    float angle_inc;          //单任务周期角度增量
    QueueHandle_t*queueAddr;    //队列句柄的地址,用于电调任务接收控制信号
}StreetMotor_Type;

//舵机控制类型
typedef struct
{
    uint8_t type;       //0:保留 1:基于当前角度增加 2:单周期增量 3:设置目标角度 4:直接设置角度
    float dat;        //目标角度
}StreetMotorCtr_Type;

/**************************蜂鸣器相关类型*****************************************/
//蜂鸣器控制
typedef struct
{
    uint8_t count;  //鸣响次数
    uint16_t fre;   //频率
    uint16_t on_ms; //鸣响时长
    uint16_t off_ms;    //关闭时长
}BeepCtr_Type;

/**************************联合体*****************************************/
typedef union
{
    BeepCtr_Type BeepCtr;
    StreetMotorCtr_Type StreetMotorCtr;
    DCMotorCtr_Type DCMotorCtr;
    ERctr_Type  ERctr;
}Ctr_Type;      //控制信息


/**************************系统状态*****************************************/
//系统状态
typedef struct
{
    uint8_t oled_page;  //当前oled的页
    uint16_t nrf_signal; //nrf信号 0:正常 其他:信号丢失时长
    RemoteControl_Type  Recive; //当前遥控器接收到的消息
}sysStatus_Type;

void MPU_Task(void*ptr);
void ReplyMaster_Task(void*ptr);
void nRF24L01_Intterrupt_Task(void*ptr);
void OLED_Task(void*ptr);
void KeyInput_Task(void*ptr);
void ER_Task(void*ptr);
void Motor_Task(void*ptr);
void StreetMotor_Task(void*ptr);
void Beep_Task(void*ptr);
void Main_Task(void*ptr);


#endif  //_USER_H_

