#ifndef _AUTO_SAIL_H_
#define _AUTO_SAIL_H_

#include "self_stm32f10x.h"

typedef struct
{
    float yaw_off;      //航向偏移
    float speed;        //速度
    float distance;     //距离
}Route_Type;

typedef struct
{
    Route_Type*list;
    uint8_t node_num;
    uint8_t node_count;
}AutoSail_Type;


#endif //_AUTO_SAIL_H_

