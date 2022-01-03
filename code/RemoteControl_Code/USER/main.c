#include "self_stm32f10x.h"
#include "bsp.h"

#include "oled12864.h"

uint32_t SysCount = 0;

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	//关闭JTAG调试接口,保留SWD调试接口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

	SysTick_Config(72000*5);	//5ms系统中断

	BSP_SPI_Init();
	BSP_Usart_Init();
	BSP_LED_Init();
	BSP_ADC_Init();

	OLED12864_Init();
						   
	OLED12864_Show_String(0,0,"HelloWorld!",1);
	OLED12864_Refresh();
	
	while(1)
	{
	}
}

void SysTick_Handler(void)
{
	for(uint8_t temp=0;temp<4;temp++)
	{
		OLED12864_Clear_PageBlock(temp,0,32);
		OLED12864_Show_fNum(temp,0,ADC_ReadVoltage(temp),1,3);
	}
	OLED12864_Show_Num(4,0,SysCount/200,1);
	OLED12864_Refresh();
	SysCount++;
}
