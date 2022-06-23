#include "water_sensor.h"

#include "BSP\bsp_adc.h"

//标准大气压ADC读数 -> 1.0575V
//27cm水压ADC读数 -> 1.3467V
//Zoom = 93.36

#define ZOOM    93.36f
#define CM_OFFSET   -128.3f       //0漂
#define port_GetADC()   Read_WaterSensorVol()
#define WaterLine   ( port_GetADC() * zoom + zero_offset )

float zero_offset = -128.3f;
float zoom = 93.36f;

float Get_WaterLine(void)
{
    float temp = WaterLine;
    if(temp < 0)
    {
        if(temp < -12.0f)
            WaterLine_ZeroOffset_Reset();
        return 0;
    }
    return temp;
}

//气压计标准大气压下0点校准
void WaterLine_ZeroOffset_Reset(void)
{
    float i = 0;
    for(uint8_t temp=0;temp<10;temp++)
    {
        i += WaterLine;
    }
    zero_offset -= (i/10);
}

void ZeroOffset_ResetOnRun(float acc_depth)
{

}

