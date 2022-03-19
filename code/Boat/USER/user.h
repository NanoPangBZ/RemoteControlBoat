#ifndef _USER_H_
#define _USER_H_

#include "BSP\bsp_spi.h"
#include "BSP\bsp_pwm.h"
#include "BSP\bsp_led.h"
#include "BSP\bsp_usart.h"
#include "BSP\bsp_timer.h"
#include "BSP\bsp_key.h"
#include "BSP\bsp_beep.h"

#include "HARDWARE\MPU6050\mpu6050.h"
#include "HARDWARE\MPU6050\eMPL\inv_mpu.h"
#include "HARDWARE\MPU6050\eMPL\inv_mpu_dmp_motion_driver.h"
#include "HARDWARE\NRF24\nrf24l01.h"
#include "HARDWARE\OLED\oled12864.h"
#include "HARDWARE\MOTOR\a4950.h"

#include "SOFTWARE\vofa_p.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

/**************************电调相关类型*****************************************/
//电调任务参数
typedef struct
{
    uint8_t channel;    //pwm管道 见bsp_pwm.c中的Target_CCR[]数组
    uint8_t max_inc;     //单任务周期脉宽最大增量 -> us单位
    uint8_t cycle;      //任务执行周期 -> ms单位
    QueueHandle_t*queueAddr;   ///队列句柄的地址,用于电调任务接收控制信号
}ER_Type;

//电调控制类型
typedef struct
{
    uint8_t type;   //0:保留 1:脉宽 2:最大增量 3:任务周期
    uint16_t dat;
}ERctr_Type;

/**************************直流电机相关类型*****************************************/
//直流电机任务参数
typedef struct
{
    uint8_t a4950_id; //a4950_id 见HARDWARE/MOTOR/a4950.c
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
//舵机任务参数  -> 与电调任务参数一致
typedef ER_Type StreetMotor_Type;   //舵机任务参数

//舵机控制类型
typedef struct
{
    uint8_t type;       //0:保留 1:基于当前角度增加 2:单周期增量 3:设置目标角度 4:直接设置角度
    float angle;        //目标角度
    uint8_t width_inc;  //单周期脉宽增量
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


/**************************系统状态*****************************************/
//系统状态
typedef struct
{
    uint8_t oled_page;  //当前oled的页
    uint16_t nrf_signal; //nrf信号 0:正常 其他:信号丢失时长
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


#endif  //_USER_H_

