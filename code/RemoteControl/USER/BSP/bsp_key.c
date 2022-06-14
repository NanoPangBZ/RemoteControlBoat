#include "bsp_key.h"

static const Pin KEY_Pin[4] = {
    {GPIO_Pin_3,GPIOB},
    {GPIO_Pin_4,GPIOB},
    {GPIO_Pin_5,GPIOB},
    {GPIO_Pin_6,GPIOB}
};

void BSP_Key_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Pin = 0x0F<<3;     //Pin3 ~ Pin6
    
    GPIO_Init(GPIOB,&GPIO_InitStruct);
}

uint8_t Key_Read_All(void)
{
    for(uint8_t temp=0;temp<4;temp++)
    {
        if(Key_Read(temp) == Key_Press)
            return temp;
    }
    return 0xff;
}

uint8_t Key_Read(uint8_t key)
{
    if(Pin_Read(KEY_Pin[key]))
        return 0;
    else
        return 1;
}
