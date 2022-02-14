#ifndef _REMOTE_H_
#define _REMOTE_H_

#include "self_stm32f10x.h"

typedef struct
{
    uint8_t ControlType;    //0000 0000
    float TargetSpeed;
    float TargetYaw;
}Send_PackType;

typedef struct
{
    uint8_t ReturnType;
    float ActualSpeed;
}Return_PackType;



#endif //_REMOTE_H_

