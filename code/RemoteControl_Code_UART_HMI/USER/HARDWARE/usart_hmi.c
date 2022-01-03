#include "usart_hmi.h"

/*******************************************************************
 * 功能:向串口屏发送字符串,自带0xff帧尾
 * 参数:要发送的字符串
 * 返回值:无
 * 备注:串口屏解析不支持"\n"换行!会报错,使用"\r"可以实现换行!
 * 2021/12/29   庞碧璋
 *******************************************************************/
void Usart_HMI_Send(char*str)
{
    uint8_t retry = 0;
    uint8_t str_len = strlen(str);
    //在末尾添加帧尾3个0xff
    for(uint8_t temp=0;temp<3;temp++)
        str[str_len+temp] = 0xff;
    //使用串口发送
    while( port_HMI_Send( (uint8_t*)str, str_len+4 ) && retry<250)
        retry++;
}

