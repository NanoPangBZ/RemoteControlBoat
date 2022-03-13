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
    #define USE_DOUBLE  1   //使用双精度浮点型
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
}PID_para;

typedef struct
{
    PID_para pid;
    float_p Output;  //输出
    float_p Target;  //目标值
    float_p Actual;  //实际值
    float_p Err[3];  //三次误差
    float_p OutputMax;
    float_p OutputMin;
}PID_Handle;

float_p PID_IncOperation(PID_Handle*handle,float_p Actual);

#endif