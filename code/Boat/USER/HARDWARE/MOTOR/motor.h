#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "self_stm32f10x.h"

#include "BSP\bsp_pwm.h"

#define MAIN_MOTOR_L(us)    TIM5->CCR1 = us - 100
#define MAIN_MOTOR_R(us)    TIM5->CCR2 = us - 100
#define SEC_MOTOR_L(us)     TIM5->CCR3 = us - 100
#define SEC_MOTOR_R(us)     TIM5->CCR4 = us - 100
#define HEAD_MOTOR(us)      TIM5->CCR3 = us
#define TOP_MOTOR(us)       TIM5->CCR1 = us
#define STREET_MOTOR1(angle)    TIM5->CCR2 = (int) ( 1000 + (angle*5.556) )
#define STREET_MOTOR2(angle)    TIM5->CCR2 = (int) ( 1000 + (angle*5.556) )

void ElectricRegulation_Init(void); //电调初始化
void StreetMotor_Reset(void);       //舵机归为
void DC_Motor_Init(void);

#endif //_MOTOR_H_

