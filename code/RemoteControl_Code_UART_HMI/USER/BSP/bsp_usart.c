#include "bsp_usart.h"

void BSP_Usart_Init(void)
{
    USART_GPIO_Init();
    USART_Config();
    USART_NVIC_Config();
    USART_DMA_Config();
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
    NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
    NVIC_Init(&NVIC_InitStruct);

    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
    USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);

    USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    USART_ClearITPendingBit(USART2,USART_IT_RXNE);
}

void USART_DMA_Config(void)
{
    DMA_InitTypeDef DMA_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2,ENABLE);

    DMA_InitStruct.DMA_BufferSize = 0;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;

    for(uint8_t temp=0;temp<2;temp++)
    {
        DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&Target_Usart[temp]->DR;
        DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&USART_Tx_Sbuffer[temp][1];

        DMA_Init(TargetDMA_Channel[temp],&DMA_InitStruct);
        USART_DMACmd(Target_Usart[temp],USART_DMAReq_Tx,ENABLE);
        DMA_ClearFlag(DMA1_FLAG_TC1);
    }

    NVIC_InitTypeDef    NVIC_InitStruct;

    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 8;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

    NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    NVIC_Init(&NVIC_InitStruct);
    NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel7_IRQn;
    NVIC_Init(&NVIC_InitStruct);

    for(uint8_t temp=0;temp<2;temp++)
        DMA_ITConfig(TargetDMA_Channel[temp],DMA_IT_TC,ENABLE);
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

uint8_t Usart_Send(uint8_t USARTx,uint8_t *dat,uint8_t len)
{
    if(Usart_BusyCheck(USARTx)==0)
    {
        uint8_t temp;
        USART_Tx_Sbuffer[USARTx-1][0] = len;
        for(temp=0;temp<len;temp++)
            USART_Tx_Sbuffer[USARTx-1][temp+1] = *(dat+temp);
        TargetDMA_Channel[USARTx-1]->CNDTR = len;
        TargetDMA_Channel[USARTx-1]->CCR |= DMA_CCR1_EN;
        return 0;
    }
    return 1;
}

uint8_t Usart_SendString(uint8_t USARTx,uint8_t*str)
{
    if(Usart_BusyCheck(USARTx) == 0)
    {
        while(*str!='\0' && Tx_Len(USARTx) < Tx_SbufferSize)
        {
            USART_Tx_Sbuffer[USARTx-1][Tx_Len(USARTx)+1] = *str;
            USART_Tx_Sbuffer[USARTx-1][0]++;
            str++;
        }
        TargetDMA_Channel[USARTx-1]->CNDTR = Tx_Len(USARTx);
        TargetDMA_Channel[USARTx-1]->CCR |= DMA_CCR1_EN;
        return 0;
    }
    return 1;
}
uint8_t Usart_BusyCheck(uint8_t USARTx)
{
    if(USART_Tx_Sbuffer[USARTx-1][0] != 0)
        return 1;
    else
        return 0;
}

uint8_t*Usart_Read(uint8_t USARTx)
{
    return USART_Rx_Sbuffer[USARTx-1];
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

void Tx_Flag_Clear(uint8_t USARTx)
{
    USART_Tx_Sbuffer[USARTx-1][0] = 0;
}

int fputc (int c, FILE *fp)
{
    while(Usart_BusyCheck(1));
	USART_SendData(USART1,c);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
	return c;
}


/******************************ISR**************************************/
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
    {
        //是否使用串口传透
        #if 1
            Rx_SbufferInput(1,USART_ReceiveData(USART1));
        #else
            while(Usart_BusyCheck(2));
            USART_SendData(USART2,USART_ReceiveData(USART1));
            while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);
        #endif
        USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    }
}


void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2,USART_IT_RXNE) == SET)
    {
        //是否使用串口传透
        #if 1
            Rx_SbufferInput(2,USART_ReceiveData(USART2));
        #else
            while(Usart_BusyCheck(1));
            USART_SendData(USART1,USART_ReceiveData(USART2));
            while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
        #endif
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);
    }
}

void DMA1_Channel4_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC4) == SET)
    {
        Tx_Flag_Clear(1);
        TargetDMA_Channel[0]->CCR &= (uint16_t)(~DMA_CCR1_EN);
        DMA_ClearITPendingBit(DMA1_IT_TC4);
    }
}

void DMA1_Channel7_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC7) == SET)
    {
        Tx_Flag_Clear(2);
        TargetDMA_Channel[1]->CCR &= (uint16_t)(~DMA_CCR1_EN);
        DMA_ClearITPendingBit(DMA1_IT_TC7);
    }
}
