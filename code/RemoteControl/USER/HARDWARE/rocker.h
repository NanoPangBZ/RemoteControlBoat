#ifndef _ROCKER_H_
#define _ROCKER_H_

/*************************************************
 * 摇杆驱动
 * 
 * 作者: 庞碧璋
 * Github: https://github.com/CodingBugStd
 * csdn:   https://blog.csdn.net/RampagePBZ
 * Encoding: utf-8
 * date:   2021/3
*************************************************/

#include "BSP\bsp_adc.h"

typedef struct
{
    uint8_t adc_channle;    //ADC通道 用于port_ADCGet()宏
    float coe;              //放大系数 -> (参考电压 / 变化范围)
    float Median_max;       //中位上限位(基于参考电压的比例值)
    float Median_min;       //中位下限位(基于参考电压的比例值)
}rocker_Type;

extern rocker_Type rockers[4];

short Rocker_Get(rocker_Type*rocker);
uint8_t Rocker_UnsignedGet(rocker_Type*rocker);

#endif

