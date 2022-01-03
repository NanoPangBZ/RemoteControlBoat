#include "vofa_p.h"

void Vofa_Input(float data,unsigned char channel)
{
	if(channel < DataNum)
		Vofa_Sbuffer.Date[channel] = data;
}

void Vofa_Send(void)
{
	port_Vofa_Send();
}

