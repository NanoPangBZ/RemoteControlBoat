#include "vofa_p.h"
#include ".\BSP\bsp_usart.h"

#define port_Send(dat,len)	Usart_Send(1,dat,len)

void Vofa_Input(float data,unsigned char channel)
{
	if(channel < DataNum)
		Vofa_Sbuffer.Date[channel] = data;
}

void Vofa_Send(void)
{
	port_Send((uint8_t*)&Vofa_Sbuffer,sizeof(Frame));
}

