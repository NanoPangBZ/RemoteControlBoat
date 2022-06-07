#ifndef _HARDWARE_DEF_H_
#define _HARDWARE_DEF_H_

#include "HARDWARE\MOTOR\a4950.h"
#include "HARDWARE\MOTOR\street_motor.h"
#include "HARDWARE\MOTOR\er.h"

//hardware_def.h -> 硬件定义

//核心板与驱动板对齐连接 - 驱动板忘记写标号了!注意别连接错

/****************************
 * bsp_pwm.h中的PWM通道定义
static __IO uint16_t* PWM_Channel[12] = {
    &TIM1->CCR1,&TIM1->CCR2,&TIM1->CCR3,&TIM1->CCR4,
    &TIM4->CCR1,&TIM4->CCR2,&TIM4->CCR3,&TIM4->CCR4,
    &TIM8->CCR1,&TIM8->CCR2,&TIM8->CCR3,&TIM8->CCR4,
};
*****************************/

//a4950直流电机驱动芯片 X 2
static a4950_Type a4950[2] = { {0,1,0,3600} , {2,3,0,3600} };
//舵机 X 2
static streetMotor_Type streetMotor[2] = {{5,180,0},{6,180,0}};
//电调 X 4
static er_Type er[4] = {{8,0,1400},{9,1,1400},{10,0,1500},{11,0,1500}};

#endif

