#ifndef _ROCKER_H_
#define _ROCKER_H_

#include "BSP\bsp_adc.h"

typedef struct
{
    uint8_t adc_channle;    //ADC通道
    float coe;              //放大系数 -> (参考电压 / 变化范围)
    float Median_max;       //中位上限位(基于参考电压的比例值)
    float Median_min;       //中位下限位(基于参考电压的比例值)
}rocker_Type;

extern rocker_Type rockers[4];

short Rocker_Get(rocker_Type*rocker);
uint8_t Rocker_UnsignedGet(rocker_Type*rocker);

#endif

