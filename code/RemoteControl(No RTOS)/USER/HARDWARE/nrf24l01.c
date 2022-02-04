#include "nrf24l01.h"
#include "nrf24l01_micro.h"
#include <stdio.h>

#define CE_LOW  Pin_Reset(nRF24L01_PIN[NRF24L01_CE])
#define CE_HIGH Pin_Set(nRF24L01_PIN[NRF24L01_CE])
#define CS_LOW  Pin_Reset(nRF24L01_PIN[NRF24L01_CS])
#define CS_HIGH Pin_Set(nRF24L01_PIN[NRF24L01_CS])

#include "BSP\bsp_spi.h"
#define port_Send(dat)      SPI_Replace_Byte(2,dat)
#define port_delay_ms(ms)   soft_delay_ms(ms)

static void spiInit(void);

/*******************************************************************
 * 功能:初始化nRF24L01,并且进入RxMode模式
 * 参数:无
 * 返回值:
 *  0:设备正常
 *  1:设备故障
 * 备注:
 *  会配置默认地址
 * 2021/12/26   庞碧璋
 *******************************************************************/
uint8_t nRF24L01_Init(void)
{        
    GPIO_InitTypeDef  GPIO_InitStructure;
    nRF24L01_Cfg    DefaultCfg;
    uint8_t err = 0;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);        
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);      
    
    
    GPIO_InitStructure.GPIO_Pin   = NRF24L01_CE_PIN ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_Init(NRF24L01_CE_GPIO, &GPIO_InitStructure);
    CE_LOW;

    GPIO_InitStructure.GPIO_Pin   = NRF24L01_CSN_PIN ; 
    GPIO_Init(NRF24L01_CSN_GPIO, &GPIO_InitStructure);  
    CS_HIGH;                                                   // 初始化后，先拉高，失能设备NRF通信，再初始化其它通信引脚  

    GPIO_InitStructure.GPIO_Pin  = NRF24L01_SCK_PIN | NRF24L01_MISO_PIN | NRF24L01_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP ;
    GPIO_Init(NRF24L01_SCK_GPIO , &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = NRF24L01_IQR_PIN;
    GPIO_Init(NRF24L01_IQR_GPIO,&GPIO_InitStructure);

    //IRQ脚中断配置
    NVIC_InitTypeDef    NVIC_InitSrtuct;
    EXTI_InitTypeDef    EXTI_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

    NVIC_InitSrtuct.NVIC_IRQChannel = NRF24L01_IQR_Channel;
    NVIC_InitSrtuct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitSrtuct.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitSrtuct.NVIC_IRQChannelSubPriority = 0;

    NVIC_Init(&NVIC_InitSrtuct);

    EXTI_InitStruct.EXTI_Line = NRF24L01_IQR_Line;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;    //下降沿

    EXTI_Init(&EXTI_InitStruct);

    spiInit();

    //默认配置
    DefaultCfg.Channel = DEFAULT_Channel;
    DefaultCfg.retry = DEFAULT_RETRY;
    DefaultCfg.retry_cycle = DEFAULT_RETRY_CYCLE;
    DefaultCfg.Rx_Length = DEFAULT_Rx_Length;
    
    MemCopy(DEFAULT_RxAddr,DefaultCfg.RX_Addr,5);
    MemCopy(DEFAULT_TxAddr,DefaultCfg.TX_Addr,5);
    nRF24L01_Config(&DefaultCfg);

    nRF24L01_Write_Reg(CONFIG,0x0f);    //启动,进入standby模式      

    err = nRF24L01_Check();

    if(err == 0)
    {
        nRF24L01_Rx_Mode();
        GPIO_EXTILineConfig(NRF24L01_IQR_SourceGPIO,NRF24L01_IQR_PinSource);    //中断线配置    检测无误后才使能这个中断
    }

    return err;
}

static void spiInit(void)
{
    CS_HIGH;                       // 失能NRF
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

/*******************************************************************
 * 功能:向nRF24L01发送命令
 * 参数:命令
 * 返回值:
 *  nRF24L01的status寄存器的值
 * 2021/12/26   庞碧璋
 *******************************************************************/
uint8_t nRF24L01_Send_Cmd(uint8_t cmd)
{
    uint8_t status;
    CS_LOW;
    status = port_Send(cmd);
    CS_HIGH;
    return status;
}

/*******************************************************************
 * 功能:向nRF24L01发送命令,并且跟随一段数据
 * 参数:
 *  cmd:命令
 *  buf:跟随的字节
 *  len:跟随的字节的长度
 * 返回值:
 *  nRF24L01的status寄存器的值
 * 2021/12/26   庞碧璋
 *******************************************************************/
uint8_t nRF24L01_Send_CmdAndData(uint8_t cmd,uint8_t*buf,uint8_t len)
{
    uint8_t status;
    CS_LOW;
    port_Send(cmd);
    for(uint8_t temp=0;temp<len;temp++)
        status = port_Send(buf[temp]);
    CS_HIGH;
    return status;
}

/*******************************************************************
 * 功能:读nRF24L01的寄存器
 * 参数:寄存器地址
 * 返回值:addr寄存器中的值
 * 2021/12/26   庞碧璋
 *******************************************************************/
uint8_t nRF24L01_Read_Reg(uint8_t addr)
{
    uint8_t reg_val;
    CS_LOW;
    port_Send(addr);
    reg_val = port_Send(0xFF);
    CS_HIGH;        
    return reg_val;
}

/*******************************************************************
 * 功能:读nRF24L01的寄存器(多字节寄存器)
 * 参数:
 *  addr:寄存器地址
 *  buf:读缓存
 *  len:长度
 * 返回值:
 *  nRF24L01的status寄存器值
 * 2021/12/26   庞碧璋
 *******************************************************************/
uint8_t nRF24L01_Read_Buf(uint8_t addr,uint8_t*buf,uint8_t len)
{
    uint8_t status;    
    CS_LOW;
    status = port_Send(addr);
    for(uint8_t i = 0; i<len ;i++){
        buf[i] = port_Send(0xFF);        
    }
    CS_HIGH;     
    return status;
}

/*******************************************************************
 * 功能:写nRF24L01的addr寄存器
 * 参数:
 *  addr:地址
 *  dat:写入的字节
 * 返回值:
 *  nRF24L01的status寄存器值
 * 2021/12/26   庞碧璋
 *******************************************************************/
uint8_t nRF24L01_Write_Reg(uint8_t addr,uint8_t dat)
{
    uint8_t status;
    CS_LOW;
    status = port_Send(addr|0x20);
    port_Send(dat);
    CS_HIGH;
    return status;
}

/*******************************************************************
 * 功能:向nRF24L01的addr寄存器中写入len个字节
 * 参数:
 *  addr:地址
 *  buf:要写入的字节
 *  len:长度
 * 返回值:
 *  nRF24L01的status寄存器值
 * 2021/12/26   庞碧璋
 *******************************************************************/
uint8_t nRF24L01_Write_Buf(uint8_t addr,uint8_t*buf,uint8_t len)
{
    uint8_t status;
    CS_LOW;
    status = port_Send(addr|0x20);
    for(uint8_t temp=0;temp<len;temp++)
        port_Send(buf[temp]);
    CS_HIGH;
    return status;
}

/*******************************************************************
 * 功能:nRF24L01检测,并且进入standby模式
 * 参数:无
 * 返回值:
 *  0:设备正常
 *  1:设备故障
 * 备注:这个函数会清空RX和TX的FIFO寄存器
 * 2021/12/26   庞碧璋
 *******************************************************************/
uint8_t nRF24L01_Check(void)
{
    //先读取寄存器,取反后写入
    //再次读出,查看是否更改
    //最后再取放
    uint8_t TestAddr[5] = {"Test"};
    uint8_t TestSbuffer[5];
    uint8_t sbuffer[5];
    uint8_t err = 0;
    CE_LOW;     //进入standby模式
    nRF24L01_Read_Buf(RX_ADDR_P0,sbuffer,5);
    nRF24L01_Write_Buf(RX_ADDR_P0,TestAddr,5);
    nRF24L01_Read_Buf(RX_ADDR_P0,TestSbuffer,5);
    for(uint8_t temp=0;temp<5;temp++)
    {
        if(TestSbuffer[temp] != TestAddr[temp])
        {
            err = 1;
        }
    }
    nRF24L01_Write_Buf(RX_ADDR_P0,sbuffer,5);
    return err;
}

//43.16.66.66.78

/*******************************************************************
 * 功能:    配置nRF24L01
 * 参数:
 *  nRF24L01_Cfg:
 *      Channel:频道,2.4G + Channel Mhz
 *      retry:自动重发次数 0~16
 *      retry_cycle:自动重发间隔 0~16 (单位250us)
 *      RX_Addr:接收地址 5Byte
 *      TX_Addr:发送地址 5Byte
 * 返回值:  nRF24L01的status寄存器值
 * 备注:会使nRF24L01进入standby模式
 * 2021/12/26   庞碧璋
 *******************************************************************/
uint8_t nRF24L01_Config(nRF24L01_Cfg*Cfg)
{
    //复制配置
    MemCopy((uint8_t*)Cfg,(uint8_t*)&CurrentCfg,sizeof(nRF24L01_Cfg));

    //条件限制
    if(CurrentCfg.Channel > 125)
        CurrentCfg.Channel = 125;
    if(CurrentCfg.Rx_Length > 32)
        CurrentCfg.Rx_Length = 32;
    if( CurrentCfg.retry > 15 )
        CurrentCfg.retry = 15;
    if( CurrentCfg.retry_cycle > 15)
        CurrentCfg.retry_cycle = 15;

    CE_LOW;

    //配置自动重发  SETUP_RETR
    nRF24L01_Write_Reg(SETUP_RETR,(CurrentCfg.retry_cycle<<4) | CurrentCfg.retry );
    //配置频道      RF_CH
    nRF24L01_Write_Reg(RF_CH,CurrentCfg.Channel);

    //设置接收长度
    nRF24L01_Write_Reg(RX_PW_P1,CurrentCfg.Rx_Length);

    nRF24L01_Write_Buf(RX_ADDR_P1,CurrentCfg.RX_Addr,5);
    nRF24L01_Write_Buf(RX_ADDR_P0,CurrentCfg.TX_Addr,5);
    nRF24L01_Write_Buf(TX_ADDR,CurrentCfg.TX_Addr,5);
    nRF24L01_Write_Reg(EN_AA,0x03);
    nRF24L01_Write_Reg(EN_RXADDR,0x03);

    return nRF24L01_Write_Reg(RF_CH,CurrentCfg.Channel);
}

/*******************************************************************
 * 功能:向nRF24L01写入发射数据,并且进入发射模式
 * 参数:
 *  buf:数据
 *  len:长度
 * 返回值:nRF24L01的status寄存器值
 * 备注:发送结束后由中断自动进入接收模式(不论是否接收到应答)
 * 2022/1/2   庞碧璋
 *******************************************************************/
uint8_t nRF24L01_Send(uint8_t*buf,uint8_t len)
{
    uint8_t status;
    CE_LOW;
    //配置为发送模式
    nRF24L01_Write_Reg(STATUS,0x70);    //清除中断标志
    nRF24L01_Send_Cmd(FLUSH_TX);        //清空TX FIFO
    nRF24L01_Send_CmdAndData(W_TX_PAYLOAD,buf,len);
    status = nRF24L01_Write_Reg(CONFIG,0x0E);    //0000 1110
    CE_HIGH;
    return status;
}

/*******************************************************************
 * 功能:nRF24L01进入监听模式
 * 参数:无
 * 返回值:nRF24L01的status寄存器值
 * 2022/1/10    庞碧璋
 *******************************************************************/
uint8_t nRF24L01_Rx_Mode(void)
{
    uint8_t status;
    CE_LOW;
    nRF24L01_Write_Reg(STATUS,0x70);             //清除中断标志
    status = nRF24L01_Write_Reg(CONFIG,0x0F);    //0000 1111
    CE_HIGH;
    return status;
}

/*******************************************************************
 * 功能:读nRF24L01的status寄存器值
 * 参数:无
 * 返回值:nRF24L01的status寄存器值
 * 2021/12/29   庞碧璋
 *******************************************************************/
uint8_t nRF24L01_Status(void)
{
    return nRF24L01_Read_Reg(STATUS);
}

/*******************************************************************
 * 功能:读nRF24L01接收到的字节
 * 参数:
 *  buf:接收缓存
 *  len:要读取的长度
 * 返回值:
 *  0:正常读取
 *  1:接收到的字节小于len,读取失败
 * 备注:这个函数会自动清除读取到的字节
 * 这里读取的是已经通过接收中断处理载入单片机中的字节,不是nRF24L01 RxFIFO
 * 中的字节
 * 2021/12/29   庞碧璋
 *******************************************************************/
uint8_t nRF24L01_Read_RxSbuffer(uint8_t*buf,uint8_t len)
{
    if(len > nRF24L01_Sbuffer[0])
        return 1;
    MemCopy(nRF24L01_Sbuffer+1,buf,len);
    nRF24L01_Push_Sbuffer(len);
    return 0;
}

/*******************************************************************
 * 功能:读nRF24L01的RxFIFO
 * 参数:
 *  buf:接收缓存
 * 返回值:
 *  读取到的字节个数
 * 备注:
 *  这个函数不会清除接收中断
 * 2022/1/31   庞碧璋
 *******************************************************************/
uint8_t nRF24L01_Read_RxFIFO(uint8_t*buf)
{
    //获取当前Rx_FIFO中的数据数量
    uint8_t len;
    len = nRF24L01_Read_Reg(R_RX_PL_WID);   //命令 读取接收到的字节长度
    //读有效数据
    CS_LOW; //片选
    port_Send(R_RX_PAYLOAD);    //发送读接收FIFO命令
    for(uint8_t temp=0;temp<len;temp++)
        buf[temp] = port_Send(0xff);
    CS_HIGH;    //取消片选
    //清除FIFO
    nRF24L01_Send_Cmd(FLUSH_RX);
    return len;
}

/*******************************************************************
 * 功能:将RxFIFO中的数据载入到单片机内部缓存中
 * 参数:无
 * 返回值:
 *  缓存区首地址
 * 备注:
 *  这个函数不会清除接收中断
 * 2022/1/31   庞碧璋
 *******************************************************************/
uint8_t*nRF24L01_FIFO_To_Sbuffer(void)
{
    uint8_t len;
    len  = nRF24L01_Read_RxFIFO(&nRF24L01_Sbuffer[ nRF24L01_Sbuffer[0] + 1]);
    nRF24L01_Sbuffer[0] += len;
    nRF24L01_Send_Cmd(FLUSH_RX);
    return nRF24L01_Sbuffer;
}

/*******************************************************************
 * 功能:读取nRF24L01接收到的字节个数
 * 参数:无
 * 返回值:nRF24L01接收到的字节个数
 * 备注:这里返回的是已经通过接收ISR载入单片机中的字节个数,不是nRF24L01
 *  RxFIFO中的字节个数
 * 2022/1/20    庞碧璋
 *******************************************************************/
uint8_t nRF24L01_Read_SbufferLen(void)
{
    return nRF24L01_Sbuffer[0];
}

/*******************************************************************
 * 功能:清除nRF24L01的接收缓存区
 * 参数:无
 * 返回值:无
 * 2022/1/20    庞碧璋
 *******************************************************************/
void nRF24L01_Clear_Sbuffer(void)
{
    nRF24L01_Sbuffer[0] = 0;
}

/*******************************************************************
 * 功能:清除nRF24L01的接收缓存区的前面部分
 * 参数:清除的长度
 * 返回值:无
 * 2022/1/20    庞碧璋
 *******************************************************************/
void nRF24L01_Push_Sbuffer(uint8_t len)
{
    if(len >= nRF24L01_Sbuffer[0])
    {
        nRF24L01_Clear_Sbuffer();
        return;
    }
    nRF24L01_Sbuffer[0] -= len;
    for(uint8_t temp=0;temp<len;temp++)
        nRF24L01_Sbuffer[temp+1] = nRF24L01_Sbuffer[temp+1+len];
}

/**********************************中断处理***************************************/

/***************************************************************************
 * 这三个函数需要自己定义,当nRF24L01发生中断后,nRF24L01_InterruptHandle()函数会
 * 根据中断类型调用对应的函数
 ***************************************************************************/
extern void nRF24L01_NoACK_ISR(void);
extern void nRF24L01_Tx_ISR(void);
extern void nRF24L01_Rx_ISR(void);

/*******************************************************************
 * 功能:nRF24L01中断处理函数
 * 参数:无
 * 返回值:无
 * 备注:可以在nRF24发生中断后调用这个函数,但是不建议直接在ISR中调用!!因为
 *  接收中断的处理代码执行时间较长,应当使用软件中断标志位,在没有其他任务时
 *  调用nRF24L01_InterruptHandle()
 *  若没有在ISR中直接调用这个函数,需要在ISR中添加nRF24L01_Write_Reg(STATUS,0xE0),否
 *  则stm32会卡在中断,同时应当保留status寄存器的值,方便后续中断处理
 *  未应答中断:使nRF24L01进入RxMode,并且清除TxFIFO,再执行自定义的nRF24L01_NoACK_ISR()函数
 *  发生完成中断:使nRF24L01进入RxMode,再执行自定义的nRF24L01_Tx_ISR()函数
 *  接收中断:将RxFIFO中的值载入到单片机内部的Sbuffer,再执行自定义的nRF24L01_Rx_ISR()函数
 * 2021/12/29   庞碧璋
 *******************************************************************/
void nRF24L01_InterruptHandle(void)
{
    //判断中断类型
    uint8_t status;
    status = nRF24L01_Status();
    //未应答中断
    if(status & (0x01<<4) )
    {
        NoACK_Handle();
        nRF24L01_NoACK_ISR();   //外部处理函数
    }
    //发送完成中断
    if(status & (0x01<<5) )
    {
        Tx_Handle();
        nRF24L01_Tx_ISR();  //外部处理函数
    }
    //接收中断
    if(status & (0x01<<6) )
    {
        Rx_Handler();
        nRF24L01_Rx_ISR();  //外部处理函数
    }
    nRF24L01_Write_Reg(STATUS,0xE0);    //清除所有中断
}

void Rx_Handler(void)
{
    nRF24L01_FIFO_To_Sbuffer();
}

void NoACK_Handle(void)
{
    nRF24L01_Send_Cmd(FLUSH_TX);
    nRF24L01_Rx_Mode();
}

void Tx_Handle(void)
{
    nRF24L01_Rx_Mode();
}

/****************************ISR**************************/
void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(NRF24L01_IQR_Line) == SET)
    {
        nRF24L01_InterruptHandle(); //中断处理
        EXTI_ClearITPendingBit(NRF24L01_IQR_Line);  //挂起中断
    }
}

