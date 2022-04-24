#include "mpuiic.h"
 
//来自 : https://blog.csdn.net/lihaotian111/article/details/117307644

/**********************************************
函数名称：MPU_IIC_Read_Byte
函数功能：MPU IIC读取一个字节
函数参数：ack: 1,发送ACK   0,发送NACK 
函数返回值：接收到的数据
注意：IIC读取字节是一个一个位读取的，读取一个字节需要读取八次
**********************************************/ 
u8 MPU_IIC_Read_Byte(unsigned char ack)
{
  uint8_t receive;
  receive = MPU_IIC_Read();
  if (!ack)
    MPU_IIC_NAck();   //发送nACK
  else
    MPU_IIC_Ack();    //发送ACK   
  return receive;
}
