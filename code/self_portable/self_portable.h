#ifndef _SELF_PORTABLE_H_
#define _SELF_PORTABLE_H_

#include "self_stm32f10x.h"

/*******************************************************************
 * self_portable.h
 * 功能:
 * 用于遥控器和船之间的通讯
 * 2022/3/19   庞碧璋
 *******************************************************************/

#define DCMotor1_Mask 0x01
#define DCMotor2_Mask 0x02
#define ZeroWaterLine_Mask  0x04
#define Grab_Mask     0x08

/*************************遥控器发送,船只接收*******************************/
//遥控器发送的数据类型
typedef struct      //32B
{
    uint8_t switch_value;   //0~7 bit -> 直流电机1 直流电机2 航向角闭环 半自动巡航
    uint8_t msg_id;     //0->无消息
    uint8_t rocker[4];  //摇杆值 ly lx ry rx       8B
}RemoteControl_Type;

/*************************船只发送,遥控器接收*******************************/
typedef struct  //32B
{
    float Voltage;
    float Depth;
    float Gyroscope[3];
    uint8_t o_msg[11];
}BoatReply_Type;

#endif
