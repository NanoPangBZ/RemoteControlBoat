#include "water_sensor.h"

#include "BSP\bsp_adc.h"

//标准大气压ADC读数 -> 1.0575V
//27cm水压ADC读数 -> 1.3467V
//Zoom = 93.36

#define ATM_ADC 1.0573
#define ZOOM    93.36f
#define port_GetADC()   Read_WaterSensorVol()

float Get_WaterLine(void)
{
    return (port_GetADC() - ATM_ADC) * ZOOM;
}


