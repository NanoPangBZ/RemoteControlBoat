#ifndef _ROCKER_H_
#define _ROCKER_H_

#include "BSP\bsp_adc.h"

typedef struct
{
    uint8_t adc_channle;
    uint8_t Median_max;     //中位上限位
    uint8_t Median_min;     //中位下限位
}rocker_Type;

static rocker_Type rockers[4] = 
{

};

void Rock_Get(rocker_Type*rocker);


#endif

