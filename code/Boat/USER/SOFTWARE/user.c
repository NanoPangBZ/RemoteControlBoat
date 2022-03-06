#include "user.h"
#include "self_stm32f10x.h"
#include <stdio.h>


void GyroscopeRefresh_Task(void*ptr)
{
}

void Test_Task(void*ptr)
{
    while(1)
    {
        printf("Test\r\n");
        vTaskDelay(1000/portTICK_RATE_MS);
    }
}

