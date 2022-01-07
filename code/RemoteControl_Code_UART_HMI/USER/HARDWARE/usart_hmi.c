#include "usart_hmi.h"

/*******************************************************************
 * 功能:向串口屏发送字符串,自带0xff帧尾
 * 参数:要发送的字符串
 * 返回值:
 *  0:发送成功
 *  1:发送失败
 * 备注:
 *  串口屏解析不支持"\n"换行!会报错,使用"\r"可以实现换行!
 *  str需要带有'\0'为结束标识符!!!!(不能直接传数组)
 * 2021/12/29   庞碧璋
 *******************************************************************/
uint8_t Usart_HMI_Send(uint8_t*str)
{
    uint8_t*addr = HMI_Sbuffer;
    uint8_t temp;
    uint16_t retry = 0;
    temp = strlen((char*)str);
    MemCopy(str,addr,temp); //拷贝到HMI_Sbuffer中  不包括'\0'
    addr += temp;
    //在末尾追加0xff
    for(uint8_t t=0;t<3;t++)
        *(addr+t) = 0xff;
    while ( port_HMI_Send(HMI_Sbuffer,temp+3) && retry++<10000);
    if(retry == 10000)
        return 1;
    else
        return 0;
}

/*******************************************************************
 * 功能:向串口屏的msg控件增加一条消息
 * 参数:消息
 * 返回值:
 *  0:正常
 *  1:MSG发送失败
 *  2:自动消息框下拉失败
 * 备注:
 *  串口屏解析不支持"\n"换行!会报错,使用"\r"可以实现换行!
 *  str需要带有'\0'最为结束标识符!
 *  传入的参数不要带有转义字符,可能会出现未知问题!!
 * 2021/12/29   庞碧璋
 *******************************************************************/
uint8_t Usart_HMI_MsgBox(uint8_t*msg)
{
    uint8_t*addr;
    uint8_t len;
    uint16_t retry = 0;
    addr = HMI_Sbuffer;                         //获取缓存地址
    sprintf((char*)addr,"msg.txt+=\"%s\\r\"",msg);     //msg.txt+="xxxxx\r"缓存格式化
    len = strlen((char*)HMI_Sbuffer);                  //计算长度,不包括'\0'
    addr += len;
    //0xff分段 结束语
    for(uint8_t temp=0;temp<3;temp++)
        *(addr+temp) = 0xff;
    addr += 3;
    sprintf((char*)addr,"msg.val_y+=16");    //13
    addr += 13;
    for(uint8_t temp=0;temp<3;temp++)
        *(addr+temp) = 0xff;
    while ( port_HMI_Send(HMI_Sbuffer,addr+3-HMI_Sbuffer) && retry++<10000);
    if(retry == 10000)
        return 1;
    else
        return 0;
}

/*******************************************************************
 * 功能:向串口屏的msg控件增加一条消息
 * 参数:消息
 * 返回值:
 *  0:正常
 *  1:MSG发送失败
 *  2:自动消息框下拉失败
 * 备注:
 *  串口屏解析不支持"\n"换行!会报错,使用"\r"可以实现换行!
 *  str需要带有'\0'最为结束标识符!
 *  传入的参数不要带有转义字符,可能会出现未知问题!!
 * 2021/12/29   庞碧璋
 *******************************************************************/
uint8_t Usart_HMI_GetSlider(uint8_t num)
{
    return 0;
}
