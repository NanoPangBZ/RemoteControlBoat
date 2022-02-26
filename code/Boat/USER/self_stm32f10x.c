#include "self_stm32f10x.h"

void Pin_Set(Pin pin)
{
    pin.GPIO->BSRR |= pin.Pin;
}

void Pin_Reset(Pin pin)  
{
    pin.GPIO->BRR |= pin.Pin;
}

void Pin_Reversal(Pin pin)
{
    if(pin.GPIO->ODR & pin.Pin)
        pin.GPIO->ODR &= ~pin.Pin;
    else
        pin.GPIO->ODR |= pin.Pin;
}

void soft_delay_ms(volatile unsigned int ms)
{
    while(ms!=0)
    {
        soft_delay_us(1000);
        ms--;
    }
}

void soft_delay_us(volatile unsigned int us)
{
    unsigned short int temp;
    while(us!=0)
    {
        for(temp=16;temp!=0;temp--);
        us--;
    }
}

void MemCopy(const uint8_t*content,uint8_t*buf,uint8_t len)
{
    if(content != buf)  //防止重复
    for(uint8_t temp=0;temp<len;temp++)
        buf[temp] = content[temp];
}

void Byte_To_Float(uint32_t*byte,float*buf)
{
    *buf = *((float*)byte);
}

void Byte_To_Short(uint16_t*byte,short*buf)
{
    *buf = *((short*)byte);
}
