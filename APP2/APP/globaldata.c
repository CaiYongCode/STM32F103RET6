#include "globaldata.h"
#include "struct.h"

RTU_CONFIG_INFO  g_E3000_Info;

CONCENTRATOR_PARAMETER g_Concentrator_Para;

// 读取预约时间标志  标志置1 初始化时、隔日读取、模式变换 月<>日
unsigned  char g_read_falsh_data_info = 0;
unsigned char g_get_at_update_time_flag = 0; //到达更新时间 
Light_Time_Info g_Light_Time_Info;

Light_Time_Info g_Week_Light_Time_Info[7];
Light_Time_Info g_Month_Light_Time_Info[31];

OS_EVENT * QMsg; 
void * QMSG_ARRAY[QSIZE];
R_msg  R_msg_Array[QSIZE];
unsigned char msg_index;

unsigned char com1_timeout_count;  //串口1通信超时计数器
unsigned short com1_recvbyte_count; //串口1通信接收数据计数器
unsigned char com1_buf[USART1_REC_LEN]={0};       //串口1缓冲区
unsigned char com1_send_buf[USART_SEND_LEN]={0}; 
unsigned char com1_send_len;

unsigned char com2_timeout_count;  //串口2通信超时计数器
unsigned char com2_recvbyte_count; //串口2通信接收数据计数器
unsigned char com2_buf[USART_REC_LEN];
unsigned char com2_send_buf[USART_SEND_LEN];

unsigned char com3_timeout_count;  //串口3通信超时计数器
unsigned char com3_recvbyte_count; //串口3通信接收数据计数器
unsigned char com3_buf[USART_REC_LEN];
unsigned char com3_send_buf[USART_SEND_LEN];

unsigned char com4_timeout_count;  //串口4通信超时计数器
unsigned char com4_recvbyte_count; //串口4通信接收数据计数器
unsigned char com4_buf[USART_REC_LEN];
unsigned char com4_send_buf[USART_SEND_LEN];

unsigned char com5_timeout_count;  //串口5通信超时计数器
unsigned char com5_recvbyte_count; //串口5通信接收数据计数器
unsigned char com5_buf[USART_REC_LEN];
unsigned char com5_send_buf[USART_SEND_LEN];
////////////////////////////////////////////////////////////////////////////////////////
unsigned char STM32_CHIP_ID[12] = {0};
//////////////////////////////////////////////
//RN8302 相关
uint32_t G_CastProtect;//投切保护标志,相应位保护看上面投切保护位
EffectParSetToSave   G_EffectPar_Info; 

PHASE_POWER_ITEM G_PhasePower_Info[4]; 

unsigned short g_Voltage_Adjust_Ratio[3] = {1000};  //触摸屏设置电压校正系数
unsigned short g_Current_Adjust_Ratio[3] = {1000};  //触摸屏设置电流调节系数

unsigned short g_Phase_Voltage[3];
unsigned short g_Phase_Amp[3];
////////////////////////////////////////////////////

//GPRS相关
unsigned char m_SimCard_num[13];
unsigned char g_GPRS_connect_OK = 0;
unsigned char g_get_SimCard_flag = 0;
unsigned char m_err_counter = 0;   //GPRS单次重连次数计数器
unsigned char g_GPRS_in_Reg_stage = 0; // GPRS 模块在注册阶段
unsigned char g_connect_step = 0;  //连接步骤索引
unsigned short g_gprs_timeout_2000ms_counter = 0;
unsigned char ICCID_and_IMSI_buf[20];	//存放ICCID和IMSI
unsigned char g_gprs_msg_num = 0;	//GPRS报文序列号
unsigned char g_GPRS_signal_strength = 0;	//网络信号强度
unsigned char g_gprs_module_model = 0;	//GPRS模块型号
unsigned char g_identify_module_model_flag = 0;
///////////////////////////////////////////////////
unsigned char g_report_state_flag = 0;  //当前是否处于上报参数状态
unsigned char g_report_step = 0;
///////////////////////////////////////////////////
//触摸屏相关
///////////////////////////////////////////////////
unsigned char g_TouchScreen_Connect_OK = 0;
unsigned short g_touchscreen_timeout_2000ms_counter = 0;
unsigned char g_TouchScreen_Password_buf[12];
unsigned char g_TouchScreen_Send_Cmd_Flag = 0;  //触摸屏有报文发送标志
unsigned char g_prompt_flag = 0;	//提示框标志,1表示在提示界面
unsigned short g_ctl_group = 0;	//控制操作组别
unsigned char g_ctl_phase = 0;	//控制操作相位
unsigned char g_now_screen_id = 0;
unsigned char g_password_target_screen_id = 0;
unsigned char g_password_judge = 0;
unsigned short g_show_damp_index = 0;
unsigned short g_show_damp_last_index = 0;
unsigned char g_showtime_buf[8][32] = {0};
unsigned char g_print_buf[64];  //打印缓存
unsigned char g_first_get_TS_rtc_time_flag = 0;
///////////////////////////////////////////////////


//PLC相关
//当前集中器下挂载的单灯节点数量 
Damp_Info g_Damp_Info[DAMP_MAX_NUM];
unsigned short g_damp_init_num = 0;
unsigned short g_lamp_total_num = 0;
unsigned short g_damp_fault_num = 0;
unsigned short g_lamp_read_num = 0;
unsigned char g_router_addr[6] = {0};
unsigned char g_add_damp_addr[6] = {0};
unsigned char g_del_damp_addr[6] = {0};
unsigned char g_ctl_damp_addr[6] = {0};
unsigned char g_set_damp_addr[6] = {0};
unsigned char g_read_damp_addr[6] = {0};
unsigned short g_Regular_Read_time_1000ms_counter = 0;	//定时抄读计数
unsigned short g_PLC_timeout_1000ms_counter = 0;	//单片机与路由通信超时计数
unsigned short g_plc_read_1000ms_couter = 0;		//抄读超时计数
unsigned short g_router_fault_reboot_num = 0;	//路由故障重启次数
unsigned char g_router_fault_flag = 0;		//路由通信故障
unsigned char g_plc_msg_num = 0;	//PLC报文序列号
unsigned char plc_comm_flag;		//载波接收标志位
unsigned char g_lamp_working_time_num;	//单灯工作时间段数
unsigned char g_lamp_working_time_index[4];	//单灯工作时间索引
unsigned char g_plc_msg_type;	//载波报文类型
unsigned char g_read_restart_flag;	//抄读重启标志
unsigned char g_read_restart_counter; //抄读重启计数
/////////////////////////////////////////
Send_bdp_FIFO  g_Send_bdp_FIFO[MAX_PLC_FIFO_DEP];
Send_GPRS_bdp_FIFO g_Send_GPRS_bdp_FIFO[MAX_GPRS_FIFO_DEP];
/////////////////////////////////////////
//按键相关
unsigned char g_key_set_flag = 0;
unsigned char g_key_function_flag = 0;	//1表示功能键按下
unsigned short g_debug_countdown_counter = 0; //调试开关灯工作模式下倒计时
/////////////////////////////////////////

unsigned char g_rtc_year,g_rtc_month,g_rtc_week,g_rtc_day,g_rtc_hour,g_rtc_min,g_rtc_sec; 
unsigned char g_ts_year,g_ts_month,g_ts_week,g_ts_day,g_ts_hour,g_ts_min,g_ts_sec; 
unsigned char g_now_year,g_now_month,g_now_week,g_now_day,g_now_hour,g_now_min,g_now_sec; 
unsigned char g_set_year,g_set_month,g_set_week,g_set_day,g_set_hour,g_set_min,g_set_sec; 
// 工作模式  周工作模式/年预约工作模式/手动
unsigned char g_working_mode = 0;

//t_s start time  / t_e end time  / t_g  group / t_l loop / t_r ratio 调光系数
unsigned short t_s[8],t_e[8],t_g[8],t_l[8],t_r[8][3],t_v[8];

///////////////////////////////////////////////////////////
unsigned char g_extmodule_relay[3][4];
CONCENTRATOR_PARAMETER g_Concentrator_Para;
unsigned char g_ext_online_flag[3] = {1,1,1};  //扩展模块在线标志
unsigned char g_ext_offline_counter[3];  //扩展模块离线计数器
///////////////////////////////////////////////////////////
unsigned short g_LM75A_temp;

unsigned char g_terminal_reboot_flag = 0;
/////////////////////////////////////////////////////////////
Time_Typedef g_DS1302_TimeValue;   //year 有效8bit 范围0--256
/////////////////////////////////////////////////////////////
//单灯工作模式  上电即亮   预约模式
unsigned char g_lamp_working_mode = 0;	//单灯工作模式

////////////////////////////////////////////////////////////
unsigned char g_mannual_debug_flag = 0;


//参数上报标志 
unsigned char g_send_concentrator_parameter = 0;
unsigned char g_rtc_time_fault = 0;	//RTC时钟故障标志
unsigned char g_ts_time_fault = 0;	//TS时钟故障标志
unsigned char g_judge_time_fault_flag = 0;	//时钟超差标志

unsigned char g_reset_E3000_8302_flag = 0;
unsigned char g_reset_ext_8302_flag = 0;

/////////////////////////////////////////////////////////////
unsigned char g_loop_total_num = 0;		//总回路数
unsigned char g_loop_OK_num = 0;		//故障回路数
/////////////////////////////////////////////////////////////
// //打印参数
// unsigned char print_buf[5] = {0};
// unsigned char print_len = 0;
// unsigned short g_terminal_reboot_num = 0;
// unsigned short g_GPRS_reconnect_num = 0;
// unsigned short g_recv_heart_num = 0;
// unsigned short g_send_heart_num = 0;

unsigned short g_update_flag = 0;
unsigned short g_update_pack_num = 0;
unsigned short g_update_timeout_1000ms_counter = 0;

