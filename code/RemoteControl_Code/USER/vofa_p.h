#ifndef _VAFA_P_H_
#define _VAFA_P_H_

#include "stm32f10x.h"
#include "bsp_usart.h"

/*************************************************
 * vofa+上位机
 * 波形图数据监控
 * (JustFloat数据格式)
 * 
 * Creat by: 庞碧璋
 * Github: https://github.com/CodingBugStd
 * csdn:   https://blog.csdn.net/RampagePBZ
 * Encoding: utf-8
 * date:    2021/1/31
*************************************************/

//宏定义通道数量
#define DataNum	10	

#define port_Vofa_Send()	Usart_Send((uint8_t*)&Vofa_Sbuffer,sizeof(Frame))

typedef struct
{
	float Date[DataNum];
	unsigned char FramEnd[4];
}Frame;

static Frame Vofa_Sbuffer = {{0},{0x00,0x00,0x80,0x7f}};

void Vofa_Input(float data,unsigned char channel);
void Vofa_Send(void);

#endif
