#ifndef _ROCKER_H_
#define _ROCKER_H_

#include "BSP\bsp_adc.h"

typedef struct
{
    uint8_t adc_channle;
    uint8_t Median_max;     //中位上限位
    uint8_t Median_min;     //中位下限位
}rocker_Type;

extern rocker_Type rockers[4];

int Rocker_Get(rocker_Type*rocker);
uint8_t Rocker_UnsignedGet(rocker_Type*rocker);

#endif

