#include "bsp_usart.h"

void BSP_Usart_Init(void)
{
    USART_GPIO_Init();
    USART_Config();
    USART_NVIC_Config();
    for(uint8_t temp=0;temp<2;temp++)
        USART_Cmd(Target_Usart[temp],ENABLE);
}

void USART_GPIO_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_2;
    GPIO_Init(GPIOA,&GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_3;
    GPIO_Init(GPIOA,&GPIO_InitStruct);
}

void USART_Config(void)
{
    USART_InitTypeDef   USART_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

    USART_InitStruct.USART_BaudRate = 115200;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;

    for(uint8_t temp=0;temp<2;temp++)
        USART_Init(Target_Usart[temp],&USART_InitStruct);

}

void USART_NVIC_Config(void)
{
    NVIC_InitTypeDef    NVIC_InitStruct;

    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_Init(&NVIC_InitStruct);
    NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
    NVIC_Init(&NVIC_InitStruct);

    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
    USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);

    USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    USART_ClearITPendingBit(USART3,USART_IT_RXNE);
}

void USART_Push(uint8_t USARTx,uint8_t len)
{
    if(USART_Rx_Sbuffer[USARTx-1][0] > len)
    {
        uint8_t temp;
        USART_Rx_Sbuffer[USARTx-1][0] -= len;
        for(temp=0; temp<USART_Rx_Sbuffer[USARTx-1][0] ; temp++)
            USART_Rx_Sbuffer[USARTx-1][temp+1] = USART_Rx_Sbuffer[USARTx-1][temp+1+len];
        return;
    }
    USART_Clear(USARTx);
}

void USART_Clear(uint8_t USARTx)
{
    USART_Rx_Sbuffer[USARTx-1][0] = 0;
}

/*******************************************************************
 * 功能:直接获取串口接收缓存区的首地址
 * 参数:
 *  USARTx:对应串口号 1,2,3,4
 * 返回值:
 *  对应串口的接收缓存区首地址
 * 备注:
 *  这个函数直接返回地址,相对拷贝缓存的方式读取接收,速度更快
 *  注意:缓存区首个字节表示当前缓存的字节数
 *  这个函数不会自动清除缓存
 * 2021/5   庞碧璋
 *******************************************************************/
uint8_t*Usart_Read(uint8_t USARTx)
{
    return USART_Rx_Sbuffer[USARTx-1];
}

/*******************************************************************
 * 功能:拷贝串口接收缓存区
 * 参数:
 *  USARTx:对应串口号 1,2,3,4
 *  buf:拷贝缓存
 *  len:长度
 * 返回值:
 *  0:拷贝成功
 *  1:缓存区有效数据小于len
 * 备注:
 *  这个函数不会自动清除缓存
 *  MemCopy->内存拷贝函数(外部)
 * 2021/5   庞碧璋
 *******************************************************************/
uint8_t Usart_RxCopy(uint8_t USARTx,uint8_t*buf,uint8_t len)
{
    if(len > Rx_Len(USARTx))
        return 1;
    MemCopy(USART_Rx_Sbuffer[USARTx-1]+1,buf,len);
    return 0;
}

/*******************************************************************
 * 功能:读取串口接收缓存区
 * 参数:
 *  USARTx:对应串口号 1,2,3,4
 *  buf:拷贝缓存
 *  len:长度
 * 返回值:
 *  0:拷贝成功
 *  1:缓存区有效数据小于len
 * 备注:
 *  这个函数会自动清除读取到的缓存
 *  MemCopy->内存拷贝函数(外部)
 * 2022/1   庞碧璋
 *******************************************************************/
uint8_t Usart_RxGet(uint8_t USARTx,uint8_t*buf,uint8_t len)
{
    if(len > Rx_Len(USARTx))
        return 1;
    MemCopy(USART_Rx_Sbuffer[USARTx-1]+1,buf,len);
    USART_Push(USARTx,len);
    return 0;
}

void Rx_SbufferInput(uint8_t USARTx,uint8_t dat)
{
    //判断缓存区是否满载
    if(USART_Rx_Sbuffer[USARTx-1][0] < Rx_SbufferSize)
    {
        USART_Rx_Sbuffer[USARTx-1][ Rx_Len(USARTx) + 1 ] = dat;
        USART_Rx_Sbuffer[USARTx-1][0]++;
    }else
        USART_Rx_Sbuffer[USARTx-1][0] = Rx_SbufferSize + 1;
}

int fputc (int c, FILE *fp)
{
	USART_SendData(USART1,c);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
	return c;
}

/******************************ISR**************************************/
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
    {
        Rx_SbufferInput(1,USART_ReceiveData(USART1));
        USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    }
}


void USART3_IRQHandler(void)
{
    if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET)
    {
        Rx_SbufferInput(2,USART_ReceiveData(USART3));
        USART_ClearITPendingBit(USART3,USART_IT_RXNE);
    }
}
