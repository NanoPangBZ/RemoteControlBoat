#ifndef _NRF24L01_H_
#define _NRF24L01_H_

#include "self_stm32f10x.h"
#include ".\BSP\bsp_usart.h"     

/*************************************************
 * nrf24l01的驱动 (只支持静态接收字节长度)
 * 需要底层SPI支持
 * 
 * 移植注意事项:
 * 移植需要重写以下函数和定义以下的宏
 * spiInit() --- 内部函数,spi初始化
 * nRF24L01_Init() --- nrf24l01硬件的初始化函数
 * port_Send(dat) --- SPI发送函数宏
 * port_delay_ms(ms) --- ms延时函数宏
 * ---nrf24引脚控制宏---
 * CE_LOW
 * CE_HIGH
 * CS_LOW
 * CS_HIGH
 * ---中断处理函数(已经写了一部分)---
 * nRF24L01_InterruptHandle()
 * 应该在发生外部中断时调用这个函数,不一定要在ISR中调用
 * 在外部应该定义以下函数->这些函数会被nRF24L01_InterruptHandle()调用
 * nRF24L01_NoACK_ISR(void);
 * nRF24L01_Tx_ISR(void);
 * nRF24L01_Rx_ISR(void);
 * 还需要MemCopy()函数支持!!(内存拷贝)
 * 
 * 若有其它设备挂在在同一spi上,在由其它设备更换至nrf24时要重新
 * 调用spi_init() <- 内部函数
 * 
 * 作者: 庞碧璋
 * Github: https://github.com/CodingBugStd
 * csdn:   https://blog.csdn.net/RampagePBZ
 * Encoding: utf-8
 * 最后更改时间: 2022/2/14
*************************************************/

// spi_port
#define  NRF24L01_SPIx                SPI2                  // SPI 端口   

// 第3脚:CE
#define  NRF24L01_CE_GPIO             GPIOB                 // CE 工作模式开启， 高电平有效，发射/接收模式控制
#define  NRF24L01_CE_PIN              GPIO_Pin_12
// 第4脚:CSN
#define  NRF24L01_CSN_GPIO            GPIOD                 // SPI 软件片选线
#define  NRF24L01_CSN_PIN             GPIO_Pin_8
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
#define  NRF24L01_IQR_GPIO           GPIOD                 // SPI IRQ
#define  NRF24L01_IQR_PIN            GPIO_Pin_9
#define  NRF24L01_IQR_Channel        EXTI9_5_IRQn
#define  NRF24L01_IQR_SourceGPIO    GPIO_PortSourceGPIOD
#define  NRF24L01_IQR_PinSource     GPIO_PinSource9
#define  NRF24L01_IQR_Line          EXTI_Line9

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

//相关配置
//nRF24初始化的默认配置
#define DEFAULT_TxAddr  "USER"
#define DEFAULT_RxAddr  "BOAT"
#define DEFAULT_Channel 0       //2400MHz频段
#define DEFAULT_RETRY   2       //最大自动重发次数
#define DEFAULT_RETRY_CYCLE 1   //重发间隔 单位:250us
#define DEFAULT_Rx_Length   32  //默认StaticPayload长度
//其它配置
#define NRF24_BUF_MAXLEN    64  //缓存区长度,NRF24_USE_BUF_LEN!=0 时才有用
#define NRF24_USE_BUF_LEN   0   //非0:缓存区第一字节用于表示缓存区长度,使能这个宏可以缓存nrf24的多次接收
#define NRF24_USE_SBUFFER   1   //非0:接收中断时将RxFIFO的值载入内部缓存nRF24L01_Sbuffer中,若NRF24_USE_BUF_LEN=0则会覆盖上次接收!

#if NRF24_USE_BUF_LEN
//缓存区第一字节表示缓存区当前长度
static uint8_t nRF24L01_Sbuffer[NRF24_BUF_MAXLEN+1] = {0};
#elif NRF24_USE_SBUFFER
static uint8_t nRF24L01_Sbuffer[32] = {0};
#endif

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
uint8_t nRF24L01_Rx_Mode(void);
#if NRF24_USE_SBUFFER
uint8_t nRF24L01_Read_RxSbuffer(uint8_t*buf,uint8_t len);
uint8_t*nRF24L01_Get_RxBufAddr(void);
uint8_t*nRF24L01_FIFO_To_Sbuffer(void);
#if NRF24_USE_BUF_LEN
uint8_t nRF24L01_Read_SbufferLen(void);
void nRF24L01_Push_Sbuffer(uint8_t len);
void nRF24L01_Clear_Sbuffer(void);
#endif  //NRF24_USE_BUF_LEN
#endif //NRF24_USE_SBUFFER

void nRF24L01_InterruptHandle(void);
void Rx_Handler(void);   //接收中断
void NoACK_Handle(void); //未应答中断
void Tx_Handle(void);    //发送完成中断

#endif

