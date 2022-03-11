#ifndef _VAFA_P_H_
#define _VAFA_P_H_

//宏定义通道数量
#define DataNum	3	

#include "BSP\bsp_usart.h"

typedef struct
{
	float Date[DataNum];
	unsigned char FramEnd[4];
}Frame;

static Frame Vofa_Sbuffer = {{0},{0x00,0x00,0x80,0x7f}};

void Vofa_Input(float data,unsigned char channel);
void Vofa_Send(void);

#endif
