#ifndef _BSP_KEY_H_
#define _BSP_KEY_H_

#include "self_stm32f10x.h"

#define Key_Release 0
#define Key_Press   1

void BSP_Key_Init(void);
uint8_t Key_Read(uint8_t key);
uint8_t Key_Read_All(void);

#endif  //_BSP_KEY_H_

