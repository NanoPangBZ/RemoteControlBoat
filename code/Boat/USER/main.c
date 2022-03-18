#include "self_stm32f10x.h"

#include "user.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#pragma	diag_suppress	870	//屏蔽汉字警告

//nRF24L01初始化结构体
nRF24L01_Cfg nRF24_Cfg;
static uint8_t RxAddr[5] = {0x43,0x16,'R','C',0xFF};	//遥控器地址
static uint8_t TxAddr[5] = {0x43,0x16,'B','T',0xFF};	//船地址

//任务参数
uint8_t oled_fre = 24;				//OLED刷新频率
uint8_t nrf_maxDelay = 200;			//nrf最大超时时间
uint8_t mpu_fre = DEFAULT_MPU_HZ;	//mpu更新频率
ER_Type	ER_is[4];					//电调任务参数
DCMotor_Type DCMotor_is[2];			//直流电机任务参数

//任务句柄
TaskHandle_t	RTOSCreateTask_TaskHandle = NULL;
TaskHandle_t	ReplyMaster_TaskHandle = NULL;
TaskHandle_t	OLED_TaskHandle = NULL;
TaskHandle_t	nRF24L01_Intterrupt_TaskHandle = NULL;
TaskHandle_t	MPU_TaskHandle = NULL;
TaskHandle_t	KeyInput_TaskHandle = NULL;
TaskHandle_t	ER_TaskHandle[4] = {NULL,NULL,NULL,NULL};	//电调任务句柄 0:main_l 1:main_r 2:sec_l 3:sec_r
TaskHandle_t	DCMotor_TaskHandle[2] = {NULL,NULL};		//直流电机控制任务句柄

//队列句柄
SemaphoreHandle_t	nRF24_ISRFlag = NULL;		//nrf24硬件中断标志
SemaphoreHandle_t	nRF24_RecieveFlag = NULL;	//nrf24接收标志(数据已经进入单片机,等待处理)
QueueHandle_t		nRF24_SendResult = NULL;	//nrf24发送结果
QueueHandle_t		ER_CmdQueue[4] = {NULL,NULL,NULL,NULL};	//电调控制命令 0:main_l 1:main_r 2:sec_l 3:sec_r
QueueHandle_t		DCMotor_CmdQueue[2] = {NULL,NULL};		//直流电机控制命令
SemaphoreHandle_t	mpuDat_occFlag = NULL;		//mpu数据占用标志(互斥信号量)
SemaphoreHandle_t	sysStatus_occFlag = NULL;	//系统状态变量占用标志(互斥信号量)

//全局变量
float mpu_data[3] = {0,0,0};    //姿态 -> mpuDat_occFlag保护
sysStatus_Type sysStatus;       //系统状态 -> sysStatus_occFlag保护

void RTOSCreateTask_Task(void*ptr);

int main(void)
{
	uint8_t nrf_err;
	uint8_t mpu_err;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	//关闭JATG调试接口,开启SWD调试接口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

	BSP_PWM_Init();
	BSP_LED_Init();
	BSP_Usart_Init();
	BSP_Timer_Init();
	BSP_Key_Init();

	//OLED初始化
	OLED12864_Init();
	OLED12864_Show_String(0,0,"hardware init",1);
	OLED12864_Refresh();

	//MPU初始化
	mpu_err = MPU_Init();
	if(mpu_err)
	{
		OLED12864_Show_String(2,0,"mpu err",1);
	}else
	{
		OLED12864_Show_String(2,0,"mpu pass",1);
		if(mpu_dmp_init())	//初始化官方的DMP固件库
			OLED12864_Show_String(3,0,"6050 dmp err",1);
	}

	//nrf24初始化和配置
	nrf_err = nRF24L01_Init();
	if(nrf_err)	//检查硬件
	{
		OLED12864_Show_String(1,0,"nrf err",1);
	}else
	{
		//nRF24L01 相关配置
		nRF24_Cfg.Channel = 50;	//2.45GHz 通讯频段
		nRF24_Cfg.retry = 5;	//最大重发次数
		nRF24_Cfg.retry_cycle = 1;	//重发周期
		nRF24_Cfg.Rx_Length = 32;	//结束长度
		MemCopy(TxAddr,nRF24_Cfg.TX_Addr,5);
		MemCopy(RxAddr,nRF24_Cfg.RX_Addr,5);
		nRF24L01_Config(&nRF24_Cfg);	//配置nRF24L01
		OLED12864_Show_String(1,0,"nrf pass",1);
		nRF24L01_Rx_Mode();
	}
	OLED12864_Refresh();

	//创建RTOS初始化任务
	if(nrf_err == 0 & mpu_err == 0)
	{
		xTaskCreate(
			RTOSCreateTask_Task,
			"init",
			128,
			NULL,
			15,
			&RTOSCreateTask_TaskHandle
		);
	}

	vTaskStartScheduler();

	while(1);
}

void RTOSCreateTask_Task(void*ptr)
{
	//全局变量赋值
    sysStatus.nrf_signal = 0;
    sysStatus.oled_page = 0;
	//建立 队列 信号量
    nRF24_ISRFlag = xSemaphoreCreateBinary();		//nrf外部中断标志,由isr给出
	nRF24_RecieveFlag = xSemaphoreCreateBinary();	//nrf发生接收中断标志
	nRF24_SendResult = xQueueCreate(1,1);			//nrf发送结果标志
    mpuDat_occFlag = xSemaphoreCreateMutex();		//mpu_data[3] 保护
    sysStatus_occFlag = xSemaphoreCreateMutex();	//sysStatus 保护

	//建立2个直流电机控制任务
	for(uint8_t temp=0;temp<2;temp++)
	{
		DCMotor_CmdQueue[temp] = xQueueCreate(3,sizeof(DCMotorCtr_Type));
		DCMotor_is[temp].recieveCmd = &DCMotor_CmdQueue[temp];
		DCMotor_is[temp].channel[0] = temp*2;
		DCMotor_is[temp].channel[1] = temp*2+1;
		xTaskCreate(
			Motor_Task,
			"MT",
			64,
			(void*)&DCMotor_is[temp],
			5,
			&DCMotor_TaskHandle[temp]
		);
	}
	//建立4个电调控制任务
	for(uint8_t temp=0;temp<4;temp++)
	{
		ER_CmdQueue[temp] = xQueueCreate(3,sizeof(ERctr_Type));
		ER_is[temp].recieveCmd = &ER_CmdQueue[temp];
		ER_is[temp].channel = 8+temp;	//Target_CCR[8~11] T8C1~T8C4
		ER_is[temp].cycle = 20;
		ER_is[temp].MaxInc = 10;
		xTaskCreate(
        	ER_Task,
        	"ER",
        	64,
        	(void*)&ER_is[temp],
        	5,
        	&ER_TaskHandle[temp]
    	);
	}
	//建立nrf回复主机任务
    xTaskCreate(
        ReplyMaster_Task,
        "Reply",
        128,
        (void*)&nrf_maxDelay,
        11,
        &ReplyMaster_TaskHandle
    );
	//建立陀螺仪任务
    xTaskCreate(
        MPU_Task,
        "mpu",
        256,
        (void*)&mpu_fre,
        10,
        &MPU_TaskHandle
    );
	//建立oled显示任务
    xTaskCreate(
        OLED_Task,
        "oled",
        256,
        (void*)&oled_fre,
        9,
        &OLED_TaskHandle
    );
	//建立nrf中断处理任务
    xTaskCreate(
        nRF24L01_Intterrupt_Task,
        "nrf interrupt",
        128,
        NULL,
        14,
        &nRF24L01_Intterrupt_TaskHandle
    );
	//建立按键输入任务
    xTaskCreate(
        KeyInput_Task,
        "key",
        64,
        NULL,
        9,
        &KeyInput_TaskHandle
    );
	//删除自身
    vTaskDelete(NULL);
}
