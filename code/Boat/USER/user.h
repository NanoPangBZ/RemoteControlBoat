#ifndef _USER_H_
#define _USER_H_

#include "BSP\bsp_spi.h"
#include "BSP\bsp_pwm.h"
#include "BSP\bsp_led.h"
#include "BSP\bsp_usart.h"
#include "BSP\bsp_timer.h"
#include "BSP\bsp_key.h"

#include "HARDWARE\MPU6050\mpu6050.h"
#include "HARDWARE\MPU6050\eMPL\inv_mpu.h"
#include "HARDWARE\MPU6050\eMPL\inv_mpu_dmp_motion_driver.h"
#include "HARDWARE\NRF24\nrf24l01.h"
#include "HARDWARE\OLED\oled12864.h"

#include "SOFTWARE\vofa_p.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

//系统状态
typedef struct
{
    uint8_t oled_page;  //当前oled的页
    uint16_t nrf_signal; //nrf信号 0:正常 其他:信号丢失时长
}sysStatus_Type;

//电调任务初始化参数
typedef struct
{
    uint8_t channel;    //pwm管道 见bsp_pwm.c中的Target_CCR[]数组
    uint8_t MaxInc;     //单任务周期脉宽最大增量 -> us单位
    uint8_t cycle;      //任务执行周期 -> ms单位
    QueueHandle_t*recieveCmd;   //命令控制队列地址,用于电调任务接收控制信号
}ER_Type;

//电调控制类型
typedef struct
{
    uint8_t type;   //0:保留 1:脉宽 2:增量 3:任务周期
    uint16_t dat;
}ERctr_Type;

void MPU_Task(void*ptr);
void ReplyMaster_Task(void*ptr);
void nRF24L01_Intterrupt_Task(void*ptr);
void OLED_Task(void*ptr);
void KeyInput_Task(void*ptr);
void ER_Task(void*ptr);

#endif  //_USER_H_

