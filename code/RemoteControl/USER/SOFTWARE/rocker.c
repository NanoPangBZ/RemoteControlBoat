#include "rocker.h"

#define port_ADCGet(ch) ADC_ReadVoltage(ch) //应当返回基于参考电压的百分比

//摇杆定义
rocker_Type rockers[4] = {
    { 0 , 53 , 47},
    { 1 , 53 , 47},
    { 2 , 53 , 47},
    { 3 , 53 , 47},
};

//return -50 ~ 50
int Rocker_Get(rocker_Type*rocker)
{
    int input;
    input = (int)(port_ADCGet(rocker->adc_channle) * 100);
    if(input > rocker->Median_max)
        return input - rocker->Median_max;
    if(input < rocker->Median_min)
        return input - rocker->Median_min;
    return 0;
}

//return 0~100
uint8_t Rocker_UnsignedGet(rocker_Type*rocker)
{
    uint8_t input;
    input = (uint8_t)(port_ADCGet(rocker->adc_channle) * 100);
    if(input > rocker->Median_max)
        return input + 50 - rocker->Median_max;
    if(input < rocker->Median_min)
        return input + 50 - rocker->Median_min ;
    return 50;
}

