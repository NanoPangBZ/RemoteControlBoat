#include "main.h"

#pragma	diag_suppress	870	//屏蔽汉字警告

//nRF24L01初始化结构体
nRF24L01_Cfg nRF24_Cfg;
static uint8_t TxAddr[5] = {0x43,0x16,'R','C',0xFF};	//遥控器地址
static uint8_t RxAddr[5] = {0x43,0x16,'B','T',0xFF};	//船地址

//任务参数
uint8_t SendFre = 50;	//nrf24通讯频率
uint8_t RockerFre = 50;	//摇杆采样频率
uint8_t HMI_Fre = 20;	//串口屏幕刷新频率

//任务句柄
TaskHandle_t RTOS_CreatTask_TaskHandle = NULL;
TaskHandle_t Main_TaskHandle = NULL;
TaskHandle_t RemoteControl_TaskHandle = NULL;
TaskHandle_t nRF24L01_Intterrupt_TaskHandle = NULL;
TaskHandle_t User_FeedBack_TaskHandle = NULL;
TaskHandle_t Rocker_TaskHandle = NULL;
TaskHandle_t HMI_TaskHandle = NULL;

//队列句柄
SemaphoreHandle_t	nRF24_ISRFlag = NULL;		//nrf24硬件中断标志
SemaphoreHandle_t	nRF24_RecieveFlag = NULL;	//nrf24接收标志(数据已经进入单片机,等待处理)
QueueHandle_t		nRF24_SendResult = NULL;	//nrf24发送结果
SemaphoreHandle_t	boatGyroscope_occFlag = NULL;		//船只姿态数据占用标志(互斥信号量)

//全局变量
uint8_t nrf_signal = 0;		//nrf信号强度 0~10
float BoatGyroscope[3];			//船只返回的姿态 boatGyroscope_occFlag保护
float BoatVoltage;
uint8_t rockerInput[4];			//摇杆输入

void RTOS_CreatTask_Task(void*ptr);

int main(void)
{
	uint8_t nrf_err;

	//中断优先级分组 4位主优先级 0位子优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	//关闭JTAG调试接口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

	//串口初始化
	BSP_Usart_Init();
	BSP_ADC_Init();	//ADC初始化

	//串口屏初始化
	soft_delay_ms(500);
	//串口屏开机要一段时间
	//为了串口屏能完整的接收到复位数据
	//初始化两次
	HMI_Reset();
	HMI_Reset();
	soft_delay_ms(500);

	//nRF24L01初始化
	nrf_err = nRF24L01_Init();
	if(nrf_err)
	{
		HMI_Msg("nrf err");
	}else
	{
		//nRF24L01 相关配置
		nRF24_Cfg.Channel = 50;	//2.45GHz 通讯频段
		nRF24_Cfg.retry = 10;	//最大重发次数
		nRF24_Cfg.retry_cycle = 1;	//重发周期
		nRF24_Cfg.Rx_Length = 32;	//结束长度
		MemCopy(TxAddr,nRF24_Cfg.TX_Addr,5);
		MemCopy(RxAddr,nRF24_Cfg.RX_Addr,5);
		nRF24L01_Config(&nRF24_Cfg);	//配置nRF24L01	
		HMI_Msg("nrf pass");
		nRF24L01_Rx_Mode();
	}

	soft_delay_ms(1000);

	xTaskCreate(
		RTOS_CreatTask_Task,
		"CreatTask",
		144,
		NULL,
		15,
		&RTOS_CreatTask_TaskHandle
	);

	vTaskStartScheduler();
	while(1);
}

void RTOS_CreatTask_Task(void*ptr)
{
    nRF24_ISRFlag = xSemaphoreCreateBinary();		//创建nrf中断信号量 -> 由单片机(硬件)外部中断给出
	nRF24_RecieveFlag = xSemaphoreCreateBinary();	//创建nrf接收中断信号量 -> 由(软件)nrf中断处理函数给出
	nRF24_SendResult = xQueueCreate(1,1);			//创建nrf发送结果消息队列
	boatGyroscope_occFlag = xSemaphoreCreateMutex();	//创建船只姿态数据占用标志(互斥信号量)
	//建立遥控任务
    xTaskCreate(
		RemoteControl_Task,
		"RC task",
		512,
		(void*)&SendFre,
		11,
		&RemoteControl_TaskHandle
	);
	//建立摇杆值测量任务
	xTaskCreate(
		Rocker_Task,
		"RK",
		32,
		(void*)&RockerFre,
		8,
		&Rocker_TaskHandle
	);
	//建立nrf中断处理任务
	xTaskCreate(
		nRF24L01_Intterrupt_Task,
		"NI task",
		72,
		NULL,
		13,
		&nRF24L01_Intterrupt_TaskHandle
	);
	#if 1
	//建立串口反馈任务 -> 串口1
	xTaskCreate(
		User_FeedBack_Task,
		"UFB task",
		64,
		NULL,
		12,
		&User_FeedBack_TaskHandle
	);
	#endif
	#if 1
	//建立串口屏任务 -> 串口2
	xTaskCreate(
		HMI_Task,
		"HMI task",
		256,
		&HMI_Fre,
		9,
		&HMI_TaskHandle
	);
	#endif
	HMI_ClearMsg();
	HMI_Msg("系统成功启动");
    vTaskDelete(NULL);
}
