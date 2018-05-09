#ifndef _EXTMODULE_H
#define _EXTMODULE_H
#include "stm32f10x.h"
#include "includes.h"
#include "Initialize.h"

#include "globaldata.h"
#include "globalfunc.h"

void ActOn_ExtModule_BDP(unsigned char * pbuf,unsigned char len);


void Send_CMD_bdp_to_ExtModule(void);

void Ext_Module_online_judge(void);

void Check_ext_enable(unsigned char num);

void Update_Ext_Status(unsigned char num);

#endif
