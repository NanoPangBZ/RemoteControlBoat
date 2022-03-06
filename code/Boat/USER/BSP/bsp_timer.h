#ifndef _BSP_TIMER_H_
#define _BSP_TIMER_H_

#include "self_stm32f10x.h"

/*******************************************************************
 * 用于测量代码执行花费的时间
 * 2022/3/7   庞碧璋
 *******************************************************************/

#define timingTimer TIM2

void BSP_Timer_Init(void);
void startTiming(void);
uint32_t stopTiming(void);

#endif  //_BSP_TIMER_H_

