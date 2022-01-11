#ifndef _NRF24L01_MICRO_H_
#define _NRF24L01_MICRO_H_

//寄存器地址
#define CONFIG          0x00
#define STATUS          0x07
#define RX_ADDR_P0      0x0A    //数据管道0接收地址 5Byte LSByte!
#define RX_ADDR_P1      0x0B    //数据管道1接收地址 5Byte LSByte!
#define TX_ADDR         0x10    //放射方的发射地址  5Byte LSByte!
#define SETUP_RETR      0x04    //低4位自动重发次数设置,高4位重发延时配置(250us单位)
#define FIFO_STATUS     0x17    //FIFO状态
/**********************************************
 * 发射方的发射地址(LSByte最先写入)，如果发射放
 * 需要收ACK确认信号，则需要配置RX_ADDR_P0
 * 的值等于TX_ADDR，并使能ARQ。
**********************************************/
#define RF_CH           0x05    //2.4G频段设置
#define FEATURE         0x1D    //特征寄存器
#define EN_AA           0x01    //使能自动确认
#define EN_RXADDR       0x02    //使能接收数据管道地址
#define RX_PW_P1        0x12    //接收管道1的静态长度设置 1~32Byte

//命令
#define FLUSH_TX        0xE1    //清空TX_FIFO
#define FLUSH_RX        0XE2    //清空RX_FIFO
#define W_TX_PAYLOAD    0xA0    //写发射负载数据
#define R_RX_PAYLOAD    0x61    //读接收数据
#define R_RX_PL_WID     0x60    //读取收到的数据字节数
#define W_TX_PAYLOAD_NO_ACK  0x0B

//MASK
#define TX_ENPTY_MASK   0x01<<4 //FIFO_STATUS寄存器 TX_EMPTY位
#define PWR_UP_MASK     0x01<<2 //CONFIG寄存器 PRW_UP位

#endif


