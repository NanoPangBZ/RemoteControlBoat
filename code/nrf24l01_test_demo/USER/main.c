#include "stm32f10x.h"

#include "bsp_usart.h"
#include "bsp_board_led.h"
#include "bsp_spi.h"

#include "vofa_p.h"
#include "oled12864.h"
#include "nrf24l01.h"

#include <stdio.h>
#include <math.h>

/*************************************************
 * RemoteControl
 * 2022机械创新大赛 遥控器软件OS基础BSP部分
 * 使用了freeRTOS
 * RTOS心跳		1	KHz
 * 系统频率		72	MHz
 * AHB			72	MHz
 * APB2			72	MHz
 * APB1			36	MHz
 * RTOS堆栈大小	5	KByte
 * RTOS栈大小	128	Byte
 * MCU堆栈大小	0.5	KByte
 * MCU栈大小	1	KByte
 * 
 * startcode中的汇编代码中断入口有更改!
 * 部分被更改到了freeRTOS的接口API上!
 * xPortPendSVHandler()
 * xPortSysTickHandler()
 * vPortSVCHandler()
 * 
 * 基础模块:
 * 	Usart
 * 	HardSPI
 * 	BoardLED
 * 	OLED12864(Soft_4pin_SPI)
 * 	Vofa_Plus
 * 	soft_delay
 * 	MsgProtocol
 * 
 * Create by: 庞碧璋
 * Github: https://github.com/CodingBugStd
 * csdn:   https://blog.csdn.net/RampagePBZ
 * Encoding:	utf-8
 * CreateDate:	2021/10/1
 * LastDate:	2021/10/1
*************************************************/

uint8_t testName[] = "nRF24L01 Repalce Test";

int main(void)
{
	//uint8_t test[5] = {0xee,0xae,0x16,0x13,0xf2};
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	SystemCoreClockUpdate();	//更新系统频率全局变量

	//将APB1时钟总线配置为36MHz
	RCC_PCLK1Config(RCC_HCLK_Div2);
	
	BSP_Usart_Init();
	BSP_SPI_Init();
	BSP_Board_LED_Init();	

	NRF24L01_Init();
	OLED12864_Init();

	OLED12864_Show_String(0,0,testName,1);
	OLED12864_Refresh();

	uint16_t temp = 0;

	while(1)
	{
		OLED12864_Clear_Page(1);
		OLED12864_Clear_Page(2);
		OLED12864_Show_Num(1,0,temp++,1);
		OLED12864_Show_Num(2,0,nRF24L01_Read_Status(),1);
		Board_LED_Reversal();
		OLED12864_Refresh();
		soft_delay_ms(500);
	}
}


