#ifndef _BSP_BOARD_LED_H_
#define _BSP_BOARD_LED_H_

#include "stm32f10x.h"
#include "self_stm32f10x.h"

/*************************************************
 * stm32f103c8t6最小系统板板载指示灯
 * 
 * Creat by: 庞碧璋
 * Github: https://github.com/CodingBugStd
 * csdn:   https://blog.csdn.net/RampagePBZ
 * Encoding: utf-8
 * date:    2021/10/1
*************************************************/

void BSP_Board_LED_Init(void);
void Board_LED_Reversal(void);


#endif
