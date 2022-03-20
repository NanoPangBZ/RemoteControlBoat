#include "street_motor.h"

#define STREET_MOTOR_NUM    4
#define PWM_Out_Port(Channel,Width)     PWM_Out(Channel,Width)  //PWM输出
#define PWM_Out_Read_Port(Channel)      PWM_Read(Channel)       //查看当前PWM输出

//舵机对应的pwm通道
uint8_t street_motor_pwm_ch[STREET_MOTOR_NUM] = {4,5,6,7};

void StreetMotor_Set(streetMotor_Type*motor,float angle)
{
    if(angle>0 && angle<180)
        PWM_Out_Port(motor->pwm_ch,AngleToWidth(angle));
}

void StreetMotor_SetWidth(streetMotor_Type*motor,uint16_t width)
{
    if(width>499 && width<2501)
    {
        PWM_Out_Port(motor->pwm_ch,width);
    }
}

unsigned int AngleToWidth(float angle)
{
    unsigned int temp;
    temp = (int)(11.111f*angle);
    temp += 500;
    return temp;
}


