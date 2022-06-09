#ifndef _BSP_KEY_H_
#define _BSP_KEY_H_

#include "self_stm32f10x.h"

#define Key_Release 0
#define Key_Press   1

typedef enum
{
    EMG = 0,
    SIG,
    Updat,
    SRST
}Key;

void BSP_Key_Init(void);
uint8_t Key_Read(Key key);

#endif  //_BSP_KEY_H_

