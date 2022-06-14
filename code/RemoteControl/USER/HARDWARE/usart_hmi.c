#include "usart_hmi.h"

#define port_Send(dat,len)  Usart_Send(2,dat,len)   //串口发送
#define port_Recive()       Usart_Read(2)+1         //读串口
#define port_Recive_Len     *Usart_Read(2)
#define port_PushSbuffer(len)  USART_Push(2,len)    //清除串口缓存区前len字节

static uint8_t Add_3FF(uint8_t*buf);   //静态函数,在buf(字符串)末尾添加3个0xFF -> 覆盖'/0'

//串口屏控件名字及其属性的字符串表
const char* txt = "txt";
const char* Msg = "msg";
const char* val_y = "val_y";
const char* val = "val";
const char* num_boxName[] = {"msp","ssp","si","dep"};
const char* float_boxName[] = {"fx","fy","fz","vo"};

uint16_t MsgHight = 0;   //当前串口屏消息框高度

/*************************************************
 * 功能:重置串口屏
 * 参数:无
 * 返回值:无
*************************************************/
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

void HMI_SetFloat(float num,uint8_t channel)
{
    uint8_t str[48];
    uint8_t len;
    short temp = (short)(num*10);
    sprintf((char*)str,"%s.%s=%d",float_boxName[channel],val,temp);
    len = Add_3FF(str);
    while( port_Send(str,len) );
}

extern uint8_t HMI_SwitchValue;

//解析串口屏发送的消息  -> 只解析单次
uint8_t HMI_Decode(void)
{
    uint8_t len = port_Recive_Len;
    uint8_t*dat = port_Recive();
    if(len != 0)
    {
        uint8_t temp = *dat;
        port_PushSbuffer(1);
        return temp;
    }
    return 0;
}
