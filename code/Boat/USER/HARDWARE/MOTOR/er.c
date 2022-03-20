#include "er.h"

#define port_PWMOut(ch,width)   PWM_Out(ch,width)
#define port_PWMReadOut(ch)     PWM_Read(ch)

void ER_Out(er_Type*er,int out)
{
    port_PWMOut(er->pwm_ch,er->median + out);
}

//获取当前电调输出 -max ~ max
int ER_ReadOut(er_Type*er)
{
    return (int)port_PWMReadOut(er->pwm_ch) - er->median;
}
