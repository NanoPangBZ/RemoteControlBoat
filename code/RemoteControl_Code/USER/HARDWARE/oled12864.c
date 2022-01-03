#include "oled12864.h"
#include "font_lib.h"

void OLED12864_Init(void)
{
    OLED12864_GPIO_Init();
    OLED12864_Hard_Reset();
}

void OLED12864_GPIO_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    for(uint8_t temp=0;temp<3;temp++)
    {
        GPIO_InitStruct.GPIO_Pin = OLED_Pin[temp].Pin;
        GPIO_Init(OLED_Pin[temp].GPIO,&GPIO_InitStruct);
        OLED12864_Set_Bit(temp);
    }
}

void OLED12864_Hard_Reset(void)
{
    OLED12864_Reset_Bit(OLED_RES);
    OLED12864_delay_ms(300);
    OLED12864_Set_Bit(OLED_RES);
    
    OLED12864_Reset_Bit(OLED_CS);

    OLED12864_Send_NumByte(OLED12864_InitCmd,28,OLED_CMD);
    OLED12864_Clear();
}

void OLED12864_Clear_Sbuffer(void)
{
    uint8_t page,x;
    for(page=0;page<8;page++)
    {
        for(x=0;x<128;x++)
            OLED12864_Sbuffer[page][x] = 0x00;
    }
}

void OLED12864_Clear(void)
{
    OLED12864_Clear_Sbuffer();
    OLED12864_Refresh();
}

void OLED12864_Refresh(void)
{
    uint8_t page;
    for(page=0;page<8;page++)
    {
        OLED12864_Set_Position(page,0);
        //for(uint8_t temp = 5;temp;temp--);     //等待最后一个字节发送完成,因为中途需要改变DC脚电平
        OLED12864_Send_NumByte(OLED12864_Sbuffer[page],128,OLED_DATA);
    }
}

void OLED12864_Set_Position(uint8_t page,uint8_t x)
{
    uint8_t dat[3];
    dat[0] = 0xb0 + page;           //页地址
    dat[1] = ((0xf0&x)>>4)|0x10;    //x坐标高四位设定
    dat[2] = 0x0f&x;                //x坐标低四位设定
    OLED12864_Send_NumByte(dat,3,OLED_CMD);
}

void OLED12864_Send_NumByte(const uint8_t*dat,uint8_t len,uint8_t cmd)
{
    if(cmd)
        OLED12864_Set_Bit(OLED_DC);
    else
        OLED12864_Reset_Bit(OLED_DC);

    for(uint8_t temp=0;temp<len;temp++)
    {
        OLED12864_SPI_Send_Byte(*dat);
        dat++;
    }
}

void OLED12864_Send_Byte(uint8_t dat,uint8_t cmd)
{
    if(cmd)
        OLED12864_Set_Bit(OLED_DC);
    else
        OLED12864_Reset_Bit(OLED_DC);
    
    OLED12864_SPI_Send_Byte(dat);
}

void OLED12864_Clear_PageBlock(uint8_t page,uint8_t x,uint8_t len)
{
    uint8_t sx = x+len;
    if(sx > x_MAX-1 || page > page_MAX-1)
        return;
    for(uint8_t temp=0;temp<len;temp++)
        OLED12864_Sbuffer[page][temp+x] = 0x00;
}

void OLED12864_Clear_Page(uint8_t page)
{
    OLED12864_Clear_PageBlock(page,0,127);
}

void OLED12864_Show_Char(uint8_t page,uint8_t x,uint8_t chr,uint8_t size)
{
    switch(size)
    {
        case 1:
            for(uint8_t temp=0;temp<6;temp++)
                OLED12864_Sbuffer[page][x+temp] = assic_0806[chr-0x20][temp];
            break;
        case 2:
            //从左至右,从上至下
            for(uint8_t s_page = 0;s_page<2;s_page++)
            {
                for(uint8_t s_x = 0; s_x<8;s_x++)
                {
                    OLED12864_Sbuffer[s_page+page][x+s_x] = assic_1608[chr-0x20][s_page*8+s_x];
                }
            }
            break;
        default:break;
    }
}

uint8_t  OLED12864_Show_Num(uint8_t page,uint8_t x,int num,uint8_t size)
{
    uint8_t chr[10];
    uint8_t reBit;
    uint8_t numSigned = 0;
    if(num<0)
    {
        OLED12864_Show_Char(page,x,'-',size);
        switch(size)
        {
            case 1:x+=6;break;
            case 2:x+=8;break;
            default:break;
        }
        numSigned = 1;
        num =- num;
    }
    if(num)
    {
        int bit = 0;
        while(num!=0)
        {
            uint16_t temp;
            temp = num%10;
            chr[bit] = temp + 0x30;
            num/=10;
            bit++;
        }       
        chr[bit--] = '\0';
        reBit = bit;
        for(int lbit = 0;bit-lbit>0;lbit++)
        {
            uint8_t temp;
            temp = chr[lbit];
            chr[lbit] = chr[bit];
            chr[bit] = temp;
            bit--;
        }
    }else
    {
        reBit = 1;
        chr[0] = '0';
        chr[1] = '\0';
    }
    OLED12864_Show_String(page,x,chr,size);
    if(numSigned)
        reBit++;
    return reBit;
}

uint8_t OLED12864_Show_fNum(uint8_t page,uint8_t x,double num,uint8_t size,uint8_t d_len)
{
    int L_Num;              //整数部分
    double R_Num;           //小数部分
    int R_Num2;             //根据小数部分化整
    uint8_t L_len = 0;      //整数部分长度
    if(d_len==0)
        d_len = 1;
    L_Num = (int)num;
    R_Num = num - L_Num;
    if(R_Num<0)
        R_Num = -R_Num;
    L_len = OLED12864_Show_Num(page,x,L_Num,size);
    switch(size)
    {
        case 1: x += 6*(L_len+1); OLED12864_Show_Char(page,x,'.',size); x+=6; break;
        case 2: x += 8*(L_len+1); OLED12864_Show_Char(page,x,'.',size); x+=8; break;
        default:break;
    }
    while(d_len!=0)
    {
        R_Num*=10;
        d_len--;
    }
    R_Num2 = (int)R_Num;
    OLED12864_Show_Num(page,x,R_Num2,size);
    return L_len+1+d_len;
}

void OLED12864_Show_String(uint8_t page,uint8_t x,uint8_t*str,uint8_t size)
{
    uint8_t sx = 0;
    while(*str!='\0')
    {
        OLED12864_Show_Char(page,x+sx,*str,size);
        switch(size)
        {
            case 1:
                sx+=6;
                break;
            case 2:
                sx+=8;
                break;
            default:
                break;
        }
        str++;
    }
}

//像素点相关操作
#if USE_POINT_CRT == 1

void OLED12864_Draw_Point(uint8_t x,uint8_t y,uint8_t bit)
{
    if(y > y_MAX-1 || x > x_MAX-1)
        return;
    uint8_t page = y/8;
    uint8_t col = y%8;
    if(bit)
        OLED12864_Sbuffer[page][x] |= (0x01<<col);
    else
        OLED12864_Sbuffer[page][x] &= ~(0x01<<col);
}

void OLED12864_Draw_Line(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2)
{
    float sx,sy;
    float k,k_1;   //斜率
    k = ((float)y2-y1) / ((float)x2-x1);
    k_1 = 1/k;
    sx = x1;
    sy = y1;
    for(;x1<=x2;x1++)
    {
        sy += k;
        OLED12864_Draw_Point(x1,(int)sy,1);
    }
    for(;y1<=y2;y1++)
    {
        sx += k_1;
        OLED12864_Draw_Point((int)sx,y1,1);
    }
}

void OLED12864_Draw_Rect(uint8_t x,uint8_t y,uint8_t len,uint8_t hight)
{
    for(uint8_t temp=0;temp<len;temp++)
    {
        OLED12864_Draw_Point(x+temp,y,1);
        OLED12864_Draw_Point(x+temp,y+hight,1);
    }
    for(uint8_t temp=0;temp<hight;temp++)
    {
        OLED12864_Draw_Point(x,y+temp,1);
        OLED12864_Draw_Point(x+len,y+temp,1);
    }
}

void OLED12864_Draw_Img(uint8_t x,uint8_t y,uint8_t len,uint8_t hight,uint8_t*img)
{
    uint8_t sx,sy;
    uint16_t dat_addr_pos;
    uint8_t page_pos;
    uint8_t bit_pos;
    for(sy=0;sy<hight;sy++)
    {
        page_pos = sy/8;
        bit_pos = sy%8;
        for(sx=0;sx<len;sx++)
        {
            dat_addr_pos = page_pos*len + sx;
            OLED12864_Draw_Point(sx+x,sy+y, *(img+dat_addr_pos) & ((0x80)>>bit_pos) );
        }
    }
}

void OLED12864_Draw_aImg(uint8_t x,uint8_t y,uint8_t*img)
{
    uint8_t len,hight;
    uint8_t sx,sy;
    uint16_t dat_addr_pos;
    uint8_t page_pos;
    uint8_t bit_pos;
    len = *(img+3) + *(img+2)*256;
    hight = *(img+5) + *(img+4)*256;
    for(sy=0;sy<hight;sy++)
    {
        page_pos = sy/8;
        bit_pos = sy%8;
        for(sx=0;sx<len;sx++)
        {
            dat_addr_pos = page_pos*len + sx + 6;
            OLED12864_Draw_Point(sx+x,sy+y, *(img+dat_addr_pos) & ((0x80)>>bit_pos) );
        }
    }
}

#endif  //USE_POINT_CRT
