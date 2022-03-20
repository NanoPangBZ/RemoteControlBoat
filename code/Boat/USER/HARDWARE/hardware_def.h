#ifndef _HARDWARE_DEF_H_
#define _HARDWARE_DEF_H_

#include "HARDWARE\MOTOR\a4950.h"
#include "HARDWARE\MOTOR\street_motor.h"
#include "HARDWARE\MOTOR\er.h"

//a4950 X 2
static a4950_Type a4950[2] = { {0,1,3600} , {2,3,3600} };
//舵机X4
static streetMotor_Type streetMotor[4] = {{4,180},{5,180},{6,180},{7,180}};
//电调X4
static er_Type er[4] = {{8,1400},{9,1400},{10,1400},{11,1400},};

#endif

