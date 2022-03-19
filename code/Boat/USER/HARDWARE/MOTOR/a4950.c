#include "a4950.h"

/*******************************************************************
 * a4950.c
 * a4950.h
 * 功能:
 * 驱动多块a4950,每块a4950能驱动一个直流电机。
 * 关于移植:
 * 需要自定义a4950的标号和设置每块a4950的2个PWM通道
 * 需要对接port_PWMOut()和port_PWMRead()
 * 需要设置最大PWM输出,a4950数量等
 * 2022/3/19   庞碧璋
 *******************************************************************/

#define port_PWMOut(ch,width)   PWM_Out(ch,width)   //PWM输出接口 ch->pwm输出的管道标号 width->脉宽
#define port_PWMRead(ch)        PWM_Read(ch)        //获取当前PWM输出接口
#define A4950_COUNT 2   //A4950的数量
#define PWM_MAX 7200    //最大PWM输出
//每块A4950对应的2个PWM通道标号
uint8_t PWM_Channel[A4950_COUNT][2] = {
    {0,1}, {2,3}
};

#define PWM_Median 3600    //PWM输出中位
//#define PWM_Median PWM_MAX/2

/*******************************************************************
 * 功能:控制A4950驱动电机
 * 参数:
 *  a4950_id:要控制的a4950标号
 *  out:pwm输出 -> 范围 : -PWM_MAX/2 ~ PWM_MAX/2
 * 返回值:无
 * 2022/3/19   庞碧璋
 *******************************************************************/
void A4950_Out(uint8_t a4950_id,int out)
{
    uint16_t pwm_width[2];
    if(out > PWM_Median)
        out = PWM_Median;
    else if(out < -PWM_Median)
        out = -PWM_Median;
    pwm_width[0] = PWM_Median + out;
    pwm_width[1] = PWM_Median - out;
    port_PWMOut(PWM_Channel[a4950_id][0],pwm_width[0]);
    port_PWMOut(PWM_Channel[a4950_id][1],pwm_width[1]);
}

/*******************************************************************
 * 功能:读取当前a4950的输出
 * 参数:
 *  a4950_id:要读取的a4950标号
 * 返回值:无
 * 2022/3/19   庞碧璋
 *******************************************************************/
int A4950_ReadOut(uint8_t a4950_id)
{
    return port_PWMRead(PWM_Channel[a4950_id][0]) - PWM_Median;
}

/*******************************************************************
 * 功能:a4950刹车
 * 参数:
 *  a4950_id:要刹车的a4950标号
 * 返回值:无
 * 2022/3/19   庞碧璋
 *******************************************************************/
void A4950_Brake(uint8_t a4950_id)
{
    port_PWMOut(PWM_Channel[a4950_id][0] , PWM_Median);
    port_PWMOut(PWM_Channel[a4950_id][1] , PWM_Median);
}


