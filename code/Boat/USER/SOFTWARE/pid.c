#include "pid.h"

/*******************************************************************
 * 功能:进行一次增量式PID运算
 * 参数:
 *  handle:要进行运算的目标PID句柄
 *  Actual:当前被控制量实际值
 * 返回值:
 *  输出值
 * 2020/6   庞碧璋
 *******************************************************************/
float_p PID_IncOperation(PID_Handle*handle,float_p Actual)
{
    float_p Inc;
    handle->Err[2] = handle->Err[1];
    handle->Err[1] = handle->Err[0];
    handle->Err[0] = handle->Target - Actual;
    Inc = handle->pid.P * (handle->Err[0] - handle->Err[1])
         + handle->pid.I * (handle->Err[0])
         + handle->pid.D * (handle->Err[0] - handle->Err[1]*2 + handle->Err[2]) ;
    handle->Output += Inc*handle->pid.out_zoom;
    if(handle->Output > handle->OutputMax)
        handle->Output = handle->OutputMax;
    if(handle->Output < handle->OutputMin)
        handle->Output = handle->OutputMin;
    return handle->Output;
}

/*******************************************************************
 * 功能:进行一次位置式PID运算
 * 参数:
 *  handle:要进行运算的目标PID句柄
 *  Actual:当前被控制量实际值
 * 返回值:
 *  输出值
 * 2020/6   庞碧璋
 *******************************************************************/
float_p PID_PosOperation(PID_Handle*handle,float_p Actual)
{
    return 0;
}
