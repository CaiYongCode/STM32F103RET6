#ifndef _DACAI_TOUCH_H
#define _DACAI_TOUCH_H
#include "stm32f10x.h"
#include "includes.h"
#include "Initialize.h"

#include "globaldata.h"
#include "globalfunc.h"

void InitTC(void);

void ActOn_TouchScreen_BDP(unsigned char* pbuf,unsigned char len);

void show_simcard_num(void);
void show_Concentrator_SN(void);
void Change_Page(unsigned char screen_id); 
void Icon_Show(unsigned char screen_id,unsigned char control_id,unsigned char icon_id);
void Set_GPRS_Status_to_TC(unsigned char status);
void Get_TS_Time(void);
void Set_TS_Time(void);
//void Set_str(unsigned char start_hour,unsigned char start_min,unsigned char end_hour,unsigned char end_min);
void Show_Light_Time(unsigned char ctl_id,unsigned char time_id);
void show_text(unsigned char screen_id,unsigned char control_id,unsigned char* pbuf,unsigned char len);
void show_temperature(void);
void show_lamp_statistics(void);
void adjust_TS_backlight(unsigned char brightness);

void show_ext_status(unsigned char num,unsigned char status);
void show_power_supply(unsigned char status);
void show_ext_config_info(unsigned char num);
void show_ctr_config_info(void);

void screen_2_page(unsigned char *pbuf,unsigned char len);
void screen_3_page(unsigned char *pbuf,unsigned char len);
void screen_4_page(unsigned char *pbuf,unsigned char len);
void screen_5_page(unsigned char *pbuf,unsigned char len);
void screen_6_page(unsigned char *pbuf,unsigned char len);
void screen_7_page(unsigned char *pbuf,unsigned char len);
void screen_8_page(unsigned char *pbuf,unsigned char len);
void screen_9_page(unsigned char *pbuf,unsigned char len);
void screen_10_page(unsigned char *pbuf,unsigned char len);
void screen_11_page(unsigned char *pbuf,unsigned char len);
void screen_12_page(unsigned char *pbuf,unsigned char len);
void screen_13_page(unsigned char *pbuf,unsigned char len);
void screen_14_page(unsigned char *pbuf,unsigned char len);
void screen_15_page(unsigned char *pbuf,unsigned char len);
void screen_17_page(unsigned char *pbuf,unsigned char len);
void screen_18_page(unsigned char *pbuf,unsigned char len);
void screen_19_page(unsigned char *pbuf,unsigned char len);
void screen_20_page(unsigned char *pbuf,unsigned char len);

void get_edit_addr(unsigned char *pbuf,unsigned char len,unsigned char *addr_buf);
void Show_Damp_Info(unsigned char index);	//显示节点状态信息
void Show_Read_Progress(unsigned char *addr);
void Show_Loop_Info(unsigned char dir);

void show_loop_statistics(void);
void show_loop_total_pages(void);
void show_loop_now_page(unsigned char page);
void Show_Loop_Info(unsigned char dir);

#endif
