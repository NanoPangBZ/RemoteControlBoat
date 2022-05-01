#ifndef _PID_H_
#define _PID_H_

/*************************************************
 * 增量式PID
 * Creat by: 庞碧璋
 * Github: https://github.com/CodingBugStd
 * csdn:   https://blog.csdn.net/RampagePBZ
 * Encoding: utf-8
 * date:    2021/8/27
*************************************************/

#ifndef USE_DOUBLE
    #define USE_DOUBLE  0   //使用单精度浮点型
#endif  //USE_DOUBLE

#if USE_DOUBLE == 1
    typedef double float_p;
#else
    typedef float  float_p;
#endif

typedef struct
{
    float_p P;
    float_p I;
    float_p D;
    float_p out_zoom;    //输出缩放
    float_p Output;  //输出
    float_p Target;  //目标值
    float_p Err[3];  //增量式为近三次误差 位置式为 [0]误差积分 [1]本次误差 [2]上次误差
    float_p OutputMax;
    float_p OutputMin;
}PID_Handle;

float_p PID_IncOperation(PID_Handle*handle,float_p Actual);
float_p PID_PosOperation(PID_Handle*handle,float_p Actual);

#endif
