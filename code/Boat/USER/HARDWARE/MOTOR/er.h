#ifndef _ER_H_
#define _ER_H_

#include "BSP\bsp_pwm.h"

/*******************************************************************
 * er.c
 * er.h
 * 功能:
 * 控制电调
 * 2022/3/19   庞碧璋
 *******************************************************************/

typedef struct
{
    uint8_t pwm_ch;     //pwm管道标号
    uint8_t dir;        //方向  0:默认 1:反向
    uint16_t median;    //中位
}er_Type;

void ER_Out(er_Type*er,int width);
int ER_ReadOut(er_Type*er);

#endif  //_ER_H_

