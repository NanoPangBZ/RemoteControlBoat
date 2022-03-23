#include "usart_hmi.h"

#define port_Send(dat,len)  Usart_Send(2,dat,len)

static uint8_t Add_3FF(uint8_t*buf);   //静态函数,在buf(字符串)末尾添加3个0xFF -> 覆盖'/0'

const char* txt = "txt";
const char* Msg = "msg";

/*************************************************
 * 功能:在buf(字符串)末尾添加3个0xFF -> 覆盖'/0'
 * 参数:缓存地址
 * 返回值:增加0xff后的长度
*************************************************/
uint8_t Add_3FF(uint8_t*buf)
{
    uint8_t len;
    len = strlen((const char*)buf);
    for(uint8_t temp=0;temp<3;temp++)
    {
        // tar 'h' 'e' 'l' 'l' '0' 0xff 0xff 0xff
        // hello
        // len = 5
        //
        buf[ len + temp ] = 0xff;
    }
    return len+3;
}

void Usart_HMI_ClearMsg(void)
{

}

