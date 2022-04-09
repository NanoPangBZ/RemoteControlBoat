#include "er.h"

#define port_PWMOut(ch,width)   PWM_Out(ch,width)
#define port_PWMReadOut(ch)     PWM_Read(ch)

//电调输出 单向电调 er->median = 1000 out取值 0 ~ 1000
//双向电调 er->median = 1500 out取值 -500 ~ 500
//中位out为0 适用双向油门
void ER_Out(er_Type*er,int out)
{
    uint16_t pwmout;
    if(er->dir)
        pwmout = er->median + out;
    else
        pwmout = er->median - out;
    //pwm脉宽限位
    if(pwmout > 2000)
        pwmout = 2000;
    if(pwmout < 1000)
        pwmout = 1000;
    port_PWMOut(er->pwm_ch,pwmout);
}

//获取当前电调输出 -500 ~ +500
int ER_ReadOut(er_Type*er)
{
    if(er->dir)
        return (int)port_PWMReadOut(er->pwm_ch) - er->median;
    else
        return (int)er->median - port_PWMReadOut(er->pwm_ch);
}
