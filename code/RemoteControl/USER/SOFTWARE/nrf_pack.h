#ifndef _NRF_PACK_H_
#define _NRF_PACK_H_

#include ".\HARDWARE\nrf24l01.h"

//30Byte
typedef struct
{
    uint8_t Cmd;
    uint8_t CmdType;        //0:手动模式 1:自动巡航模式
    uint16_t PackID;        //数据包编号 用于应答确认
    uint8_t PackData[26];   //有效数据
}Remote_PackType;

typedef struct
{
    float P;
    float I;
    float D;
}PID_DataPack;




#endif //_NRF_PACK_H_

