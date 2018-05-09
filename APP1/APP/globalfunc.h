#ifndef _GLOBALFUNC_H
#define _GLOBALFUNC_H
#include "stm32f10x.h"
#include "includes.h"
#include "Initialize.h"
#include "struct.h"
//#include "const.h"
////////////////////////////////////////////////////////////////////////////////////////////////

void SendMessage(unsigned short msg_type,unsigned int msg_len,int msg_addr);


void uart_send_char(unsigned char * str);
void gprs_data_send(unsigned char * pbuf,unsigned short len);


void COM1_Send_BDP(unsigned char * buf,unsigned char len);
void COM2_Send_BDP(unsigned char * buf,unsigned char len);
void COM3_Send_BDP(unsigned char * buf,unsigned char len);
void COM4_Send_BDP(unsigned char * buf,unsigned char len);
void COM5_Send_BDP(unsigned char * buf,unsigned char len);

unsigned char AsciiToHex(unsigned char asc_hi, unsigned char asc_lo);

unsigned char i_to_a(unsigned char val);

unsigned char a_to_i(unsigned char val);

unsigned char h_to_i(unsigned char val);

unsigned short complement_conversion(short value);

void HEXaddr_to_BCDaddr(unsigned char *hexaddr,unsigned char *bcdaddr);
void BCDaddr_to_HEXaddr(unsigned char *bcdaddr,unsigned char *hexaddr);

unsigned char checksum(unsigned  char * str,unsigned short len);

void Enable_RS485_Send(void);
void Enable_RS485_Recv(void);

void Read_STM32_CHIP_ID(void);

void Read_Flash_RN8302_Config_Info(void);
void Write_RN8302_Config_to_Flash(unsigned short * buf);

void Read_Flash_RTU_Info(void);
void Write_RTU_info_to_Flash(void);

void Read_Week_Light_Time_Info(void);
void Write_Week_Light_Time_Info(void);

void Read_Month_Light_Time_Info(void);
void Write_Month_Light_Time_Info(unsigned char month,unsigned char days);

void Write_RTB42_Addr_to_Flash(unsigned char *buf);

//读取下位单灯信息
void Read_Damp_Info(void);
//设置下位单灯信息
void Write_Damp_Info(void);

void Read_Concentrator_Config_Info(void);
void Write_Concentrator_Config_Info(void);

unsigned char Read_CMD_Info(unsigned short hour,unsigned short min,unsigned short sec);
void Write_CMD_Info(void);

void Write_APP_Flag_to_Flash(unsigned char workSpace);

unsigned char data_conversion(unsigned char* buf,unsigned short data,unsigned char decimal);

//判断当前时间是否在单灯时间之内
unsigned char judge_light_time_stage(unsigned char hour,unsigned char  min);

void Write_Update_Program_to_Flash(u8 *appbuf,u32 appsize);

#endif
