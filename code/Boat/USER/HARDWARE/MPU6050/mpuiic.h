#ifndef __MPUIIC_H
#define __MPUIIC_H
#include "self_stm32f10x.h"

//IO方向设置  ---PB11
#define MPU_SDA_IN()  {GPIOA->CRH &= 0XFFF0FFFF;GPIOA->CRH |= 8<<16;}   //上拉/下拉 输入模式
#define MPU_SDA_OUT() {GPIOA->CRH &= 0XFFF0FFFF;GPIOA->CRH |= 3<<16;}	//推挽输出  输出模式

#define IIC_SDA_GPIO    GPIOA
#define IIC_SDA_Pin     GPIO_Pin_12
#define IIC_SCL_GPIO    GPIOA
#define IIC_SCL_Pin     GPIO_Pin_11
#define SDA 0
#define SCL 1

static Pin IIC_Pin[2] = {
    {IIC_SDA_Pin,IIC_SDA_GPIO},
    {IIC_SCL_Pin,IIC_SCL_GPIO}
};

//IIC所有操作函数
void MPU_IIC_Delay(void);								//IIC延时2ms函数
void MPU_IIC_Init(void);                //初始化IIC的IO口				 
void MPU_IIC_Start(void);								//发送IIC开始信号
void MPU_IIC_Stop(void);	  						//发送IIC停止信号
void MPU_IIC_Send_Byte(u8 txd);					//IIC发送一个字节
u8 MPU_IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 MPU_IIC_Wait_Ack(void); 							//IIC等待ACK信号
void MPU_IIC_Ack(void);									//IIC发送ACK信号
void MPU_IIC_NAck(void);								//IIC不发送ACK信号


void IMPU_IC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 MPU_IIC_Read_One_Byte(u8 daddr,u8 addr);	  

#endif
