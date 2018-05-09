#ifndef _MESSAGEPROC_H
#define _MESSAGEPROC_H
#include "stm32f10x.h"
#include "includes.h"
#include "Initialize.h"

#include "globaldata.h"
#include "globalfunc.h"

void MsgProc_Task(void * pdata);                 //

//void ActOn_Timer_MSG(void);

void ActOn_CONFIG_BDP(unsigned char* pbuf);



void Get_RTU_Info_to_PC(void);  //获取RTU信息返回PC机

void ActOn_Key_BDP(unsigned char val);

void sys_recovery_info(void);

#endif
