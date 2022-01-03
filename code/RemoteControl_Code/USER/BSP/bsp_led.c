#include "bsp_led.h"

void BSP_LED_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStruct;

    LED_CLK_Init();

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    
    for(uint8_t temp=0;temp<3;temp++)
    {
        GPIO_InitStruct.GPIO_Pin = LED_Pin[temp].Pin;
        GPIO_Init(LED_Pin[temp].GPIO,&GPIO_InitStruct);
        Pin_Set(LED_Pin[temp]);
    }
}

void LED_ON(uint8_t num)
{
    if(num < 3)
        Pin_Reset(LED_Pin[num]);
}

void LED_OFF(uint8_t num)
{
    if(num < 3)
        Pin_Set(LED_Pin[num]);
}

void LED_Reversal(uint8_t num)
{
    if(num < 3)
        Pin_Reversal(LED_Pin[num]);
}

