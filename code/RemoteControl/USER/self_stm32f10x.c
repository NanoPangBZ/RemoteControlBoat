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

void soft_delay_ms(unsigned int ms)
{
    while(ms!=0)
    {
        soft_delay_us(1000);
        ms--;
    }
}

void soft_delay_us(unsigned int us)
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
