#ifndef _A4950_H_
#define _A4950_H_

#include "BSP\bsp_pwm.h"

/*******************************************************************
 * a4950.c
 * a4950.h
 * 功能:
 * 驱动多块a4950,每块a4950能驱动一个直流电机。
 * 关于移植:
 * 需要定义a4950硬件,并且设置它的两个pwm通道和半最大脉宽
 * 需要对接port_PWMOut() -> 能根据PWM通道标号在对应的管脚输出PWM波
 * 需要对接port_PWMRead() -> 能根据PWM通道标号获得对应管脚PWM的当前输出
 * 需要设置最大PWM输出,a4950数量等
 * 2022/3/19   庞碧璋
 *******************************************************************/

typedef struct
{
    uint8_t pwm1_ch;        //a4950 PWM1的通道标号 见 BSP\bsp_pwm.c PWM_Channel[]数组
    uint8_t pwm2_ch;        //a4950 PWM2的通道标号 见 BSP\bsp_pwm.c PWM_Channel[]数组
    uint16_t half_max;      //PWM通道的最大脉宽的一半
}a4950_Type;

void A4950_Out(a4950_Type*a4950,int out);
int A4950_ReadOut(a4950_Type*a4950);
void A4950_Brake(a4950_Type*a4950);

#endif

