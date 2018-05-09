#ifndef _GPRS_H
#define _GPRS_H
#include "stm32f10x.h"
#include "includes.h"
#include "Initialize.h"

#include "globaldata.h"
#include "globalfunc.h"

void Send_GPRS_Bdp(void);

unsigned char Add_bpd_to_GPRS_RingBuf(unsigned char send_counter, unsigned char * pbuf, unsigned char len);


void send_registered(void);
unsigned char Server_reg_bak_info(unsigned char* pbuf); //服务器反馈注册信息


void assemble_gprs_send_bdp(unsigned char message_type, unsigned char * databuf,unsigned char datalen);
///////////////////////////////////////////////////////////////////////////
//服务器命令解析
///////////////////////////////////////////////////////////////////////////
void ActOn_GPRS_BDP(unsigned char* pbuf,unsigned short len);
//服务器报文有效性判断
unsigned char bdp_valid(unsigned char * pbuf,unsigned short len);
//服务器下发升级命令处理
void act_on_update_proc(unsigned char* pbuf,unsigned short len);
//服务器下发设置和控制命令处理
void act_on_cmd_bdp(unsigned char* pbuf,unsigned short len);

///////////////////////////////////////////////////////////////////////////
//开灯和调光
void cmd_01_message(unsigned char* pbuf,unsigned short len);
//关灯
void cmd_02_message(unsigned char* pbuf,unsigned short len);
//下发周工作时间
void cmd_03_message(unsigned char* pbuf,unsigned short len);
//设置卡号
void cmd_04_message(unsigned char* pbuf,unsigned short len);
//下发当前时间
void cmd_05_message(unsigned char* pbuf,unsigned short len);
//重启
void cmd_06_message(unsigned char* pbuf,unsigned short len);
//下发月度工作时间
//void cmd_07_message(unsigned char* pbuf,unsigned short len);
//离线工作模式设定
void cmd_08_message(unsigned char* pbuf,unsigned short len);
//设定单灯分组
void cmd_09_message(unsigned char* pbuf,unsigned short len);
//请求参数上传
void cmd_0A_message(unsigned char* pbuf,unsigned short len);
//添加从节点
void cmd_0B_message(unsigned char* pbuf,unsigned short len);
//删除从节点
void cmd_0C_message(unsigned char* pbuf,unsigned short len);
//查询版本号
void cmd_0E_message(unsigned char* pbuf,unsigned short len);
//设置单灯回路顺序
void cmd_0F_message(unsigned char* pbuf,unsigned short len);
//设置启用控制回路
void cmd_10_message(unsigned char* pbuf,unsigned short len);
//设置启用采集回路
void cmd_11_message(unsigned char* pbuf,unsigned short len);
//设置联网IP
void cmd_12_message(unsigned char* pbuf,unsigned short len);
//设置联网域名
void cmd_13_message(unsigned char* pbuf,unsigned short len);
//设置端口号
void cmd_14_message(unsigned char* pbuf,unsigned short len);
//单回路开
void cmd_15_message(unsigned char* pbuf,unsigned short len);
//单回路关
void cmd_16_message(unsigned char* pbuf,unsigned short len);
//设置经纬度
void cmd_17_message(unsigned char* pbuf,unsigned short len);
//上报参数
void cmd_FF_message(unsigned char* pbuf,unsigned short len);
///////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////
//心跳发起报文
void send_gprs_heart_bdp(void);
//返回服务器命令执行结果报文
void send_result_return_bdp(unsigned char result);

///////////////////////////////////////////////////////////////////////////
//参数上传
///////////////////////////////////////////////////////////////////////////
//上报集中器参数
void send_concentrator_parameter_bdp(void);
//上报版本号
void send_software_version_bdp(void);
//上传CCID和IMSI
void send_CCID_and_IMSI_bdp(void);
//上传单个节点参数
//void send_single_damp_info_bdp(unsigned char index);
//上报集中器总用电量
//void send_concentrator_Electricity_bdp(unsigned char *pbuf);
//上报STM32芯片ID
void send_STM32_CHIP_ID_bdp(void);
//上报集中器温度
void send_concentrator_temperature_bdp(void);
//上传单个节点参数(包含组属)
void send_single_lamp_para_bdp(unsigned char index);
//上报信号质量
void send_singnal_quality_bdp(void);

//发送调试信息报文
void send_debug_bdp(unsigned char *contents, unsigned char len);
//上报回路开关命令
void send_loop_debuf_inf0_to_PC(void);
//上报单灯开关命令
void send_lamp_debuf_inf0_to_PC(unsigned char *addr,unsigned short group,unsigned char *contents);
//时钟超差，发送系统时间到服务器
void send_local_time_bdp(unsigned char alarm);
//上报工作时间
void send_local_working_time_bdp(void);
///////////////////////////////////////////////////////////////////////////
//告警上传
///////////////////////////////////////////////////////////////////////////
//上报集中器故障
void send_concentrator_fault_bdp(unsigned char fault,unsigned char num);
//上报单灯故障
void send_lamp_fault_bdp(unsigned char index,unsigned char *contents);

///////////////////////////////////////////////////////////////////////////
//解除告警
///////////////////////////////////////////////////////////////////////////
//解除集中器故障报文
void send_concentrator_fault_release_bdp(unsigned char fault,unsigned char num);
//解除单灯故障报文
void send_lamp_fault_release_bdp(unsigned char index,unsigned char fault);

///////////////////////////////////////////////////////////////////////////
//事件上传
///////////////////////////////////////////////////////////////////////////
//上报终端重启
void send_terminal_reboot_bdp(void);
//单灯上线
void send_lamp_online_bdp(unsigned char index);
//单灯下线
void send_lamp_offline_bdp(unsigned char index);
//箱门打开
void send_Boxdoor_open_bdp(unsigned char index);
//箱门关闭
void send_Boxdoor_close_bdp(unsigned char index);

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
#endif
