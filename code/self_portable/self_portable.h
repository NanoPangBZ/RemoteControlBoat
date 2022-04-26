#ifndef _SELF_PORTABLE_H_
#define _SELF_PORTABLE_H_

#include "self_stm32f10x.h"

/*******************************************************************
 * self_portable.c/.h
 * 功能:
 * 用于遥控器和船之间的通讯
 * 2022/3/19   庞碧璋
 *******************************************************************/

/*************************遥控器发送,船只接收*******************************/

//直流电机单周期最大增量    4B
typedef struct
{
    uint16_t motor_1;
    uint16_t motor_2;
}DC_maxIncType;

//直流电机目标速度  4B
typedef struct
{
    short motor_1;
    short motor_2;
}DC_targetType;

//舵机单周期角度最大增量    12B
typedef struct
{
    float angle1_inc;
    float angle2_inc;
    float angle3_inc;
}SM_maxIncType;

//舵机目标角度
typedef struct  //13B
{
    uint8_t cmd;    //0:直接输出 1:周期增量靠近
    float angle1;
    float angle2;
    float angle3;
}SM_targetType;

//电调目标设置  //8B
typedef struct
{
    uint16_t main_l;
    uint16_t main_r;
    uint16_t sec_l;
    uint16_t sec_r;
}ER_targetType;

//电调PWM单周期最大增量 //2B
typedef struct
{
    uint8_t main;
    uint8_t sec;
}ER_maxIncType;

//航向角修正pid设置 //13B
typedef struct
{
    uint8_t k;  //缩放单位 0.01 (0~2.56)
    float p;
    float i;
    float d;
}Yaw_pidType;

//控制数据  13B
typedef union
{
    ER_maxIncType ER_maxInc;
    ER_targetType ER_target;
    SM_maxIncType SM_maxInc;
    SM_targetType SM_target;
    DC_maxIncType DC_maxInc;
    DC_targetType DC_target;
    Yaw_pidType   Yaw_pid;
}ControlDat;

//遥控器发送的数据类型
typedef struct      //20B
{
    uint8_t cmd;        //附加命令 0:保留 1:按rocker值执行 2:航向角闭环
    uint8_t replyType;  //请求遥控器返回的数据类型
    uint8_t rocker[4];  //摇杆值 ly lx ry rx       8B
    uint8_t switch_value;   //0~7 bit -> 直流电机1 直流电机2 航向角闭环 半自动巡航
}RemoteControl_Type;

/*************************船只发送,遥控器接收*******************************/
typedef struct   //
{
    float Voltage;
    float Gyroscope[3];
}BoatReply_Type;

#endif

