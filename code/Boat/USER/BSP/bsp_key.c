#include "bsp_key.h"

void BSP_Key_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    for(uint8_t temp=0;temp<4;temp++)
    {
        GPIO_InitStruct.GPIO_Pin = KEY_Pin[temp].Pin;
        GPIO_Init(KEY_Pin[temp].GPIO,&GPIO_InitStruct);
    }
    PinIn(1);
}

