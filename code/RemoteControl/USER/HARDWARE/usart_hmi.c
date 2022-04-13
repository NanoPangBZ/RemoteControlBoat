#include "usart_hmi.h"

#define port_Send(dat,len)  Usart_Send(1,dat,len)

static uint8_t Add_3FF(uint8_t*buf);   //静态函数,在buf(字符串)末尾添加3个0xFF -> 覆盖'/0'

const char* txt = "txt";
const char* Msg = "msg";
const char* val_y = "val_y";
const char* val = "val";
const char* num_boxName[] = {"msp","ssp"};

uint16_t MsgHight = 0;   //当前串口屏消息框高度

void HMI_Reset(void)
{
    uint8_t str[16] = "rest";
    uint8_t len;
    len = Add_3FF(str);
    port_Send(str,len);
    soft_delay_ms(1000);
    MsgHight = 0;
}

/*************************************************
 * 功能:在buf(字符串)末尾添加3个0xFF -> 覆盖'/0'
 * 参数:缓存地址
 * 返回值:增加0xff后的长度
*************************************************/
uint8_t Add_3FF(uint8_t*str)
{
    uint8_t len;
    len = strlen((const char*)str);
    for(uint8_t temp=0;temp<3;temp++)
    {
        // tar 'h' 'e' 'l' 'l' '0' 0xff 0xff 0xff
        // hello
        // len = 5
        str[ len + temp ] = 0xff;
    }
    return len+3;
}

/*************************************************
 * 功能:清空串口屏消息框
 * 参数:无
 * 返回值:无
*************************************************/
void HMI_ClearMsg(void)
{
    uint8_t str[16];
    uint8_t len = 0;
    sprintf((char*)str,"%s.%s=\"\"",Msg,txt);
    len = Add_3FF(str);
    while( port_Send(str,len) );
    MsgHight = 0;
}

/*************************************************
 * 功能:显示一条消息到串口屏的消息框
 * 参数:消息
 * 返回值:无
*************************************************/
void HMI_Msg(char*msg)
{
    uint8_t str[48];
    uint8_t len = 0;
    sprintf((char*)str,"%s.%s+=\"%s\\r\"",Msg,txt,msg);
    len = Add_3FF(str);
    while( port_Send(str, len) );
    MsgHight += HMI_Msg_TxtHight;
    if(MsgHight > HMI_Msg_BoxHight)
    {
        sprintf((char*)str,"%s.%s+=%d",Msg,val_y,HMI_Msg_TxtHight);
        len = Add_3FF(str);
        while( port_Send(str,len) );
    }
}

/*************************************************
 * 功能:更改HMI数字框显示的数据
 * 参数:
 * channle 0:主油门数字框 1:副油门数字框
 * 返回值:无
*************************************************/
void HMI_SetNum(int num,uint8_t channel)
{
    uint8_t str[48];
    uint8_t len;
    sprintf((char*)str,"%s.%s=%d",num_boxName[channel],val,num);
    len = Add_3FF(str);
    while( port_Send(str,len) );
}
