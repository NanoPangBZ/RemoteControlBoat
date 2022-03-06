#include "mpuiic.h"
 
//来自 : https://blog.csdn.net/lihaotian111/article/details/117307644

#define delay_us(us)    soft_delay_us(us)
#define SDA_Set()       Pin_Set(IIC_Pin[SDA])
#define SDA_Reset()     Pin_Reset(IIC_Pin[SDA])
#define SCL_Set()       Pin_Set(IIC_Pin[SCL])
#define SCL_Reset()     Pin_Reset(IIC_Pin[SCL])
#define MPU_READ_SDA    Pin_Read(IIC_Pin[SDA])

/**********************************************
函数名称：MPU_IIC_Delay
函数功能：MPU IIC延时函数，延时2ms
函数参数：无
函数返回值：无
**********************************************/
void MPU_IIC_Delay(void)
{
	delay_us(2);
}

/**********************************************
函数名称：MPU_IIC_Init
函数功能：MPU IIC初始化
函数参数：无
函数返回值：无
**********************************************/
void MPU_IIC_Init(void)
{					     
  GPIO_InitTypeDef  GPIO_InitStructure;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);			//先使能外设IO PORTB时钟 
		
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;	  //端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 				  //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				  //IO口速度为50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);					 					//根据设定参数初始化GPIO 
	
  GPIO_SetBits(GPIOA,GPIO_Pin_11|GPIO_Pin_12);						  //PB10,PB11 输出高	
}

/**********************************************
函数名称：MPU_IIC_Start
函数功能：MPU IIC发送起始信号
函数参数：无
函数返回值：无
**********************************************/
void MPU_IIC_Start(void)
{
	MPU_SDA_OUT();     //SDA线 输出
	SDA_Set();	  	  
	SCL_Set();
	MPU_IIC_Delay();
 	SDA_Reset();     //START:当SCL线处于高电平时,SDA线突然从高变低,发送起始信号
	MPU_IIC_Delay();
	SCL_Reset();		 //钳住I2C总线，准备发送或接收数据 
}

/**********************************************
函数名称：MPU_IIC_Stop
函数功能：MPU IIC发送停止信号
函数参数：无
函数返回值：无
**********************************************/
void MPU_IIC_Stop(void)
{
	MPU_SDA_OUT();		 //SDA线输出
	SCL_Reset();
	SDA_Reset();		 //STOP:当SCL线处于高电平时,SDA线突然从低变高,发送停止信号
 	MPU_IIC_Delay();
	SCL_Set(); 
	SDA_Set();		 //发送I2C总线结束信号
	MPU_IIC_Delay();							   	
}

/**********************************************
函数名称：MPU_IIC_Wait_Ack
函数功能：MPU IIC等待信号到来
函数参数：无
函数返回值：1:接收应答信号成功  0:接收应答信号失败
**********************************************/
u8 MPU_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	MPU_SDA_IN();  //SDA设置为输入  
	SDA_Set();MPU_IIC_Delay();
	SCL_Set();MPU_IIC_Delay();
	while(MPU_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			MPU_IIC_Stop();
			return 1;
		}
	}
	SCL_Reset();//时钟输出0
	return 0;
} 

/**********************************************
函数名称：MPU_IIC_Ack
函数功能：MPU IIC产生应答信号
函数参数：无
函数返回值：无
**********************************************/
void MPU_IIC_Ack(void)
{
	SCL_Reset();
	MPU_SDA_OUT();
	SDA_Reset();
	MPU_IIC_Delay();
	SCL_Set();
	MPU_IIC_Delay();
	SCL_Reset();
}

/**********************************************
函数名称：MPU_IIC_NAck
函数功能：MPU IIC不产生应答信号
函数参数：无
函数返回值：无
**********************************************/   
void MPU_IIC_NAck(void)
{
	SCL_Reset();
	MPU_SDA_OUT();
	SDA_Set();
	MPU_IIC_Delay();
	SCL_Set();
	MPU_IIC_Delay();
	SCL_Reset();
}

/**********************************************
函数名称：MPU_IIC_Send_Byte
函数功能：MPU IIC发送一个字节
函数参数：txd：要发送的数据
函数返回值：无
注意：IIC发送字节是一个一个位发送的，发送一个字节需要发送八次
**********************************************/
void MPU_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
		MPU_SDA_OUT(); 	    
    SCL_Reset();		//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        if((txd&0x80)>>7)
        {
            SDA_Set();
        }else
        {
            SDA_Reset();
        }
        //MPU_IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		    SCL_Set();
		    MPU_IIC_Delay(); 
		    SCL_Reset();	
		    MPU_IIC_Delay();
    }	 
} 	    

/**********************************************
函数名称：MPU_IIC_Read_Byte
函数功能：MPU IIC读取一个字节
函数参数：ack: 1,发送ACK   0,发送NACK 
函数返回值：接收到的数据
注意：IIC读取字节是一个一个位读取的，读取一个字节需要读取八次
**********************************************/ 
u8 MPU_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
  MPU_SDA_IN();              //SDA设置为输入
    for(i=0;i<8;i++)
	  {
        SCL_Reset();
        MPU_IIC_Delay();
				SCL_Set();
        receive<<=1;
        if(MPU_READ_SDA)receive++;   //如果读到了数据
				MPU_IIC_Delay(); 
    }					 
    if (!ack)
        MPU_IIC_NAck();   //发送nACK
    else
        MPU_IIC_Ack();    //发送ACK   
    return receive;
}
