#include "TimerTask.h"
#include "RN8302.h"
#include "BIT.h"
#include "GPRS.h"
#include "PLC.h"
#include "dacai_touch.h"
#include "Key.h"
#include "IOctl.h"
#include "wdg.h"
#include "LM75A.h"
#include "globaldata.h"
#include "ExtModule.h"
#include "DS1302.h"
#include "Lamp.h"
#include "Update.h"
/********************************************************************************************************** 
**********************************************************************************************************/
extern OS_EVENT * QMsg; 


/********************************************************************************************************** 
** Function name:      Task_2000ms_Timer
** Descriptions:      
** input parameters:   void *pdata
** output parameters:  无
** Returned value:     无
**********************************************************************************************************/
unsigned short m_2000ms_counter = 0;

unsigned char m_working_time_num;
unsigned char m_send_working_time_flag = 0;
unsigned short m_heart_counter = 0;
unsigned short m_send_PLC_sys_time_Counter = 0;

unsigned char b_send_bus_state = 0;
unsigned char m_show_working_time_couter;
unsigned char m_show_working_time_flag;
unsigned char m_extmodule_relay[3][4];
void Task_2000ms_Timer(void)
{		
	{ 	
		m_2000ms_counter++;
////////////////////////////////////////////////////////////////////////////////////
		//20秒发一次心跳报文
		if(g_GPRS_connect_OK == ONLINE)
		{			
			m_heart_counter++;
			if(m_heart_counter >= 10)  
			{
				send_gprs_heart_bdp();
				m_heart_counter = 0;
			}
		}
//////////////////////////////////////////////////////////////////////////////
		//获取工作时间和时长 只有当当日时间为0:07的时候，才读取FLASH一次
		//获取到系统时间才获取工作时间
		if(g_first_get_TS_rtc_time_flag == 1)
		{
			get_working_time_and_hours();
			//显示工作时间
			show_working_time();
		}
		//////////////////////////////////////////////////////////////////////////////
		//预约开关控制  在线 离线都是 本地控
		//只有在非调试模式的时候,同时服务器下发工作模式为周或者月预约工作模式的时候，本地控制才起作用
		if ((g_mannual_debug_flag == 0) && ((g_working_mode == MONTH_MODE) || (g_working_mode == WEEK_MODE)) )    
		{
			//获取到系统时间才进行本地控制
			if(g_first_get_TS_rtc_time_flag == 1)
			{
				Loop_Lamp_Control_Func();	     //本地控制
			}
		}	

//////////////////////////////////////////////////////////////////////////////		
		m_send_PLC_sys_time_Counter++;
		if(m_send_PLC_sys_time_Counter > 120)     //4分钟同步一次PLC时间
		{
			b_send_bus_state = 1 - b_send_bus_state;
			if(b_send_bus_state == 1)
			{
				send_system_time_bdp();	          //同步单灯系统时间
			}
			else
			{
				send_working_time_bdp();		 //发送工作时间到单灯
			}
			
			m_send_PLC_sys_time_Counter = 0;
		}
//////////////////////////////////////////////////////////////////////////////			
	}			
}

unsigned short m_1000ms_counter = 0;
unsigned char m_show_damp_1000ms_counter = 0;
unsigned char m_e3000_relay_status = 0;
unsigned char bbb_2000ms_flag = 0;
unsigned char m_get_power_info_counter = 0;
unsigned char m_plc_cmd_resend_counter = 0;
unsigned char m_send_time_fault_flag = 0;
void Task_1000ms_Timer(void *pdata)
{	
	unsigned short value = 0;
	
#ifdef  DEBUG_MODE
	IWDG_Init(5,2000);    //分频数为128,重载值低11位有效  小于 2047,溢出时间为8s	
#endif

	while(1)
	{	
		m_1000ms_counter++;
		//运行灯1S循环亮灭
		Set_RUN_LED_Status(m_1000ms_counter % 2);
		//处于升级状态,不进行其他任务 8bb
		if(g_update_flag != 1)
		{
			//载波通信状态指示灯
			Plc_Led_Ctl();
	
///////////////////////////////////////////////////////////////////////////////////////		
			//2S任务
			bbb_2000ms_flag = 1 - bbb_2000ms_flag;
			if(bbb_2000ms_flag == 0)	
			{
				Task_2000ms_Timer();
			}			
///////////////////////////////////////////////////////////////////////////////////////	
			//获取并验证RTC和TS时间	
			Check_RTC_Time();
			Check_TS_Time();
			//默认使用触摸屏时间,
			//触摸屏时钟故障,使用触摸屏时间
			if(g_ts_time_fault == 0)
			{
				g_now_year = g_ts_year;
				g_now_month = g_ts_month;
				g_now_day = g_ts_day;
				g_now_hour = g_ts_hour;
				g_now_min = g_ts_min;
				g_now_sec = g_ts_sec;
				g_now_week = g_ts_week;
			}
			else
			{			
				g_now_year = g_rtc_year;
				g_now_month = g_rtc_month;
				g_now_day = g_rtc_day;
				g_now_hour = g_rtc_hour;
				g_now_min = g_rtc_min;
				g_now_sec = g_rtc_sec;
				g_now_week = g_rtc_week;
			}
///////////////////////////////////////////////////////////////////////////////////////			
			//RTC和TS时钟全部故障则上报时钟故障，否则解除时钟故障
			if( (g_rtc_time_fault == 1)&&(g_ts_time_fault == 1) )
			{
				if(m_send_time_fault_flag == 0)
				{
					send_concentrator_fault_bdp(CONCENTRATOR_FAULT_3,0);
					m_send_time_fault_flag = 1;
				}
			}
			else //如果两者有一个时钟正常，且之前上报过故障，则解除时钟故障
			{
				if( m_send_time_fault_flag == 1)
				{
					send_concentrator_fault_release_bdp(CONCENTRATOR_FAULT_3,0);
					m_send_time_fault_flag = 0;
				}
			}
///////////////////////////////////////////////////////////////////////////////////////	
			//每20秒保存一次系统时间 以备重启还原状态用
			if((m_1000ms_counter % 20) == 19)  
			{
				Write_CMD_Info();	//保存开关灯命令
			}
///////////////////////////////////////////////////////////////////////////////////////		
			//相差2度上报并更新显示
			value = Get_LM75A_Temp_Value();
			if( 20 <= abs(g_LM75A_temp - value) )  
			{		
				g_LM75A_temp = value;
				show_temperature();//显示温度
				send_concentrator_temperature_bdp();
			}
///////////////////////////////////////////////////////////////////////////////////////		
			//路由通信故障判断
			g_PLC_timeout_1000ms_counter++;
			if(g_PLC_timeout_1000ms_counter > PLC_COMMUNICATION_OVERTIMR)	//单片机与路由通信超时,5分钟
			{						
				g_router_fault_reboot_num++;	
				//超过三次复位路由仍然建立不了通信
				if(g_router_fault_reboot_num >= 3)
				{					
					g_router_fault_reboot_num = 0;						
					if( g_router_fault_flag != 1 )
					{
						send_concentrator_fault_bdp(CONCENTRATOR_FAULT_1,0);	//上报集中器与路由通信故障
						g_router_fault_flag = 1;
					}								
				}
				g_PLC_timeout_1000ms_counter = 0;
				send_hardware_init_bdp();	//路由复位
			}	
			//路由抄读超时判断
			if(g_lamp_total_num != 0)	//从节点档案不为空
			{	
				g_plc_read_1000ms_couter++;
				if(g_plc_read_1000ms_couter > PLC_READ_OVERTIMR)
				{
					send_plc_restore_bdp();		//恢复抄读
					g_plc_read_1000ms_couter = 0;
				}
			}
			//路由抄读完成，60S后重启抄读
			if(g_read_restart_flag == 1)
			{
				g_read_restart_counter++;
				if(g_read_restart_counter >= 60)
				{
					show_lamp_statistics();		//显示单灯统计信息
					clear_lamp_read_flag();	//重启前清空单灯参数
					send_plc_reboot_bdp();
					
					g_read_restart_flag = 0;
					g_read_restart_counter = 0;
				}
			}
			//检测单灯开关和故障
			check_lamp_status();
//////////////////////////////////////////////////////////////////////////////////////////////////////
			//检查供电方式 并处理
			check_power_supply();
			 //扩展模块发送周期命令报文
			Send_CMD_bdp_to_ExtModule();
			//扩展模块在线判断
			Ext_Module_online_judge();
			//手动调试开关灯模式倒计时
			if(g_debug_countdown_counter > 0)
			{
			   g_debug_countdown_counter --;
			   //退出到正常工作状态，之前开关灯状态还保留
			   if(g_debug_countdown_counter == 0)
			   	{
			   	   g_mannual_debug_flag = 0;    //关闭调试模式
				   Set_DEBUG_LED_Status(CLOSE);  
			   	}
			}
//////////////////////////////////////////////////////////////////////////////////////////////////////
		

/////////////////////////////////////////////////////////////////////////////////////////////////////
			//定时采集电压电流状态
			get_power_info();

			//判断参数是否需要上报到服务器
			//上报时机  5分钟上报一次  
			//开关灯后时10秒钟后上报
			if(g_send_concentrator_parameter == 1)  //开关回路时候
			{
				//判断是否有回路由开到关，需要复位8302
				judge_Loop_status();
				
				m_get_power_info_counter++;  //电压电流检测计时
				//关灯后10S复位8302
				if( (g_reset_E3000_8302_flag == 1)&&(m_get_power_info_counter == 10) )
				{
					Reset_8302_Module();
					RN8302_Init();
					g_reset_E3000_8302_flag = 0;
				}

				//获取集中器电压电流并根据电流变动进行上报
				if(m_get_power_info_counter >= 20)	//20S上报
				{			
					//上报集中器参数
					send_concentrator_parameter_bdp();

					m_get_power_info_counter = 0;
					g_send_concentrator_parameter = 0;
				}
			}
			else  //非开关回路时候
			{
				//5分钟定时上报
				if((m_1000ms_counter % 300) == 20) 
				{
					send_concentrator_parameter_bdp();
				}
			}
/////////////////////////////////////////////////////////////////////////////////////////////////////  					 	
			SendMessage(TIMER_1000MS_MESSAGE_TYPE,0,0);
		}
/////////////////////////////////////////////////////////////////////////////////////////////////////		
		//验证升级状态
		Check_Update_Status();
//////////////////////////////////////////////////////////////////////////////////////////////////////			
		IWDG_Feed();  //喂狗
		
		OSTimeDly(500);
	}
}

void Task_Key_Scan(void *pdata)
{
	while(1)
	{
		Key_Scan();
		OSTimeDly(10);
	}		
}


unsigned char m_start_cmd[8] = {0};	//8段工作开始时间开灯标志
unsigned char m_end_cmd[8] = {0};	//8段工作停止时间开灯标志
//回路以及单灯  开关控制 定时执行
void Loop_Lamp_Control_Func(void)
{
	unsigned char i = 0;
	unsigned short m_val = 0;	            //实时时间

	//获取实时时间
	m_val = g_now_hour * 60 + g_now_min;

	//依次检测8段工作时间
	for(i = 0; i < 8; i++)
	{	
		if(t_v[i] == 1)                      //当前时间段有效
		{ 
			if( m_val == t_s[i])	         //开灯时刻到  
			{			
				if(m_start_cmd[i] == 0)		 //保证只执行一次
				{
				    m_start_cmd[i] = 1;
					/////////////////////////////////////////////////////////////////////////////////////
					open_time_stage(i);
					/////////////////////////////////////////////////////////////////////////////////////
					send_local_time_bdp(0);
				}
				
			}
			else
			{
			  //过了t_s[i](分钟)时，把标志位置零 同时因为已经不是t_s[i]时刻，所以上面已经进不去了，保证只有一次进入并执行开灯代码
			  m_start_cmd[i] = 0;
			}
           //////////////////////////////////////////////////////////////////////////////////////////
			if(m_val == t_e[i])   //关灯时刻到
			{
				if(m_end_cmd[i] == 0)		   //保证只执行一次
				{
                    m_end_cmd[i] = 1;	
					
					close_time_stage(i);
				/////////////////////////////////////////////////////////////////////////////////////
					send_local_time_bdp(0);
				}
			}
			else
			{
			   m_end_cmd[i] = 0;
			}
			//////////////////////////////////////////////////////////////////////////////////////
		}			
	}
}

unsigned char m_now_month = 0;
void get_working_time_and_hours(void)
{
  ////////////////////////////////////////////////////////////////////////
   //只有当0:07分时间到，并且只读一次(g_get_at_update_time_flag = 0)才读取FLASH一次
   
	if((g_read_falsh_data_info == 1) && (g_get_at_update_time_flag == 0))
		{
		    get_working_time(g_working_mode);
			get_working_hours();
			g_read_falsh_data_info = 0;     // 确保只读一次
			g_get_at_update_time_flag = 1;  // 确保只读一次

		}
	/////////////////////////////////////////////////////////////////////////
	if((g_now_hour == 0) &&(g_now_min == 0x07)) // 每天0:07分 更新时间
	{
		g_read_falsh_data_info = 1;
	}
	else
	{
		g_read_falsh_data_info = 0;     // 确保只读一次
		g_get_at_update_time_flag = 0;   //清空标志，为了下一次更新
	}
}

void get_working_time(unsigned char working_mode)
{
	unsigned char i = 0;
	unsigned char index = 0;
	
	m_working_time_num = 0;
   //////////////////////////////////////////////////////////////////////////////////////////////////////


	if(working_mode == WEEK_MODE)	//按周工作模式
	{
		Read_Week_Light_Time_Info();
	}
	else if(working_mode == MONTH_MODE)	//按月工作模式
	{
		Read_Month_Light_Time_Info();
	}
	//////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////
    for(i = 0;i < 8;i++)
	{
		//判断工作时间是否有效
		t_v[i] = check_time_valid(g_Light_Time_Info.s_time[i],g_Light_Time_Info.e_time[i]);
		if(t_v[i] == 1)
		{		
			//拼装工作时间
			sprintf((char*)g_showtime_buf[index],"%2d:%2d-%2d:%2d",g_Light_Time_Info.s_time[i]>>8,g_Light_Time_Info.s_time[i]&0xff,\
						g_Light_Time_Info.e_time[i]>>8,g_Light_Time_Info.e_time[i]&0xff);				
			index++;
			m_working_time_num++;
		}
	}
	//清空无效工作时间显示缓存
	for(i = index;i < 8;i++)
	{
		memset(g_showtime_buf[i],0,32);
	}
	//////////////////////////////////////////////////////////////////////////
	//获取新的工作时间之后     显示工作时间标志位置1
	m_show_working_time_flag = 1;
	//////////////////////////////////////////////////////////////////////////
	//上报本地时间
	send_local_time_bdp(0);	
	//上报工作时间
	send_local_working_time_bdp();
}

void get_working_hours(void)
{
	unsigned char i = 0;
	
	g_lamp_working_time_num = 0;
	
	for(i = 0;i < 8;i++)
		{
			t_s[i] = g_Light_Time_Info.s_time[i]/256*60+g_Light_Time_Info.s_time[i]%256;
			t_e[i] = g_Light_Time_Info.e_time[i]/256*60+g_Light_Time_Info.e_time[i]%256;
			t_g[i] = g_Light_Time_Info.group[i];
			t_l[i] = g_Light_Time_Info.loop[i];
			t_r[i][0] = g_Light_Time_Info.ratio[i][0];
			t_r[i][1] = g_Light_Time_Info.ratio[i][1];
			t_r[i][2] = g_Light_Time_Info.ratio[i][2];
			
			if(t_g[i] != 0)	//对单灯
			{	
				if(g_lamp_working_time_num < 4)		//只取4段	
				{
					g_lamp_working_time_index[g_lamp_working_time_num] = i;
					g_lamp_working_time_num++;
				}
				
			}
		}
}

void open_time_stage(unsigned char satge_inedx)
{

	unsigned char contents[18] = {0};	    //单灯命令内容
	unsigned short m_swicth = 0;	        //回路或组别暂存
	unsigned char m_open = 0;		        //两分钟内开灯标志
	unsigned char m = 0,n = 0;
	unsigned char addr[6] = {0};	        //单灯地址

    if(t_g[satge_inedx] == 0)		     //对E3000命令
	{
		m_swicth = t_l[satge_inedx];
		//E3000回路1开关判断
		//当前要开
		if( (m_swicth & 0x01) == 0x01 )	
		{
			if(g_Concentrator_Para.e3000_relay_enable[0] != 0xFF)	//E3000继电器启用
			{
				E3000_Relay_Ctl(OPEN);		//打开E3000所有继电器
				g_Concentrator_Para.e3000_relay_enable[0] = 100;
			}
		}
		
		else
		{						
			if(g_Concentrator_Para.e3000_relay_enable[0] != 0xFF)	//E3000继电器启用
			{
				E3000_Relay_Ctl(CLOSE);		//关闭E3000所有继电器
				g_Concentrator_Para.e3000_relay_enable[0] = 0;
			}
		}
		//扩展回路开关判断
		m_swicth = m_swicth >> 1;
		for(m = 0 ; m < MAX_EXT_MODULE_NUM ; m++)
		{
			for(n = 0 ; n < 4 ; n++)
			{
				if(g_Concentrator_Para.ext_relay_enable[m][n] != 0xFF)	//扩展模块继电器启用
				{
					m_open = ( m_swicth >> (m*4+n) ) & 0x01;
					
					//命令开同时扩展在线，执行开灯
					if( (m_open == 1)&&(g_ext_online_flag[m] == ONLINE) )	
					{										
						g_extmodule_relay[m][n] = 1;
						g_Concentrator_Para.ext_relay_enable[m][n] = 100;
					}
					
					else
					{
						g_extmodule_relay[m][n] = 0;
						g_Concentrator_Para.ext_relay_enable[m][n] = 0;
					}
					
				}
			}
			
		}
		
		g_send_concentrator_parameter = 1;  //20S后上报参数
//////////////////////////////////////////////////////		
		send_loop_debuf_inf0_to_PC();
	}
    else				//对单灯命令
	{
		contents[0] = t_l[satge_inedx]/256;
		contents[1] = t_l[satge_inedx]%256;
		contents[15] = t_r[satge_inedx][2];
		contents[16] = t_r[satge_inedx][1];
		contents[17] = t_r[satge_inedx][0];
		send_plc_cmd_bdp(addr,t_g[satge_inedx],contents);

////////////////////////////////////////////////////////		
		send_lamp_debuf_inf0_to_PC(addr,t_g[satge_inedx],contents);
	}


}


void close_time_stage(unsigned char satge_inedx)
{
    unsigned char contents[18] = {0};	    //单灯命令内容
	unsigned char m_open = 0;		        //两分钟内开灯标志
	unsigned char m = 0,n = 0 ,j = 0;
	unsigned char addr[6] = {0};	        //单灯地址
	unsigned short m_group = 0;		        //单灯组
	unsigned short m_g = 0;					//本时间段的单灯组
	unsigned short m_other_g = 0;		//其他时间段的单灯组

	if(t_g[satge_inedx] == 0)		//对E3000命令
	{	
		m_open = 0;	//默认执行关灯
		//检测回路在其他7段工作时间的开关状态
		for(j = 0;j < 8;j++)
		{
			//不与本段工作时间比较
			if(j == satge_inedx)
			{
				continue;
			}
			if(t_g[j] == 0)	//对E3000命令
			{
			    //如果在2分钟内有开灯操作，则不执行关灯  还有跨零点的情况考虑上
				if( abs(t_s[j]-t_e[satge_inedx]) < 2)
				{
					m_open = 1;
					break;
				}
			}							
		}
		if(m_open == 0)	//执行关灯
		{
			E3000_Relay_Ctl(CLOSE);		//关闭E3000所有继电器
			if(g_Concentrator_Para.e3000_relay_enable[0] != 0xFF)	//回路启用
			{															
				g_Concentrator_Para.e3000_relay_enable[0] = 0;
			}
						
			//关闭扩展所有继电器
			for(m = 0;m < MAX_EXT_MODULE_NUM;m++)
			{
				for(n = 0;n < 4;n++)
				{
					g_extmodule_relay[m][n] = 0;
					if(g_Concentrator_Para.ext_relay_enable[m][n] != 0xFF)	//扩展继电器启用
					{
						g_Concentrator_Para.ext_relay_enable[m][n] = 0;
					}								
				}
			}
			
			g_send_concentrator_parameter = 1;  //20S后上报参数
			
			send_loop_debuf_inf0_to_PC();
		}
	}
	else				//对单灯命令
	{
		m_group = t_g[satge_inedx];
		m_g = t_g[satge_inedx];
		for(m = 0;m < 16;m++)
		{							
			if( ((m_g>>m)&0x01) == 1 )
			{
				for(j = 0;j < 8;j++)
				{
					//不与本段工作时间比较
					if(j == satge_inedx)
					{
						continue;
					}
					if(t_g[j] != 0) //对单灯命令
					{
						m_other_g = t_g[j];
						//如果该单灯组在2分钟内有开灯操作，则不执行关灯
						if(((m_other_g>>m)&0x01) == 1)
						{
							if( abs(t_s[j]-t_e[satge_inedx]) < 2)
							{
								m_group &= (~(1<<m));	//该组置0，则该组不执行关灯
								break;
							}
						}
					}
				}
			}
		}
		send_plc_cmd_bdp(addr,m_group,contents);
//////////////////////////////////////////////////	
		send_lamp_debuf_inf0_to_PC(addr,m_group,contents);		
	}
	
}

//显示工作时间
void show_working_time(void)
{
	unsigned char i = 0;
	
	if(m_working_time_num >= 5)	//超过4段工作时间，20S循环显示
	{
		m_show_working_time_couter++;
		if(m_show_working_time_couter >= 10)	//20S循环
		{
			if(m_show_working_time_flag == 1)	//显示前4段工作时间
			{
				for(i = 0;i < 4;i++)
				{
					Show_Light_Time(i,i);
				}
				m_show_working_time_flag = 0;
			}
			else	//显示后4段工作时间
			{
				for(i = 4;i < 8;i++)
				{
					Show_Light_Time(i-4,i);
				}
				m_show_working_time_flag = 1;
			}
			m_show_working_time_couter = 0;
		}
	}
	else	//小于4段工作时间,只显示1次
	{
		if(m_show_working_time_flag == 1)
		{
			for(i = 0;i < 4;i++)
			{
				Show_Light_Time(i,i);
			}
			
			m_show_working_time_flag = 0;
		}
	}
}
//验证工作时间是否有效
unsigned char check_time_valid(unsigned short s_time,unsigned short e_time)
{
	if( (s_time == 0)&&(e_time == 0) )
	{
		return 0;
	}
	else if( (s_time == 0xFFFF)||(e_time == 0xFFFF) )
	{
		return 0;
	}
	return 1;
}

//获取集中器电压电流信息
void get_power_info(void)
{
	unsigned char i = 0;
	unsigned short m_amp[3] = {0};
	unsigned short m_vol[3] = {0};
	unsigned char buf[16] = {0};
	unsigned char len = 0;
	
	Read_PhasePower_Info();//读取电压电流值
	for(i = 0; i < 3;i++)
	{
		m_amp[i] = g_Phase_Amp[i];
		m_vol[i] = g_Phase_Voltage[i];
		
		g_Phase_Voltage[i] = G_PhasePower_Info[i].Ux * 10 * g_Voltage_Adjust_Ratio[i]/1000;
		g_Phase_Amp[i] = (G_PhasePower_Info[i].Ix / 10) * g_Current_Adjust_Ratio[i]/1000;  //IX的分辨率为*100	
		//屏蔽板间电流小于0.1A清零
		if(g_Phase_Amp[i] <= 10)
		{
			g_Phase_Amp[i] = 0;
		}
		
		g_Concentrator_Para.Voltage[i] = g_Phase_Voltage[i];
		g_Concentrator_Para.e3000_current[i] = g_Phase_Amp[i];
	}
	for(i = 0; i < 3;i++)
	{
		//电流变化超过1%并且变化值大于1A，上传服务器，同时更新界面显示
		//当电流从非0值变为0值时，更新界面显示
		if( (((m_amp[i]*LARGE_CURRENT_VARIATION_RATIO) <= abs(m_amp[i]-g_Phase_Amp[i]))
			&&(CURRENT_VARIATION_THRESHOLD < abs(m_amp[i]-g_Phase_Amp[i])))
			||((m_amp[i] != 0)&&(g_Phase_Amp[i] == 0)) )
		{		
			len = data_conversion(buf,g_Phase_Amp[i],DECIMAL2);
			show_text(DGUS_STATE_SHOW_ADDR,16+i,buf,len);
		}
		//电压变化值大于5V，更新界面显示
		if(VOLTAGE_VARIATION_THRESHOLD <= abs(m_vol[i]-g_Phase_Voltage[i]) )
		{			
			len = data_conversion(buf,g_Phase_Voltage[i],DECIMAL1);
			show_text(DGUS_STATE_SHOW_ADDR,13+i,buf,len);
		}
	}
}

unsigned char m_extmodule_relay[3][4];
//检测回路状态
void judge_Loop_status(void)
{
   unsigned char m = 0, n = 0;
	
	//判断集中器继电器状态变化，由开到关则复位8302
	if(m_e3000_relay_status != g_Concentrator_Para.e3000_relay_enable[0])
	{
		m_e3000_relay_status = g_Concentrator_Para.e3000_relay_enable[0];
		//关灯后复位8302
		if(g_Concentrator_Para.e3000_relay_enable[0] == 0)
		{
			g_reset_E3000_8302_flag = 1;
			g_reset_ext_8302_flag = 1;		
		}
	}
   //判断扩展继电器状态变化，任1继电器由开到关则复位8302

	for(m = 0;m < MAX_EXT_MODULE_NUM;m++)
	{
			for(n = 0;n < 4;n++)
			{
				if(m_extmodule_relay[m][n] != g_extmodule_relay[m][n])	//相应回路状态变化
				{
					m_extmodule_relay[m][n] = g_extmodule_relay[m][n];
					if(g_extmodule_relay[m][n] == 0)	//由开到关
					{
						g_reset_E3000_8302_flag = 1;
						g_reset_ext_8302_flag = 1;				
					}
				}								
			}
	} 
}

/////////////////////////////////////////////////////
unsigned char m_rtc_time_check_times = 0;	//RTC时间验证次数
unsigned char m_rtc_time_correct_counter = 0;//RTC时间正确计数
unsigned char m_rtc_sec = 0;	//RTC秒暂存
//获取并验证RTC时间
void Check_RTC_Time(void)
{
	//获取RTC时间
	Get_RTC_Time();	

	//比对两次采集的秒数据，相差1或2S为正常
	if( (abs(m_rtc_sec - g_rtc_sec) <= 3)&&(m_rtc_sec != g_rtc_sec) )
	{
		m_rtc_time_correct_counter++;
	}
	m_rtc_sec = g_rtc_sec;
	m_rtc_time_check_times++;
	
	//每轮进行10次比较，并作时钟故障判断
	if(m_rtc_time_check_times >= 10)
	{
		//正常计数大于等于8次，则判断时钟正常，否则故障
		if(m_rtc_time_correct_counter >= 8)
		{
			g_rtc_time_fault = 0;
		}
		else
		{
			g_rtc_time_fault = 1;
		}
		
		m_rtc_time_check_times = 0;
		m_rtc_time_correct_counter = 0;
	}
}

unsigned char m_ts_time_check_times = 0;	//TS时间验证次数
unsigned char m_ts_time_correct_counter = 0;//TS时间正确计数
unsigned char m_ts_sec = 0;	//TS秒暂存
//获取并验证TS时间
void Check_TS_Time(void)
{		
	Get_TS_Time();
	
	//比对两次采集的秒数据，相差1或2S为正常
	if( (abs(m_ts_sec - g_ts_sec) <= 3)&&(m_ts_sec != g_ts_sec) )
	{
		m_ts_time_correct_counter++;
	}
	m_ts_sec = g_ts_sec;
	m_ts_time_check_times++;
	//每轮进行10次比较，并作时钟故障判断
	if(m_ts_time_check_times >= 10)
	{
		//正常计数大于等于8次，则判断时钟正常，否则故障
		if(m_ts_time_correct_counter >= 8)
		{
			g_ts_time_fault = 0;
		}
		else
		{
			g_ts_time_fault = 1;
		}
		
		m_ts_time_check_times = 0;
		m_ts_time_correct_counter = 0;
	}
}
/////////////////////////////////////////////////////

