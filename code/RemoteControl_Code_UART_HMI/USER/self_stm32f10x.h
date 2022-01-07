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

typedef struct
{
    uint16_t Pin;
    GPIO_TypeDef*GPIO;
}Pin;

void Pin_Set(Pin pin);
void Pin_Reset(Pin pin);
void Pin_Reversal(Pin pin);
void soft_delay_ms(unsigned int ms);
void soft_delay_us(unsigned int us);

void MemCopy(const uint8_t*content,uint8_t*buf,uint8_t len);
void Byte_To_Float(uint32_t*byte,float*buf);
void Byte_To_Short(uint16_t*byte,short*buf);

#endif
