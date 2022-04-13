#ifndef _USART_HMI_H_
#define _USART_HMI_H_

#include <string.h>
#include "BSP\bsp_usart.h"

#define HMI_Msg_TxtHight    16  //定义串口屏消息框字高
#define HMI_Msg_BoxHight    64  //定义串口屏消息框高度

void HMI_Reset(void);
void HMI_ClearMsg(void);
void HMI_Msg(char*msg);
void HMI_SetNum(int num,uint8_t channel);

#endif  //_USART_HMI_H_

