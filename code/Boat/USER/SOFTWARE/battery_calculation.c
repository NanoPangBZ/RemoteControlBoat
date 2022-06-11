#include "battery_calculation.h"

typedef struct
{
    float pre;
    float vol;
}Vol_Table;

const Vol_Table Li_Table[12] = 
{
    {0,3.00f},{5,3.45f},{10,3.68f},{20,3.74f},
    {30,3.77f},{40,3.79f},{50,3.82f},{60,3.87f},
    {70,3.92f},{80,3.98f},{90,4.06f},{100,4.20f}
};

float Voltage_To_Percentage(float vol,uint8_t BatCount)
{
    for(uint8_t temp=0;temp<12;temp++)
    {
        if(vol < Li_Table[temp].vol*BatCount)
        {
            return Li_Table[temp-1].pre + (vol - Li_Table[temp-1].vol*BatCount) / 
            ( (Li_Table[temp].vol - Li_Table[temp-1].vol) * BatCount ) * (Li_Table[temp].pre - Li_Table[temp-1].pre);
        }
    }
    return 100.0f;
}
