#include "rocker.h"

#define port_ADCGet(ch) ADC_ReadVoltage(ch) //应当返回基于参考电压的百分比

//摇杆定义 LY LX RY RX
rocker_Type rockers[4] = {
    { 0 , 6.1f , 0.505f , 0.495f},
    { 1 , 6.1f , 0.500f , 0.493f},
    { 2 , 6.1f , 0.505f , 0.494f},
    { 3 , 5.9f , 0.510f , 0.495f},
};

//return -50 ~ 50
short Rocker_Get(rocker_Type*rocker)
{
    float input;
    input = port_ADCGet(rocker->adc_channle);
    if(input > rocker->Median_max)
        return (short)((input - rocker->Median_max)*rocker->coe*100);
    if(input < rocker->Median_min)
        return (short)((input - rocker->Median_min)*rocker->coe*100);
    return 0;
}

//return 0~100
uint8_t Rocker_UnsignedGet(rocker_Type*rocker)
{
    float input;
    input = port_ADCGet(rocker->adc_channle);
    if(input > rocker->Median_max)
        return (uint8_t)( (input - rocker->Median_max)*rocker->coe*100 + 50);
    if(input < rocker->Median_min)
        return (uint8_t)( (input - rocker->Median_min)*rocker->coe*100 + 50);
    return 50;
}

