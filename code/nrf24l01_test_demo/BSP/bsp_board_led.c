#include "bsp_board_led.h"

void BSP_Board_LED_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	GPIOC->ODR |= GPIO_Pin_13;
}

void Board_LED_Reversal(void)
{
	if(GPIOC->ODR & GPIO_Pin_13)
		GPIOC->ODR &= ~GPIO_Pin_13;
	else
		GPIOC->ODR |= GPIO_Pin_13;
}
