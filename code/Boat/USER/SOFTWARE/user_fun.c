#include "user_fun.h"
#include "main.h"

//执行从遥控器接收到的命令
void OS_ResponesReceive(RemoteControl_Type*receive)
{
    Ctr_Type ctr;
    if(receive->cmd == 1)
    {
        //主电调油门配置
        int ER_Base;
        int ER_sc;
        ER_Base = (receive->rocker[0] - 50) * 10;
        if(ER_Base != 0)
        {
            ER_sc = (receive->rocker[1] - 50) * 0.02 *ER_Base;
        }else
        {
            ER_sc = (receive->rocker[1] - 50) * 8;
        }
        ctr.ERctr.type = 1;
        //左
        ctr.ERctr.dat = ER_Base + ER_sc;
        xQueueSend(ER_CmdQueue[0],&ctr.ERctr,0);
        //右
        ctr.ERctr.dat = ER_Base - ER_sc;
        xQueueSend(ER_CmdQueue[1],&ctr.ERctr,0);
        //副电调油门配置
        ER_Base = ER_Base / 2 ;
        ER_sc = ER_sc / 2;
        ctr.ERctr.dat = ER_Base + ER_sc;
        xQueueSend(ER_CmdQueue[2],&ctr.ERctr,0);
        ctr.ERctr.dat = ER_Base - ER_sc;
        xQueueSend(ER_CmdQueue[3],&ctr.ERctr,0);
        //直流电机
        ctr.DCMotorCtr.type = 1;
        if(receive->switch_value & 0x01 != 0)
            ctr.DCMotorCtr.dat = 3600;
        else
            ctr.DCMotorCtr.dat = 0;
        xQueueSend(DCMotor_CmdQueue[0],&ctr.DCMotorCtr,2);
        xQueueSend(DCMotor_CmdQueue[1],&ctr.DCMotorCtr,2);
        //云台
        ctr.StreetMotorCtr.type = 1;
        ctr.StreetMotorCtr.dat = (float)(receive->rocker[2] -50 ) * 0.06f;
        xQueueSend(STMotor_CmdQueue[0],&ctr.StreetMotorCtr,0);
        ctr.StreetMotorCtr.dat = (float)(receive->rocker[3] -50) * 0.06f;
        xQueueSend(STMotor_CmdQueue[1],&ctr.StreetMotorCtr,0);
    }
}

//紧急停止
void OS_EMG_Stop(void)
{
    Ctr_Type ctr;
    //紧急停止所有无刷电机 -> 由无刷电机控制任务处理
    ctr.ERctr.type = 1;
    ctr.ERctr.dat = 0;
    for(uint8_t temp=0;temp<4;temp++)
        xQueueSend(ER_CmdQueue[temp],&ctr.ERctr,3);
    //紧急停止所有直流电机
    ctr.DCMotorCtr.type = 1;
    ctr.DCMotorCtr.dat = 0;
    for(uint8_t temp=0;temp<2;temp++)
    {
        A4950_Out(&a4950[temp],0);
        xQueueSend(DCMotor_CmdQueue[temp],&ctr.DCMotorCtr,3);
    }
}

//鸣响蜂鸣器
void OS_Beep(uint16_t on_ms,uint16_t off_ms,uint8_t count,uint8_t fre_id)
{
    BeepCtr_Type ctr;
    ctr.count = count;
    ctr.off_ms = off_ms;
    ctr.on_ms = on_ms;
    ctr.fre = Mu_Fre[fre_id];
    xQueueSend(Beep_CmdQueue,&ctr,3);
}

//重启nrf
void OS_nrf_Restart(void)
{
    vTaskSuspend(nRF24L01_Intterrupt_TaskHandle);   //挂起中断服务
    taskENTER_CRITICAL();
    nRF24L01_Restart();
    taskEXIT_CRITICAL();
    vTaskResume(nRF24L01_Intterrupt_TaskHandle);    //解挂
}
