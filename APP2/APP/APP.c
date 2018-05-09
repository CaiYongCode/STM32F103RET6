#include "APP.h"
#include "MessageProc.h"
#include "TimerTask.h"
#include "globalfunc.h"
#include "dacai_touch.h"
#include "RN8302.h"
#include "IOctl.h"
#include "PLC.h"
#include "wdg.h"
#include "LM75A.h"
#include "DS1302.h"
#include "TCP_Reconnect.h"
#include "Lamp.h"
#include "Update.h"
/*************************************************任务堆栈声明********************************************/
OS_STK  Task_MsgProc_STK[MSG_PROC_SIZE];
//OS_STK  Task_2000MS_TIMER_STK[TIMER_2000MS_STK_SIZE];
OS_STK  Task_1000MS_TIMER_STK[TIMER_1000MS_STK_SIZE]; 
OS_STK  Task_KEY_SCAN_STK[KEY_SCAN_STK_SIZE];
OS_STK  Task_TCP_RECONNECT_STK[TCP_RECONNECT_STK_SIZE];
/********************************************************************************************************** 
** Function name:      TaskStart
** Descriptions:       起始任务，用于建立最初的系统必备任务
** input parameters:   void *pdata
** output parameters:  无
** Returned value:     无
**********************************************************************************************************/


 
void TaskStart(void * pdata)
{
	pdata = pdata;
    
	System_App_Init();
	
	QMsg = OSQCreate((void *)&QMSG_ARRAY[0],QSIZE);

	

	OSTaskCreate(Task_Key_Scan, (void * )0, (OS_STK *)&Task_KEY_SCAN_STK[KEY_SCAN_STK_SIZE-1], Key_Scan_TASK_Prio);
	OSTaskCreate(MsgProc_Task, (void * )0, (OS_STK *)&Task_MsgProc_STK[MSG_PROC_SIZE-1], MsgProc_TASK_Prio);
//	OSTaskCreate(Task_2000ms_Timer, (void * )0, (OS_STK *)&Task_2000MS_TIMER_STK[TIMER_2000MS_STK_SIZE-1], Timer_2000ms_TASK_Prio);
	
	OSTaskCreate(Tcp_Reconnect_proc, (void * )0, (OS_STK *)&Task_TCP_RECONNECT_STK[TCP_RECONNECT_STK_SIZE-1], TCP_RECONNETC_TASK_Prio);
	OSTaskCreate(Task_1000ms_Timer, (void * )0, (OS_STK *)&Task_1000MS_TIMER_STK[TIMER_1000MS_STK_SIZE-1], Timer_1000ms_TASK_Prio);
	OSTaskDel(OS_PRIO_SELF);//删除自己	OSTaskSuspend(START_TASK_Prio);
}



void System_App_Init(void)
{	
	unsigned char i = 0;
			
	g_terminal_reboot_flag = 1;
	
	
	Read_STM32_CHIP_ID();

	Read_Flash_RTU_Info();
	
	//默认启用域名"kuitun.daominglight.com"
	if(g_E3000_Info.ip_set != 1)
	{
		memcpy(g_E3000_Info.ip,"dev.daominglight.com",20);
		g_E3000_Info.ip_len = 20;
	}
	//默认启用端口"24680"
	if(g_E3000_Info.port_set != 1)
	{
		memcpy(g_E3000_Info.port,"24680",5);
		g_E3000_Info.port_len = 5;
	}
	
	Get_Router_Addr();
	send_set_route_addr_bdp();
	
	//读取集中器配置信息
	Read_Concentrator_Config_Info();
	//默认启用集中器自身1条回路3相电压3相电流
	if(g_Concentrator_Para.e3000_config_flag != 1)
	{
		g_Concentrator_Para.e3000_relay_enable[0] = 0;
		for(i = 0;i < 3;i++)
		{
			g_Concentrator_Para.e3000_V_enable[i] = 0;
			g_Concentrator_Para.e3000_A_enable[i] = 0;
		}
	}
	
	Read_Damp_Info();
	
	if(g_lamp_working_mode != 1)	//单灯非预约工作模式
	{
		g_lamp_working_mode = 0;
	}

	Read_Flash_RN8302_Config_Info();
	G_EffectPar_Info.Work_Mode = 0;   //混补模式
	G_EffectPar_Info.CT = 20*3.3;    //第一级互感器为100:5
	if(G_EffectPar_Info.adjust_valid_flag == 1)
	{
	   g_Voltage_Adjust_Ratio[0] = G_EffectPar_Info.voltage_adjust_ratio[0];
	   g_Voltage_Adjust_Ratio[1] = G_EffectPar_Info.voltage_adjust_ratio[1];
	   g_Voltage_Adjust_Ratio[2] = G_EffectPar_Info.voltage_adjust_ratio[2];
	   g_Current_Adjust_Ratio[0] = G_EffectPar_Info.current_adjust_ratio[0];
	   g_Current_Adjust_Ratio[1] = G_EffectPar_Info.current_adjust_ratio[1];
	   g_Current_Adjust_Ratio[2] = G_EffectPar_Info.current_adjust_ratio[2];
	}
	else
	{
	   g_Voltage_Adjust_Ratio[0] = 1000;
	   g_Voltage_Adjust_Ratio[1] = 1000;
	   g_Voltage_Adjust_Ratio[2] = 1000;
	   g_Current_Adjust_Ratio[0] = 1000;
	   g_Current_Adjust_Ratio[1] = 1000;
	   g_Current_Adjust_Ratio[2] = 1000;
	}

	RN8302_Init();

	I2C_LM75A_Init();
	
	DS1302_Init();
		
	OSTimeDly(500);
	InitTC();		

	Reset_GPRS_Module();              //复位GPRS模块
	g_identify_module_model_flag = 1;	
		
	if(g_lamp_total_num != 0)	//从节点档案不为空
	{	
		send_plc_reboot_bdp();
	}
/////////////////////////////////////////////////////////////////////////////////	
    g_read_falsh_data_info = 1; //初始化读取当日预约时间信息标志
	
/////////////////////////////////////////////////////////////////////////////////	
//	g_update_flag = FLASH_ReadHalfWord(APP_UPDATE_FLAG_ADDR);
	Write_APP_Flag_to_Flash(SPACE2);
}





