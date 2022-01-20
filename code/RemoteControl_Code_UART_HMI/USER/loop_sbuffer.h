#ifndef _LOOP_SBUFFER_H_
#define _LOOP_SBUFFER_H_

#include "stm32f10x.h"

#define byte    uint8_t

typedef struct
{
    byte offside;
    byte len;
    byte SbufferLen;
    byte*SbufferHead;
}LoopSbuffer_Handle;

void Sbuffer_FreeAll(LoopSbuffer_Handle*handle);
byte Sbuffer_Push( LoopSbuffer_Handle*handle , byte len);
byte Sbuffer_GiveUp( LoopSbuffer_Handle*handle , byte len);

#endif  //_LOOP_SBUFFER_H_

