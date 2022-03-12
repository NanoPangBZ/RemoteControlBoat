#ifndef _BSP_KEY_H_
#define _BSP_KEY_H_

#include "self_stm32f10x.h"

#define Key_Press   0
#define Key_Release 1
#define Key1
#define Key2
#define Key3
#define Key4

const Pin KEY_Pin[4] = {
    {GPIOA,GPIO_Pin_0},
    {GPIOA,GPIO_Pin_0},
    {GPIOA,GPIO_Pin_0},
    {GPIOA,GPIO_Pin_0}
};

#define Key_Input(key)  Pin_Read(KEY_Pin[key]) ? Key_Press : Key_Release

void BSP_Key_Init(void);


#endif  //_BSP_KEY_H_

