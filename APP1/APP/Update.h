#ifndef _UPDATE_H
#define _UPDATE_H

#include "stm32f10x_it.h"

void ActOn_Update_BDP(unsigned char * pbuf,unsigned short len);
void Update_Ready(void);
void Update_Stop(void);
void send_update_ready_bdp(unsigned char status);
void send_update_request_bdp(void);
void send_update_result_bdp(unsigned char result);
void send_update_package_request_bdp(unsigned char num);
void Write_Update_Program_to_Flash(u8 *appbuf,u32 appsize);
void Show_Update_Progress(unsigned char packNum,unsigned char totalNum);
void Check_Update_Status(void);
void Delay_s(unsigned char s);

#endif
