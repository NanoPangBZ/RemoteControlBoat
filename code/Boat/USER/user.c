#include "user.h"   
#include "main.h"
#include <stdio.h>

uint8_t statis_receive = 0;

//回复主机
void ReplyMaster_Task(void*ptr)
{
    uint8_t MaxWait = *(uint8_t*)ptr / portTICK_RATE_MS;    //换算成心跳周期
    uint8_t*sbuf = nRF24L01_Get_RxBufAddr();    //nrf缓存地址
    RemoteControl_Type  nrf_receive;    //接收到的数据
    BoatReply_Type      nrf_send;       //准备发送回去的数据
    uint8_t resualt;        //发射结果接收
    uint8_t signal = 1;     //信号丢失标志 1:丢失
    while(1)
    {
        //等待nrf接收到主机的信号
        while(xSemaphoreTake(nRF24_RecieveFlag,MaxWait) == pdFALSE)
        {
            //过长时间没有接收到主机信号
            OS_EMG_Stop();     //紧急停止
            //更新至系统状态
            if(xSemaphoreTake(sysStatus_occFlag,1) == pdTRUE)
            {
                sysStatus.nrf_signal += 1;
                xSemaphoreGive(sysStatus_occFlag);  //释放资源
            }
            //首次丢失信号鸣响蜂鸣器
            if(signal != 1)
            {
                signal = 1;
                //蜂鸣器警告
                OS_Beep(100,100,3,3);
            }
            //有可能是本机nrf挂了,重启一次nrf
            OS_nrf_Restart();
        }

        //拷贝接收到的数据
        MemCopy(sbuf,(uint8_t*)&nrf_receive,sizeof(RemoteControl_Type));

        //准备回复的数据
        if(xSemaphoreTake(mpuDat_occFlag,0) == pdPASS)
        {
            MemCopy((uint8_t*)mpu_data,(uint8_t*)nrf_send.Gyroscope,12);
            xSemaphoreGive(mpuDat_occFlag);     //释放资源
        }
        nrf_send.Voltage = BatVol;
        nrf_send.Depth = Depth;
        nRF24L01_Send((uint8_t*)&nrf_send,32);

        xQueueReceive(nRF24_SendResult,&resualt,MaxWait);   //等待回复结果 -> nRF24L01_Send()

        //响应从遥控器接收到的命令和数据
        OS_ResponesReceive(&nrf_receive);
        
        //判断是否需要系统状态标志
        //能运行到这里说明信号没有丢失
        if(signal == 1)
        {
            if(xSemaphoreTake(sysStatus_occFlag,1) == pdTRUE)
            {
                sysStatus.nrf_signal = 0;
                xSemaphoreGive(sysStatus_occFlag);  //释放资源
                signal = 0;
                OS_Beep(100,100,2,0);
            }
        }
        LED_CTR(0,LED_Reserval);
        statis_receive++;
    }
}

void AutoRun_Task(void*ptr)
{
    #if 0
    AutoSail_Type handle = *(AutoSail_Type*)ptr;
    Route_Type that_node = *handle.list;
    while(1)
    {
    }
    #endif
}

void Voltage_Task(void*ptr)
{
    TickType_t  time = xTaskGetTickCount();
    BatVol = Read_BatVol();
    while(1)
    {
        //49:1 权重比
        BatVol = BatVol * 0.98f +  Read_BatVol() * 0.02f;
        vTaskDelayUntil(&time,100);  //0.1s更新一次电池电压
    }
}

void DepthSensor_Task(void*ptr)
{
    uint8_t Cycle = (1000 / *(uint8_t*)ptr) / portTICK_RATE_MS; //频率换算成心跳周期
    TickType_t  time = xTaskGetTickCount();
    Depth = Get_WaterLine();
    while(1)
    {
        Depth = Depth*0.9 + Get_WaterLine()*0.1;
        vTaskDelayUntil(&time,Cycle);
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

float angle = 0.0f;
uint8_t StreetSW = 0;

//按键输入响应
void KeyInput_Task(void*ptr)
{
    TickType_t time = xTaskGetTickCount();
    while(1)
    {
        switch(Key_Read_All())
        {
        case 0:
            if(StreetSW)
            {
                StreetSW = 0;
            }else
            {
                StreetSW = 1;
            }
            OS_Switch_OLED();
            break;
        case 1:
            WaterLine_ZeroOffset_Reset();
            break;
        case 2:
            angle += 1.0f;
        break;
        case 3:
            angle -= 1.0f;
        break;
        }
        //StreetMotor_Set(&streetMotor[StreetSW],angle);
        vTaskDelayUntil(&time,100/portTICK_PERIOD_MS);
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
        if(target_angle > 180.0f)
            target_angle = 180.0f;
        if(target_angle < 0.0f)
            target_angle = 0.0f;
        if(angle < target_angle)
        {
            angle += SMT.angle_inc;
            if(angle > target_angle)
                angle = target_angle;
            StreetMotor_Set(&SMT.streetMotor,angle);
        }else if(angle > target_angle)
        {
            angle -= SMT.angle_inc;
            if(angle < target_angle)
                angle = target_angle;
            StreetMotor_Set(&SMT.streetMotor,angle);
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

//屏幕刷新
void OLED_Task(void*ptr)
{
    uint8_t Cycle = (1000 / *(uint8_t*)ptr) / portTICK_RATE_MS;     //频率换算成心跳周期
    TickType_t  time = xTaskGetTickCount();
    float gyroscope[3];
    sysStatus_Type sys = {0};
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
        OLED12864_Show_String(1,55,sbuf,2);
        //显示吃水深度
        OLED12864_Clear_PageBlock(3,48,128);
        OLED12864_Clear_PageBlock(4,48,128);
        sprintf((char*)sbuf,"Dep:%.0fCM",Depth);
        OLED12864_Show_String(3,48,sbuf,2);

        OLED12864_Clear_Page(5);
        sprintf((char*)sbuf,"%.0f",angle);
        OLED12864_Show_String(5,0,sbuf,1);

        OLED12864_Refresh();
        vTaskDelayUntil(&time,Cycle);
    }
}
