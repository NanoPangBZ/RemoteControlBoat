#ifndef _HARDWARE_DEF_H_
#define _HARDWARE_DEF_H_

#include "HARDWARE\MOTOR\a4950.h"
#include "HARDWARE\MOTOR\street_motor.h"
#include "HARDWARE\MOTOR\er.h"

//hardware_def.h -> 硬件定义

//物理接口
//核心板与驱动板对齐连接 - 驱动板忘记写标号了!注意别连接错

#define ER_MainL    0
#define ER_MainR    1
#define ER_SecL     2
#define ER_SecR     3

#define SM_D    0
#define SD_U    1

//a4950直流电机驱动芯片 X 2
static a4950_Type a4950[2] = { {0,1,3600} , {2,3,3600} };
//舵机 X 4
static streetMotor_Type streetMotor[4] = {{4,180},{5,180},{6,180},{7,180}};
//电调 X 4
static er_Type er[4] = {{8,0,1400},{9,1,1400},{10,0,1400},{11,0,1400}};

#endif

