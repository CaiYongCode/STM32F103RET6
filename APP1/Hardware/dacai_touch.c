#include "dacai_touch.h"
#include "struct.h"
#include "PLC.h"
#include "globaldata.h"
#include "GPRS.h"
#include "globalfunc.h"
#include "DS1302.h"
#include "Lamp.h"
#include "IOctl.h"
#include "ExtModule.h"


unsigned char page = 0;

void InitTC(void)
{
	Change_Page(DGUS_STATE_SHOW_ADDR);

	show_simcard_num();
	show_Concentrator_SN();	
}

//解析串口屏信息
void ActOn_TouchScreen_BDP(unsigned char* pbuf,unsigned char len)
{	
	unsigned char Screen_id = 0;
	
	if(pbuf[1] == 0xF7)  //获取当前触摸屏RTC时间
	{
		
		g_ts_year = (pbuf[2]>>4)*10 + (pbuf[2]&0x0F);
		g_ts_month = (pbuf[3]>>4)*10 + (pbuf[3]&0x0F);
		g_ts_week = pbuf[4];
		if(g_ts_week == 0)
		{
			g_ts_week = 7;
		}
		g_ts_day = (pbuf[5]>>4)*10 + (pbuf[5]&0x0F);
		g_ts_hour = (pbuf[6]>>4)*10 + (pbuf[6]&0x0F);
		g_ts_min  = (pbuf[7]>>4)*10 + (pbuf[7]&0x0F);
		g_ts_sec  = (pbuf[8]>>4)*10 + (pbuf[8]&0x0F);
		
		if(g_first_get_TS_rtc_time_flag == 0)
		{
			g_first_get_TS_rtc_time_flag = 1;
			SendMessage(FIRST_GET_TS_RTC_MESSAGE_TYPE,0,0);
		}
	}
	else if(pbuf[1] == 0xB1)
	{
		Screen_id = pbuf[4];	
		
		switch(Screen_id)
		{
			case 2:    //状态显示界面
				g_now_screen_id = Screen_id;
				screen_2_page(pbuf,len);
				break;
			case 3:    //节点管理界面
				g_now_screen_id = Screen_id;
				screen_3_page(pbuf,len);
				break;
			case 4:    //功能设置界面
				g_now_screen_id = Screen_id;
				screen_4_page(pbuf,len);
				break;
			case 5:		//时间设置界面
				g_now_screen_id = Screen_id;
				screen_5_page(pbuf,len);
				break;
			case 6:		//单灯设置界面
				g_now_screen_id = Screen_id;
				screen_6_page(pbuf,len);
				break;
			case 7:    //节点抄读界面
				g_now_screen_id = Screen_id;
				screen_7_page(pbuf,len);
				break;
			case 8:		//手动控制
				g_now_screen_id = Screen_id;
				screen_8_page(pbuf,len);
				break;	
			case 9:		//扩展模块设置界面
				g_now_screen_id = Screen_id;
				screen_9_page(pbuf,len);
				break;
			case 10:		//扩展模块1设置界面
				g_now_screen_id = Screen_id;
				screen_10_page(pbuf,len);
				break;
			case 11:		//扩展模块2设置界面
				g_now_screen_id = Screen_id;
				screen_11_page(pbuf,len);
				break;
			case 12:		//扩展模块3设置界面
				g_now_screen_id = Screen_id;
				screen_12_page(pbuf,len);
				break;
			case 13:    //单灯信息索引界面
 				g_now_screen_id = Screen_id;
				screen_13_page(pbuf,len);
				break;
			case 14:  //提示界面
				screen_14_page(pbuf,len);
				break;
			case 15:  //集中器配置界面
				g_now_screen_id = Screen_id;
				screen_15_page(pbuf,len);
				break;
			case 17:  //单灯地址设置界面
				g_now_screen_id = Screen_id;
				screen_17_page(pbuf,len);
				break;
			case 18:  //单灯调试界面
				g_now_screen_id = Screen_id;
				screen_18_page(pbuf,len);
				break;
			case 19:  //密码输入界面
				g_now_screen_id = Screen_id;
				screen_19_page(pbuf,len);
				break;
			case 20:  
				g_now_screen_id = Screen_id;
				break;
			default:
				break;					
		}
	}	
}

//状态显示界面
void screen_2_page(unsigned char *pbuf,unsigned char len)
{
// 	unsigned char Control_id;
// 	
// 	Control_id = pbuf[6];
// 						
// 	switch(Control_id)
// 	{
// 		case 0x01: 	   	
// 			break;
// 		case 0x02: 			
// 			break;
// 		case 0x03:
// 			break;
// 		case  0x04:				
//  			break;
// 		default:
// 			break;		
// 	}
}

//节点管理界面
void screen_3_page(unsigned char *pbuf,unsigned char len)
{
	unsigned char Control_id;
	unsigned char addr[6] = {0};

	show_text(DGUS_DAMP_MANAGE_SHOW_ADDR,11,NULL,0);	//清空提示框

	Control_id = pbuf[6];
			
	switch(Control_id)
	{
		case 0x03: 	 
			g_now_screen_id = DGUS_STATE_SHOW_ADDR;
			break;
		case 0x04: 
			g_now_screen_id = DGUS_STATE_SHOW_ADDR;		
			break;
		case 0x05: 
			g_now_screen_id = DGUS_FUNCTION_SET_SHOW_ADDR;
			break;
		case  0x06: 	//添加从节点编辑框
		 	get_edit_addr(pbuf,len,g_add_damp_addr);
	    break;			
		case  0x07:		//删除从节点编辑框
			get_edit_addr(pbuf,len,g_del_damp_addr);
	    break;
		case 0x08: //添加从节点按钮
		    switch(Add_One_Lamp_Info(g_add_damp_addr))
		    	{
		    	  case INVALID_LAMP_ADDR:
				  	show_text(DGUS_PROMPT_SHOW_ADDR,1,"请输入8位有效地址",strlen("请输入8位有效地址"));
					Icon_Show(DGUS_PROMPT_SHOW_ADDR,3,1);
					Change_Page(DGUS_PROMPT_SHOW_ADDR);
				  	break;
				  case LAMP_ADDR_FULL:
				  	show_text(DGUS_PROMPT_SHOW_ADDR,1,"节点档案已满",strlen("节点档案已满"));
					Icon_Show(DGUS_PROMPT_SHOW_ADDR,3,2);
					Change_Page(DGUS_PROMPT_SHOW_ADDR);
					break;
				  case ADD_LAMP_ADDR_SUCESS:					
				  	HEXaddr_to_BCDaddr(g_add_damp_addr,addr);
			        send_add_damp_bdp(addr);
				  	break;	
				  default:
				  	break;
				  	
		    	}
	   	break;
		case 0x09:	//删除从节点按钮 
		    if(Del_One_Lamp_Info(g_del_damp_addr) != DEL_LAMP_ADDR_SUCESS)
			{
				show_text(DGUS_PROMPT_SHOW_ADDR,1,"请输入8位有效地址",strlen("请输入8位有效地址"));
				Icon_Show(DGUS_PROMPT_SHOW_ADDR,3,1);
				Change_Page(DGUS_PROMPT_SHOW_ADDR);
				break;
			}
			else
			{
				HEXaddr_to_BCDaddr(g_del_damp_addr,addr);
				send_del_damp_bdp(addr);
			}
			break;
		case 10:	//查看从节点
			g_now_screen_id = DGUS_DAMP_SHOW_ADDR;
			break;
		case 12:	//查看回路信息
			Show_Loop_Info(1);
			break;
		case 13:	//清空从节点档案			
//			Clear_All_Lamp_Info();
			break;
		default:
			break;	
	}
	
	
}

//功能设置界面
void screen_4_page(unsigned char *pbuf,unsigned char len)
{
	unsigned char Control_id;
	Control_id = pbuf[6];
	switch(Control_id)
	{
		case 0x03: 	 
			g_now_screen_id = DGUS_STATE_SHOW_ADDR;
			break;
		case 0x04: 
			g_now_screen_id = DGUS_STATE_SHOW_ADDR;		
			break;
		case 0x05:	//
			g_now_screen_id = DGUS_DAMP_MANAGE_SHOW_ADDR;
			break;
		case 0x06:	//
			g_now_screen_id = DGUS_TIME_SET_SHOW_ADDR;
			break;
		case 0x07: 	 
			g_now_screen_id = DGUS_DAMP_READ_SHOW_ADDR;			
			break;
		case 0x08:	//
			g_now_screen_id = DGUS_MANUAL_CONTROL_SHOW_ADDR;
			break;
		case 0x09:
			g_now_screen_id = DGUS_CONCENTRATOR_SET_SHOW_ADDR;
			show_ctr_config_info();
			break;
		default:
			break;	
	}
}



//时间和卡号设置界面
void screen_5_page(unsigned char *pbuf,unsigned char len)
{
	unsigned char i = 0;
	unsigned char Control_id;
	
	Control_id = pbuf[6];

	switch(Control_id)
	{
		case 3:
			g_now_screen_id = DGUS_STATE_SHOW_ADDR;
			break;
		case 4:
			if((len != 26) || (pbuf[8] != 0x31))	//未输入13位号码，或者首位不为1
			{
				show_text(DGUS_PROMPT_SHOW_ADDR,1,"请输入13位有效号码",strlen("请输入13位有效号码"));
				Icon_Show(DGUS_PROMPT_SHOW_ADDR,3,1);
				Change_Page(DGUS_PROMPT_SHOW_ADDR);	
			}
			else
			{
				for(i = 0;i < 13;i++)	
				{
					g_E3000_Info.simcard_num[i] = a_to_i(pbuf[i]);
				}							
			}
			break;
		case 5:		//设置号码
			Write_RTU_info_to_Flash();
			show_simcard_num();	
			break;
		case 6:		//年
			g_set_year = a_to_i(pbuf[8])* 10 + a_to_i(pbuf[9]);
			break;
		case 7: 	 //月
			if(len == 15)
			{
				g_set_month = a_to_i(pbuf[8])* 10 + a_to_i(pbuf[9]);
			}
			if(len == 14)
			{
				g_set_month = a_to_i(pbuf[8]);
			}
			break;
		case 8: 	 //日
			if(len == 15)
			{
				g_set_day = a_to_i(pbuf[8])* 10 + a_to_i(pbuf[9]);
			}
			if(len == 14)
			{
				g_set_day = a_to_i(pbuf[8]);
			}
			break;	
		case 9: 	 //时
			if(len == 15)
			{
				g_set_hour = a_to_i(pbuf[8])* 10 + a_to_i(pbuf[9]);
			}
			if(len == 14)
			{
				g_set_hour = a_to_i(pbuf[8]);
			}
			break;
		case 10:	//分
			if(len == 15)
			{
				g_set_min = a_to_i(pbuf[8])* 10 + a_to_i(pbuf[9]);
			}
			if(len == 14)
			{
				g_set_min = a_to_i(pbuf[8]);
			}
			break;
		case 11:	//秒
			if(len == 15)
			{
				g_set_sec = a_to_i(pbuf[8])* 10 + a_to_i(pbuf[9]);
			}
			if(len == 14)
			{
				g_set_sec = a_to_i(pbuf[8]);
			}
			break;
		case 12:	//设置时间
			g_now_year = g_set_year;	
			g_now_month = g_set_month;
			g_now_day = g_set_day;
			g_now_hour = g_set_hour;
			g_now_min = g_set_min;
			g_now_sec = g_set_sec;
		
			Set_TS_Time();			//发送系统时间到触摸屏
			Set_RTC_Time();		////发送系统时间到DS1302
			break;
		case 13:
			g_now_screen_id = DGUS_FUNCTION_SET_SHOW_ADDR;
			break;
		default:
			break;
	}
}

//单灯设置界面
void screen_6_page(unsigned char *pbuf,unsigned char len)
{
	unsigned char Control_id;
	
	Control_id = pbuf[6];

	switch(Control_id)
	{
		case 8:
			if(g_lamp_working_mode == 1)
			{
				Icon_Show(DGUS_MANUAL_CONTROL_SHOW_ADDR,13,1);	//显示勾选	
			}
			break;
		default:
			break;
	}
}

//节点抄读界面
void screen_7_page(unsigned char *pbuf,unsigned char len)
{
	unsigned short i = 0;
	unsigned char Control_id;
	unsigned char addr[6] = {0};
	unsigned char valid = 0;
	
	show_text(DGUS_SET_DAMP_ADDR_SHOW_ADDR,1,NULL,0);	//清空提示框

	Control_id = pbuf[6];

	switch(Control_id)
	{
		case 4:		//节点输入框	
			get_edit_addr(pbuf,len,g_read_damp_addr);
			for(i = 0;i < DAMP_MAX_NUM;i++)
			{
				if(((g_Damp_Info[i].SN[0] == g_read_damp_addr[0])&&(g_Damp_Info[i].SN[1] == g_read_damp_addr[1])\
					&&(g_Damp_Info[i].SN[2] == g_read_damp_addr[2])&&(g_Damp_Info[i].SN[3] == g_read_damp_addr[3] )))
				{	
					valid = 1;
					break;
				}
			}
			if(valid == 0)
			{
				show_text(DGUS_PROMPT_SHOW_ADDR,1,"节点不存在",strlen("节点不存在"));
				Icon_Show(DGUS_PROMPT_SHOW_ADDR,3,1);
				Change_Page(DGUS_PROMPT_SHOW_ADDR);
			}
			break;
		case 5:		//抄读按钮
			HEXaddr_to_BCDaddr(g_add_damp_addr,addr);
			send_monitor_damp_bdp(addr);
			break;
		case 6:		//启动轮询
			clear_lamp_read_flag();	//重启前清空单灯参数
			send_plc_reboot_bdp();
			break;
		case 7:		//暂停轮询
			send_plc_stop_bdp();
			break;
	}
}

//手动控制界面
unsigned char cmd1 = 0;   //通道1 开关命令   
unsigned char ratio1 = 0; //通道1调光比例    
unsigned char cmd2 = 0;  //通道2 开关命令  
unsigned char ratio2 = 0; //通道2调光比例  
unsigned char ratio3 = 0; //通道3调光比例  
void screen_8_page(unsigned char *pbuf,unsigned char len)
{
	unsigned short i = 0;
	unsigned char Control_id;
	unsigned char m_group = 0;
	unsigned char addr[6] = {0};
	unsigned char valid = 0;
	unsigned char contents[18] = {0};

	Control_id = pbuf[6];				
	switch(Control_id)
	{
		case 3: 	
			g_now_screen_id = DGUS_STATE_SHOW_ADDR;
			break;
		case 4: 	//单灯节点输入
			get_edit_addr(pbuf,len,g_ctl_damp_addr);
			for(i = 0;i < DAMP_MAX_NUM;i++)
			{
				if(((g_Damp_Info[i].SN[0] == g_ctl_damp_addr[0])&&(g_Damp_Info[i].SN[1] == g_ctl_damp_addr[1])\
					&&(g_Damp_Info[i].SN[2] == g_ctl_damp_addr[2])&&(g_Damp_Info[i].SN[3] == g_ctl_damp_addr[3] )))
				{	
					show_text(DGUS_MANUAL_CONTROL_SHOW_ADDR,5,NULL,0);	//单灯控制，清空组号
					Icon_Show(DGUS_MANUAL_CONTROL_SHOW_ADDR,6,0);	//取消全部勾选
					g_ctl_group = 0;
					valid = 1;
				}
			}
			if(valid == 0)
			{
				show_text(DGUS_PROMPT_SHOW_ADDR,1,"节点不存在",strlen("节点不存在"));
				Icon_Show(DGUS_PROMPT_SHOW_ADDR,3,1);
				Change_Page(DGUS_PROMPT_SHOW_ADDR);
			}
			break;
		case 5:	//组号输入框
			if(len == 15)
			{
				m_group = a_to_i(pbuf[8])* 10 + a_to_i(pbuf[9]);
			}
			if(len == 14)
			{
				m_group = a_to_i(pbuf[8]);
			}
			for(i = 0;i < DAMP_MAX_NUM;i++)	//验证组别是否有效
			{
				if( (g_Damp_Info[i].m_bit_status.group == m_group)&&(m_group != 0) )
				{
					g_ctl_group = (1<<(m_group-1));
					show_text(DGUS_MANUAL_CONTROL_SHOW_ADDR,4,NULL,0);	//组控，清空单节点
					memset(g_ctl_damp_addr,0,6);
					Icon_Show(DGUS_MANUAL_CONTROL_SHOW_ADDR,6,0);	//取消全部勾选
					valid = 1;
				}
			}
			if(valid == 0)
			{
				show_text(DGUS_PROMPT_SHOW_ADDR,1,"组别无效",strlen("组别无效"));
				Icon_Show(DGUS_PROMPT_SHOW_ADDR,3,1);
				Change_Page(DGUS_PROMPT_SHOW_ADDR);
			}
			break;
		case 6:	//全部复选框
			if(pbuf[8] == 0)
			{
				if(g_lamp_total_num == 0)
				{
					show_text(DGUS_PROMPT_SHOW_ADDR,1,"节点档案为空",strlen("节点档案为空"));
					Icon_Show(DGUS_PROMPT_SHOW_ADDR,3,2);
					Change_Page(DGUS_PROMPT_SHOW_ADDR);
				}
				else
				{
					Icon_Show(DGUS_MANUAL_CONTROL_SHOW_ADDR,6,1);	//显示全部勾选
					g_ctl_group = 0xFFFF;
					show_text(DGUS_MANUAL_CONTROL_SHOW_ADDR,4,NULL,0);	//清空节点
					show_text(DGUS_MANUAL_CONTROL_SHOW_ADDR,5,NULL,0);	//清空组号
					memset(g_ctl_damp_addr,0,6);
				}
				
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_MANUAL_CONTROL_SHOW_ADDR,6,0);	//取消全部勾选
			}			
			break;
		case 7:	//回路1调光系数
			if(len == 14)
			{
				ratio1 = a_to_i(pbuf[8]);
			}
			else if(len == 15)
			{
				ratio1 = a_to_i(pbuf[8])*10+a_to_i(pbuf[9]);
			}
			else if(len == 16)
			{
				ratio1 = 100;
			}
			break;
		case 8:	//回路2调光系数
			if(len == 14)
			{
				ratio2 = a_to_i(pbuf[8]);
			}
			else if(len == 15)
			{
				ratio2 = a_to_i(pbuf[8])*10+a_to_i(pbuf[9]);
			}
			else if(len == 16)
			{
				ratio2 = a_to_i(pbuf[8])*100+a_to_i(pbuf[9])*10+a_to_i(pbuf[10]);
			}
			break;
		case 9:	//回路3调光系数
			if(len == 14)
			{
				ratio3 = a_to_i(pbuf[8]);
			}
			else if(len == 15)
			{
				ratio3 = a_to_i(pbuf[8])*10+a_to_i(pbuf[9]);
			}
			else if(len == 16)
			{
				ratio3 = a_to_i(pbuf[8])*100+a_to_i(pbuf[9])*10+a_to_i(pbuf[10]);
			}
			break;
		case 10:	
			if(ratio1 != 0)	//回路1开
			{
				contents[1] = contents[1]|0x01;
			}
			if(ratio2 != 0)	//回路2开
			{
				contents[1] = contents[1]|0x02;
			}
			if(ratio3 != 0)	//回路3开
			{
				contents[1] = contents[1]|0x04;
			}
			contents[15] = ratio3;
			contents[16] = ratio2;
			contents[17] = ratio1;
			
			if(g_ctl_group == 0)	//单灯开
			{			
				for(i = 0;i < DAMP_MAX_NUM;i++)
				{
					if(((g_Damp_Info[i].SN[0] == g_ctl_damp_addr[0])&&(g_Damp_Info[i].SN[1] == g_ctl_damp_addr[1])\
						&&(g_Damp_Info[i].SN[2] == g_ctl_damp_addr[2])&&(g_Damp_Info[i].SN[3] == g_ctl_damp_addr[3] )))
					{	
						g_Damp_Info[i].m_bit_status.switch_flag = 1;
						HEXaddr_to_BCDaddr(g_ctl_damp_addr,addr);
						send_plc_cmd_bdp(addr,g_ctl_group,contents);
						break;
					}
				}
			}
			else if(g_ctl_group == 0xFFFF)	//全开
			{
				send_plc_cmd_bdp(addr,g_ctl_group,contents);
				for(i = 0;i < DAMP_MAX_NUM;i++)
				{
					if(g_Damp_Info[i].m_bit_status.status == 1)
					{
						g_Damp_Info[i].m_bit_status.switch_flag = 1;
					}
				}
			}
			else 	//组开
			{
				for(i = 0;i < DAMP_MAX_NUM;i++)
				{
					if(g_Damp_Info[i].m_bit_status.group == g_ctl_group)
					{
						g_Damp_Info[i].m_bit_status.switch_flag = 1;
					}	
				}
				send_plc_cmd_bdp(addr,g_ctl_group,contents);
			}
			break;
		case 11:   //关	
			if(g_ctl_group == 0)	//单灯关
			{
				for(i = 0;i < DAMP_MAX_NUM;i++)
				{
					if(((g_Damp_Info[i].SN[0] == g_ctl_damp_addr[0])&&(g_Damp_Info[i].SN[1] == g_ctl_damp_addr[1])\
						&&(g_Damp_Info[i].SN[2] == g_ctl_damp_addr[2])&&(g_Damp_Info[i].SN[3] == g_ctl_damp_addr[3])))
					{	
						g_Damp_Info[i].m_bit_status.switch_flag = 0;
						HEXaddr_to_BCDaddr(g_ctl_damp_addr,addr);
						send_plc_cmd_bdp(addr,g_ctl_group,contents);
						break;
					}
				}
			}
			else if(g_ctl_group == 0xFFFF)	//全关
			{
				for(i = 0;i < DAMP_MAX_NUM;i++)
				{
					if(g_Damp_Info[i].m_bit_status.status == 1)
					{
						g_Damp_Info[i].m_bit_status.switch_flag = 0;
					}
				}
				send_plc_cmd_bdp(addr,g_ctl_group,contents);
			}
			else	//组关
			{
				for(i = 0;i < DAMP_MAX_NUM;i++)
				{
					if(g_Damp_Info[i].m_bit_status.group == g_ctl_group)
					{
						g_Damp_Info[i].m_bit_status.switch_flag = 0;
					}	
				}
				send_plc_cmd_bdp(addr,g_ctl_group,contents);
			}
			break;
		case 12:	//返回
			g_now_screen_id = DGUS_FUNCTION_SET_SHOW_ADDR;
			break;
		case 13:	//单灯工作模式复选框
			if(pbuf[8] == 0)
			{
				Icon_Show(DGUS_MANUAL_CONTROL_SHOW_ADDR,13,1);	//显示勾选	
				g_lamp_working_mode = 1;
				Write_RTU_info_to_Flash();
				send_working_mode_bdp();	//预约模式
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_MANUAL_CONTROL_SHOW_ADDR,13,0);	//取消全部勾选
				g_lamp_working_mode = 0;
				Write_RTU_info_to_Flash();
				send_working_mode_bdp();	//上电全开
			}			
			break;
		default:
			break;		
	}
}

//扩展设置界面
void screen_9_page(unsigned char *pbuf,unsigned char len)
{
	unsigned char Control_id;

	Control_id = pbuf[6];				
	switch(Control_id)
	{
		case 1:
			break;
		case 2:
			break;
		case 3: 	
			break;
		case 4: 	//
			break;
		case 5:
			break;
		case 6: 	//
			break;
		case 7:		//扩展模块1在线则进入扩展模块1设置界面
//			if(g_ext_online_flag[0] == ONLINE)
			{
				Change_Page(DGUS_EXPANSION1_SET_SHOW_ADDR);
				show_ext_config_info(0);
			}
			break;
		case 8:		//扩展模块2在线则进入扩展模块2设置界面
//			if(g_ext_online_flag[1] == ONLINE)
			{
				Change_Page(DGUS_EXPANSION2_SET_SHOW_ADDR);
				show_ext_config_info(1);
			}
			break;
		case 9: 	//扩展模块3在线则进入扩展模块3设置界面
//			if(g_ext_online_flag[2] == ONLINE)
			{
				Change_Page(DGUS_EXPANSION3_SET_SHOW_ADDR);
				show_ext_config_info(2);
			}			
			break;
		default:
			break;
	}
}

//扩展模块1设置界面
void screen_10_page(unsigned char *pbuf,unsigned char len)
{
	unsigned char i = 0;
	unsigned char Control_id = 0;;

	Control_id = pbuf[6];				
	switch(Control_id)
	{
		case 1:		//保存
			//验证扩展启用
			Check_ext_enable(0);
			//更新扩展模块状态
			Update_Ext_Status(0);
			//保存配置信息
			Write_Concentrator_Config_Info(); 
			show_text(DGUS_EXPANSION1_SET_SHOW_ADDR,20,"已保存 ",strlen("已保存 "));
			break;
		case 2:		//清空
			for(i = 4;i <= 19;i++)
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,i,0);	//取消勾选
			}
			
			g_Concentrator_Para.ext_enable[0] = 0xFF;	//扩展模块1未启用
			for(i = 0;i < 4;i++)
			{
				g_extmodule_relay[0][i] = 0;
				g_Concentrator_Para.ext_relay_enable[0][i] = 0xFF;
			}
			for(i = 0;i < 12;i++)
			{
				g_Concentrator_Para.ext_phase_enable[0][i] = 0xFF;
			}
			Write_Concentrator_Config_Info(); 	
			show_ext_status(0,2);	//显示扩展模块未启用
			show_text(DGUS_EXPANSION1_SET_SHOW_ADDR,20,"已清空 ",strlen("已清空 "));
			break;
		case 3: 	//返回
			show_text(DGUS_EXPANSION1_SET_SHOW_ADDR,20,NULL,0);
			break;
		case 4: 
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,4,1);	//勾选
				g_Concentrator_Para.ext_relay_enable[0][0] = 0;		//扩展模块1继电器1启用
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,4,0);	//取消勾选
				g_Concentrator_Para.ext_relay_enable[0][0] = 0xFF;		//扩展模块1继电器1禁用
				g_extmodule_relay[0][0] = 0;	//关闭扩展模块1继电器1
			}			
			break;
		case 5:	
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,5,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[0][0] = 0;		//扩展模块1继电器1启用A相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,5,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[0][0] = 0xFF;		//扩展模块1继电器1禁用A相
			}					
			break;
		case 6:
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,6,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[0][1] = 0;		//扩展模块1继电器1启用B相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,6,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[0][1] = 0xFF;		//扩展模块1继电器1禁用B相
			}
			break;
		case 7: 	
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,7,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[0][2] = 0;		//扩展模块1继电器1启用C相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,7,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[0][2] = 0xFF;		//扩展模块1继电器1禁用C相
			}
			break;
		case 8:
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,8,1);	//勾选
				g_Concentrator_Para.ext_relay_enable[0][1] = 0;		//扩展模块1继电器2启用
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,8,0);	//取消勾选
				g_Concentrator_Para.ext_relay_enable[0][1] = 0xFF;		//扩展模块1继电器2禁用
				g_extmodule_relay[0][1] = 0;	//关闭扩展模块1继电器2
			}			
			break;
		case 9: 	
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,9,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[0][3] = 0;		//扩展模块1继电器2启用A相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,9,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[0][3] = 0xFF;		//扩展模块1继电器2禁用A相
			}
			break;
		case 10:
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,10,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[0][4] = 0;		//扩展模块1继电器2启用B相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,10,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[0][4] = 0xFF;		//扩展模块1继电器2禁用B相
			}
			break;
		case 11:
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,11,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[0][5] = 0;		//扩展模块1继电器2启用C相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,11,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[0][5] = 0xFF;		//扩展模块1继电器2禁用C相
			}
			break;
		case 12:
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,12,1);	//勾选
				g_Concentrator_Para.ext_relay_enable[0][2] = 0;		//扩展模块1继电器3启用
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,12,0);	//取消勾选
				g_Concentrator_Para.ext_relay_enable[0][2] = 0xFF;		//扩展模块1继电器3禁用
				g_extmodule_relay[0][2] = 0;	//关闭扩展模块1继电器3
			}			
			break;
		case 13: 	
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,13,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[0][6] = 0;		//扩展模块1继电器3启用A相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,13,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[0][6] = 0xFF;		//扩展模块1继电器3禁用A相
			}
			break;
		case 14: 
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,14,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[0][7] = 0;		//扩展模块1继电器3启用B相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,14,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[0][7] = 0xFF;		//扩展模块1继电器3禁用B相
			}					
			break;
		case 15:
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,15,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[0][8] = 0;		//扩展模块1继电器3启用C相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,15,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[0][8] = 0xFF;		//扩展模块1继电器3禁用C相
			}
			break;
		case 16:
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,16,1);	//勾选
				g_Concentrator_Para.ext_relay_enable[0][3] = 0;		//扩展模块1继电器4启用
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,16,0);	//取消勾选
				g_Concentrator_Para.ext_relay_enable[0][3] = 0xFF;		//扩展模块1继电器4禁用
				g_extmodule_relay[0][3] = 0;	//关闭扩展模块1继电器4
			}		
			break;
		case 17:
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,17,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[0][9] = 0;		//扩展模块1继电器4启用A相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,17,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[0][9] = 0xFF;		//扩展模块1继电器4禁用A相
			}
			break;
		case 18: 
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,18,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[0][10] = 0;		//扩展模块1继电器4启用B相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,18,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[0][10] = 0xFF;		//扩展模块1继电器4禁用B相
			}						
			break;
		case 19: 
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,19,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[0][11] = 0;		//扩展模块1继电器4启用C相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR,19,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[0][11] = 0xFF;		//扩展模块1继电器4禁用C相
			}		
			break;
		default:
			break;
	}
}

//扩展模块2设置界面
void screen_11_page(unsigned char *pbuf,unsigned char len)
{
	unsigned char i = 0;
	unsigned char Control_id = 0;;

	Control_id = pbuf[6];				
	switch(Control_id)
	{
		case 1:		//保存
			//验证扩展启用
			Check_ext_enable(1);
			//更新扩展模块状态
			Update_Ext_Status(1);
			//保存配置信息
			Write_Concentrator_Config_Info(); 
			show_text(DGUS_EXPANSION2_SET_SHOW_ADDR,20,"已保存 ",strlen("已保存 "));
			break;
		case 2:		//清空
			for(i = 4;i <= 19;i++)
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,i,0);	//取消勾选
			}
			
			g_Concentrator_Para.ext_enable[1] = 0xFF;	//扩展模块2未启用
	
			for(i = 0;i < 4;i++)
			{
				g_extmodule_relay[1][i] = 0;
				g_Concentrator_Para.ext_relay_enable[1][i] = 0xFF;
			}
			for(i = 0;i < 12;i++)
			{
				g_Concentrator_Para.ext_phase_enable[1][i] = 0xFF;
			}
			Write_Concentrator_Config_Info(); 
			show_ext_status(1,2);	//显示扩展模块未启用	
			show_text(DGUS_EXPANSION2_SET_SHOW_ADDR,20,"已清空 ",strlen("已清空 "));
			break;
		case 3: 	//返回
			show_text(DGUS_EXPANSION2_SET_SHOW_ADDR,20,NULL,0);
			break;
		case 4: 
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,4,1);	//勾选
				g_Concentrator_Para.ext_relay_enable[1][0] = 0;		//扩展模块2继电器1启用
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,4,0);	//取消勾选
				g_Concentrator_Para.ext_relay_enable[1][0] = 0xFF;		//扩展模块2继电器1禁用
				g_extmodule_relay[1][0] = 0;	//关闭扩展模块2继电器1
			}			
			break;
		case 5:	
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,5,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[1][0] = 0;		//扩展模块2继电器1启用A相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,5,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[1][0] = 0xFF;		//扩展模块2继电器1禁用A相
			}					
			break;
		case 6:
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,6,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[1][1] = 0;		//扩展模块2继电器1启用B相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,6,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[1][1] = 0xFF;		//扩展模块2继电器1禁用B相
			}
			break;
		case 7: 	
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,7,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[1][2] = 0;		//扩展模块2继电器1启用C相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,7,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[1][2] = 0xFF;		//扩展模块2继电器1禁用C相
			}
			break;
		case 8:
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,8,1);	//勾选
				g_Concentrator_Para.ext_relay_enable[1][1] = 0;		//扩展模块2继电器2启用
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,8,0);	//取消勾选
				g_Concentrator_Para.ext_relay_enable[1][1] = 0xFF;		//扩展模块1继电器2禁用
				g_extmodule_relay[1][1] = 0;	//关闭扩展模块2继电器2
			}			
			break;
		case 9: 	
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,9,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[1][3] = 0;		//扩展模块2继电器2启用A相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,9,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[1][3] = 0xFF;		//扩展模块2继电器2禁用A相
			}
			break;
		case 10:
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,10,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[1][4] = 0;		//扩展模块2继电器2启用B相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,10,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[1][4] = 0xFF;		//扩展模块2继电器2禁用B相
			}
			break;
		case 11:
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,11,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[1][5] = 0;		//扩展模块2继电器3启用C相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,11,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[1][5] = 0xFF;		//扩展模块2继电器3禁用C相
			}
			break;
		case 12:
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,12,1);	//勾选
				g_Concentrator_Para.ext_relay_enable[1][2] = 0;		//扩展模块2继电器3启用
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,12,0);	//取消勾选
				g_Concentrator_Para.ext_relay_enable[1][2] = 0xFF;		//扩展模块2继电器3禁用
				g_extmodule_relay[1][2] = 0;	//关闭扩展模块2继电器3
			}			
			break;
		case 13: 	
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,13,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[1][6] = 0;		//扩展模块2继电器3启用A相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,13,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[1][6] = 0xFF;		//扩展模块2继电器3禁用A相
			}
			break;
		case 14: 
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,14,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[1][7] = 0;		//扩展模块2继电器3启用B相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,14,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[1][7] = 0xFF;		//扩展模块2继电器3禁用B相
			}						
			break;
		case 15:
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,15,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[1][8] = 0;		//扩展模块2继电器3启用C相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,15,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[1][8] = 0xFF;		//扩展模块1继电器3禁用C相
			}
			break;
		case 16:
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,16,1);	//勾选
				g_Concentrator_Para.ext_relay_enable[1][3] = 0;		//扩展模块1继电器4启用
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,16,0);	//取消勾选
				g_Concentrator_Para.ext_relay_enable[1][3] = 0xFF;		//扩展模块2继电器4禁用
				g_extmodule_relay[1][3] = 0;	//关闭扩展模块2继电器4
			}		
			break;
		case 17:
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,17,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[1][9] = 0;		//扩展模块2继电器4启用A相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,17,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[1][9] = 0xFF;		//扩展模块2继电器4禁用A相
			}
			break;
		case 18: 
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,18,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[1][10] = 0;		//扩展模块2继电器4启用B相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,18,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[1][10] = 0xFF;		//扩展模块2继电器4禁用B相
			}					
			break;
		case 19: 
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,19,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[1][11] = 0;		//扩展模块2继电器4启用C相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION2_SET_SHOW_ADDR,19,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[1][11] = 0xFF;		//扩展模块2继电器4禁用C相
			}		
			break;
		default:
			break;
	}
}

//扩展模块3设置界面
void screen_12_page(unsigned char *pbuf,unsigned char len)
{
	unsigned char i = 0;
	unsigned char Control_id = 0;;

	Control_id = pbuf[6];				
	switch(Control_id)
	{
		case 1:		//保存
			//验证扩展启用
			Check_ext_enable(2);
			//更新扩展模块状态
			Update_Ext_Status(2);
			//保存配置信息
			Write_Concentrator_Config_Info(); 
			show_text(DGUS_EXPANSION3_SET_SHOW_ADDR,20,"已保存 ",strlen("已保存 "));
			break;
		case 2:		//清空
			for(i = 4;i <= 19;i++)
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,i,0);	//取消勾选
			}
			
			g_Concentrator_Para.ext_enable[2] = 0xFF;	//扩展模块3未启用

			for(i = 0;i < 4;i++)
			{
				g_extmodule_relay[2][i] = 0;
				g_Concentrator_Para.ext_relay_enable[2][i] = 0xFF;
			}
			for(i = 0;i < 12;i++)
			{
				g_Concentrator_Para.ext_phase_enable[2][i] = 0xFF;
			}
			Write_Concentrator_Config_Info(); 		
			show_ext_status(2,2);	//显示扩展模块未启用
			show_text(DGUS_EXPANSION3_SET_SHOW_ADDR,20,"已清空 ",strlen("已清空 "));
			break;
		case 3: 	//返回
			show_text(DGUS_EXPANSION3_SET_SHOW_ADDR,20,NULL,0);
			break;
		case 4: 
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,4,1);	//勾选
				g_Concentrator_Para.ext_relay_enable[2][0] = 0;		//扩展模块3继电器1启用
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,4,0);	//取消勾选
				g_Concentrator_Para.ext_relay_enable[2][0] = 0xFF;		//扩展模块3继电器1禁用
				g_extmodule_relay[2][0] = 0;	//关闭扩展模块3继电器1
			}			
			break;
		case 5:	
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,5,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[2][0] = 0;		//扩展模块3继电器1启用A相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,5,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[2][0] = 0xFF;		//扩展模块3继电器1禁用A相
			}					
			break;
		case 6:
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,6,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[2][1] = 0;		//扩展模块3继电器1启用B相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,6,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[2][1] = 0xFF;		//扩展模块3继电器1禁用B相
			}
			break;
		case 7: 	
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,7,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[2][2] = 0;		//扩展模块3继电器1启用C相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,7,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[2][2] = 0xFF;		//扩展模块3继电器1禁用C相
			}
			break;
		case 8:
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,8,1);	//勾选
				g_Concentrator_Para.ext_relay_enable[2][1] = 0;		//扩展模块3继电器2启用
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,8,0);	//取消勾选
				g_Concentrator_Para.ext_relay_enable[2][1] = 0xFF;		//扩展模块3继电器2禁用
				g_extmodule_relay[2][1] = 0;	//关闭扩展模块3继电器2
			}			
			break;
		case 9: 	
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,9,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[2][3] = 0;		//扩展模块3继电器2启用A相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,9,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[2][3] = 0xFF;		//扩展模块3继电器2禁用A相
			}
			break;
		case 10:
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,10,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[2][4] = 0;		//扩展模块3继电器2启用B相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,10,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[2][4] = 0xFF;		//扩展模块3继电器2禁用B相
			}
			break;
		case 11:
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,11,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[2][5] = 0;		//扩展模块3继电器3启用C相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,11,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[2][5] = 0xFF;		//扩展模块3继电器3禁用C相
			}
			break;
		case 12:
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,12,1);	//勾选
				g_Concentrator_Para.ext_relay_enable[2][2] = 0;		//扩展模块3继电器3启用
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,12,0);	//取消勾选
				g_Concentrator_Para.ext_relay_enable[2][2] = 0xFF;		//扩展模块3继电器3禁用
				g_extmodule_relay[2][2] = 0;	//关闭扩展模块3继电器3
			}			
			break;
		case 13: 	
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,13,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[2][6] = 0;		//扩展模块3继电器3启用A相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,13,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[2][6] = 0xFF;		//扩展模块3继电器3禁用A相
			}
			break;
		case 14: 
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,14,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[2][7] = 0;		//扩展模块3继电器3启用B相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,14,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[2][7] = 0xFF;		//扩展模块3继电器3禁用B相
			}						
			break;
		case 15:
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,15,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[2][8] = 0;		//扩展模块3继电器3启用C相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,15,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[2][8] = 0xFF;		//扩展模块3继电器3禁用C相
			}
			break;
		case 16:
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,16,1);	//勾选
				g_Concentrator_Para.ext_relay_enable[2][3] = 0;		//扩展模块3继电器4启用
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,16,0);	//取消勾选
				g_Concentrator_Para.ext_relay_enable[2][3] = 0xFF;		//扩展模块3继电器4禁用
				g_extmodule_relay[2][3] = 0;	//关闭扩展模块3继电器4
			}		
			break;
		case 17:
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,17,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[2][9] = 0;		//扩展模块3继电器4启用A相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,17,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[2][9] = 0xFF;		//扩展模块3继电器4禁用A相
			}
			break;
		case 18: 
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,18,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[2][10] = 0;		//扩展模块3继电器4启用B相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,18,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[2][10] = 0xFF;		//扩展模块3继电器4禁用B相
			}						
			break;
		case 19: 
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,19,1);	//勾选
				g_Concentrator_Para.ext_phase_enable[2][11] = 0;		//扩展模块3继电器4启用C相
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_EXPANSION3_SET_SHOW_ADDR,19,0);	//取消勾选
				g_Concentrator_Para.ext_phase_enable[2][11] = 0xFF;		//扩展模块3继电器4禁用C相
			}		
			break;
		default:
			break;
	}
}

//单灯信息索引界面
void screen_13_page(unsigned char *pbuf,unsigned char len)
{
	unsigned short i = 0;
	unsigned char Control_id = 0;
	unsigned char addr[6] = {0};
	unsigned char valid = 0;

	Control_id = pbuf[6];				
	switch(Control_id)
	{
		case 11: 	//单灯节点输入
			get_edit_addr(pbuf,len,addr);
			for(i = 0;i < DAMP_MAX_NUM;i++)
			{
				if(((g_Damp_Info[i].SN[0] == addr[0])&&(g_Damp_Info[i].SN[1] == addr[1])\
					&&(g_Damp_Info[i].SN[2] == addr[2])&&(g_Damp_Info[i].SN[3] == addr[3] )))
				{	
					Show_Damp_Info(i);
					valid = 1;
					break;
				}
			}
			if(valid == 0)
			{
				show_text(DGUS_PROMPT_SHOW_ADDR,1,"节点不存在",strlen("节点不存在"));
				Icon_Show(DGUS_PROMPT_SHOW_ADDR,3,1);
				Change_Page(DGUS_PROMPT_SHOW_ADDR);
			}
			break;
		default:
			break;
		
	}
}

//提示界面
void screen_14_page(unsigned char *pbuf,unsigned char len)
{
	unsigned char Control_id;
	
	Control_id = pbuf[6];

	switch(Control_id)
	{
		case 2: 	 //确定
			if(g_now_screen_id < 2)
			{
				g_now_screen_id = DGUS_STATE_SHOW_ADDR;
			}
			Change_Page(g_now_screen_id);
			break;
		default:
			break;
	}
}

//集中器配置界面
void screen_15_page(unsigned char *pbuf,unsigned char len)
{
	unsigned char i = 0;
	unsigned char Control_id = 0;;

	Control_id = pbuf[6];				
	switch(Control_id)
	{
		case 1:		//保存
			g_Concentrator_Para.e3000_config_flag = 1;
			Write_Concentrator_Config_Info(); 
			show_text(DGUS_CONCENTRATOR_SET_SHOW_ADDR,20,"已保存 ",strlen("已保存 "));
			break;
		case 2:		//清空
			g_Concentrator_Para.e3000_config_flag = 1;
			for(i = 4;i <= 10;i++)
			{
				Icon_Show(DGUS_CONCENTRATOR_SET_SHOW_ADDR,i,0);	//取消勾选
			}
			g_Concentrator_Para.e3000_relay_enable[0] = 0xFF;	////集中器继电器1禁用
			for(i = 0;i < 3;i++)								//3相电压电流检测禁用
			{			
				g_Concentrator_Para.e3000_V_enable[i] = 0xFF;
				g_Concentrator_Para.e3000_A_enable[i] = 0xFF;
			}
			E3000_Relay_Ctl(CLOSE);		//关
			Write_Concentrator_Config_Info();
			show_text(DGUS_CONCENTRATOR_SET_SHOW_ADDR,20,"已清空 ",strlen("已清空 "));			
			break;
		case 3: 	//返回
			show_text(DGUS_CONCENTRATOR_SET_SHOW_ADDR,20,NULL,0);
			break;
		case 4: 
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_CONCENTRATOR_SET_SHOW_ADDR,4,1);	//显示勾选
				g_Concentrator_Para.e3000_relay_enable[0] = 0;			//集中器继电器1启用
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_CONCENTRATOR_SET_SHOW_ADDR,4,0);	//取消勾选
				g_Concentrator_Para.e3000_relay_enable[0] = 0xFF;		//集中器继电器1禁用
				E3000_Relay_Ctl(CLOSE);		//关
			}			
			break;
		case 5:	
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_CONCENTRATOR_SET_SHOW_ADDR,5,1);	//勾选
				g_Concentrator_Para.e3000_V_enable[0] = 0;		//集中器电压检测A相启用
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_CONCENTRATOR_SET_SHOW_ADDR,5,0);	//取消勾选
				g_Concentrator_Para.e3000_V_enable[0] = 0xFF;		//集中器电压检测A相禁用
			}					
			break;
		case 6:	
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_CONCENTRATOR_SET_SHOW_ADDR,6,1);	//勾选
				g_Concentrator_Para.e3000_V_enable[1] = 0;		//集中器电压检测B相启用
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_CONCENTRATOR_SET_SHOW_ADDR,6,0);	//取消勾选
				g_Concentrator_Para.e3000_V_enable[1] = 0xFF;		//集中器电压检测B相禁用
			}					
			break;
		case 7:	
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_CONCENTRATOR_SET_SHOW_ADDR,7,1);	//勾选
				g_Concentrator_Para.e3000_V_enable[2] = 0;		//集中器电压检测C相启用
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_CONCENTRATOR_SET_SHOW_ADDR,7,0);	//取消勾选
				g_Concentrator_Para.e3000_V_enable[2] = 0xFF;		//集中器电压检测C相禁用
			}				
			break;
		case 8:	
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_CONCENTRATOR_SET_SHOW_ADDR,8,1);	//勾选
				g_Concentrator_Para.e3000_A_enable[0] = 0;		//集中器电流检测A相启用
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_CONCENTRATOR_SET_SHOW_ADDR,8,0);	//取消勾选
				g_Concentrator_Para.e3000_A_enable[0] = 0xFF;		//集中器电流检测A相禁用
			}	
			break;
		case 9:	
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_CONCENTRATOR_SET_SHOW_ADDR,9,1);	//勾选
				g_Concentrator_Para.e3000_A_enable[1] = 0;		//集中器电流检测B相启用
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_CONCENTRATOR_SET_SHOW_ADDR,9,0);	//取消勾选
				g_Concentrator_Para.e3000_A_enable[1] = 0xFF;		//集中器电流检测B相禁用
			}		
			break;
		case 10:	
			if(pbuf[8] == 0)	
			{
				Icon_Show(DGUS_CONCENTRATOR_SET_SHOW_ADDR,10,1);	//勾选
				g_Concentrator_Para.e3000_A_enable[2] = 0;		//集中器电流检测C相启用
			}
			else if(pbuf[8] == 1)
			{
				Icon_Show(DGUS_CONCENTRATOR_SET_SHOW_ADDR,10,0);	//取消勾选
				g_Concentrator_Para.e3000_A_enable[2] = 0xFF;		//集中器电流检测C相禁用
			}		
			break;
		default:
			break;
		}
	}

//节点地址设定界面
unsigned char m_old_addr[6] = {0};
unsigned char m_new_addr[6] = {0};
unsigned char m_set_loops = 0;
void screen_17_page(unsigned char *pbuf,unsigned char len)
{
	unsigned char Control_id = 0;
	unsigned char old_addr[6] = {0};
	unsigned char new_addr[6] = {0};

	Control_id = pbuf[6];
	
	show_text(DGUS_SET_DAMP_ADDR_SHOW_ADDR,1,NULL,0);	//清空提示文本框

	switch(Control_id)
	{
		case 2:		//原地址输入框
			get_edit_addr(pbuf,len,m_old_addr);
			if( (m_old_addr[0] != 0xFA)||((m_old_addr[1]&0xF0) != 0x30) )	//无效地址
			{
				show_text(DGUS_PROMPT_SHOW_ADDR,1,"请输入8位有效地址",strlen("请输入8位有效地址"));
				Icon_Show(DGUS_PROMPT_SHOW_ADDR,3,1);
				Change_Page(DGUS_PROMPT_SHOW_ADDR);
				break;
			}
			break;
		case 4:  	//新地址输入框
			get_edit_addr(pbuf,len,m_new_addr);
			if( (m_new_addr[0] != 0xFA)||((m_new_addr[1]&0xF0) != 0x30) )	//无效地址
			{
				show_text(DGUS_PROMPT_SHOW_ADDR,1,"请输入8位有效地址",strlen("请输入8位有效地址"));
				Icon_Show(DGUS_PROMPT_SHOW_ADDR,3,1);
				Change_Page(DGUS_PROMPT_SHOW_ADDR);
				break;
			}
			break;
		case 6:			
			HEXaddr_to_BCDaddr(m_old_addr,old_addr);
			HEXaddr_to_BCDaddr(m_new_addr,new_addr);
			send_set_damp_addr_bdp(old_addr,new_addr,m_set_loops);	//发送设置地址命令			
			break;
		case 9:		//回路数输入框
			m_set_loops = a_to_i(pbuf[8]);
			break;
		default:
			break;
	}
	
	
}

//单灯设置界面
void screen_18_page(unsigned char *pbuf,unsigned char len)
{
	unsigned char Control_id;
	
	Control_id = pbuf[6];

	switch(Control_id)
	{
		case 8:
			send_system_time_bdp();			//发送系统时间到单灯
			break;
		case 9:
			send_working_time_bdp();		//发送工作时间到单灯
			break;
		case 10:		//清空单灯节点档案
			show_text(DGUS_PASSWORD_INPUT_SHOW_ADDR,2,NULL,0);	//清除密码
			g_password_judge = 0;
			break;
		default:
			break;
	}
}


//密码输入
void screen_19_page(unsigned char *pbuf,unsigned char len)
{
	unsigned char Control_id;
	
	Control_id = pbuf[6];

	switch(Control_id)
	{
		case 1: 	 //返回状态显示界面	
			g_now_screen_id = DGUS_STATE_SHOW_ADDR;
			break;
		case 2: 	 //密码输入
			show_text(DGUS_PASSWORD_INPUT_SHOW_ADDR,2,"****",strlen("****"));
			if((pbuf[8] == '2')&&(pbuf[9] == '0')&&(pbuf[10] == '1')&&(pbuf[11] == '7'))
			{
				g_password_judge = 1;
			}
			else
			{
				show_text(DGUS_PROMPT_SHOW_ADDR,1,"密码错误",strlen("密码错误"));
				Icon_Show(DGUS_PROMPT_SHOW_ADDR,3,1);
				Change_Page(DGUS_PROMPT_SHOW_ADDR);
			}			
			break;
		case 3: 	 //确定
			if(g_password_judge == 1)
			{
				Clear_All_Lamp_Info();
				g_password_judge = 0;					
			}
			break;
		case 4: 	 //清空
			show_text(DGUS_PASSWORD_INPUT_SHOW_ADDR,2,NULL,0);	//清除密码
			g_password_judge = 0;
			break;
		default:
			break;
	}
}




void show_simcard_num(void)
{
	unsigned char num[13] = {0};
	unsigned char i = 0;

	for(i = 0; i < 13; i++)
	{
		if(g_E3000_Info.simcard_num[i] == 0xFF)
		{
			g_E3000_Info.simcard_num[i] = 0;
		}
		num[i] = i_to_a(g_E3000_Info.simcard_num[i]);
	}
	show_text(DGUS_STATE_SHOW_ADDR,11,num,13);
}

void show_Concentrator_SN(void)
{
	unsigned char sn[8] = {0};
	unsigned char i = 0;

	for(i = 0; i < 8; i++)
	{
		sn[i] = g_E3000_Info.SN[i];
	}
	show_text(DGUS_STATE_SHOW_ADDR,12,sn,8);
}

// void Set_str(unsigned char start_hour,unsigned char start_min,unsigned char end_hour,unsigned char end_min)
// {  
// //	sprintf(m_showtime_buf,"%2d:%2d-%2d:%2d",start_hour,start_min,end_hour,end_min);
// }

void Show_Light_Time(unsigned char ctl_id,unsigned char time_id)
{
	unsigned char buf[32] = {0};

	buf[0] = 0xEE;
	buf[1] = 0xB1;
	buf[2] = 0x10;
	buf[3] = 0x00;
	buf[4] = 0x02;
	buf[5] = 0x00;
	switch(ctl_id)
	{
		case 0:
			buf[6] = 5;
			break;
		case 1:
			buf[6] = 6;
			break;
		case 2:
			buf[6] = 7;
			break;
		case 3:
			buf[6] = 8;
			break;
		default:
			break;
	}
	memcpy((char *)&buf[7],(char *)g_showtime_buf[time_id],strlen((char *)g_showtime_buf[time_id]));

	buf[7 + strlen((char *)g_showtime_buf[time_id])] = 0xFF;
	buf[8 + strlen((char *)g_showtime_buf[time_id])] = 0xFC;
	buf[9 + strlen((char *)g_showtime_buf[time_id])] = 0xFF;
	buf[10 + strlen((char *)g_showtime_buf[time_id])] = 0xFF;

	COM5_Send_BDP(buf,22);
}


void get_edit_addr(unsigned char *pbuf,unsigned char len,unsigned char *addr_buf)
{
	unsigned char i = 0;
	unsigned char length = 0;
	unsigned int addr = 0;
	
	length = len-13;
	
	for(i = 0;i < 8;i++)
	{
		addr = (addr<<4)|a_to_i(pbuf[8+i]);
	}
	
	if( ((addr&0xFFF00000) == 0xFA300000)&&(length == 8) )	//合法节点地址
	{
		for(i = 4; i > 0; )
		{
			i--;
			addr_buf[i] = addr&0xFF; 
			addr = addr>>8;
		}
	}
	else
	{
		show_text(DGUS_PROMPT_SHOW_ADDR,1,"请输入8位有效地址",strlen("请输入8位有效地址"));
		Icon_Show(DGUS_PROMPT_SHOW_ADDR,3,1);
		Change_Page(DGUS_PROMPT_SHOW_ADDR);
	}
}

//图标显示
void Icon_Show(unsigned char screen_id,unsigned char control_id,unsigned char icon_id)
{
	unsigned char buf[12] = {0};	

	buf[0] = 0xEE;
	buf[1] = 0xB1;
	buf[2] = 0x23;
	buf[3] = 0x00;
	buf[4] = screen_id;
	buf[5] = 0x00;
	buf[6] = control_id;
	buf[7] = icon_id;	
	buf[8] = 0xFF;
	buf[9] = 0xFC;
	buf[10] = 0xFF;
	buf[11] = 0xFF;

	COM5_Send_BDP(buf,12);
}

//状态 0 无连接   1  连接正常
void Set_GPRS_Status_to_TC(unsigned char status)
{
	int i = 0;

	for(i = 2;i <= 7;i++)
	{
		Icon_Show(i,1,status);
	}
}



//切换界面
void Change_Page(unsigned char screen_id)
{
	unsigned char buf[10] = {0};
   
	buf[0] = 0xEE;
	buf[1] = 0xB1;
	buf[2] = 0x00;
	buf[3] = 0x00;
	buf[4] = screen_id;
	buf[5] = 0xFF;
	buf[6] = 0xFC;
	buf[7] = 0xFF;
	buf[8] = 0xFF;
	COM5_Send_BDP(buf,9);
}


void Get_TS_Time(void)
{
	unsigned char buf[6] = {0};
   
   buf[0] = 0xEE;
   buf[1] = 0x82;
   buf[2] = 0xFF;
   buf[3] = 0xFC;
   buf[4] = 0xFF;
   buf[5] = 0xFF;
   COM5_Send_BDP(buf,6);
}

void Set_TS_Time(void)
{
	unsigned char buf[16] = {0};
	
	buf[0] = 0xEE;
	buf[1] = 0x81; 
	
	buf[2] = g_set_sec/10*16+g_set_sec%10;
	buf[3] = g_set_min/10*16+g_set_min%10;
	buf[4] = g_set_hour/10*16+g_set_hour%10;
	buf[5] = g_set_day/10*16+g_set_day%10;
	buf[6] = 0;
	buf[7] = g_set_month/10*16+g_set_month%10;
	buf[8] = g_set_year/10*16+g_set_year%10;

	buf[9] = 0xFF;
	buf[10] = 0xFC;
	buf[11] = 0xFF;
	buf[12] = 0xFF;
	COM5_Send_BDP(buf,13);
}

unsigned char touch_buf[128] = {0};
void show_text(unsigned char screen_id,unsigned char control_id,unsigned char* pbuf,unsigned char len)
{
	unsigned char i = 0;
	
    memset((char *)touch_buf,0,128);
	touch_buf[0] = 0xEE;
	touch_buf[1] = 0xB1;
	touch_buf[2] = 0x10;
	touch_buf[3] = 0x00;
	touch_buf[4] = screen_id;
	touch_buf[5] = 0x00;
	touch_buf[6] = control_id;
	
	for(i = 0; i < len; i++)
	{
		touch_buf[7 + i] = pbuf[i];
	}
	touch_buf[7+len] = 0xFF;
	touch_buf[8+len] = 0xFC;
	touch_buf[9+len] = 0xFF;
	touch_buf[10+len] = 0xFF;

	COM5_Send_BDP(touch_buf,len+11);
}

//显示温度
void show_temperature(void)
{
	unsigned char buf[7] = {0};
	unsigned char temp_buf[7] = {0};
	unsigned char len = 0;
	len = data_conversion(buf,g_LM75A_temp&0x7FFF,DECIMAL1);
	if( 0x8000 == (g_LM75A_temp&0x8000) )	//负温度
	{
		temp_buf[0] = 0x2D;		//"-"
		memcpy(temp_buf,buf,len);
		show_text(DGUS_STATE_SHOW_ADDR,10,temp_buf,len+1);	
	}
	else if( 0 == (g_LM75A_temp&0x8000) )
	{
		show_text(DGUS_STATE_SHOW_ADDR,10,buf,len);	
	}
}

//显示单灯统计信息
void show_lamp_statistics(void)
{
	unsigned char len = 0;
	unsigned char buf[4] = {0};	
	
	//显示单灯总数
	len = data_conversion(buf,g_lamp_total_num,0);
	show_text(DGUS_STATE_SHOW_ADDR,19,buf,len);
	//显示单灯正常数量
	len = data_conversion(buf,(g_lamp_total_num-g_damp_fault_num),0);
	show_text(DGUS_STATE_SHOW_ADDR,20,buf,len);
	//显示单灯故障数量
	len = data_conversion(buf,g_damp_fault_num,0);
	show_text(DGUS_STATE_SHOW_ADDR,21,buf,len);	
}

void adjust_TS_backlight(unsigned char brightness)
{
	unsigned char buf[7] = {0};

	buf[0] = 0xEE;
	buf[1] = 0x60;
	buf[2] = brightness;
	buf[3] = 0xFF;
	buf[4] = 0xFC;
	buf[5] = 0xFF;
	buf[6] = 0xFF;

	COM5_Send_BDP(buf,7);
}

//显示扩展模块状态
void show_ext_status(unsigned char num,unsigned char status)
{
	if(g_Concentrator_Para.ext_enable[num] == 0xFF)		//扩展模块未启用
	{
		status = 2;
	}
	Icon_Show(DGUS_STATE_SHOW_ADDR,23+num,status);
}

void show_power_supply(unsigned char status)
{
	Icon_Show(DGUS_STATE_SHOW_ADDR,22,status);		//供电图标显示
}

//显示扩展模块配置信息
void show_ext_config_info(unsigned char num)
{
	unsigned char i = 0,j = 0;
	for(i = 0;i < 4;i++)
	{
		if(g_Concentrator_Para.ext_relay_enable[num][i] != 0xFF)
		{
			Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR+num,4*(i+1),1);	//勾选			
		}
		else
		{
			Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR+num,4*(i+1),0);	//取消勾选
		}
		
		for(j = 0;j < 3;j++)
		{
			if(g_Concentrator_Para.ext_phase_enable[num][i*3+j] != 0xFF)
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR+num,4*(i+1)+1+j,1);	//勾选
			}
			else
			{
				Icon_Show(DGUS_EXPANSION1_SET_SHOW_ADDR+num,4*(i+1)+1+j,0);	//取消勾选
			}
		}
	}
}

//显示集中器配置信息
void show_ctr_config_info(void)
{
	unsigned char i = 0;
	
	if(g_Concentrator_Para.e3000_relay_enable[0] != 0xFF)	//集中器继电器1启用
	{
		Icon_Show(DGUS_CONCENTRATOR_SET_SHOW_ADDR,4,1);	//勾选		
	}
	else
	{
		Icon_Show(DGUS_CONCENTRATOR_SET_SHOW_ADDR,4,0);	//取消勾选	
	}
	for(i = 0;i < 3;i++)
	{
		if(g_Concentrator_Para.e3000_V_enable[i] != 0xFF)
		{
			Icon_Show(DGUS_CONCENTRATOR_SET_SHOW_ADDR,5+i,1);	//勾选
		}
		else
		{
			Icon_Show(DGUS_CONCENTRATOR_SET_SHOW_ADDR,5+i,0);	//取消勾选
		}
		if(g_Concentrator_Para.e3000_A_enable[i] != 0xFF)
		{
			Icon_Show(DGUS_CONCENTRATOR_SET_SHOW_ADDR,8+i,1);	//勾选
		}
		else
		{
			Icon_Show(DGUS_CONCENTRATOR_SET_SHOW_ADDR,8+i,0);	//取消勾选
		}
	}
}

//显示已存在的节点信息
void Show_Damp_Info(unsigned char index)
{
	unsigned char buf[16] = {0};
	unsigned char len1 = 0,len2 = 0,len3 = 0;

	//打印状态
	{			
		if( (g_Damp_Info[index].fault_flag == 0) && (g_Damp_Info[index].m_bit_status.lamp_online_flag != LAMP_STATE_OFFLINE))	//正常	D5 FD B3 A3
		{
			buf[0] = 0xD5;
			buf[1] = 0xFD;
			buf[2] = 0xB3;
			buf[3] = 0xA3;
		}
		else	//故障	B9 CA D5 CF
		{
			buf[0] = 0xB9;
			buf[1] = 0xCA;
			buf[2] = 0xD5;
			buf[3] = 0xCF;
		}
		show_text(DGUS_DAMP_SHOW_ADDR,12,buf,4);
	}
	
	//打印相位值
	if(3 >= g_Damp_Info[index].m_bit_status.phase)
	{
		if(0 == g_Damp_Info[index].m_bit_status.phase)		//无
		{
			buf[0] = 0x30;
		}
		if(1 == g_Damp_Info[index].m_bit_status.phase)		//A相
		{
			buf[0] = 0x41;
		}
		else if(2 == g_Damp_Info[index].m_bit_status.phase)	//B相
		{
			buf[0] = 0x42;
		}
		else if(3 == g_Damp_Info[index].m_bit_status.phase)	//C相
		{
			buf[0] = 0x43;
		}
		
		show_text(DGUS_DAMP_SHOW_ADDR,13,buf,1);
	}
			
	//打印电压值
	len1 = data_conversion(buf,g_Damp_Info[index].voltage,DECIMAL1);
	show_text(DGUS_DAMP_SHOW_ADDR,14,buf,len1);

	//打印电流值
	len1 = data_conversion(buf,g_Damp_Info[index].current[0],DECIMAL2);					//回路1电流
	buf[len1] = 0x2F;
	len2 = data_conversion(&buf[len1+1],g_Damp_Info[index].current[1],DECIMAL2);			//回路2电流
	buf[len1+len2+1] = 0x2F;
	len3 = data_conversion(&buf[len1+len2+2],g_Damp_Info[index].current[2],DECIMAL2);	//回路3电流	
	show_text(DGUS_DAMP_SHOW_ADDR,15,buf,len1+len2+len3+2);
	
	//打印功率值
	len1 = data_conversion(buf,g_Damp_Info[index].active_power[0],DECIMAL1);				//回路1有功功率
	buf[len1] = 0x2F;
	len2 = data_conversion(&buf[len1+1],g_Damp_Info[index].active_power[1],DECIMAL1);		//回路2有功功率
	buf[len1+len2+1] = 0x2F;
	len3 = data_conversion(&buf[len1+len2+2],g_Damp_Info[index].active_power[2],DECIMAL1);	//回路3有功功率
	show_text(DGUS_DAMP_SHOW_ADDR,16,buf,len1+len2+len3+2);

}

//显示抄读进度
void Show_Read_Progress(unsigned char *addr)
{
	unsigned char len = 0,i = 0;
	unsigned char num_buf[3] = {0};
	unsigned char buf[32] = {0x54,0,0,0,0,0x2C,0xD5,0xFD,0xD4,0xDA,0xB3,0xAD,0xB6,0xC1,0,0,0,0,0,0,0,0,0x2C,0xB5,0xDA};
	unsigned char buf1[2] = {0xD5,0xB5};	//“盏”
	unsigned char temp_buf[8] = {0};
	unsigned char time_buf[4] = {0};
	
	time_buf[0] = i_to_a(g_now_hour/10);
	time_buf[1] = i_to_a(g_now_hour%10);
	time_buf[2] = i_to_a(g_now_min/10);
	time_buf[3] = i_to_a(g_now_min%10);
	memcpy(&buf[1],time_buf,4);
	
	for(i = 0;i < 4;i++)
	{
		temp_buf[2*i] = i_to_a(addr[i]/16);
		temp_buf[2*i+1] = i_to_a(addr[i]%16);
	}
	memcpy(&buf[14],temp_buf,8);
	
	len = data_conversion(num_buf,g_lamp_read_num,0);
	memcpy(&buf[25],num_buf,len);
	memcpy(&buf[25+len],buf1,2);
	
	show_text(DGUS_STATE_SHOW_ADDR,26,buf,25+len+2);	
}

//显示总回路统计信息
void show_loop_statistics(void)
{
	unsigned char len = 0,i = 0,j = 0;
	unsigned char buf[4] = {0};	
	unsigned char m_loop_total_num = 0;
	unsigned char m_loop_OK_num = 0;

	if(g_Concentrator_Para.e3000_relay_enable[0] != 0xFF)	//集中器1号继电器启用
	{
		m_loop_total_num++;
		m_loop_OK_num++;
	}
	for(i = 0;i < MAX_EXT_MODULE_NUM;i++)
	{
		for(j = 0;j < 4;j++)
		{
			if(g_Concentrator_Para.ext_relay_enable[i][j] != 0xFF)	//扩展模块继电器启用
			{
				m_loop_total_num++;
			}
		}			
	}
	//统计正常回路数
	for(i = 0;i < MAX_EXT_MODULE_NUM;i++)
	{
		if( (g_Concentrator_Para.ext_enable[i] != 0xFF)&&(g_ext_online_flag[i] == ONLINE) )	//扩展模块启用并且在线
		{
			for(j = 0;j < 4;j++)
			{
				if( g_Concentrator_Para.ext_relay_enable[i][j] != 0xFF )	//扩展模块继电器启用
				{
					m_loop_OK_num++;
				}
			}	
		}			
	}
	
	if(m_loop_OK_num > m_loop_total_num)
	{
		m_loop_OK_num = m_loop_total_num;
	}
	
	if( (m_loop_total_num != g_loop_total_num)||(m_loop_OK_num != g_loop_OK_num) )
	{
		//总回路数
		len = data_conversion(buf,m_loop_total_num,0);
		show_text(DGUS_STATE_SHOW_ADDR,19,buf,len);
		//正常回路数
		len = data_conversion(buf,m_loop_OK_num,0);
		show_text(DGUS_STATE_SHOW_ADDR,20,buf,len);
		//故障回路数
		len = data_conversion(buf,(m_loop_total_num-m_loop_OK_num),0);
		show_text(DGUS_STATE_SHOW_ADDR,21,buf,len);	
	}
	g_loop_total_num = m_loop_total_num;
	g_loop_OK_num = m_loop_OK_num;
}

//显示回路总页数
void show_loop_total_pages(void)
{
	unsigned char num = 0;
	unsigned char buf[2] = {0};
	unsigned char len = 0;

	num = g_loop_total_num/10;
	if( (g_loop_total_num%10) > 0 )
	{
		num++;
	}
	len = data_conversion(buf,num,0);
	show_text(DGUS_LOOP_SHOW_ADDR,7,buf,len);
}

//显示回路当前页
void show_loop_now_page(unsigned char page)
{
	unsigned char buf[2] = {0};
	unsigned char len = 0;

	len = data_conversion(buf,page,0);
	show_text(DGUS_LOOP_SHOW_ADDR,6,buf,len);
}

//显示回路信息
unsigned char m_ext_num = 0;
unsigned char m_ext_loop_num = 0;
void Show_Loop_Info(unsigned char dir)
{
	unsigned short i = 0,j = 0,m = 0;
	unsigned char control_id = 11;
	unsigned char buf[16] = {0};
	unsigned char m_num = 0;
	unsigned char len1 = 0;
	unsigned short m_power = 0;
	

	show_loop_now_page(page);

	if(g_loop_total_num == 0)
	{
		for(control_id = 11;control_id <= 120;control_id++)	//清空所有显示框
		{
			show_text(DGUS_LOOP_SHOW_ADDR,control_id,NULL,0);
		}
		
		return;
	}
		
	//显示集中器自身参数
	if(	page == 1)	//显示第一页
	{
		//打印回路号1	
		buf[0] = 0x31;
		show_text(DGUS_LOOP_SHOW_ADDR,11,buf,1);
		if(g_Concentrator_Para.e3000_relay_enable[0] != 0xFF)	//回路1启用
		{						
			//打印状态
			{			
				if(1)	//正常	D5 FD B3 A3
				{
					buf[0] = 0xD5;
					buf[1] = 0xFD;
					buf[2] = 0xB3;
					buf[3] = 0xA3;
				}
				else	//故障	B9 CA D5 CF
				{
					buf[0] = 0xB9;
					buf[1] = 0xCA;
					buf[2] = 0xD5;
					buf[3] = 0xCF;
				}
				show_text(DGUS_LOOP_SHOW_ADDR,12,buf,4);
			}
								
			//打印电压值
			for(i = 0;i < 3;i++)
			{
				if(g_Concentrator_Para.e3000_V_enable[i] != 0xFF)
				{
					len1 = data_conversion(buf,g_Phase_Voltage[i],DECIMAL1);
					buf[len1] = 0x2F;
					show_text(DGUS_LOOP_SHOW_ADDR,13+i,buf,len1+1);
				}
				else
				{
					buf[0] = 0x2D;
					buf[1] = 0x2F;
					len1 = 2;
					show_text(DGUS_LOOP_SHOW_ADDR,13+i,buf,2);
				}
			}
			

			//打印电流值
			for(i = 0;i < 3;i++)
			{
				if(g_Concentrator_Para.e3000_A_enable[i] != 0xFF)
				{
					len1 = data_conversion(buf,g_Phase_Amp[i],DECIMAL2);
					buf[len1] = 0x2F;
					show_text(DGUS_LOOP_SHOW_ADDR,16+i,buf,len1+1);
				}
				else
				{
					buf[0] = 0x2D;
					buf[1] = 0x2F;
					len1 = 2;
					show_text(DGUS_LOOP_SHOW_ADDR,16+i,buf,2);
				}				
			}
			
			//打印功率值
			for(i = 0;i < 3;i++)
			{
				if( (g_Concentrator_Para.e3000_V_enable[i] != 0xFF)&&(g_Concentrator_Para.e3000_A_enable[i] != 0xFF) )
				{
					m_power = g_Phase_Voltage[i]*g_Phase_Amp[i]/1000;
					len1 = data_conversion(buf,m_power,DECIMAL0);
					buf[len1] = 0x2F;
					show_text(DGUS_LOOP_SHOW_ADDR,19+i,buf,len1+1);
				}
				else
				{
					buf[0] = 0x2D;
					buf[1] = 0x2F;
					len1 = 2;
					show_text(DGUS_LOOP_SHOW_ADDR,19+i,buf,2);
				}				
			}
		}
		else
		{
			buf[0] = 0x2D;
			buf[1] = 0x2F;
			len1 = 2;
			show_text(DGUS_LOOP_SHOW_ADDR,12,buf,2);
		}
	}
	
	//显示扩展回路信息
	if(page == 1)
	{
		m_ext_num = 0;
		m_ext_loop_num = 0;
		control_id = 22;
	}
	else if(page == 2)
	{
		m_ext_num = 2;
		m_ext_loop_num = 1;
		control_id = 11;
	}
	
	for(i = m_ext_num;i < MAX_EXT_MODULE_NUM;i++)
	{
		for(j = m_ext_loop_num;j < 4;j++)
		{
			//打印回路号	
			m_num = i*4+j+2;
			len1 = data_conversion(buf,m_num,DECIMAL0);
			show_text(DGUS_LOOP_SHOW_ADDR,control_id,buf,len1);
			
			if(page == 1)
			{
				control_id = m_num*11;
			}
			else
			{
				control_id = (m_num-10)*11;
			}
			
			
			if(g_Concentrator_Para.ext_relay_enable[i][j] == 0xFF)	//回路禁用
			{
				buf[0] = 0x2D;
				show_text(DGUS_LOOP_SHOW_ADDR,control_id+1,buf,1);
				for(m = 0;m <= 9;m++)	//清空该回路其他信息显示框
				{
					show_text(DGUS_LOOP_SHOW_ADDR,control_id+2+m,NULL,0);
				}
			}
			else
			{			
				//打印状态
				{		
					//扩展在线
					if(g_ext_online_flag[i] == ONLINE)	//正常	D5 FD B3 A3
					{
						buf[0] = 0xD5;
						buf[1] = 0xFD;
						buf[2] = 0xB3;
						buf[3] = 0xA3;
					}
					else	//故障	B9 CA D5 CF
					{
						buf[0] = 0xB9;
						buf[1] = 0xCA;
						buf[2] = 0xD5;
						buf[3] = 0xCF;
					}
					show_text(DGUS_LOOP_SHOW_ADDR,control_id+1,buf,4);
				}
									
				//打印电压值
				for(m = 0;m < 3;m++)
				{
					if(g_Concentrator_Para.e3000_V_enable[m] != 0xFF)
					{
						len1 = data_conversion(buf,g_Phase_Voltage[m],DECIMAL1);
						buf[len1] = 0x2F;
						show_text(DGUS_LOOP_SHOW_ADDR,control_id+2+m,buf,len1+1);
					}
					else
					{
						buf[0] = 0x2D;
						buf[1] = 0x2F;
						show_text(DGUS_LOOP_SHOW_ADDR,control_id+2+m,buf,2);
					}
				}
				

				//打印电流值
				for(m = 0;m < 3;m++)
				{
					if(g_Concentrator_Para.ext_phase_enable[i][j*3+m] != 0xFF)		//电流禁用
					{
						len1 = data_conversion(buf,g_Concentrator_Para.ext_current[i][j*3+m],DECIMAL2);
						buf[len1] = 0x2F;
						show_text(DGUS_LOOP_SHOW_ADDR,control_id+5+m,buf,len1+1);
					}
					else
					{
						buf[0] = 0x2D;
						buf[1] = 0x2F;
						show_text(DGUS_LOOP_SHOW_ADDR,control_id+5+m,buf,2);
					}
					
				}
				
				//打印功率值
				for(m = 0;m < 3;m++)
				{
					if( (g_Concentrator_Para.e3000_V_enable[m] != 0xFF)&&(g_Concentrator_Para.ext_phase_enable[i][j*3+m] != 0xFF) )
					{
						m_power = g_Phase_Voltage[m]*g_Concentrator_Para.ext_current[i][j*3+m]/1000;
						len1 = data_conversion(buf,m_power,DECIMAL0);
						buf[len1] = 0x2F;
						show_text(DGUS_LOOP_SHOW_ADDR,control_id+8+m,buf,len1+1);	
					}
					else
					{
						buf[0] = 0x2D;
						buf[1] = 0x2F;
						show_text(DGUS_LOOP_SHOW_ADDR,control_id+8+m,buf,2);	
					}
						
				}					
			}
			
			control_id = control_id+11;
			
			if( page == 1)	//第一页
			{
				if(m_num == 10)	//第一页显示完成
				{					
					return;
				}
			}
		}
	}
	
	if(page == 2)
	{
		for(control_id = 44;control_id <= 120;control_id++)	//清空所有显示框
		{
			show_text(DGUS_LOOP_SHOW_ADDR,control_id,NULL,0);
		}
	}
}
