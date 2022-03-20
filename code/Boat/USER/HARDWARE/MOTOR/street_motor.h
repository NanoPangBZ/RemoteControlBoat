#ifndef _STREET_MOTOR_DRIVER_H_
#define _STREET_MOTOR_DRIVER_H_

/*********************************************************
 * PWM输出接口 PWM_Out_Port(Channel,Width)
 * Channel: 通道标号        unsigned char
 * Width:   脉宽(单位us)    unsigned short int
 * 无返回值
 * 接口要求:能在规定通道输出50Hz的PWM波,脉宽参数Width
 * 
 * 读取PWM输出接口  PWM_Out_Read_Port(Channel)
 * Channel:通道标号
 * 返回值:unsigned short int 类型,标识当前脉宽(us)
 * 
 * 注意:更改某通道脉宽时要等待上一周期结束
 *      2021/5/30   庞碧璋
*********************************************************/

#include "BSP\bsp_pwm.h"

typedef struct
{
    uint8_t pwm_ch;     //PWM的通道标号 见 BSP\bsp_pwm.c PWM_Channel[]数组
    uint8_t max_angle;
}streetMotor_Type;

void StreetMotor_Set(streetMotor_Type*motor,float angle);
void StreetMotor_SetWidth(streetMotor_Type*motor,uint16_t width);
unsigned int AngleToWidth(float angle);

#endif

