#include "user.h"


//5ms调用一次
void nRF24L01_Test(void)
{
    static uint8_t sbuffer[32];
    uint8_t*usart_recieve;
    uint8_t len;
    uint8_t status;
    usart_recieve = Usart_Read(1);
    len = *usart_recieve;
    //若串口有接收到数据
    if(len != 0)
    {
        if(len > 32)
            len = 32;
        MemCopy(usart_recieve+1,sbuffer,len);
        USART_Clear(1);
        nRF24L01_Send(sbuffer,32);
        printf("nRF24L01 进入发射状态\r\n");
        return;
    }
    //判断nrf24是否有中断发生
    status = nRF24L01_Status()&0x0f;    //屏蔽低4位
    if(status != 0)
    {
        if(status & (0x01<<4))  //发生最大重发次数中断
        {
            prinf("未应答 - 发送失败\r\n");
            nRF24L01_Rx_Mode(); //进入接收模式
        }
        if(status & (0x01<<5))
        {
            printf("接收到应答信号 - 发送成功\r\n");
            nRF24L01_Rx_Mode(); //进入接收模式
        }
        if(status & (0x01<<6))  //RX FIFO接收到数据
        {
            printf("接收到数据\r\n");
            nRF24L01_InterruptHandle();
        }
    }
}
