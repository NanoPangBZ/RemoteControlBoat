#include "water_sensor.h"

#include "BSP\bsp_adc.h"
#define port_ReadVoltage()  ADC_ReadVoltage(1)*3.3

#define ZOOM    1.0f    //缩放系数

float Get_WaterLine(void)
{
    return port_ReadVoltage()*ZOOM;
}

