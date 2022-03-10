#ifndef _USER_H_
#define _USER_H_

#include "BSP\bsp_led.h"
#include "BSP\bsp_usart.h"

#include "HARDWARE\OLED\oled12864.h"
#include "HARDWARE\NRF24\nrf24l01.h"
#include "HARDWARE\MPU6050\mpu6050.h"
#include "HARDWARE\MPU6050\eMPL\inv_mpu.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

void RTOSCreateTask_Task(void*ptr);
void Gyroscope_Task(void*ptr);
void ReplyMaster_Task(void*ptr);
void nRF24L01_Intterrupt_Task(void*ptr);
void OLED_Task(void*ptr);
void User_FeedBack_Task(void*ptr);


#endif  //_USER_H_

