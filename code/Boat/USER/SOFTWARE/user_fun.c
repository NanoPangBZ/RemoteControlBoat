#include "user_fun.h"
#include "main.h"
#include "user.h"

extern int DC_Out;
extern float angle;

#define UPDC_SPEED  2800
#define OPDC_SPEED  -3600
#define R_ST_C      65.0f  
#define R_ST_O      160.0f
#define L_ST_O      70.0f   //
#define L_ST_C      145.0f

//执行从遥控器接收到的命令
void OS_ResponesReceive(RemoteControl_Type *receive)
{
    Ctr_Type ctr;
    //主电调油门配置
    int ER_Base;
    int ER_sc;
    ER_Base = (receive->rocker[0] - 50) * 10;
    ER_sc = (receive->rocker[1] - 50) * 10;
    ctr.ERctr.type = 1;
    //主电调
    ctr.ERctr.dat = ER_Base;
    xQueueSend(ER_CmdQueue[0], &ctr.ERctr, 0);
    xQueueSend(ER_CmdQueue[1], &ctr.ERctr, 0);
    //副电调油门配置
    ctr.ERctr.dat = ER_sc;
    xQueueSend(ER_CmdQueue[2], &ctr.ERctr, 0);
    xQueueSend(ER_CmdQueue[3], &ctr.ERctr, 0);
    //直流电机
    ctr.DCMotorCtr.type = 1;
    if( ( receive->switch_value & DCMotor1_Mask ) != 0)
        ctr.DCMotorCtr.dat = UPDC_SPEED;
    else
        ctr.DCMotorCtr.dat = 0;
    xQueueSend(DCMotor_CmdQueue[0],&ctr.DCMotorCtr,2);
    if( ( receive->switch_value  & DCMotor2_Mask ) != 0)
        ctr.DCMotorCtr.dat = OPDC_SPEED;
    else
        ctr.DCMotorCtr.dat = 0;
    xQueueSend(DCMotor_CmdQueue[1],&ctr.DCMotorCtr,2);
    //气压计归零
    if( (receive->switch_value & ZeroWaterLine_Mask) !=0 )
        WaterLine_ZeroOffset_Reset();
    OLED12864_Show_Num(6,0,receive->switch_value,1);
    //前爪控制
    #if 1
    ctr.StreetMotorCtr.type = 3;
    if( (receive->switch_value &= Grab_Mask) != 0)
    {
        ctr.StreetMotorCtr.dat = R_ST_O;
        xQueueSend(STMotor_CmdQueue[0],&ctr.StreetMotorCtr,2);
        ctr.StreetMotorCtr.dat = L_ST_O;
        xQueueSend(STMotor_CmdQueue[1],&ctr.StreetMotorCtr,2);
    }else
    {
        ctr.StreetMotorCtr.dat = R_ST_C;
        xQueueSend(STMotor_CmdQueue[0],&ctr.StreetMotorCtr,2);
        ctr.StreetMotorCtr.dat = L_ST_C;
        xQueueSend(STMotor_CmdQueue[1],&ctr.StreetMotorCtr,2);
    }
    #endif
}

void OS_AutoRun(void)
{
}

//航向角闭环
void OS_YawControl(float yaw, float angle)
{
}

//紧急停止
void OS_EMG_Stop(void)
{
    Ctr_Type ctr;
    //紧急停止所有无刷电机 -> 由无刷电机控制任务处理
    ctr.ERctr.type = 1;
    ctr.ERctr.dat = 0;
    for (uint8_t temp = 0; temp < 4; temp++)
        xQueueSend(ER_CmdQueue[temp], &ctr.ERctr, 3);
    //紧急停止所有直流电机
    ctr.DCMotorCtr.type = 1;
    ctr.DCMotorCtr.dat = 0;
    for (uint8_t temp = 0; temp < 2; temp++)
    {
        A4950_Out(&a4950[temp], 0);
        xQueueSend(DCMotor_CmdQueue[temp], &ctr.DCMotorCtr, 3);
    }
}

//鸣响蜂鸣器
void OS_Beep(uint16_t on_ms, uint16_t off_ms, uint8_t count, uint8_t fre_id)
{
    BeepCtr_Type ctr;
    ctr.count = count;
    ctr.off_ms = off_ms;
    ctr.on_ms = on_ms;
    ctr.fre = Mu_Fre[fre_id];
    xQueueSend(Beep_CmdQueue, &ctr, 3);
}

//重启nrf
void OS_nrf_Restart(void)
{
    vTaskSuspend(nRF24L01_Intterrupt_TaskHandle); //挂起中断服务
    taskENTER_CRITICAL();
    nRF24L01_Restart();
    taskEXIT_CRITICAL();
    vTaskResume(nRF24L01_Intterrupt_TaskHandle); //解挂
}

//开启或关闭oled任务 节约时序
void OS_Switch_OLED(void)
{
    taskENTER_CRITICAL();
    if(OLED_TaskHandle == NULL)
    {
        xTaskCreate(
            OLED_Task,
            "oled",
            256,
            (void*)&oled_fre,
            8,
            &OLED_TaskHandle
        );
    }else
    {
        for(uint8_t temp=0;temp<8;temp++)
            OLED12864_Clear_Page(temp);
        vTaskDelete(OLED_TaskHandle);
        OLED_TaskHandle = NULL;
    }
    taskEXIT_CRITICAL();
}
