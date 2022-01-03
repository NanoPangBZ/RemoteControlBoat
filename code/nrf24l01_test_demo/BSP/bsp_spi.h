#ifndef _BSP_SPI_H_
#define _BSP_SPI_H_

#include "stm32f10x.h"
#include "self_stm32f10x.h"

/***********************************************************************************
 * 基于stm32f103c8t6的SPI支持包
 * 使用了官方固件库
 * 硬件SPI
 * 
 * SPI1 SPI2
 * SPI1 -> nRF24 主机模式,全双工
 * SPI2 -> OLED12864 主机模式,单向发送(主->从)
 * 
 * 备注:SPI_Send_Byte()不会等待SPI接收完成,如果波特率太小,使用SPI_Send_Byte()或
 * SPI_Send_NumByte()连续发送时会导致SPIx->DR寄存器还没接收完从机的最后一位然后
 * 被赋值产生错误!
 * 
 * 2021/10/17: 因为使用nRF24L01时需要先初始化其它非SPi引脚并且配置电平后再进行SPI的硬件
 *             初始化,否则会导致nRF24的时序混乱,所以SPI1单独从BSP分离到nrf24l01初始化
 * 
 * Create by: 庞碧璋
 * Github: https://github.com/CodingBugStd
 * csdn:   https://blog.csdn.net/RampagePBZ
 * Encoding: utf-8
 * create date: 2021/10/12
 * last date:   2021/10/17
**********************************************************************************/

#define SPI_Pin_Count    5
#define SPI_Pin_CLK_Init()  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); \
                            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE)
#define SPI_CLK_Init()      RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE)               
#if 0
#define SPI_CLK_Init()      RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE); \
                            RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE)
#endif

static SPI_TypeDef* Target_SPI[2] = {SPI1,SPI2};

static const Pin SPI_Pin[SPI_Pin_Count] = {
    {GPIO_Pin_5,GPIOA},{GPIO_Pin_6,GPIOA},{GPIO_Pin_7,GPIOA},
    {GPIO_Pin_13,GPIOB},{GPIO_Pin_15,GPIOB}};

void BSP_SPI_Init(void);
void SPI_Config(void);
void SPI_GPIO_Config(void);

//发送和接收
uint8_t SPI_Replace_Byte(uint8_t SPIx,uint8_t TxData);                  //直接使用SPI交换一个字节
uint8_t SPI_Send_Byte(uint8_t SPIx,uint8_t TxData);                     //直接使用SPI发送一个字节
void SPI_Replace_NumByte(uint8_t SPIx,uint8_t*dat,uint8_t len,uint8_t*buf);      //使用SPI交换len个字节
void SPI_Send_NumByte(uint8_t SPIx,uint8_t*dat,uint8_t len);                 //使用SPI发送len个字节,只保留最后交换的字节
void SPI_Recieve_NumByte(uint8_t SPIx,uint8_t dat,uint8_t len,uint8_t*buf);      //使用SPI接收len个字节,使用dat做为交换字节

#endif
