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

typedef struct
{
    uint8_t oled_page;  //当前oled的页
    uint16_t nrf_signal; //nrf信号 0:正常 其他:信号丢失时长
}sysStatus_Type;

void RTOSCreateTask_Task(void*ptr);
void MPU_Task(void*ptr);
void ReplyMaster_Task(void*ptr);
void nRF24L01_Intterrupt_Task(void*ptr);
void OLED_Task(void*ptr);
void KeyInput_Task(void*ptr);


#endif  //_USER_H_

