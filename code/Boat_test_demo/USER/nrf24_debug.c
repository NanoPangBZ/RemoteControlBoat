#include "nrf24_debug.h"

void nRF24L01_UserBoard_Test(void)
{
    nRF24L01_Cfg    Cfg;

    if(nRF24L01_Init())
    {
        printf("nRF24L01 Err!\r\n");
        while(1);
    }else
    {
        printf("nRF24L01 Pass\r\n");
    }

    Cfg.Channel = 0;
    Cfg.retry = 5;
    Cfg.retry_cycle = 1;
    Cfg.Rx_Length = 32;
    MemCopy(User_BoardAddr,Cfg.RX_Addr,5);
    MemCopy(Boat_BoardAddr,Cfg.TX_Addr,5);

    nRF24L01_Config(&Cfg);

    for(uint8_t temp=0;temp<32;temp++)
        sbuffer[temp] = temp;

    while(1)
    {
        soft_delay_ms(1000);
        printf("Send 32 Byte\r\n");
        nRF24L01_Send(sbuffer,32);
    }
}

void nRF24L01_BoatBoard_Test(void)
{
    nRF24L01_Cfg    Cfg;

    if(nRF24L01_Init())
    {
        printf("nRF24L01 Err!\r\n");
        while(1);
    }else
    {
        printf("nRF24L01 Pass\r\n");
    }

    Cfg.Channel = 0;
    Cfg.retry = 5;
    Cfg.retry_cycle = 1;
    Cfg.Rx_Length = 32;
    MemCopy(User_BoardAddr,Cfg.TX_Addr,5);
    MemCopy(Boat_BoardAddr,Cfg.RX_Addr,5);

    nRF24L01_Config(&Cfg);
    nRF24L01_Rx_Mode();

    static uint16_t dat = 0;

    while(1)
    {
        soft_delay_ms(500);
        if(nRF24L01_Read_RxLen() != 0)
        {
            dat += nRF24L01_Read_RxLen();
            nRF24L01_Clear_Sbuffer();
        }
        printf("Recieve:%d\r\n",dat);
    }
}

void nRF24L01_Rx_Handle(void)
{
    printf("Run Rx ISR\r\n");
}

void nRF24L01_Tx_Handle(void)
{
    printf("Run Tx ISR\r\n");
}

void nRF24L01_NoACK_Handle(void)
{
    printf("Run NoACK ISR\r\n");
}
