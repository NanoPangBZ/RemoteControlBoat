#include "street_motor.h"

#define STREET_MOTOR_NUM    4
#define PWM_Out_Port(Channel,Width)     PWM_Out(Channel,Width)  //PWM输出
#define PWM_Out_Read_Port(Channel)      PWM_Read(Channel)       //查看当前PWM输出

//舵机对应的pwm通道
uint8_t street_motor_pwm_ch[STREET_MOTOR_NUM] = {4,5,6,7};

void StreetMotor_Set(unsigned char motor_id,float angle)
{
    if(motor_id < STREET_MOTOR_NUM && angle>0 && angle<180)
        PWM_Out_Port(street_motor_pwm_ch[motor_id],AngleToWidth(angle));
}

void StreetMotor_SetWidth(unsigned char motor_id,uint16_t width)
{
    if(motor_id < STREET_MOTOR_NUM && width>499 && width<2501)
    {
        PWM_Out_Port(street_motor_pwm_ch[motor_id],width);
    }
}

unsigned int AngleToWidth(float angle)
{
    unsigned int temp;
    temp = (int)(11.111f*angle);
    temp += 500;
    return temp;
}


