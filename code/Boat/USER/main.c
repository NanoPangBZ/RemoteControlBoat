#include "main.h"

#pragma	diag_suppress	870	//屏蔽汉字警告

//nRF24L01初始化结构体
nRF24L01_Cfg nRF24_Cfg;
static uint8_t RxAddr[5] = {0x43,0x16,'R','C',0xFF};	//遥控器地址
static uint8_t TxAddr[5] = {0x43,0x16,'B','T',0xFF};	//船地址

//任务参数
uint8_t main_fre = 50;				//主任务频率
uint8_t oled_fre = 20;				//OLED刷新频率
uint8_t WaterLine_fre = 50;			//水位测量频率
uint8_t nrf_maxDelay = 200;			//nrf最大超时时间
uint8_t mpu_fre = DEFAULT_MPU_HZ;	//mpu更新频率
ER_Type	ER_is[4];					//电调任务参数
DCMotor_Type DCMotor_is[2];			//直流电机任务参数
StreetMotor_Type STMotor_is[3];		//舵机任务参数

//任务句柄
TaskHandle_t	RTOSCreateTask_TaskHandle = NULL;	//创建任务句柄
TaskHandle_t	ReplyMaster_TaskHandle = NULL;		//主机回复任务句柄
TaskHandle_t	OLED_TaskHandle = NULL;				//oled刷新任务句柄
TaskHandle_t	nRF24L01_Intterrupt_TaskHandle = NULL;	//nrf中断任务句柄
TaskHandle_t	MPU_TaskHandle = NULL;				//陀螺仪刷新任务句柄
TaskHandle_t	KeyInput_TaskHandle = NULL;			//按键任务句柄
TaskHandle_t	Voltage_TaskHandle = NULL;			//电池电压检测任务句柄
TaskHandle_t	DepthSensor_TaskHandle = NULL;		//水位深度检测任务句柄
TaskHandle_t	Beep_TaskHandle = NULL;				//蜂鸣器任务句柄
TaskHandle_t	ER_TaskHandle[4] = {NULL,NULL,NULL,NULL};	//电调任务句柄
TaskHandle_t	DCMotor_TaskHandle[2] = {NULL,NULL};		//直流电机控制任务句柄
TaskHandle_t	StreetMotor_TaskHandle[4] = {NULL,NULL,NULL,NULL};	//舵机任务句柄

//队列句柄
SemaphoreHandle_t	nRF24_ISRFlag = NULL;		//nrf24硬件中断标志
SemaphoreHandle_t	nRF24_RecieveFlag = NULL;	//nrf24接收标志(数据已经进入单片机,等待处理)
QueueHandle_t		nRF24_SendResult = NULL;	//nrf24发送结果
QueueHandle_t		wait_handle_receive = NULL;	//等待处理的接收信息
QueueHandle_t		ER_CmdQueue[4] = {NULL,NULL,NULL,NULL};	//电调任务命令接收队列
QueueHandle_t		DCMotor_CmdQueue[2] = {NULL,NULL};		//直流电机任务命令接收队列
QueueHandle_t		STMotor_CmdQueue[4] = {NULL,NULL,NULL,NULL};	//舵机任务命令接收队列
QueueHandle_t		Beep_CmdQueue = NULL;		//蜂鸣器命令队列
SemaphoreHandle_t	mpuDat_occFlag = NULL;		//mpu数据占用标志(互斥信号量)
SemaphoreHandle_t	sysStatus_occFlag = NULL;	//系统状态变量占用标志(互斥信号量)

//全局变量
float mpu_data[3] = {0,0,0};    //姿态 -> mpuDat_occFlag保护
float BatVol = 0.0f;			//电池电压
float Depth = 0.0f;				//吃水深度
sysStatus_Type sysStatus;       //系统状态 -> sysStatus_occFlag保护
PID_Handle	Yaw_pid_Handle;		//航向角pid

//PID参数以及限位
#define YAW_P	1.0f
#define YAW_I	0.0f
#define YAW_D	0.0f
#define YAW_ZOOM	2.0f
#define YAW_MAX	100
#define YAW_MIN	-100

void RTOSCreateTask_Task(void*ptr);		//初始化任务
void Er_Cal(void);	//油门行程校准

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
	//BSP_Timer_Init();
	BSP_Key_Init();
	BSP_Beep_Init();
	BSP_ADC_Init();
	BSP_i2c_Init();

	//OLED初始化 -> 需要SPI
	OLED12864_Init();
	OLED12864_Show_String(0,0,"hardware init",1);
	OLED12864_Refresh();

	Er_Cal();	//油门行程校准

	//MPU初始化	-> 需要IIC
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

	//nrf24初始化和配置 -> 需要SPI
	nrf_err = nRF24L01_Init();
	if(nrf_err)	//检查硬件
	{
		OLED12864_Show_String(1,0,"nrf err.restarting",1);
		OLED12864_Refresh();
		while(nrf_err == 1)
		{
			nrf_err = nRF24L01_Init();
			soft_delay_ms(100);
		}
	}

	//nRF24L01 相关配置
	nRF24_Cfg.Channel = 50;	//2.45GHz 通讯频段
	nRF24_Cfg.retry = 10;	//最大重发次数
	nRF24_Cfg.retry_cycle = 1;	//重发周期
	nRF24_Cfg.Rx_Length = 32;	//结束长度
	MemCopy(TxAddr,nRF24_Cfg.TX_Addr,5);
	MemCopy(RxAddr,nRF24_Cfg.RX_Addr,5);
	nRF24L01_Config(&nRF24_Cfg);	//配置nRF24L01
	nRF24L01_Rx_Mode();

	OLED12864_Clear_Page(1);
	OLED12864_Show_String(1,0,"nrf pass",1);
	OLED12864_Refresh();

	//创建RTOS初始化任务
	if(nrf_err == 0 & mpu_err == 0)
	{
		xTaskCreate(
			RTOSCreateTask_Task,
			"init",
			256,
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
	//航向角pid
	MemFill((uint8_t*)&Yaw_pid_Handle,0,sizeof(Yaw_pid_Handle));
	Yaw_pid_Handle.P = YAW_P;
	Yaw_pid_Handle.I = YAW_I;
	Yaw_pid_Handle.D = YAW_D;
	Yaw_pid_Handle.out_zoom = YAW_ZOOM;
	Yaw_pid_Handle.OutputMax = YAW_MAX;
	Yaw_pid_Handle.OutputMin = YAW_MIN;
	//系统状态sysStatus
    sysStatus.nrf_signal = 0;
	//建立 队列 信号量
    nRF24_ISRFlag = xSemaphoreCreateBinary();		//nrf外部中断标志,由isr给出
	nRF24_RecieveFlag = xSemaphoreCreateBinary();	//nrf发生接收中断标志
	nRF24_SendResult = xQueueCreate(1,1);			//nrf发送结果标志
    mpuDat_occFlag = xSemaphoreCreateMutex();		//mpu_data[3] 保护
    sysStatus_occFlag = xSemaphoreCreateMutex();	//sysStatus 保护
	Beep_CmdQueue = xQueueCreate(3,sizeof(BeepCtr_Type));
	//建立2个舵机控制任务
	for(uint8_t temp=0;temp<2;temp++)
	{
		STMotor_CmdQueue[temp] = xQueueCreate(3,sizeof(StreetMotorCtr_Type));
		STMotor_is[temp].queueAddr = &STMotor_CmdQueue[temp];
		STMotor_is[temp].streetMotor = streetMotor[temp];	//见hardware_def.h
		STMotor_is[temp].cycle = 40;	//25Hz执行频率
		STMotor_is[temp].angle_inc = 5.0f;
		xTaskCreate(
			StreetMotor_Task,
			"SM",
			64,
			(void*)&STMotor_is[temp],
			5,
			&StreetMotor_TaskHandle[temp]
		);
	}
	//建立2个直流电机控制任务
	for(uint8_t temp=0;temp<2;temp++)
	{
		DCMotor_CmdQueue[temp] = xQueueCreate(3,sizeof(DCMotorCtr_Type));	//创建命令接收队列
		DCMotor_is[temp].queueAddr = &DCMotor_CmdQueue[temp];				//设置命令接收队列地址
		DCMotor_is[temp].cycle = 20;	//50Hz执行频率
		DCMotor_is[temp].max_inc = 50;
		DCMotor_is[temp].a4950 = a4950[temp];	//见hardware_def.h
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
		ER_CmdQueue[temp] = xQueueCreate(3,sizeof(ERctr_Type));		//创建命令接收队列
		ER_is[temp].queueAddr = &ER_CmdQueue[temp];					//设置命令接收队列地址
		ER_is[temp].er = er[temp];	//见hardware_def.h
		ER_is[temp].cycle = 20;
		ER_is[temp].max_inc = 20;
		xTaskCreate(
        	ER_Task,
        	"ER",
        	72,
        	(void*)&ER_is[temp],
        	5,
        	&ER_TaskHandle[temp]
    	);
	}
	#if 1
	//建立nrf回复主机任务
    xTaskCreate(
        ReplyMaster_Task,
        "Reply",
        256,
        (void*)&nrf_maxDelay,
        11,
        &ReplyMaster_TaskHandle
    );
	#endif
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
        8,
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
	//建立水位检测任务
    xTaskCreate(
        DepthSensor_Task,
        "depth",
        48,
        &WaterLine_fre,
        9,
        &Voltage_TaskHandle
    );
	//建立电池电压检测任务
    xTaskCreate(
        Voltage_Task,
        "batvol",
        48,
        NULL,
        3,
        &Voltage_TaskHandle
    );
	//建立按键输入任务
    xTaskCreate(
        KeyInput_Task,
        "key",
        64,
        NULL,
        7,
        &KeyInput_TaskHandle
    );
	//建立蜂鸣器任务
	xTaskCreate(
		Beep_Task,
		"Beep",
		48,
		NULL,
		2,
		&Beep_TaskHandle
	);
	//鸣响,表示开始运行
	Beep_ON(Mu_Fre[4]);
	soft_delay_ms(800);
	Beep_OFF();
	//删除自身
    vTaskDelete(NULL);
}

//油门校准
void Er_Cal(void)
{
	//油门行程校准
	if(Key_Read(0) == Key_Press)
	{
		OLED12864_Show_String(0,0,"calibration",1);
		OLED12864_Refresh();
		soft_delay_ms(2000);
		ER_UndirOut(&er[0],0);
		ER_UndirOut(&er[1],0);
		ER_UndirOut(&er[2],0);
		ER_UndirOut(&er[3],0);
		while(Key_Read(0) != Key_Press);
		OLED12864_Show_String(1,0,"Set Max",1);
		OLED12864_Refresh();
		ER_UndirOut(&er[0],500);
		ER_UndirOut(&er[1],500);
		ER_UndirOut(&er[2],500);
		ER_UndirOut(&er[3],500);
		soft_delay_ms(300);
		while(Key_Read(0) != Key_Press);
		OLED12864_Show_String(1,0,"Set Med",1);
		OLED12864_Refresh();
		ER_UndirOut(&er[0],0);
		ER_UndirOut(&er[1],0);
		ER_UndirOut(&er[2],0);
		ER_UndirOut(&er[3],0);
		soft_delay_ms(300);
	}
}
