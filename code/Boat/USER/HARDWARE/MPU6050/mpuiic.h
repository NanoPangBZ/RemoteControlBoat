#ifndef __MPUIIC_H
#define __MPUIIC_H

#include "BSP\bsp_i2c.h"

#define MPU_IIC_Delay() i2c_Delay(MPU6050)
#define MPU_IIC_Start() I2C_Start(MPU6050)
#define MPU_IIC_Stop()  I2C_Stop(MPU6050)
#define MPU_IIC_Send_Byte(dat)  I2C_SendByte(MPU6050,dat)   
#define MPU_IIC_Read()  I2C_ReadByte(MPU6050)
#define MPU_IIC_Wait_Ack()      I2C_WaitAck(MPU6050)
#define MPU_IIC_Ack()   I2C_Ack(MPU6050)
#define MPU_IIC_NAck()  I2C_Nack(MPU6050)
	 
u8 MPU_IIC_Read_Byte(unsigned char ack);//IIC读取一个字节,带回复

#endif
