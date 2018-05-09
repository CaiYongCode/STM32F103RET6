#include "MessageProc.h"
#include "struct.h"
#include "GPRS.h"
#include "dacai_touch.h"
#include "PLC.h"
#include "wdg.h"
#include "ExtModule.h"
#include "TCP_Reconnect.h"
#include "IOctl.h"
#include "TimerTask.h"
/********************************************************************************************************** 
** Function name:      Task_COM_PROC
** Descriptions:       串口处理
** input parameters:   void *pdata
** output parameters:  无
** Returned value:     无h
**********************************************************************************************************/
extern OS_EVENT * QMsg; 

unsigned char m_touch_com_buf[USART_REC_LEN];
volatile unsigned char m_touch_com_len;

unsigned char m_gprs_com_buf[USART1_REC_LEN];
volatile unsigned short m_gprs_com_len;

unsigned char m_config_com_buf[USART_REC_LEN];
volatile unsigned char m_config_com_len;

unsigned char m_PLC_com_buf[USART_REC_LEN];
volatile unsigned char m_PLC_com_len;

unsigned char m_485_com_buf[USART_REC_LEN];
volatile unsigned char m_485_com_len;

unsigned char m_485_1_com_buf[USART_REC_LEN];
volatile unsigned char m_485_1_com_len;

void MsgProc_Task(void *pdata)
{
	R_msg  msg;
	INT8U  err;
	
	while(1)
	{ 	
		msg = *(R_msg *)OSQPend(QMsg,0,&err); // 等待消息队列发送消息过来
		if(err == OS_ERR_NONE)
		{
			switch (msg.msg_type)
			{
				case COM_1_MESSAGE_TYPE:   //GPRS 信息
					m_gprs_com_len = msg.msg_len;
					memset(m_gprs_com_buf,0,USART1_REC_LEN);
					memcpy(m_gprs_com_buf,(unsigned char *)msg.msg_addr, m_gprs_com_len);
					//当E3000处于模块识别、TCP连接调用Connect_and_Init流程
					if((g_identify_module_model_flag == 1) || (g_GPRS_in_Reg_stage == TRUE))
					{
						ActOn_TCP_Connect_Bdp(m_gprs_com_buf,m_gprs_com_len);
					}
					else  //正常 工作的时候调用GPRS流程
					{
						ActOn_GPRS_BDP(m_gprs_com_buf,m_gprs_com_len);
					}			
					break;
				case COM_2_MESSAGE_TYPE:  //PLC
					m_PLC_com_len = msg.msg_len;
					memset(m_PLC_com_buf,0,USART_REC_LEN);
					memcpy(m_PLC_com_buf,(unsigned char *)msg.msg_addr, m_PLC_com_len);			
					
//					COM4_Send_BDP(m_PLC_com_buf,m_PLC_com_len);
					
					ActOn_RTB42_BDP(m_PLC_com_buf,m_PLC_com_len);  //响应载波路由模块发送过来的串口数据
					plc_comm_flag = 2;		
					
					break;
				case COM_5_MESSAGE_TYPE:  //触摸屏
					m_touch_com_len = msg.msg_len;
					memset(m_touch_com_buf,0,USART_REC_LEN);
					memcpy(m_touch_com_buf,(unsigned char *)msg.msg_addr, m_touch_com_len);				
					ActOn_TouchScreen_BDP(m_touch_com_buf,m_touch_com_len);
					break;
				case COM_3_MESSAGE_TYPE:  //485_1	扩展模块	JP3
					m_485_1_com_len = msg.msg_len;
					memset(m_485_1_com_buf,0,USART_REC_LEN);
					memcpy(m_485_1_com_buf,(unsigned char *)msg.msg_addr, m_485_1_com_len);
					ActOn_ExtModule_BDP(m_485_1_com_buf,m_485_1_com_len);
					break;
				case COM_4_MESSAGE_TYPE:    //485	
					m_485_com_len = msg.msg_len;
					memset(m_485_com_buf,0,USART_REC_LEN);
					memcpy(m_485_com_buf,(unsigned char *)msg.msg_addr, m_485_com_len);
					ActOn_CONFIG_BDP(m_485_com_buf);	
					break;
				case TIMER_1000MS_MESSAGE_TYPE:	
					Send_GPRS_Bdp();
					Send_RTB42_Bdp();   //周期定时发送PLC报文
					break;
				case TIMER_10S_MESSAGE_TYPE:						
					break;
				case KEY_MESSAGE_TYPE:				
					ActOn_Key_BDP(msg.msg_addr);
					break;
				case FIRST_GET_TS_RTC_MESSAGE_TYPE:
                    sys_recovery_info();
					break;
				default:
					break;
			}
		}
		else
		{
						
		}
	}
}



// void ActOn_Timer_MSG(void)
// {		
//  
// }

//SN配置功能函数
void ActOn_CONFIG_BDP(unsigned char* pbuf)
{	
	unsigned char i = 0;
	
	if(pbuf[0] == 0x7E)
	{
		switch(pbuf[1])
		{
		  case 0x50:    //获取SN
			Get_RTU_Info_to_PC();
			break;
		  case 0x51:   //设置SN			 
			for(i = 0; i < 8 ; i++)
			{
			   g_E3000_Info.SN[i] =  pbuf[i + 3];
			}	
			Write_RTU_info_to_Flash();
			show_Concentrator_SN();			
			break;  				
		  default:
			break;
		}
	}
		
}
//发送SN到PC
 void Get_RTU_Info_to_PC(void)
 {
	unsigned short i = 0;
	
	Read_Flash_RTU_Info();
	
	com4_send_buf[0] = 0x7E;
	com4_send_buf[1] = 0x52;
	com4_send_buf[2] = 40;
	for(i = 0; i < 8 ; i++)
	{
	   com4_send_buf[3 + i] =  g_E3000_Info.SN[i];
	}	
	com4_send_buf[39] = 0xE7;

	COM4_Send_BDP(com4_send_buf, 40);
 }

 //按键功能函数
void ActOn_Key_BDP(unsigned char val)
{
	unsigned char m = 0,n = 0;
	switch(val)
	{
		case  KEY_OK_BTN:	//确定
			//调试模式下，按确定键，回路全开
			if(g_mannual_debug_flag == 1)
			{ 
				if(g_Concentrator_Para.e3000_relay_enable[0] != 0xFF)	//E3000继电器启用
				{
					E3000_Relay_Ctl(OPEN);		//打开E3000所有继电器
					g_Concentrator_Para.e3000_relay_enable[0] = 100;
				}
						
				for(m = 0;m < MAX_EXT_MODULE_NUM;m++)
				{
					for(n = 0;n < 4;n++)
					{
						if(g_Concentrator_Para.ext_relay_enable[m][n] != 0xFF)	//扩展模块继电器启用
						{	
							if( g_ext_online_flag[m] == ONLINE )	//扩展在线						
							{
								g_extmodule_relay[m][n] = 1;
								g_Concentrator_Para.ext_relay_enable[m][n] = 100;
							}
						}
					}
				}
				g_send_concentrator_parameter = 1;  //20S后上报参数  仅针对回路
			}
			break;
		case  KEY_RETURN_BTN:  //返回 
			//调试模式下，按返回键，回路全关
			if(g_mannual_debug_flag == 1)
			{
				if(g_Concentrator_Para.e3000_relay_enable[0] != 0xFF)	//E3000继电器启用
				{
					E3000_Relay_Ctl(CLOSE);		//关闭E3000所有继电器
					g_Concentrator_Para.e3000_relay_enable[0] = 0;
				}
				for(m = 0;m < MAX_EXT_MODULE_NUM;m++)
				{
					for(n = 0;n < 4;n++)
					{
						if(g_Concentrator_Para.ext_relay_enable[m][n] != 0xFF)	//扩展模块继电器启用
						{			
							g_extmodule_relay[m][n] = 0;
							g_Concentrator_Para.ext_relay_enable[m][n] = 0;
						}
					}
				}
				g_send_concentrator_parameter = 1;  //20S后上报参数  仅针对回路
			}
			break;
		case  KEY_SET_BTN:    //设置键			
			break;
		case  KEY_FUNCTION_BTN:   //功能键,开启/关闭调试模式
			g_mannual_debug_flag = 1 - g_mannual_debug_flag;
			if(g_mannual_debug_flag == 1)
			{
				Set_DEBUG_LED_Status(OPEN);
				g_debug_countdown_counter = 600;  //默认调试开始后 600秒后自动还原为工作模式
			}
			else
			{
				Set_DEBUG_LED_Status(CLOSE);
				g_debug_countdown_counter = 0;
			}
			break;
		default:
			break;   
	}
}

void sys_recovery_info(void)
{
	unsigned char i = 0, j = 0;
	unsigned char m_stage_index = 0;
	
	g_now_year = g_ts_year;
	g_now_month = g_ts_month;
	g_now_day = g_ts_day;
	g_now_hour = g_ts_hour;
	g_now_min = g_ts_min;
	g_now_sec = g_ts_sec;
	g_now_week = g_ts_week;
	//断电时间和启动时间差小于60s，则恢复重启前状态
	if( Read_CMD_Info(g_now_hour,g_now_min,g_now_sec) == 1 )
	{
		if(g_Concentrator_Para.e3000_relay_enable[0] == 100)
		{
			E3000_Relay_Ctl(OPEN);		//打开E3000所有继电器
		}
		else
		{
			E3000_Relay_Ctl(CLOSE);		//关闭E3000所有继电器
		}
		//扩展回路开灯且启用则亮度置100
		for(i = 0;i < MAX_EXT_MODULE_NUM;i++)
		{
			for(j = 0;j< 4;j++)
			{
				if( (g_extmodule_relay[i][j] == 1)
					&&(g_Concentrator_Para.ext_relay_enable[i][j] != 0xFF) )	//扩展继电器启用
				{
					g_Concentrator_Para.ext_relay_enable[i][j] = 100;
				}
			}
		}
	}
	else
	{
	    //自动重启，默认当前处于什么阶段
	     get_working_time(g_working_mode);
		 get_working_hours();
			
	    m_stage_index = judge_light_time_stage(g_now_hour,g_now_min);
		if(m_stage_index > 0)  //当前处于第m_stage_index时间段内
			{
			    open_time_stage(m_stage_index - 1);  //执行相应时间段内的开回路和单灯动作
			}
	}
}
