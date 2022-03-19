#ifndef _SELF_PORTABLE_H_
#define _SELF_PORTABLE_H_

#include "self_stm32f10x.h"

typedef union
{
    uint8_t a;
}Control_Cmd;

typedef union
{
    float a;
}Control_para;

typedef struct
{
    uint8_t type;   //0:保留 1:遥控指令
    uint8_t len;
    Control_Cmd cmd;
}Remote_Pack;




#endif

