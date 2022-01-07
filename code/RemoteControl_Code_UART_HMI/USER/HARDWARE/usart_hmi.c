#include "usart_hmi.h"

static void Add_End(uint8_t*buf);   //静态函数,添加结束标志
static uint8_t HMI_Send(uint8_t*buf,uint8_t len);  //静态函数,发送buf

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
    temp = strlen((char*)str);
    MemCopy(str,addr,temp+1); //拷贝到HMI_Sbuffer中  包括'\0' ,不然Add_End()函数无法找到末尾
    addr += temp;
    //在末尾追加0xff
    Add_End(HMI_Sbuffer);
    return HMI_Send(HMI_Sbuffer,temp+3);
}

/*******************************************************************
 * 功能:向串口屏的msg控件增加一条消息
 * 参数:消息
 * 返回值:
 *  0:正常
 *  1:MSG发送失败
 * 备注:
 *  串口屏解析不支持"\n"换行!会报错,使用"\r"可以实现换行!
 *  str需要带有'\0'最为结束标识符!
 *  传入的参数不要带有转义字符,可能会出现未知问题!!
 * 2021/12/29   庞碧璋
 *******************************************************************/
uint8_t Usart_HMI_MsgBox(uint8_t*msg)
{
    uint8_t*addr;
    addr = HMI_Sbuffer;                         //获取缓存地址
    sprintf((char*)addr,"msg.txt+=\"%s\\r\"",msg);     //msg.txt+="xxxxx\r"缓存格式化
    addr += strlen((char*)HMI_Sbuffer);    //计算偏移量,不包括'\0'
    Add_End(HMI_Sbuffer);           //添加帧尾
    addr += 3;
    sprintf((char*)addr,"msg.val_y+=16");    //13
    addr += 13;
    Add_End(HMI_Sbuffer);           //添加帧尾
    return HMI_Send(HMI_Sbuffer,addr+3-HMI_Sbuffer);
}

/*******************************************************************
 * 功能:在字符串后增加3个0xff帧尾标志符,会覆盖'\0'
 * 参数:要添加帧尾的字符串
 * 返回值:无
 * 2022/1/7   庞碧璋
 *******************************************************************/
void Add_End(uint8_t*buf)
{
    uint8_t len;
    len = strlen((char*)buf);
    buf += len;
    //覆盖'\0'
    for(uint8_t temp=0;temp<3;temp++)
        buf[temp] = 0xff;
}

/*******************************************************************
 * 功能:发送buf
 * 参数:
 *  buf:数据
 *  len:数据长度
 * 返回值:
 *  0:发送成功
 *  1:发送超时
 * 2022/1/7   庞碧璋
 *******************************************************************/
uint8_t HMI_Send(uint8_t*buf,uint8_t len)
{
    uint16_t retry = 0;
    while ( port_HMI_Send(buf,len) && retry++<10000);
    if(retry == 10000)
        return 1;
    else
        return 0;
}

uint8_t Usart_HMI_MsgClear(void)
{
    uint8_t*addr;
    addr = HMI_Sbuffer;
    MemCopy("msg.txt=\" \"",addr,12);
    Add_End(HMI_Sbuffer);
    addr += 14;
    MemCopy("msg.val_y=0",addr,12);
    Add_End(HMI_Sbuffer);
    return HMI_Send(HMI_Sbuffer,28);
}
