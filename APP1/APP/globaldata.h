#ifndef _GLOBALDATA_H
#define _GLOBALDATA_H
#include "stm32f10x.h"
#include "includes.h"
#include "Initialize.h"
#include "struct.h"
#include "const.h"
////////////////////////////////////////////////////////////////////////////////////////////////
#define USART1_REC_LEN	(6*1024)	//串口1接收数据长度6K
#define USART_REC_LEN  	128  	//串口接收数据长度128
#define USART_SEND_LEN  	256  	
////////////////////////////////////////////////////////////////////////////////////////////////
//消息队列类型定义

#define COM_1_MESSAGE_TYPE                  1        //串口1消息
#define COM_2_MESSAGE_TYPE                  2        //串口2消息
#define TIMER_1000MS_MESSAGE_TYPE           3        //1000ms消息
#define COM_3_MESSAGE_TYPE                  4        //串口3消息
#define COM_4_MESSAGE_TYPE                  5        //串口2消息
#define COM_5_MESSAGE_TYPE                  6        //串口5消息
#define TIMER_2000MS_MESSAGE_TYPE           7        //2000ms消息
#define KEY_MESSAGE_TYPE                    8        //面板按键消息
#define TIMER_10S_MESSAGE_TYPE              9        //10s消息
#define FIRST_GET_TS_RTC_MESSAGE_TYPE       20       // 第一次获取TS时间信息
////////////////////////////////////////////////////////////////////////////////////////
#define QSIZE     32

////////////////////////////////////////////////////////////////////////////////////////
#define PLC_COMMUNICATION_OVERTIMR			600	//通信超时时间10分钟
#define PLC_READ_OVERTIMR					300	//抄读超时时间5分钟
#define DAMP_MAX_NUM	400	//最多可存储的单灯数量

///////////////////////////////////////////////////////////////////////////////////////
#define LARGE_CURRENT_VARIATION_RATIO	0.01	//大电流变动比例1% 
#define SMALL_CURRENT_VARIATION_RATIO	0.05	//小电流变动比例5% 

#define CURRENT_VARIATION_THRESHOLD		100		//电流变动阈值1A
#define VOLTAGE_VARIATION_THRESHOLD		50		//电压变动阈值5V
////////////////////////////////////////////////////////////////////////////////////////
#define CONCENTRATOR_FAULT_1	1	//集中器与路由通信失败
#define CONCENTRATOR_FAULT_2	2	//集中器与扩展模块通信失败
#define CONCENTRATOR_FAULT_3	3	//防盗告警
#define CONCENTRATOR_FAULT_4	4	//UPS供电
#define CONCENTRATOR_FAULT_5	5	//同步时间与RTC时间超差故障

#define LAMP_FAULT_1	1			//单灯漏电
////////////////////////////////////////////////////////////////////////////////////////
//PLC FIFO 深度
#define MAX_PLC_FIFO_DEP  	6  

#define MAX_GPRS_FIFO_DEP  	10  
////////////////////////////////////////////////////////////////////////////////////////

extern  RTU_CONFIG_INFO  g_E3000_Info;
extern unsigned char g_read_falsh_data_info;
extern unsigned char g_get_at_update_time_flag;
extern Light_Time_Info g_Light_Time_Info;
extern CONCENTRATOR_PARAMETER g_Concentrator_Para;
extern Light_Time_Info g_Week_Light_Time_Info[7];
extern Light_Time_Info g_Month_Light_Time_Info[31];
/************************************************************************************************
**消息队列声明
************************************************************************************************/


extern OS_EVENT * QMsg; 
extern void * QMSG_ARRAY[QSIZE];
extern R_msg  R_msg_Array[QSIZE];
extern unsigned char msg_index;
/************************************************************************************************/

extern unsigned char com1_timeout_count;  //串口1通信超时计数器
extern unsigned short com1_recvbyte_count; //串口1通信接收数据计数器
extern unsigned char com1_buf[USART1_REC_LEN];       //串口1缓冲区
extern unsigned char com1_send_buf[USART_SEND_LEN];
extern unsigned char com1_send_len;

extern unsigned char com2_timeout_count;  //串口2通信超时计数器
extern unsigned char com2_recvbyte_count; //串口2通信接收数据计数器
extern unsigned char com2_buf[USART_REC_LEN];
extern unsigned char com2_send_buf[USART_SEND_LEN];

extern unsigned char com3_timeout_count;  //串口3通信超时计数器
extern unsigned char com3_recvbyte_count; //串口3通信接收数据计数器
extern unsigned char com3_buf[USART_REC_LEN];
extern unsigned char com3_send_buf[USART_SEND_LEN];

extern unsigned char com4_timeout_count;  //串口4通信超时计数器
extern unsigned char com4_recvbyte_count; //串口4通信接收数据计数器
extern unsigned char com4_buf[USART_REC_LEN];
extern unsigned char com4_send_buf[USART_SEND_LEN];

extern unsigned char com5_timeout_count;  //串口5通信超时计数器
extern unsigned char com5_recvbyte_count; //串口5通信接收数据计数器
extern unsigned char com5_buf[USART_REC_LEN];
extern unsigned char com5_send_buf[USART_SEND_LEN];
////////////////////////////////////////////////////////////////////////////////////////
extern unsigned char STM32_CHIP_ID[12];
///////////////////////////////////////////////////
//RN8302 相关
extern uint32_t G_CastProtect;//投切保护标志,相应位保护看上面投切保护位
extern EffectParSetToSave   G_EffectPar_Info; 

extern PHASE_POWER_ITEM G_PhasePower_Info[4]; 

extern unsigned short g_Voltage_Adjust_Ratio[3];
extern unsigned short g_Current_Adjust_Ratio[3];

extern unsigned short g_Phase_Voltage[3];
extern unsigned short g_Phase_Amp[3];
///////////////////////////////////////////////////
extern unsigned char m_SimCard_num[13];
extern unsigned char g_GPRS_connect_OK;
extern unsigned char g_get_SimCard_flag;
extern unsigned char g_GPRS_in_Reg_stage;
extern unsigned char g_connect_step;
extern unsigned char m_err_counter;
extern unsigned short g_gprs_timeout_2000ms_counter;
extern unsigned char ICCID_and_IMSI_buf[20];	//存放ICCID和IMSI
extern unsigned char g_gprs_msg_num;
extern unsigned char g_GPRS_signal_strength;
extern unsigned char g_gprs_module_model;
extern unsigned char g_identify_module_model_flag;


//////////////////////////////////////////////////
extern unsigned char g_report_state_flag;  //当前是否处于上报参数状态
extern unsigned char g_report_step;
//////////////////////////////////////////////////////////
extern unsigned char g_TouchScreen_Connect_OK;
extern unsigned short g_touchscreen_timeout_2000ms_counter;
extern unsigned char g_TouchScreen_Password_buf[12];
extern unsigned char g_TouchScreen_Send_Cmd_Flag;
extern unsigned char g_prompt_flag;
extern unsigned short g_ctl_group;
extern unsigned char g_ctl_phase;
extern unsigned char g_now_screen_id;
extern unsigned char g_password_target_screen_id;
extern unsigned char g_password_judge;
extern unsigned short g_show_damp_index;
extern unsigned short g_show_damp_last_index;
extern unsigned char g_showtime_buf[8][32];
extern unsigned char g_print_buf[64]; 
extern unsigned char g_first_get_TS_rtc_time_flag;
//////////////////////////////////////////////////

//PLC相关
extern  unsigned short g_damp_init_num;
extern  unsigned short g_lamp_total_num;
extern unsigned short g_damp_fault_num;
extern unsigned short g_lamp_read_num;
extern  Damp_Info g_Damp_Info[DAMP_MAX_NUM];
extern unsigned char g_router_addr[6];
extern unsigned char g_add_damp_addr[6];
extern unsigned char g_del_damp_addr[6];
extern unsigned char g_ctl_damp_addr[6];
extern unsigned char g_set_damp_addr[6];
extern unsigned char g_read_damp_addr[6];
extern unsigned short g_Regular_Read_time_1000ms_counter;
extern unsigned short g_PLC_timeout_1000ms_counter;
extern unsigned short g_plc_read_1000ms_couter;
extern unsigned short g_router_fault_reboot_num;
extern unsigned char g_router_fault_flag;
extern unsigned char g_plc_msg_num;
extern unsigned char plc_comm_flag;	
extern unsigned char g_lamp_working_time_num;	//单灯工作时间段数
extern unsigned char g_lamp_working_time_index[4];
extern unsigned char g_plc_msg_type;
extern unsigned char g_read_restart_flag;
extern unsigned char g_read_restart_counter;
/////////////////////////////////////////
extern Send_bdp_FIFO      g_Send_bdp_FIFO[MAX_PLC_FIFO_DEP];
extern Send_GPRS_bdp_FIFO g_Send_GPRS_bdp_FIFO[MAX_GPRS_FIFO_DEP];

/////////////////////////////////////////
extern unsigned char g_key_set_flag;
extern unsigned char g_key_function_flag;
extern unsigned short g_debug_countdown_counter;

extern unsigned char g_working_mode;
extern unsigned short t_s[8],t_e[8],t_g[8],t_l[8],t_r[8][3],t_v[8];

extern unsigned char g_rtc_year,g_rtc_month,g_rtc_week,g_rtc_day,g_rtc_hour,g_rtc_min,g_rtc_sec; 
extern unsigned char g_ts_year,g_ts_month,g_ts_week,g_ts_day,g_ts_hour,g_ts_min,g_ts_sec; 
extern unsigned char g_now_year,g_now_month,g_now_week,g_now_day,g_now_hour,g_now_min,g_now_sec; 
extern unsigned char g_set_year,g_set_month,g_set_week,g_set_day,g_set_hour,g_set_min,g_set_sec; 

///////////////////////////////////////////////////////////
extern unsigned short g_LM75A_temp;

////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
extern unsigned char g_extmodule_relay[3][4];
extern CONCENTRATOR_PARAMETER g_Concentrator_Para;
extern unsigned char g_ext_online_flag[3];  //扩展模块在线标志
extern unsigned char g_ext_offline_counter[3];  //扩展模块离线计数器

extern unsigned char g_terminal_reboot_flag;

/////////////////////////////////////////////////////////////
extern Time_Typedef g_DS1302_TimeValue;   //year 有效8bit 范围0--256
/////////////////////////////////////////////////////////////
extern unsigned char g_lamp_working_mode;
/////////////////////////////////////////////////////////////
extern unsigned char g_mannual_debug_flag;


extern unsigned char g_send_concentrator_parameter;
extern unsigned char g_rtc_time_fault;
extern unsigned char g_ts_time_fault;
extern unsigned char g_judge_time_fault_flag;

extern unsigned char g_reset_E3000_8302_flag;
extern unsigned char g_reset_ext_8302_flag;
/////////////////////////////////////////////////////////////
extern unsigned char g_loop_total_num;		//总回路数
extern unsigned char g_loop_OK_num;		//故障回路数
/////////////////////////////////////////////////////////////
// extern unsigned char print_buf[5];
// extern unsigned char print_len;
// extern unsigned short g_terminal_reboot_num;
// extern unsigned short g_GPRS_reconnect_num;
// extern unsigned short g_recv_heart_num;
// extern unsigned short g_send_heart_num;

extern unsigned short g_update_flag;
extern unsigned short g_update_pack_num;
extern unsigned short g_update_timeout_1000ms_counter;


#endif
