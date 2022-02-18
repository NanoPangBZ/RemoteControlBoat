#include "bsp_spi.h"

/*******************************************************************
 * 功能:使用SPI交换1个字节
 * 参数:
 *  SPIx:   1 或 2 ,分别表示使用硬件SPI1和SPI2
 *  TxData: 使用SPI通讯时(主机模式)发送的字节
 * 返回值:  接收到的字节
 * 2021/10/16   庞碧璋
 *******************************************************************/
uint8_t SPI_Replace_Byte(uint8_t SPIx,uint8_t TxData)
{
    uint8_t retry=0;				 	
	while (SPI_I2S_GetFlagStatus(Target_SPI[SPIx - 1], SPI_I2S_FLAG_TXE) == RESET)
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(Target_SPI[SPIx - 1], TxData);
	retry=0;
	while (SPI_I2S_GetFlagStatus(Target_SPI[SPIx - 1], SPI_I2S_FLAG_RXNE) == RESET)
		{
		retry++;
		if(retry>200)return 0;
		}
	return SPI_I2S_ReceiveData(Target_SPI[SPIx - 1]);
}

/*******************************************************************
 * 功能:使用SPI交换多个字节,接收到的字节存储在buf数组中
 * 参数:
 *  SPIx:   1 或 2 ,分别表示使用硬件SPI1和SPI2
 *  dat:    使用SPI接收时(主机模式)发送的字节首地址
 *  len:    交换的长度
 * 返回值:  无
 * 备注:    传参时可以让dat=buf,节省空间
 * 2021/10/16   庞碧璋
 *******************************************************************/
void SPI_Replace_NumByte(uint8_t SPIx,uint8_t*dat,uint8_t len,uint8_t*buf)
{
    for(uint8_t temp=0;temp<len;temp++)
        buf[temp] = SPI_Replace_Byte(SPIx,dat[temp]);
}

/**************************************************************************
 * 功能:使用SPI发1个字节
 * 参数:
 *  SPIx:   1 或 2 ,分别表示使用硬件SPI1和SPI2
 *  TxData: 待发送数据的首地址
 * 返回值:  1 或 0,1表示发送失败,0表示发送成功
 * 备注:    SPI_Send_Byte()舍去了接收等待,以此加快发送速度,在波特率过低的情
 *          况下可能会出错!可以使用SPI_Replace_Byte()代替这个函数
 * 2021/10/16   庞碧璋
 ***********************************************************************/
uint8_t SPI_Send_Byte(uint8_t SPIx,uint8_t TxData)
{
    uint8_t retry=0;				 	
	while (SPI_I2S_GetFlagStatus(Target_SPI[SPIx - 1], SPI_I2S_FLAG_TXE) == RESET)
		{
		retry++;
		if(retry>200)return 1;
		}			  
	SPI_I2S_SendData(Target_SPI[SPIx - 1], TxData);
    return 0;
}


/**************************************************************************
 * 功能:使用SPI发送多个字节
 * 参数:
 *  SPIx:   1 或 2 ,分别表示使用硬件SPI1和SPI2
 *  dat:    待发送数据的首地址
 *  len:    发送的长度
 * 返回值:  无
 * 备注:    SPI_Send_Byte()舍去了接收等待,以此加快发送速度,在波特率过低的情
 *          况下可能会出错!可以使用SPI_Replace_NumByte()代替这个函数
 * 2021/10/16   庞碧璋
 ***********************************************************************/
void SPI_Send_NumByte(uint8_t SPIx,uint8_t*dat,uint8_t len)
{
    for(uint8_t temp=0;temp < len;temp++)
        SPI_Send_Byte(SPIx,dat[temp]);
}


/*******************************************************************
 * 功能:使用SPI接收多个字节,接收到的字节存储在buf数组中
 * 参数:
 *  SPIx:   1 或 2 ,分别表示使用硬件SPI1和SPI2
 *  dat:    使用SPI接收时(主机模式)发送的字节
 *  len:    接收的长度
 * 返回值:  无
 * 备注:    注意buf的长度
 * 2021/10/16   庞碧璋
 *******************************************************************/
void SPI_Recieve_NumByte(uint8_t SPIx,uint8_t dat,uint8_t len,uint8_t*buf)
{
    for(uint8_t temp=0;temp<len;temp++)
        buf[temp] = SPI_Replace_Byte(SPIx,dat);
}
