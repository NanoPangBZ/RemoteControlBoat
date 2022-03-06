#include "motor.h"

void ElectricRegulation_Init(void)
{
    MAIN_MOTOR_L(2000); //最大油门
    MAIN_MOTOR_R(2000); 
    SEC_MOTOR_L(2000);
    SEC_MOTOR_R(2000);
    soft_delay_ms(1000);
    MAIN_MOTOR_L(1500); //中位油门
    MAIN_MOTOR_R(1500);
    SEC_MOTOR_L(1500);
    SEC_MOTOR_R(1500);
    soft_delay_ms(500);
}

