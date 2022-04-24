#ifndef _BSP_I2C_H_
#define _BSP_I2C_H_

#include "self_stm32f10x.h"

/***********************************************************************************
 * 软件模拟I2C
 * 依赖self_stm32f10x.h
 * 作者: 庞碧璋
 * Github: https://github.com/CodingBugStd
 * csdn:   https://blog.csdn.net/RampagePBZ
 * Encoding: utf-8
 * date: 2022/3/30
**********************************************************************************/

#define MPU6050_IIC_SDA_GPIO    GPIOA
#define MPU6050_IIC_SDA_Pin     GPIO_Pin_12
#define MPU6050_IIC_SCL_GPIO    GPIOA
#define MPU6050_IIC_SCL_Pin     GPIO_Pin_11

typedef enum
{
    MPU6050 = 0,
}i2c_device;

void BSP_i2c_Init(void);
void I2C_Start(i2c_device device);
void I2C_Stop(i2c_device device);
void I2C_SendByte(i2c_device device,uint8_t byte);
uint8_t I2C_ReadByte(i2c_device device);
uint8_t I2C_WaitAck(i2c_device device);
uint8_t I2C_NoWaitACK(i2c_device device);
void I2C_Ack(i2c_device device);
void I2C_Nack(i2c_device device);
void i2c_Delay(i2c_device device);

#endif

