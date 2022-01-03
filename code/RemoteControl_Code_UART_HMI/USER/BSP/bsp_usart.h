#ifndef _BSP_USART_H_
#define _BSP_USART_H_

#include "stm32f10x.h"
#include <stdio.h>

/*************************************************
 * 基于stm32f103c8t6的串口支持包
 * 使用了官方固件库
 * printf()重定向至串口一(重定向没有使用DMA发送)
 * 使能了串口1,2,3
 * 发送使用了DMA发送,带DMA软件在忙标志位,由ISR清除
 * 
 * Create by: 庞碧璋
 * Github: https://github.com/CodingBugStd
 * csdn:   https://blog.csdn.net/RampagePBZ
 * Encoding: utf-8
 * create date: 2021/6/22
 * last date:   2021/10/1
*************************************************/

#define Rx_SbufferSize  72
#define Tx_SbufferSize  48

//每行第一个元素表示该缓存区存放的数据个数
static uint8_t USART_Rx_Sbuffer[2][Rx_SbufferSize + 1] = {{0},{0}};
static uint8_t USART_Tx_Sbuffer[2][Tx_SbufferSize + 1] = {{0},{0}};
//获取缓存区数据长度    只能在bsp_usart文件内使用! 缓存区对外不可见!
#define Tx_Len(USARTx)  USART_Tx_Sbuffer[USARTx-1][0]
#define Rx_Len(USARTx)  USART_Rx_Sbuffer[USARTx-1][0]
//获取最后一字节
#define Last_Byte(addr) *(addr+*addr)

static USART_TypeDef* Target_Usart[2] = {USART1,USART2};
static DMA_Channel_TypeDef* TargetDMA_Channel[2] = {DMA1_Channel4,DMA1_Channel7};

//初始化
void BSP_Usart_Init(void);
void USART_GPIO_Init(void);
void USART_Config(void);
void USART_NVIC_Config(void);
void USART_DMA_Config(void);

//发送&接收
uint8_t*Usart_Read(uint8_t USARTx);                             //读取串口接收缓存区,返回缓存区首地址,缓存区首地址为当前接收到的数据个数,不是数据!
uint8_t Usart_Send(uint8_t USARTx,uint8_t *dat,uint8_t len);    //串口DMA发送 USARTx(串口):1~3 *dat(数据首地址) len:数据长度
uint8_t Usart_SendString(uint8_t USARTx,uint8_t*dat);           //串口DMA发送字符串
uint8_t Usart_BusyCheck(uint8_t USARTx);                        //DMA在忙检查 0:free 1:busy
void USART_Clear(uint8_t USARTx);                               //清除串口接收缓存
void USART_Push(uint8_t USARTx,uint8_t len);                    //串口接收缓存前移len长度

//内部函数
void Rx_SbufferInput(uint8_t USARTx,uint8_t dat);   //将dat载入对应缓冲区
void Tx_Flag_Clear(uint8_t USARTx);                 //清除DMA软件在忙标志位

//printf()重定向
int fputc (int c, FILE *fp);

#endif
