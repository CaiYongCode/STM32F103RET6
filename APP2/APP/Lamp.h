#ifndef _LAMP_H
#define _LAMP_H
#include "stm32f10x.h"
#include "includes.h"
#include "Initialize.h"

#include "globaldata.h"
#include "globalfunc.h"

void Get_Router_Addr(void);

unsigned char Add_One_Lamp_Info(unsigned char * pbuf);

unsigned char Del_One_Lamp_Info(unsigned char * pbuf);

void add_damp_addr(unsigned char *pbuf);
void del_damp_addr(unsigned char *pbuf);

void clear_lamp_read_flag(void);

unsigned char check_damp_file(void);

void check_lamp_status(void);

void Clear_All_Lamp_Info(void);

#endif
