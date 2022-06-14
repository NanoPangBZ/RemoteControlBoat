#include "rocker.h"

#define port_ADCGet(ch) ADC_ReadVoltage(ch) //应当返回基于参考电压的百分比

//摇杆定义 LY LX RY RX
rocker_Type rockers[4] = {
    { 0 , 6.11f , 0.505f , 0.497f},
    { 1 , 6.11f , 0.500f , 0.486f},
    { 2 , 6.11f , 0.505f , 0.490f},
    { 3 , 5.92f , 0.510f , 0.498f},
};

/*******************************************************************
 * 功能:读取摇杆当前值
 * 参数:rocker_Type
 * 返回值:
 *  摇杆当前位置 -50 ~ 50
 * 2022/3   庞碧璋
 *******************************************************************/
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

/*******************************************************************
 * 功能:读取摇杆当前值
 * 参数:rocker_Type
 * 返回值:
 *  摇杆当前位置 0 ~ 100
 * 2022/3   庞碧璋
 *******************************************************************/
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

