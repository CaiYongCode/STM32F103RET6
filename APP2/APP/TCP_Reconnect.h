#ifndef _TCP_RECONNECT_H
#define _TCP_RECONNECT_H
#include "stm32f10x.h"
#include "includes.h"
#include "Initialize.h"

#include "globaldata.h"
#include "globalfunc.h"


///////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////
void Close_Connect(void);
void identify_gprs_module_model(void);
void SIM900A_connect_Step_by_Step(void);
void EC20_connect_Step_by_Step(void);
void SIM900A_connect_to_gprs_net_proc(unsigned char* pbuf,unsigned short len);
void EC20_connect_to_gprs_net_proc(unsigned char* pbuf);

void Connect_Server(void);


void Tcp_Reconnect_proc(void *pdata);                  

void ActOn_TCP_Connect_Bdp(unsigned char * pbuf, unsigned char len);

void Reset_parameter_to_Reconnect(void);

void Report_Func(void);

#endif
