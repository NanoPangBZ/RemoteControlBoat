#ifndef _NRF24L01_H_
#define _NRF24L01_H_

#include "stm32f10x.h"
#include <stdio.h>
#include "self_stm32f10x.h"

/*****************************************************************************
 ** 改自:魔女开发板团队
 ** 二改:庞碧璋
 ** 注意事项: 依赖self_stm32f10x.h 加入了Pin类型方便引脚初始化
 **          移植时注意!IQR中断需要重新编写
 ** date:2021/10/17
****************************************************************************/
// spi_port
#define  NRF24L01_SPIx                SPI1                                 // SPI 端口   
// EN clock
#define  NRF24L01_SPI_EN_CLOCK        RCC->APB2ENR |= RCC_APB2ENR_SPI1EN  // SPI时钟

// 第3脚:CE
#define  NRF24L01_CE_GPIO             GPIOB                 // CE 工作模式开启， 高电平有效，发射/接收模式控制
#define  NRF24L01_CE_PIN              GPIO_Pin_1
// 第4脚:CSN
#define  NRF24L01_CSN_GPIO            GPIOA                 // SPI 软件片选线
#define  NRF24L01_CSN_PIN             GPIO_Pin_4
// 第5脚:SCK
#define  NRF24L01_SCK_GPIO            GPIOA                 // SPI SCK
#define  NRF24L01_SCK_PIN             GPIO_Pin_5
// 第65脚:MOSI
#define  NRF24L01_MOSI_GPIO           GPIOA                 // SPI MOSI
#define  NRF24L01_MOSI_PIN            GPIO_Pin_7
// 第7脚:MISO
#define  NRF24L01_MISO_GPIO           GPIOA                 // SPI MISO
#define  NRF24L01_MISO_PIN            GPIO_Pin_6
// 第8脚:IRQ　　　　　　　　　　　　　　　　　　　　　　　　// 闲时高电平，中断时低电平
#define  NRF24L01_IRQ_GPIO            GPIOB                 // IRQ 中断信号， 低电平有效，中断输出
#define  NRF24L01_IRQ_PIN             GPIO_Pin_0
#define  NRF24L01_IRQ_IRQn            EXTI0_IRQn            // 中断号
#define  NRF24L01_IRQ_IRQHANDLER      EXTI0_IRQHandler      // 中断函数

//对接self_stm32f10x.h
#define NRF24L01_CE     0
#define NRF24L01_CSN    1
#define NRF24L01_MOSI   2
#define NRF24L01_MISO   3
#define NRF24L01_IRQ    4
static const Pin nRF24L01_Pin[5] = 
{
    {NRF24L01_CE_PIN,NRF24L01_CE_GPIO},
    {NRF24L01_CSN_PIN,NRF24L01_CSN_GPIO},
    {NRF24L01_MOSI_PIN,NRF24L01_MOSI_GPIO},
    {NRF24L01_MISO_PIN,NRF24L01_MISO_GPIO},
    {NRF24L01_IRQ_PIN,NRF24L01_IRQ_GPIO},
};

/*****************************************************************************
 ** 声明  全局函数
 ** 数量：5个
****************************************************************************/
uint8_t  NRF24L01_Init(void);              // NRF24L01初始化, 并进入接收模式

uint8_t nRF24L01_Read_Status(void);

#endif
