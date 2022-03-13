#ifndef _BSP_USART_H_
#define _BSP_USART_H_

#include "self_stm32f10x.h"
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

#define Rx_SbufferSize  32
#define Tx_SbufferSize  64

//初始化
void BSP_Usart_Init(void);
void USART_GPIO_Init(void);
void USART_Config(void);
void USART_NVIC_Config(void);
void USART_DMA_Config(void);

//发送&接收
uint8_t*Usart_Read(uint8_t USARTx);                             //读取串口接收缓存区,返回缓存区首地址,缓存区首地址为当前接收到的数据个数,不是数据!
uint8_t Usart_RxCopy(uint8_t USARTx,uint8_t*buf,uint8_t len);   //拷贝串口接收到的数据
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
