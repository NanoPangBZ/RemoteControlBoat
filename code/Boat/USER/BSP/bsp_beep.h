#ifndef _BSP_BEEP_H_
#define _BSP_BEEP_H_

#include "self_stm32f10x.h"

#define BEEP_TIM    TIM2

#define Beep_OFF()  BEEP_TIM->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN)) 

extern uint16_t Mu_Fre[8];

void BSP_Beep_Init(void);
void Beep_ON(uint16_t fre);

#endif
