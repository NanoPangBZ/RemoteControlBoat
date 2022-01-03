#include "bsp_usart.h"

void BSP_Usart_Init(void)
{
    USART_GPIO_Init();
    USART_Config();
    USART_NVIC_Config();
    USART_DMA_Config();
    USART_Cmd(USART1,ENABLE);
}

void USART_GPIO_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

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

    USART_InitStruct.USART_BaudRate = 115200;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1,&USART_InitStruct);

}

void USART_NVIC_Config(void)
{
    NVIC_InitTypeDef    NVIC_InitStruct;

    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_Init(&NVIC_InitStruct);

    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);

    USART_ClearITPendingBit(USART1,USART_IT_RXNE);
}

void USART_DMA_Config(void)
{
    DMA_InitTypeDef DMA_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

    DMA_InitStruct.DMA_BufferSize = 0;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)USART_Tx_Sbuffer;

    DMA_Init(Usart_Tx_DMA_Channel,&DMA_InitStruct);
    USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
    DMA_ClearFlag(DMA1_FLAG_TC4);

    NVIC_InitTypeDef    NVIC_InitStruct;

    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 8;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

    NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    NVIC_Init(&NVIC_InitStruct);

    DMA_ITConfig(Usart_Tx_DMA_Channel,DMA_IT_TC,ENABLE);
}

void USART_Push(uint8_t len)
{
    if(USART_Rx_Sbuffer[0] > len)
    {
        uint8_t temp;
        USART_Rx_Sbuffer[0] -= len;
        for(temp=0; temp<USART_Rx_Sbuffer[0] ; temp++)
            USART_Rx_Sbuffer[temp+1] = USART_Rx_Sbuffer[temp+1+len];
        return;
    }
    USART_Clear();
}

void USART_Clear(void)
{
    USART_Rx_Sbuffer[0] = 0;
}

uint8_t Usart_Send(uint8_t *dat,uint8_t len)
{
    if(Usart_BusyCheck()==0)
    {
        uint8_t temp;
        USART_Tx_Sbuffer[0] = len;
        for(temp=0;temp<len;temp++)
            USART_Tx_Sbuffer[temp+1] = *(dat+temp);
        Usart_Tx_DMA_Channel->CNDTR = len;
        Usart_Tx_DMA_Channel->CCR |= DMA_CCR1_EN;
        return 0;
    }
    return 1;
}

uint8_t Usart_SendString(uint8_t*str)
{
    if(Usart_BusyCheck() == 0)
    {
        while(*str!='\0' && Tx_Len < Usart_Tx_SbufferSize)
        {
            USART_Tx_Sbuffer[Tx_Len + 1] = *str;
            USART_Tx_Sbuffer[0]++;
            str++;
        }
        Usart_Tx_DMA_Channel->CNDTR = Tx_Len;
        Usart_Tx_DMA_Channel->CCR |= DMA_CCR1_EN;
        return 0;
    }
    return 1;
}

uint8_t Usart_BusyCheck(void)
{
    if( USART_Tx_Sbuffer[0] != 0)
        return 1;
    else
        return 0;
}

uint8_t*Usart_Read(void)
{
    return USART_Rx_Sbuffer;
}

void Rx_SbufferInput(uint8_t dat)
{
    //判断缓存区是否满载
    if(USART_Rx_Sbuffer[0] < Usart_Rx_SbufferSize)
    {
        USART_Rx_Sbuffer[ Rx_Len + 1 ] = dat;
        USART_Rx_Sbuffer[0]++;
    }else
        USART_Rx_Sbuffer[0] = Usart_Rx_SbufferSize + 1;
}

void Usart_Tx_Flag_Clear(void)
{
    USART_Tx_Sbuffer[0] = 0;
}

int fputc (int c, FILE *fp)
{
    while(Usart_BusyCheck());
	USART_SendData(USART1,c);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
	return c;
}


/******************************ISR**************************************/
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
    {
        Rx_SbufferInput(USART_ReceiveData(USART1));
        USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    }
}

void DMA1_Channel4_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC4) == SET)
    {
        Usart_Tx_Flag_Clear();
        Usart_Tx_DMA_Channel->CCR &= (uint16_t)(~DMA_CCR1_EN);
        DMA_ClearITPendingBit(DMA1_IT_TC4);
    }
}

