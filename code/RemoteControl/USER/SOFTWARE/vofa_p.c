#include "vofa_p.h"

void Vofa_Input(float data,unsigned char channel)
{
	if(channel < DataNum)
		Vofa_Sbuffer.Date[channel] = data;
}

void Vofa_Send(void)
{
	Usart_Send(1,(uint8_t*)&Vofa_Sbuffer,sizeof(Frame));
}

