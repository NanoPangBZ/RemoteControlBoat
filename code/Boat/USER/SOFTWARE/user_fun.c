#include "user_fun.h"
#include "main.h"

void ResponesRecive_Function(RemoteControl_Type*receive)
{
    Ctr_Type ctr;
    if(receive->cmd == 1)
    {
        int ER_Base;
        int ER_sc;
        ER_Base = (receive->rocker[0] - 50) * 10;
        if(ER_Base != 0)
        {
            ER_sc = (receive->rocker[1] - 50) * 0.8 *ER_Base;
        }else
        {
            ER_sc = (receive->rocker[1] - 50) * 8;
        }
        ctr.ERctr.type = 1;
        //左
        ctr.ERctr.dat = ER_Base + ER_sc;
        xQueueSend(ER_CmdQueue[0],&ctr.ERctr,0);
        //右
        ctr.ERctr.dat = ER_Base - ER_sc;
        xQueueSend(ER_CmdQueue[1],&ctr.ERctr,0);
    }
}

