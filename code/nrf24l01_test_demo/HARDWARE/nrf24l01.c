#include "nrf24l01.h"

static void  spiInit(void);                          // spi通信配置



//对接self_stm32f10x.h
#define CE_PIN_LOW      Pin_Reset(nRF24L01_Pin[NRF24L01_CE])
#define CE_PIN_HIGH     Pin_Set(nRF24L01_Pin[NRF24L01_CE])
#define CS_PIN_HIGH     Pin_Set(nRF24L01_Pin[NRF24L01_CSN])
#define CS_PIN_LOW      Pin_Reset(nRF24L01_Pin[NRF24L01_CSN])

//对接bsp_spi.h
#include "bsp_spi.h"
#define nRF24L01_SPI_Send(dat)  SPI_Replace_Byte(1,dat)

/*****************************************************************************
 * 函  数： NRF24L01_Init
 * 功  能： NRF24L01初始化
 *         初始化后，模块处于接收状态，监听空中的信号  -> 以删减,待机模式
 * 参  数： 
 * 返回值： 
 * 备  注： 魔女开发板团队  资料存放Q群：262901124        最后修改_2021年07月16日
*****************************************************************************/
u8 NRF24L01_Init(void)
{        
    GPIO_InitTypeDef  GPIO_InitStructure;         

    // 使能SPI时钟
    if(NRF24L01_SPIx==SPI1)    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN ;   // SPI时钟，外设时钟要在GPIO配置前使能，要理解这种能耗安排的思想
    if(NRF24L01_SPIx==SPI2)    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN ;
    //if(NRF24L01_SPIx==SPI3)    RCC->APB1ENR |= RCC_APB1ENR_SPI3EN ;            
    // 使能GPIO时钟            
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPDEN | RCC_APB2ENR_IOPEEN ;          
    
    // 第3脚：CE
    GPIO_InitStructure.GPIO_Pin   = NRF24L01_CE_PIN ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_Init(NRF24L01_CE_GPIO, &GPIO_InitStructure);
    CE_PIN_LOW ;
    // 第4脚：CS
    GPIO_InitStructure.GPIO_Pin   = NRF24L01_CSN_PIN ; 
    GPIO_Init(NRF24L01_CSN_GPIO, &GPIO_InitStructure);  
    CS_PIN_HIGH;                                                   // 初始化后，先拉高，失能设备NRF通信，再初始化其它通信引脚  
    // 第5脚：SCK
    GPIO_InitStructure.GPIO_Pin  = NRF24L01_SCK_PIN ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP ;
    GPIO_Init(NRF24L01_SCK_GPIO , &GPIO_InitStructure );
    // 第6脚：MOSI
    GPIO_InitStructure.GPIO_Pin  = NRF24L01_MOSI_PIN ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP ;
    GPIO_Init(NRF24L01_MOSI_GPIO , &GPIO_InitStructure );    
    // 第7脚：MISO
    GPIO_InitStructure.GPIO_Pin  = NRF24L01_MISO_PIN ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP ;
    GPIO_Init(NRF24L01_MISO_GPIO , &GPIO_InitStructure );
    // 第8脚：IRQ
    GPIO_InitStructure.GPIO_Pin   = NRF24L01_IRQ_PIN ;     // 闲时高电平，中断时会被拉低
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_Init(NRF24L01_IRQ_GPIO, &GPIO_InitStructure);
    // 中断配置
    #if 0
    EXTI_InitTypeDef    EXTI_InitStruct;

    EXTI_InitStruct.EXTI_Line = EXTI_Line0;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;

    EXTI_StructInit(&EXTI_InitStruct);

    NVIC_InitTypeDef    NVIC_InitStruct;

    NVIC_InitStruct.NVIC_IRQChannel = NRF24L01_IRQ_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStruct);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource0);
    #endif
        
    spiInit ();                                            // SPI通信部分, 独立成一个函数，方便多次重复调用。因为SPI可以控制多个不同设备，怕a设备初始化后，又被b设备初始化成其配置。  
    return 0;
}

/*****************************************************************************
*函  数： spiInit
*功  能： SPI 配置
*        外部函数每次被调用，先用本函数重新配置1次SPI, 避免多个设备共用同一SPI配置
*参  数： 
*返回值： 
*****************************************************************************/
static void spiInit(void)
{
    CS_PIN_HIGH;                   // 失能NRF
    NRF24L01_SPIx->CR1  = 0;       // 清0
    NRF24L01_SPIx->CR1 |= 0<<0;    // 采样沿数, NRF要求上升沿采样   CPHA:时钟相位,0x1=在第2个时钟边沿进行数据采样， 
    NRF24L01_SPIx->CR1 |= 0<<1;    // 时钟线闲时极性,  CPOL:时钟极性,0x1=空闲状态时，SCK保持高电平
    NRF24L01_SPIx->CR1 |= 1<<2;    // 主从模式,       0=从，1=主
    NRF24L01_SPIx->CR1 |= 3<<3;    // 波特率控制[5:3], 0=fPCLK/2,  1=/4倍  2=/8  3/16
    NRF24L01_SPIx->CR1 |= 0<<7;    // LSB先行，        0=MSB,  1=LSB
    NRF24L01_SPIx->CR1 |= 1<<8;    // 内部从器件选择,根据9位设置(失能内部NSS)
    NRF24L01_SPIx->CR1 |= 1<<9;    // 软件从器件管理 :  0=禁止软件管理从设备， 1=使能软件从器件管理(软件NSS)
    NRF24L01_SPIx->CR1 |= 0<<11;   // 数据帧格式,       0=8位,  1=16位    
    NRF24L01_SPIx->CR1 |= 1<<6;    // 使能  
}

uint8_t nRF24L01_Read_Status(void)
{
    uint8_t status;
    CS_PIN_LOW;
    status = nRF24L01_SPI_Send(0xff);
    CS_PIN_HIGH;
    return status;
}

void NRF24L01_IRQ_IRQHANDLER(void)
{

}
