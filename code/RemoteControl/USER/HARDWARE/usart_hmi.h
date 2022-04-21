#ifndef _USART_HMI_H_
#define _USART_HMI_H_

/*************************************************
 * 串口屏驱动
 * 
 * 作者: 庞碧璋
 * Github: https://github.com/CodingBugStd
 * csdn:   https://blog.csdn.net/RampagePBZ
 * Encoding: utf-8
 * date:   2021/3
*************************************************/

#include <string.h>
#include "BSP\bsp_usart.h"

typedef enum
{
    DecodeErr = 0,
    MainMotor_ON = 1,
    MainMotor_OFF = 2,
    SecMotor_ON = 3,
    SecMotor_OFF = 4
}HMI_DecodeType;

#define HMI_Msg_TxtHight    16  //定义串口屏消息框字高
#define HMI_Msg_BoxHight    64  //定义串口屏消息框高度

void HMI_SetSign(uint8_t sign);
void HMI_Reset(void);
void HMI_ClearMsg(void);
void HMI_Msg(char*msg);
void HMI_SetNum(int num,uint8_t channel);

#endif  //_USART_HMI_H_

