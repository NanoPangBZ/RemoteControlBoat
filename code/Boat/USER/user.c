#include "user.h"
#include "self_stm32f10x.h"
#include <stdio.h>

//其它参数
extern nRF24L01_Cfg nRF24_Cfg;

//任务句柄
extern TaskHandle_t OLED_TaskHandle;
extern TaskHandle_t nRF24L01_Intterrupt_TaskHandle;

//队列 信号
extern SemaphoreHandle_t nRF24_ISRFlag;         //nRF外部中断标志
extern SemaphoreHandle_t nRF24_RecieveFlag;     //nRF接收中断标志
extern QueueHandle_t     nRF24_SendResult;      //nRF发送结果队列(长度1)
extern SemaphoreHandle_t mpuDat_occFlag;		//mpu数据占用标志(互斥信号量)
extern SemaphoreHandle_t sysStatus_occFlag;     //系统状态变量占用标志(互斥信号量)
extern QueueHandle_t     Beep_CmdQueue;
extern QueueHandle_t	 ER_CmdQueue[4];
extern QueueHandle_t     DCMotor_CmdQueue[2];
extern QueueHandle_t     STMotor_CmdQueue[3];

//全局变量
extern float mpu_data[3];               //姿态 -> mpuDat_occFlag保护
extern float BatVol;
extern sysStatus_Type sysStatus;       //系统状态 -> sysStatus_occFlag保护

//回复主机
void ReplyMaster_Task(void*ptr)
{
    uint8_t MaxWait = *(uint8_t*)ptr / portTICK_RATE_MS;    //换算成心跳周期
    uint8_t*sbuf = nRF24L01_Get_RxBufAddr();    //nrf缓存地址
    RemoteControl_Type  nrf_receive;
    uint8_t resualt;        //发射结果接收
    uint8_t signal = 1;     //信号丢失标志
    BeepCtr_Type beep;      //蜂鸣器控制
    while(1)
    {
        //nrf半双工从机模式,等待nrf接收到主机的信号
        while(xSemaphoreTake(nRF24_RecieveFlag,MaxWait) == pdFALSE)
        {
            //过长时间没有接收到主机信号
            //紧急停止代码 -> 暂时不在这里加入急停代码,只更新系统状态,由MainTask()完成急停代码
            //...
            //更新至系统状态
            if(xSemaphoreTake(sysStatus_occFlag,1) == pdTRUE)
            {
                sysStatus.nrf_signal += 1;
                xSemaphoreGive(sysStatus_occFlag);  //释放资源
            }
            //任务内标志
            if(signal != 1)
            {
                signal = 1;
                //蜂鸣器警告
                beep.count = 3;
                beep.fre = Mu_Fre[3];
                beep.off_ms = 100;
                beep.on_ms = 100;
                xQueueSend(Beep_CmdQueue,&beep,0);
            }
            //有可能是本机nrf挂了,重启nrf
            vTaskSuspend(nRF24L01_Intterrupt_TaskHandle);   //挂起中断服务
            taskENTER_CRITICAL();
            nRF24L01_Restart();
            taskEXIT_CRITICAL();
            vTaskResume(nRF24L01_Intterrupt_TaskHandle);
        }

        //拷贝接收到的数据
        MemCopy(sbuf,(uint8_t*)&nrf_receive,sizeof(RemoteControl_Type));

        //回复主机
        if(xSemaphoreTake(mpuDat_occFlag,1) == pdPASS)
        {
            MemCopy((uint8_t*)mpu_data,sbuf,12);
            xSemaphoreGive(mpuDat_occFlag);     //释放资源
        }
        nRF24L01_Send(sbuf,32);
        xQueueReceive(nRF24_SendResult,&resualt,MaxWait);   //等待回复结果 -> nRF24L01_Send()

        //处理主机发送的数据 -> 更新到sysStatus.Recive中,由Main_Task处理
        if(xSemaphoreTake(sysStatus_occFlag,5) == pdPASS)
        {
            MemCopy((uint8_t*)&nrf_receive,(uint8_t*)&sysStatus.Recive,sizeof(RemoteControl_Type));
            xSemaphoreGive(sysStatus_occFlag);
        }

        //判断是否需要系统状态标志
        //能运行到这里说明信号没有丢失
        if(signal == 1)
        {
            if(xSemaphoreTake(sysStatus_occFlag,1) == pdTRUE)
            {
                sysStatus.nrf_signal = 0;
                xSemaphoreGive(sysStatus_occFlag);  //释放资源
                signal = 0;
                beep.count = 2;
                beep.fre = Mu_Fre[0];
                beep.off_ms = 100;
                beep.on_ms = 100;
                xQueueSend(Beep_CmdQueue,&beep,0);
            }
        }
        LED_CTR(0,LED_Reserval);
    }
}

//主要任务
void Main_Task(void*ptr)
{
    uint8_t cycle = (1000 / *(uint8_t*)ptr ) / portTICK_PERIOD_MS;  //频率转周期
    TickType_t time = xTaskGetTickCount();
    sysStatus_Type status;
    Ctr_Type ctr;
    int ER_BaseOut;     //电调基准输出
    int ER_sc;          //对称电调差速 -> 用于差速转向
    while(1)
    {
        //获得当前系统状态
        if(xSemaphoreTake(sysStatus_occFlag,5) == pdPASS)
        {
            status = sysStatus;
            xSemaphoreGive(sysStatus_occFlag);
        }
        //执行从nrf接收到的指令
        if(status.Recive.cmd == 1 && status.nrf_signal == 0)
        {
            //电调输出配置
            ER_BaseOut = (status.Recive.rocker[0] - 50) * 10;
            //配置油门差 用于转向
            if(ER_BaseOut == 0)
            {
                ER_sc = (status.Recive.rocker[1] - 50) * 5;
            }else
            {
                ER_sc = (int)( (status.Recive.rocker[1] - 50 ) * ER_BaseOut / 80 ) ;     //差速
            }
            //配置控制类型 ERctr->控制电调 type->控制类型
            ctr.ERctr.type = 1;
            //左电调
            ctr.ERctr.dat = ER_BaseOut + ER_sc;
            xQueueSend(ER_CmdQueue[0],&ctr.ERctr,0);    //向左电调任务发送控制命令
            //右电调
            ctr.ERctr.dat = ER_BaseOut - ER_sc;
            xQueueSend(ER_CmdQueue[1],&ctr.ERctr,0);    //向右电调任务发送控制命令
            //执行遥控器其他指令
            switch(status.Recive.type)
            {
                //执行直流电机运行
                case 6:
                    ctr.DCMotorCtr.type = 1;
                    //控制直流电机1
                    ctr.DCMotorCtr.dat = status.Recive.dat.DC_target.motor_1;
                    xQueueSend(DCMotor_CmdQueue[0],&ctr.DCMotorCtr,2);
                    //控制直流电机2
                    ctr.DCMotorCtr.dat = status.Recive.dat.DC_target.motor_2;
                    xQueueSend(DCMotor_CmdQueue[1],&ctr.DCMotorCtr,2);
                    break;
                default:break;
            }
        }else
        {
            //紧急停止无刷电机
            ctr.ERctr.type = 1;
            ctr.ERctr.dat = 0;
            //发送命令到所有电调任务
            xQueueSend(ER_CmdQueue[0],&ctr.ERctr,portMAX_DELAY);
            xQueueSend(ER_CmdQueue[1],&ctr.ERctr,portMAX_DELAY);
            xQueueSend(ER_CmdQueue[2],&ctr.ERctr,portMAX_DELAY);
            xQueueSend(ER_CmdQueue[3],&ctr.ERctr,portMAX_DELAY);
            //紧急停止直流电机
            ctr.DCMotorCtr.type = 1;
            ctr.DCMotorCtr.dat = 0;
            //发送命令到所有直流电机任务
            xQueueSend(DCMotor_CmdQueue[0],&ctr.DCMotorCtr,portMAX_DELAY);
            xQueueSend(DCMotor_CmdQueue[1],&ctr.DCMotorCtr,portMAX_DELAY);
        }
        vTaskDelayUntil(&time,cycle);
    }
}

void Voltage_Task(void*ptr)
{
    TickType_t  time = xTaskGetTickCount();
    while(1)
    {
        BatVol = Read_BatVol() * 4.01; //读取电池电压
        vTaskDelayUntil(&time,50);  //0.05s更新一次电池电压
    }
}

//nrf24中断处理
void nRF24L01_Intterrupt_Task(void*ptr)
{
    while(1)
    {
        xSemaphoreTake(nRF24_ISRFlag,portMAX_DELAY);
        nRF24L01_InterruptHandle();     //isr处理函数
    }
}

//屏幕刷新
void OLED_Task(void*ptr)
{
    uint8_t Cycle = (1000 / *(uint8_t*)ptr) / portTICK_RATE_MS;     //频率换算成心跳周期
    TickType_t  time = xTaskGetTickCount();
    float gyroscope[3];
    sysStatus_Type sys = {0,0};
    uint8_t sbuf[32];
    while(1)
    {
        time = xTaskGetTickCount();
        //获取系统状态
        if(xSemaphoreTake(sysStatus_occFlag,1) == pdTRUE)
        {
            sys = sysStatus;
            xSemaphoreGive(sysStatus_occFlag);  //释放资源
        }
        //显示对应页面信息
        switch (sys.oled_page)
        {
        case 0:
            //将陀螺仪数据载入gyroscope 并且更新oled的姿态显示
            if(xSemaphoreTake(mpuDat_occFlag,1) == pdPASS)
            {
                MemCopy((uint8_t*)mpu_data,(uint8_t*)gyroscope,12);
                xSemaphoreGive(mpuDat_occFlag); //释放资源
                OLED12864_Clear_PageBlock(1,0,48);
                OLED12864_Clear_PageBlock(2,0,48);
                OLED12864_Clear_PageBlock(3,0,48);
                sprintf((char*)sbuf,"x:%.1f",gyroscope[0]);
                OLED12864_Show_String(1,0,sbuf,1);
                sprintf((char*)sbuf,"y:%.1f",gyroscope[1]);
                OLED12864_Show_String(2,0,sbuf,1);
                sprintf((char*)sbuf,"z:%.1f",gyroscope[2]);
                OLED12864_Show_String(3,0,sbuf,1);
            }
            OLED12864_Show_Num(7,0,time/portTICK_RATE_MS/1000,1);
            break;
        }
        //显示nrf信号状态
        OLED12864_Clear_Page(0);
        if(sys.nrf_signal != 0)
        {
            sprintf((char*)sbuf,"loss:%d",sys.nrf_signal);
        }else{
            sprintf((char*)sbuf,"connect");
        }
        OLED12864_Show_String(0,0,sbuf,1);
        //显示电压
        sprintf((char*)sbuf,"Vol:%.1fV",BatVol);
        OLED12864_Show_String(2,55,sbuf,2);
        //数据反馈 er->电调油门  MT->直流电机油门
        sprintf((char*)sbuf,"er:%d",ER_ReadOut(&er[0]));
        OLED12864_Clear_Page(4);
        OLED12864_Show_String(4,0,sbuf,1);
        sprintf((char*)sbuf,"MT:%d",A4950_ReadOut(&a4950[1]));
        OLED12864_Clear_Page(5);
        OLED12864_Show_String(5,0,sbuf,1);
        
        OLED12864_Refresh();
        vTaskDelayUntil(&time,Cycle);
    }
}

//姿态更新
void MPU_Task(void*ptr)
{
    uint8_t Cycle = (1000 / *(uint8_t*)ptr) / portTICK_RATE_MS; //频率换算成心跳周期
    TickType_t  time = xTaskGetTickCount();
    float fsbuf[3];
    while(1)
    {
        //进入临界区,防止软件iic时序错误
        //taskENTER_CRITICAL();
        mpu_dmp_get_data(&fsbuf[0],&fsbuf[1],&fsbuf[2]);
        //taskEXIT_CRITICAL();
        //更新陀螺仪数据
        if(xSemaphoreTake(mpuDat_occFlag,1) == pdPASS)
        {
            MemCopy((uint8_t*)fsbuf,(uint8_t*)mpu_data,12);
            xSemaphoreGive(mpuDat_occFlag); //释放资源
        }
        vTaskDelayUntil(&time,Cycle);
    }
}

//按键输入响应
void KeyInput_Task(void*ptr)
{
    TickType_t time = xTaskGetTickCount();
    ERctr_Type  e_ctr[2];
    DCMotorCtr_Type d_ctr;
    d_ctr.type = 4;
    d_ctr.dat = 1;
    e_ctr[0].type = 1;
    e_ctr[1].type = 1;
    e_ctr[0].dat = 0;
    e_ctr[1].dat = 0;
    while(1)
    {
        if(Key_Read(0) == Key_Press)
        {
            d_ctr.dat = 50;
            xQueueSend(DCMotor_CmdQueue[0],&d_ctr,0);
            xQueueSend(DCMotor_CmdQueue[1],&d_ctr,0);
        }else
        if(Key_Read(1) == Key_Press)
        {
            d_ctr.dat = -50;
            xQueueSend(DCMotor_CmdQueue[0],&d_ctr,0);
            xQueueSend(DCMotor_CmdQueue[1],&d_ctr,0);
        }else
        if(Key_Read(2) == Key_Press)
        {
            e_ctr[0].dat += 10;
            e_ctr[1].dat += 10;
            xQueueSend(ER_CmdQueue[0],&e_ctr[1],0);
            xQueueSend(ER_CmdQueue[1],&e_ctr[1],0);
        }else
        if(Key_Read(3) == Key_Press)
        {
            e_ctr[0].dat -= 10;
            e_ctr[1].dat -= 10;
            xQueueSend(ER_CmdQueue[0],&e_ctr[1],0);
            xQueueSend(ER_CmdQueue[1],&e_ctr[1],0);
        }
        vTaskDelayUntil(&time,40/portTICK_PERIOD_MS);
    }
}

//电机控制任务 可重入
void Motor_Task(void*ptr)
{
    DCMotor_Type DCMT = *(DCMotor_Type*)ptr;    //任务参数
    DCMotorCtr_Type ctr;                        //接收到的命令
    TickType_t  time = xTaskGetTickCount();
    int out;                //a4950的当前输出
    int target_out = 0;     //a4950的目标输出
    while(1)
    {   
        //根据接收到的命令更改任务参数
        while(xQueueReceive(*DCMT.queueAddr,&ctr,0) == pdPASS)
        {
            switch(ctr.type)
            {
                case 1:target_out = ctr.dat;break;
                case 2:target_out = 0;A4950_Brake(&DCMT.a4950);break;
                case 3:DCMT.max_inc = ctr.dat;break;
                case 4:target_out += ctr.dat;break;
            }
        }
        //获取当前pwm输出
        out = A4950_ReadOut(&DCMT.a4950);
        //更新本任务周期pwm输出
        if(out < target_out)
        {
            out += DCMT.max_inc;
            if(out > target_out)
                out = target_out;
            A4950_Out(&DCMT.a4950,target_out);
        }else if(out > target_out)
        {
            out -= DCMT.max_inc;
            if(out < target_out)
                out = target_out;
            A4950_Out(&DCMT.a4950,target_out);
        }
        vTaskDelayUntil(&time,DCMT.cycle / portTICK_PERIOD_MS);
    }
}

//电调任务 可重入
void ER_Task(void*ptr)
{
    ER_Type ERT = *(ER_Type*)ptr ;
    ERctr_Type ctr;
    TickType_t  time = xTaskGetTickCount();
    int target_out = 0 ;
    int out;
    while(1)
    {
        //查看是否有新的指令
        while(xQueueReceive(*ERT.queueAddr,&ctr,5) == pdTRUE)
        {
            switch (ctr.type)
            {
            case 1: target_out = ctr.dat; break;
            case 2: ERT.max_inc = ctr.dat; break;
            case 3: ERT.cycle = ctr.dat;break;
            }
        }
        //获取当前电调输出
        out = ER_ReadOut(&ERT.er);
        //更新电调输出
        if(out > target_out)
        {
            if(out - target_out > ERT.max_inc)
                out -= ERT.max_inc;
            else
                out = target_out;
            ER_Out(&ERT.er,out);
        }else if(out < target_out)
        {
            if(target_out - out > ERT.max_inc)
                out += ERT.max_inc;
            else
                out = target_out;
            ER_Out(&ERT.er,out);
        }
        vTaskDelayUntil(&time,ERT.cycle/portTICK_PERIOD_MS);
    }
}

//舵机任务 可重入
void StreetMotor_Task(void*ptr)
{
    StreetMotor_Type SMT = *(StreetMotor_Type*)ptr;
    StreetMotorCtr_Type ctr;
    TickType_t  time = xTaskGetTickCount();
    float target_angle = 90.0;
    float angle = 90.0;
    while(1)
    {
        
        while(xQueueReceive(*SMT.queueAddr,&ctr,0) == pdPASS)
        {
            switch (ctr.type)
            {
            case 1:target_angle += ctr.dat; break;
            case 2:SMT.angle_inc = ctr.dat; break;
            case 3:target_angle = ctr.dat; break;
            case 4:target_angle = angle = ctr.dat;
                StreetMotor_Set(&SMT.streetMotor,target_angle);
                break;
            }
        }
        if(angle < target_angle)
        {
            angle += SMT.angle_inc;
            if(angle > target_angle)
                angle = target_angle;
            StreetMotor_Set(&SMT.streetMotor,target_angle);
        }else if(angle > target_angle)
        {
            angle -= SMT.angle_inc;
            if(angle < target_angle)
                angle = target_angle;
            StreetMotor_Set(&SMT.streetMotor,target_angle);
        }
        vTaskDelayUntil(&time,SMT.cycle/portTICK_PERIOD_MS);
    }
}

//蜂鸣器任务
void Beep_Task(void*ptr)
{
    BeepCtr_Type ctr;
    while(1)
    {
        xQueueReceive(Beep_CmdQueue,&ctr,portMAX_DELAY);
        for(uint8_t temp=0;temp<ctr.count;temp++)
        {
            Beep_ON(ctr.fre);
            vTaskDelay(ctr.on_ms/portTICK_PERIOD_MS);
            Beep_OFF();
            vTaskDelay(ctr.off_ms/portTICK_PERIOD_MS);
        }
    }
}

