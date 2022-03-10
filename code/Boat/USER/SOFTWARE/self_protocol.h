#ifndef _SELF_PROTOCOL_H_
#define _SELF_PROTOCOL_H_

#include "stm32f10x.h"
#include "SOFTWARE\pid.h"

typedef struct
{
    uint8_t packID;     //包ID
    uint8_t dataType;
}UserPack_TypeDef;

typedef struct
{
    uint8_t packID;     //包ID
    uint8_t status;     //当前船状态
    short temperature;  //温度
    float Gyroscope[3]; //姿态
}BoatPack_TypeDef;

#endif //_SELF_PROTOCOL_H_

