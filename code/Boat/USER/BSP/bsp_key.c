#include "bsp_key.h"

static const Pin KEY_Pin[4] = {
    {GPIO_Pin_2,GPIOE},
    {GPIO_Pin_3,GPIOE},
    {GPIO_Pin_4,GPIOE},
    {GPIO_Pin_5,GPIOE}
};

void BSP_Key_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Pin = 0x0F<<2;     //Pin2 ~ Pin5
    
    GPIO_Init(GPIOE,&GPIO_InitStruct);
}

uint8_t Key_Read(uint8_t key)
{
    return Pin_Read(KEY_Pin[key]) ? Key_Press : Key_Release;
}
