#ifndef _SELF_STM32F10X_H_
#define _SELF_STM32F10X_H_

#include "stm32f10x.h"

/**************************************************************************************
 * stm32f10x的相关类型定义
 * 方便快速连续初始化引脚 和 直接操作引脚 
 * 软件延时
 * 
 * Creat by: 庞碧璋
 * Github: https://github.com/CodingBugStd
 * csdn:   https://blog.csdn.net/RampagePBZ
 * Encoding: utf-8
 * date:    2021/8/22
 * last date: 2021/10/16
**************************************************************************************/

//用于快速初始化IO口以及操作IO口
typedef struct
{
    uint16_t Pin;           //Pin的标号
    GPIO_TypeDef*GPIO;      //所属的GPIO端口
}Pin;

//以下宏的pin应当为Pin结构体
#define Pin_Set(pin)    pin.GPIO->ODR |= pin.Pin
#define Pin_Reset(pin)  pin.GPIO->ODR &= ~pin.Pin
#define Pin_Read(pin)   (pin.GPIO->IDR & pin.Pin)?1:0

void Pin_Reversal(Pin pin);
void soft_delay_ms(uint16_t ms);
void soft_delay_us(uint16_t us);
void MemCopy(const uint8_t*content,uint8_t*buf,uint8_t len);
void MemFill(uint8_t*buf,uint8_t fillDat,uint8_t len);

#if 0
void startDebugTiming(void);         //开始计时
uint16_t endDebugTiming(void);       //结束计时 返回计时时长 us
#endif

#endif
