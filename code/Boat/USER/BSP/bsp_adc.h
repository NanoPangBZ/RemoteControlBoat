#ifndef _BSP_ADC_H_
#define _BSP_ADC_H_

#include "self_stm32f10x.h"

/*************************************************
 * 软件触发 多通道 非扫描 ADC捕获
 * 
 * Create by: 庞碧璋
 * Github: https://github.com/CodingBugStd
 * csdn:   https://blog.csdn.net/RampagePBZ
 * Encoding: utf-8
 * create date: 21年电赛
 * last date:   2021/12/20
*************************************************/

#define ADC_GPIO_CLK()  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE)
#define ADC_CLK()       RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2,ENABLE)

#define Read_BatVol()   ADC_ReadVoltage(ADC1,0) * 4.01f     //读取电池电压
#define Read_WaterSensorVol()   ADC_ReadVoltage(ADC2,1)     //读取深度传感器(压强式)电压值

void BSP_ADC_Init(void);
float ADC_ReadVoltage(ADC_TypeDef*ADC,uint8_t channel_num);

#endif

