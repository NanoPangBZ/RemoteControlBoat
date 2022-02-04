#ifndef _NRF24L01_H_
#define _NRF24L01_H_

#include "self_stm32f10x.h"
#include ".\BSP\bsp_usart.h"     

#define DEFAULT_TxAddr  "USER"
#define DEFAULT_RxAddr  "BOAT"
#define DEFAULT_Channel 0       //2400MHz频段
#define DEFAULT_RETRY   2       //最大自动重发次数
#define DEFAULT_RETRY_CYCLE 1   //重发间隔 单位:250us
#define DEFAULT_Rx_Length   32  //StaticPayload长度
#define nRF24L01_SbufferSize    64

// spi_port
#define  NRF24L01_SPIx                SPI2                  // SPI 端口   

// 第3脚:CE
#define  NRF24L01_CE_GPIO             GPIOB                 // CE 工作模式开启， 高电平有效，发射/接收模式控制
#define  NRF24L01_CE_PIN              GPIO_Pin_11
// 第4脚:CSN
#define  NRF24L01_CSN_GPIO            GPIOB                 // SPI 软件片选线
#define  NRF24L01_CSN_PIN             GPIO_Pin_12
// 第5脚:SCK
#define  NRF24L01_SCK_GPIO            GPIOB                 // SPI SCK
#define  NRF24L01_SCK_PIN             GPIO_Pin_13
// 第65脚:MOSI
#define  NRF24L01_MOSI_GPIO           GPIOB                 // SPI MOSI
#define  NRF24L01_MOSI_PIN            GPIO_Pin_15
// 第7脚:MISO
#define  NRF24L01_MISO_GPIO           GPIOB                 // SPI MISO
#define  NRF24L01_MISO_PIN            GPIO_Pin_14
// 第8脚:IRQ
#define  NRF24L01_IQR_GPIO           GPIOA                 // SPI MISO
#define  NRF24L01_IQR_PIN            GPIO_Pin_8
#define  NRF24L01_IQR_Channel        EXTI9_5_IRQn
#define  NRF24L01_IQR_SourceGPIO    GPIO_PortSourceGPIOA
#define  NRF24L01_IQR_PinSource     GPIO_PinSource8
#define  NRF24L01_IQR_Line          EXTI_Line8

#define NRF24L01_CE     0
#define NRF24L01_CS     1
#define NRF24L01_MOSI   2
#define NRF24L01_MISO   3
#define NRF24L01_SCK    4

static const Pin nRF24L01_PIN[5] = {
    {NRF24L01_CE_PIN,NRF24L01_CE_GPIO},
    {NRF24L01_CSN_PIN,NRF24L01_CSN_GPIO},
    {NRF24L01_MOSI_PIN,NRF24L01_MOSI_GPIO},
    {NRF24L01_MISO_PIN,NRF24L01_MISO_GPIO},
    {NRF24L01_SCK_PIN,NRF24L01_SCK_GPIO}
};

//第一个元素为有效长度
static uint8_t nRF24L01_Sbuffer[nRF24L01_SbufferSize+1] = {0};

typedef struct
{
    uint8_t Channel;    //频道  0~125 -> 1MHz + 2.4G
    uint8_t retry;      //自动重发次数 最大16
    uint8_t retry_cycle;//自动重发时间间隔 250us单位 最大16(4000us)
    uint8_t Rx_Length;  //静态接收长度
    uint8_t RX_Addr[5]; //接收地址    ->  发射方地址
    uint8_t TX_Addr[5]; //发射方地址  ->  目标接收地址
}nRF24L01_Cfg;

static nRF24L01_Cfg CurrentCfg;     //当前nRF24L01的配置

//底层函数

uint8_t nRF24L01_Send_Cmd(uint8_t cmd);
uint8_t nRF24L01_Send_CmdAndData(uint8_t cmd,uint8_t*buf,uint8_t len);
uint8_t nRF24L01_Read_Reg(uint8_t addr);
uint8_t nRF24L01_Read_Buf(uint8_t addr,uint8_t*buf,uint8_t len);
uint8_t nRF24L01_Write_Reg(uint8_t addr,uint8_t dat);
uint8_t nRF24L01_Write_Buf(uint8_t addr,uint8_t*buf,uint8_t len);

//用户函数

uint8_t nRF24L01_Init(void);
uint8_t nRF24L01_Check(void);
uint8_t nRF24L01_Status(void);
uint8_t nRF24L01_Config(nRF24L01_Cfg*Cfg);
uint8_t nRF24L01_Send(uint8_t*buf,uint8_t len);
uint8_t nRF24L01_Read_RxFIFO(uint8_t*buf);
uint8_t*nRF24L01_FIFO_To_Sbuffer(void);
uint8_t nRF24L01_Read_RxSbuffer(uint8_t*buf,uint8_t len);
uint8_t nRF24L01_Read_SbufferLen(void);
void nRF24L01_Clear_Sbuffer(void);
void nRF24L01_Push_Sbuffer(uint8_t len);
uint8_t nRF24L01_Rx_Mode(void);

void nRF24L01_InterruptHandle(void);
void Rx_Handler(void);   //接收中断
void NoACK_Handle(void); //未应答中断
void Tx_Handle(void);    //发送完成中断



#endif

