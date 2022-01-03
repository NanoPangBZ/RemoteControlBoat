#include "bsp_key.h"

static void Key_GPIO_Init(void);
static void Key_EXTI_Init(void);

void BSP_Key_Init(void)
{
    Key_GPIO_Init();
    Key_EXTI_Init();
}

void Key_GPIO_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStruct;

    KEY_GPIO_CLK_Init();

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    for(uint8_t temp = 0 ; temp<8 ; temp++)
    {
        GPIO_InitStruct.GPIO_Pin = Key_Pin[temp].Pin;
        GPIO_Init(Key_Pin[temp].GPIO,&GPIO_InitStruct);
    }
}

void Key_EXTI_Init(void)
{
    NVIC_InitTypeDef    NVIC_InitStruct;

    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

    for(uint8_t temp=0;temp<8;temp++)
    {
        NVIC_InitStruct.NVIC_IRQChannel = 0;
        NVIC_Init(&NVIC_InitStruct);
        GPIO_EXTILineConfig(0,0);
    }
}

uint8_t Key_Read(void)
{
    uint8_t re = 0;

    for(uint8_t temp=0;temp<8;temp++)
    {
        re |= Key_Pin[7-temp].GPIO->IDR;
        re = re<<1;
    }
    return re;
}
