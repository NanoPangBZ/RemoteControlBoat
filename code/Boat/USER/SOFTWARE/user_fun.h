#ifndef _USER_FUN_H_
#define _USER_FUN_H_

#include "user.h"

void OS_ResponesRecive(RemoteControl_Type*receive);
void OS_EMG_Stop(void);
void OS_Beep(uint16_t on_ms,uint16_t off_ms,uint8_t count,uint8_t fre_id);
void OS_nrf_Restart(void);

#endif


