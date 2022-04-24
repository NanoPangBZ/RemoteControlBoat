#include "bsp_i2c.h"

typedef struct
{
    uint8_t delay;  //间接控制i2c频率,越低频率越高
    Pin SDA;
    Pin SCL;
}i2c_busType;

static i2c_busType i2c_Bus[1] = {
    {5,{MPU6050_IIC_SDA_Pin,MPU6050_IIC_SDA_GPIO} , {MPU6050_IIC_SCL_Pin,MPU6050_IIC_SCL_GPIO} }
};

#define SDA_H(device)   Pin_Set(i2c_Bus[device].SDA)
#define SDA_L(device)   Pin_Reset(i2c_Bus[device].SDA)
#define SCL_H(device)   Pin_Set(i2c_Bus[device].SCL)
#define SCL_L(device)   Pin_Reset(i2c_Bus[device].SCL)
#define SDA_read(device)	Pin_Read(i2c_Bus[device].SDA)

void i2c_Delay(i2c_device device)
{
	uint8_t i;
	uint8_t k = i2c_Bus[device].delay;
	for (i = 0; i < k; i++);
}

void BSP_i2c_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	for(uint8_t temp=0;temp<1;temp++)
	{
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_Pin = i2c_Bus[temp].SDA.Pin;
		GPIO_Init(i2c_Bus[temp].SDA.GPIO,&GPIO_InitStruct);

		GPIO_InitStruct.GPIO_Pin = i2c_Bus[temp].SCL.Pin;
		GPIO_Init(i2c_Bus[temp].SCL.GPIO,&GPIO_InitStruct);
	}

}

void I2C_Start(i2c_device device)
{
	SDA_H(device);
	SCL_H(device);
	i2c_Delay(device);
	SDA_L(device);
	i2c_Delay(device);
	SCL_L(device);
	i2c_Delay(device);
}

void I2C_Stop(i2c_device device)
{
	SDA_L(device);
	SCL_H(device);
	i2c_Delay(device);
	SDA_H(device);
}

void I2C_SendByte(i2c_device device,uint8_t byte)
{
	uint8_t i = 0;
	for(i=0;i<8;i++)
	{
		SCL_L(device);
		if(byte&0x80)
			{SDA_H(device);}
		else
			{SDA_L(device);}
		i2c_Delay(device);
		SCL_H(device);
		i2c_Delay(device);
		SCL_L(device);
		i2c_Delay(device);
		if(i==7)
			SDA_H(device);	//释放总线
		byte<<=1;	//左移一位
	}
}

uint8_t I2C_ReadByte(i2c_device device)
{
	uint8_t i = 0;
	uint8_t value = 0;
	for(i=0;i<8;i++)
	{
		value<<=1;	//左移一位
		SCL_H(device);
		i2c_Delay(device);
		if(SDA_read(device)==1)
			value++;
		SCL_L(device);
		i2c_Delay(device);
	}
	return value;
}

uint8_t I2C_WaitAck(i2c_device device)
{
	uint8_t re = 0;
	SDA_H(device);	/* CPU释放SDA总线 */
	i2c_Delay(device);
	SCL_H(device);	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	i2c_Delay(device);
	if (SDA_read(device))	/* CPU读取SDA口线状态 */
		re = 1;
	else
		re = 0;
	SCL_L(device);
	i2c_Delay(device);
	return re;
}

uint8_t I2C_NoWaitACK(i2c_device device)
{
	SDA_H(device);
	i2c_Delay(device);
	SCL_H(device);
	i2c_Delay(device);
	SCL_L(device);
	return 0;
}

void I2C_Ack(i2c_device device)
{
	SDA_L(device);
	i2c_Delay(device);
	SCL_H(device);
	i2c_Delay(device);
	SCL_L(device);
	i2c_Delay(device);
	SDA_H(device);
}

void I2C_Nack(i2c_device device)
{
	SDA_H(device);
	i2c_Delay(device);
	SCL_H(device);
	i2c_Delay(device);
	SCL_L(device);
	i2c_Delay(device);
}
