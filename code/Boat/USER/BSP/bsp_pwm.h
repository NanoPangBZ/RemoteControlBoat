#ifndef _BSP_TIM_H_
#define _BSP_TIM_H_

#include "self_stm32f10x.h"

/**************************************
 * stm32f103 pwm输出代码模板
 * 2021/5   作者:庞碧璋
**************************************/

void BSP_PWM_Init(void);
void PWM_Out(uint8_t Channel,uint16_t CCR);
uint16_t PWM_Read(uint8_t Channel);

#endif
