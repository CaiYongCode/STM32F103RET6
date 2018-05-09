#include "PLC.h"
#include "globaldata.h"
#include "globalfunc.h"
#include "GPRS.h"
#include "dacai_touch.h"
#include "Lamp.h"



volatile unsigned char m_send_PLC_index = 0;
volatile unsigned char m_add_PLC_index = 0;
volatile unsigned char m_PLC_RingBuf_Num = 0;

unsigned short m_1000ms_plc_counter;
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//发送PLC报文   周期调用   所有报文缓存到FIFO中，每个6秒周期调用
void Send_RTB42_Bdp(void)
{
   
   
   if(m_PLC_RingBuf_Num > 0 )
   	{
		m_1000ms_plc_counter++;
   	    if(g_Send_bdp_FIFO[m_send_PLC_index].send_counter > 0)
   	    	{

			 if((m_1000ms_plc_counter % 12) == 1)
			 	{
			 	   //这个时刻发送抄读暂停报文
			 	   send_plc_stop_bdp();
			 	}
			 if((m_1000ms_plc_counter % 12) == 3)  //第3秒发送报文
			 	{
	   	    	OSIntEnter();
				 
	   	    	    //发送报文	   	    	    					
					COM2_Send_BDP(g_Send_bdp_FIFO[m_send_PLC_index].buf, g_Send_bdp_FIFO[m_send_PLC_index].len);
					g_plc_msg_type = g_Send_bdp_FIFO[m_send_PLC_index].type;
				OSIntExit();
			 	}

			 if((m_1000ms_plc_counter % 12) == 11) 
			 	{
			 	OSIntEnter();
			 	   //这个时刻发送抄读开始报文
				   	if(g_lamp_total_num != 0)	//从节点档案不为空
					{	
						send_plc_restore_bdp();
					}
					g_Send_bdp_FIFO[m_send_PLC_index].send_counter--;
			 		if(g_Send_bdp_FIFO[m_send_PLC_index].send_counter == 0)  //此条报文发送完毕
						{							
						     memset((char *)g_Send_bdp_FIFO[m_send_PLC_index].buf,0,156);//清空缓存区
						    g_Send_bdp_FIFO[m_send_PLC_index].len = 0;
							g_Send_bdp_FIFO[m_send_PLC_index].type = 0;
							
							m_send_PLC_index++;
							if(m_send_PLC_index == MAX_PLC_FIFO_DEP )
						   	{
						   	   m_send_PLC_index = 0;
						   	}
							
							m_PLC_RingBuf_Num -- ;
						}
						
					m_1000ms_plc_counter = 0;
				OSIntExit();
			 	}
   	    	}
   	}
   else
   	{
   	   //缓存区中无报文发送
   	}

}


unsigned char Add_bpd_to_PLC_RingBuf(unsigned char send_counter, unsigned char * pbuf, unsigned char len,unsigned char type)
{
     unsigned char m_ret  = 0;
     if(m_PLC_RingBuf_Num == MAX_PLC_FIFO_DEP)  //存储空间已经满了
     	{
     	   m_ret = 0;
     	}
	 else
	 	{
	 	OSIntEnter();
	 	    //赋值
	 	   g_Send_bdp_FIFO[m_add_PLC_index].send_counter = send_counter;
		   g_Send_bdp_FIFO[m_add_PLC_index].len = len;
		   g_Send_bdp_FIFO[m_send_PLC_index].type = type;
		   memcpy((char *)g_Send_bdp_FIFO[m_add_PLC_index].buf,pbuf,len);
	 	  
	 	   m_add_PLC_index++;
		   if(m_add_PLC_index == MAX_PLC_FIFO_DEP )
		   	{
		   	   m_add_PLC_index = 0;
		   	}
		   m_PLC_RingBuf_Num++;
		   m_ret = 1;
		OSIntExit();
	 	}
	 

	 return m_ret;
}
//////////////////////////////////////////////////////////////////////////////////////////////
void ActOn_RTB42_BDP(unsigned char *pbuf,unsigned char len)
{	
	unsigned char m_AFN = 0;	//应用功能码
	unsigned char m_DUI = 0;	//数据单元标识
	unsigned char m_error = 0;	//错误状态字
	unsigned char m_len = 0;
	
	//如果报文接收长度与实际长度不相同，丢弃报文
	m_len = pbuf[2]*256+pbuf[1];
	if(m_len != len)
	{
		return;
	}
	
	g_PLC_timeout_1000ms_counter = 0;	    //路由通信超时计数清零
	g_router_fault_reboot_num    = 0;		//路由故障重启计数清零
	//一旦收到路由发过来的数据，解除路由通信故障
	if( g_router_fault_flag == 1 )	//路由通信故障状态
	{
 		send_concentrator_fault_release_bdp(CONCENTRATOR_FAULT_1,0);//解除路由通信故障
 		g_router_fault_flag = 0;		//路由通信故障位置0
	}
	
	g_plc_msg_num = pbuf[9];
	m_AFN = pbuf[10];
	m_DUI = pbuf[11];
	m_error = pbuf[13];
	
	if((pbuf[22] == 0x13)&&(pbuf[23] == 0x01))	//路由监控从节点回复报文
	{
		if(pbuf[28] != 0)
		{
			data_handle_func(&pbuf[30]);		//抄读数据处理
			show_text(DGUS_DAMP_READ_SHOW_ADDR,9,"抄读成功",strlen("抄读成功"));
		}
		else
		{
			show_text(DGUS_DAMP_READ_SHOW_ADDR,9,"抄读失败",strlen("抄读失败"));
		}
	}
	
	switch(m_AFN)
	{
		case 0x00:			
			if(m_DUI == 0x01)	//确认报文	
			{
				confirm_message_handle_func();			
			}
			else if(m_DUI== 0x02)	//否认报文
			{
				deny_message_handle_func(m_error);
			}
			break;
		case 0x14:	//路由请求抄读内容
			if(m_DUI == 0x01)
			{
				request_read_handle_func(pbuf);
			}
			break;
		case 0x06:	//路由上报
			route_report_handle_func(pbuf);
			break;
		default:
			break;
  }
}
///////////////////////////////////////////////////////////////////////
//确认报文处理函数
void confirm_message_handle_func(void)
{
	if(g_plc_msg_type == PLC_MSG_TYPE_2)		//清空节点
	{
		show_text(DGUS_PROMPT_SHOW_ADDR,1,"已清空",strlen("已清空"));
		Icon_Show(DGUS_PROMPT_SHOW_ADDR,3,0);
		Change_Page(DGUS_PROMPT_SHOW_ADDR);
	}
	else if(g_plc_msg_type == PLC_MSG_TYPE_3)		//启动抄读
	{
		//如果在抄读界面，则显示提示
		if(g_now_screen_id == DGUS_DAMP_READ_SHOW_ADDR)
		{
			show_text(DGUS_DAMP_READ_SHOW_ADDR,9,"启动抄读",strlen("启动抄读"));
		}
		else //否则在状态显示界面提示
		{
			show_text(DGUS_STATE_SHOW_ADDR,26,"启动抄读",strlen("启动抄读"));
		}
	}
	else if(g_plc_msg_type == PLC_MSG_TYPE_4)		//暂停抄读
	{
		//如果在抄读界面，则显示提示
		if(g_now_screen_id == DGUS_DAMP_READ_SHOW_ADDR)
		{
			show_text(DGUS_DAMP_READ_SHOW_ADDR,9,"暂停抄读",strlen("暂停抄读"));
		}
		else //否则在状态显示界面提示
		{
			show_text(DGUS_STATE_SHOW_ADDR,26,"暂停抄读",strlen("暂停抄读"));
		}
	}
	else if(g_plc_msg_type == PLC_MSG_TYPE_5)		//恢复抄读
	{
		//如果在抄读界面，则显示提示
		if(g_now_screen_id == DGUS_DAMP_READ_SHOW_ADDR)
		{
			show_text(DGUS_DAMP_READ_SHOW_ADDR,9,"恢复抄读",strlen("恢复抄读"));
		}
		else //否则在状态显示界面提示
		{
			show_text(DGUS_STATE_SHOW_ADDR,26,"恢复抄读",strlen("恢复抄读"));
		}
	}
	else if(g_plc_msg_type == PLC_MSG_TYPE_7)			//添加从节点
	{
//		HEXaddr_to_BCDaddr(g_add_damp_addr,addr);
//		send_monitor_damp_bdp(addr);	//路由添加节点后，监控从节点，点抄一次
		add_damp_addr(g_add_damp_addr);
		//如果在节点管理界面，则显示提示
		if(g_now_screen_id == DGUS_DAMP_MANAGE_SHOW_ADDR)
		{
			show_text(DGUS_DAMP_MANAGE_SHOW_ADDR,11,"已添加",strlen("已添加"));
		}
	}				
	else if(g_plc_msg_type == PLC_MSG_TYPE_8)		//删除从节点	
	{
		del_damp_addr(g_del_damp_addr);
		//如果在节点管理界面，则显示提示
		if(g_now_screen_id == DGUS_DAMP_MANAGE_SHOW_ADDR)
		{
			show_text(DGUS_DAMP_MANAGE_SHOW_ADDR,11,"已删除",strlen("已删除"));
		}
	}		
	else if(g_plc_msg_type == PLC_MSG_TYPE_10)		//手动设置节点地址
	{
		//如果在节点管理界面，则显示提示
		if(g_now_screen_id == DGUS_SET_DAMP_ADDR_SHOW_ADDR)
		{
			show_text(DGUS_SET_DAMP_ADDR_SHOW_ADDR,1,"已下发",strlen("已下发"));
		}
	}
	
	g_plc_msg_type = 0;
}

//否认报文处理函数
void deny_message_handle_func(unsigned char error)
{
	
	if(error == 0x01)
	{
		if(g_now_screen_id == DGUS_DAMP_MANAGE_SHOW_ADDR)
		{
			show_text(DGUS_DAMP_MANAGE_SHOW_ADDR,11,"无效数据单元",strlen("无效数据单元"));
		}
	}
	else if(error == 0x06)
	{
		if(g_now_screen_id == DGUS_DAMP_MANAGE_SHOW_ADDR)
		{
			show_text(DGUS_DAMP_MANAGE_SHOW_ADDR,11,"节点重复",strlen("节点重复"));
		}
	}
	else if(error == 0x07)
	{
		if(g_now_screen_id == DGUS_DAMP_MANAGE_SHOW_ADDR)
		{
			show_text(DGUS_DAMP_MANAGE_SHOW_ADDR,11,"节点不存在",strlen("节点不存在"));
		}
	}	
	g_plc_msg_type = 0;
}

unsigned char m_last_addr[6] = {0};
//路由请求抄读报文处理函数
void request_read_handle_func(unsigned char *pbuf)
{
	unsigned short i = 0,j = 0;
	unsigned char addr[6] = {0};
	unsigned char m = 0,eq = 1;

	g_plc_read_1000ms_couter = 0;
	
	//日常抄读请求
	BCDaddr_to_HEXaddr(&pbuf[14],addr);
	for(i = 0;i < DAMP_MAX_NUM;i++)
	{
		if((g_Damp_Info[i].SN[0] == addr[0])&&(g_Damp_Info[i].SN[1] == addr[1])&&(g_Damp_Info[i].SN[2] == addr[2])\
			&&(g_Damp_Info[i].SN[3] == addr[3])&&(g_Damp_Info[i].SN[4] == addr[4])&&(g_Damp_Info[i].SN[5] == addr[5]))
		{
			//如果前后两次请求抄读的节点不同，则判断前一节点是否抄读失败
			for(m = 0;m < 6;m++)
			{
				if(m_last_addr[m] != addr[m])
				{
					eq = 0;
					break;
				}
			}
			if( (eq == 0)||(g_lamp_read_num == g_lamp_total_num) )
			{
				read_fail_judge_func(m_last_addr);
			}
			//保存节点，作判断抄读失败用
			for(j = 0;j < 6;j++)	
			{
				m_last_addr[j] = addr[j];
			}
				
			//抄读数量大于节点总数则重启抄读
			if(g_lamp_read_num >= g_lamp_total_num)
			{
				g_read_restart_flag = 1;
			}
			//保存节点相位
			g_Damp_Info[i].m_bit_status.phase = pbuf[13];					
			//根据节点抄读标志位做相应处理和回复
			if( g_Damp_Info[i].m_bit_status.read_flag == 0)	//本轮第一次请求抄读该节点
			{				
				g_lamp_read_num++;
				Show_Read_Progress(addr);	//显示抄读进度
				send_read_need_bdp(&pbuf[14]);	//回复路由该节点需要抄读
				g_Damp_Info[i].m_bit_status.read_flag = 0x01;		
			}
			else if( g_Damp_Info[i].m_bit_status.read_flag == 0x01)		//抄读过但未成功
			{
				Show_Read_Progress(addr);	//显示抄读进度
				send_read_need_bdp(&pbuf[14]);	//回复路由该节点需要抄读	
			}
			else if( g_Damp_Info[i].m_bit_status.read_flag == 0x02)		//抄读成功
			{
				send_read_reply_bdp(1);				
			}
			return;
		}	
	}
	send_read_reply_bdp(0);	//节点不存在，继续后续节点抄读
}

//路由上报报文处理函数
void route_report_handle_func(unsigned char *pbuf)
{
	unsigned char m_DUI = 0;	//数据单元标识
	
	m_DUI = pbuf[11];
	
	if(m_DUI == 0x02)	//路由上报抄读数据
	{		
		data_handle_func(&pbuf[20]);
	}
	else if(m_DUI == 0x04)	//路由上报工况变动信息
	{
		if(pbuf[13] == 0x01)	//抄表任务结束
		{			
			g_read_restart_flag = 1;
		}
	}
	send_plc_ack_bdp();
}


////////////////////////////////////////////////////////////////////////////////////////
//载波交互报文
////////////////////////////////////////////////////////////////////////////////////////
//硬件初始化,路由复位
void send_hardware_init_bdp(void)
{
	unsigned char m_sum = 0;
	memset((char *)com2_send_buf,0,64);

	g_plc_msg_num++;

	com2_send_buf[0] = 0x68;
	com2_send_buf[1] = 0x0F;
	com2_send_buf[2] = 0x00;
	com2_send_buf[3] = 0x41;
	com2_send_buf[4] = 0x00;
	com2_send_buf[5] = 0x00;
	com2_send_buf[6] = 0x04;			//预计应答字节数
	com2_send_buf[7] = 0x00;
	com2_send_buf[8] = 0x00;
	com2_send_buf[9] = g_plc_msg_num;	//报文序列号
	com2_send_buf[10] = 0x01;
	com2_send_buf[11] = 0x01;
	com2_send_buf[12] = 0x00;
	
	m_sum = checksum(&com2_send_buf[3],10);
	com2_send_buf[13] = m_sum;
	com2_send_buf[14] = 0x16;
	Add_bpd_to_PLC_RingBuf(1,com2_send_buf,15,PLC_MSG_TYPE_1);
	
	//COM2_Send_BDP(com2_send_buf,15);
}
//参数区初始化，清空从节点信息
void send_parameter_init_bdp(void)
{
	//68 0F 00 41 00 00 04 00 00 04 01 02 00 4C 16 
	unsigned char m_sum = 0;
	memset((char *)com2_send_buf,0,64);

	g_plc_msg_num++;

	com2_send_buf[0] = 0x68;
	com2_send_buf[1] = 0x0F;
	com2_send_buf[2] = 0x00;
	com2_send_buf[3] = 0x41;
	com2_send_buf[4] = 0x00;
	com2_send_buf[5] = 0x00;
	com2_send_buf[6] = 0x04;			//预计应答字节数
	com2_send_buf[7] = 0x00;
	com2_send_buf[8] = 0x00;
	com2_send_buf[9] = g_plc_msg_num;	//报文序列号
	com2_send_buf[10] = 0x01;
	com2_send_buf[11] = 0x02;
	com2_send_buf[12] = 0x00;
	
	m_sum = checksum(&com2_send_buf[3],10);
	com2_send_buf[13] = m_sum;
	com2_send_buf[14] = 0x16;
	Add_bpd_to_PLC_RingBuf(1, com2_send_buf, 15,PLC_MSG_TYPE_2);
	//COM2_Send_BDP(com2_send_buf,15);
}

//重启路由抄读报文  
//68 0F 00 41 00 00 04 00 00 26 12 01 00 7E 16 
void send_plc_reboot_bdp(void)
{
	unsigned char m_sum = 0;
	memset((char *)com2_send_buf,0,64);
	
	g_plc_msg_num++;
	
	com2_send_buf[0] = 0x68;
	com2_send_buf[1] = 0x0F;
	com2_send_buf[2] = 0x00;
	com2_send_buf[3] = 0x41;
	com2_send_buf[4] = 0x00;
	com2_send_buf[5] = 0x00;
	com2_send_buf[6] = 0x04;			//预计应答字节数
	com2_send_buf[7] = 0x00;
	com2_send_buf[8] = 0x00;
	com2_send_buf[9] = g_plc_msg_num;	//报文序列号
	com2_send_buf[10] = 0x12;
	com2_send_buf[11] = 0x01;
	com2_send_buf[12] = 0x00;
	
	m_sum = checksum(&com2_send_buf[3],10);
	com2_send_buf[13] = m_sum;
	com2_send_buf[14] = 0x16;

//	Add_bpd_to_PLC_RingBuf(1, com2_send_buf, 15);
	COM2_Send_BDP(com2_send_buf,15);
	
	g_plc_msg_type = PLC_MSG_TYPE_3;
}

//暂停路由抄读报文
//68 0F 00 41 00 00 04 00 00 1D 12 02 00 76 16
void send_plc_stop_bdp(void)
{
	unsigned char m_sum = 0;
  memset((char *)com2_send_buf,0,64);
	
	g_plc_msg_num++;
	
	com2_send_buf[0] = 0x68;
	com2_send_buf[1] = 0x0F;
	com2_send_buf[2] = 0x00;
	com2_send_buf[3] = 0x41;
	com2_send_buf[4] = 0x00;
	com2_send_buf[5] = 0x00;
	com2_send_buf[6] = 0x04;			//预计应答字节数
	com2_send_buf[7] = 0x00;
	com2_send_buf[8] = 0x00;
	com2_send_buf[9] = g_plc_msg_num;	//报文序列号
	com2_send_buf[10] = 0x12;
	com2_send_buf[11] = 0x02;
	com2_send_buf[12] = 0x00;
	
	m_sum = checksum(&com2_send_buf[3],10);
	com2_send_buf[13] = m_sum;
	com2_send_buf[14] = 0x16;
//	Add_bpd_to_PLC_RingBuf(1, com2_send_buf, 15);
	COM2_Send_BDP(com2_send_buf,15);
	
	g_plc_msg_type = PLC_MSG_TYPE_4;
}

//恢复
void send_plc_restore_bdp(void)
{
	//68 0F 00 41 00 00 04 00 00 0A 12 04 00 65 16 
	unsigned char m_sum = 0;
	memset((char *)com2_send_buf,0,64);
	
	g_plc_msg_num++;
	
	com2_send_buf[0] = 0x68;
	com2_send_buf[1] = 0x0F;
	com2_send_buf[2] = 0x00;
	com2_send_buf[3] = 0x41;
	com2_send_buf[4] = 0x00;
	com2_send_buf[5] = 0x00;
	com2_send_buf[6] = 0x04;			//预计应答字节数
	com2_send_buf[7] = 0x00;
	com2_send_buf[8] = 0x00;
	com2_send_buf[9] = g_plc_msg_num;	//报文序列号
	com2_send_buf[10] = 0x12;
	com2_send_buf[11] = 0x04;
	com2_send_buf[12] = 0x00;
	
	m_sum = checksum(&com2_send_buf[3],10);
	com2_send_buf[13] = m_sum;
	com2_send_buf[14] = 0x16;
//	Add_bpd_to_PLC_RingBuf(1, com2_send_buf, 15);
	COM2_Send_BDP(com2_send_buf,15);
	
	g_plc_msg_type = PLC_MSG_TYPE_5;
}

//设置主节点地址
void send_set_route_addr_bdp(void)
{
	//68 15 00 41 00 00 5F 00 00 1A 05 01 00 01 00 00 00 00 00 C1 16
	unsigned char m_sum = 0;
	memset((char *)com2_send_buf,0,64);

	g_plc_msg_num++;
	
	com2_send_buf[0] = 0x68;
	com2_send_buf[1] = 0x15;
	com2_send_buf[2] = 0x00;
	com2_send_buf[3] = 0x41;
	com2_send_buf[4] = 0x00;
	com2_send_buf[5] = 0x00;
	com2_send_buf[6] = 0x5F;			//预计应答字节数
	com2_send_buf[7] = 0x00;
	com2_send_buf[8] = 0x00;
	com2_send_buf[9] = g_plc_msg_num;	//报文序列号
	com2_send_buf[10] = 0x05;
	com2_send_buf[11] = 0x01;
	com2_send_buf[12] = 0x00;
	com2_send_buf[13] = g_router_addr[0];
	com2_send_buf[14] = g_router_addr[1];
	com2_send_buf[15] = g_router_addr[2];	
	com2_send_buf[16] = g_router_addr[3];
	com2_send_buf[17] = g_router_addr[4];
	com2_send_buf[18] = g_router_addr[5];
	
	m_sum = checksum((unsigned char *)(&com2_send_buf[3]),16);
	com2_send_buf[19] = m_sum;
	com2_send_buf[20] = 0x16;
//	Add_bpd_to_PLC_RingBuf(1, com2_send_buf, 21);
	COM2_Send_BDP(com2_send_buf,21);
}

//发送添加载波从节点报文
void send_add_damp_bdp(unsigned char *pbuf)
{
	//68 17 00 41 00 00 5F 00 00 59 11 01 00 01 00 00 00 00 00 05 01 12 16
	unsigned char m_sum = 0;
  memset((char *)com2_send_buf,0,64);

	g_plc_msg_num++;
	
	com2_send_buf[0] = 0x68;
	com2_send_buf[1] = 0x17;
	com2_send_buf[2] = 0x00;
	com2_send_buf[3] = 0x41;
	com2_send_buf[4] = 0x00;
	com2_send_buf[5] = 0x00;
	com2_send_buf[6] = 0x5F;			//预计应答字节数
	com2_send_buf[7] = 0x00;
	com2_send_buf[8] = 0x00;
	com2_send_buf[9] = g_plc_msg_num;	//报文序列号
	com2_send_buf[10] = 0x11;
	com2_send_buf[11] = 0x01;
	com2_send_buf[12] = 0x00;
	com2_send_buf[13] = 0x01;
	com2_send_buf[14] = pbuf[0];
	com2_send_buf[15] = pbuf[1];
	com2_send_buf[16] = pbuf[2];	
	com2_send_buf[17] = pbuf[3];
 	com2_send_buf[18] = pbuf[4];
 	com2_send_buf[19] = pbuf[5];
	com2_send_buf[20] = 0x01;
	
	m_sum = checksum((unsigned char *)(&com2_send_buf[3]),18);
	com2_send_buf[21] = m_sum;
	com2_send_buf[22] = 0x16;
	Add_bpd_to_PLC_RingBuf(1, com2_send_buf, 23,PLC_MSG_TYPE_7);
//	COM2_Send_BDP(com2_send_buf,23);
}

//发送删除载波从节点报文
void send_del_damp_bdp(unsigned char * pbuf)
{
	//68 16 00 41 00 00 5F 00 00 42 11 02 00 01 00 00 00 00 00 06 FC 16  
	unsigned char m_sum = 0;
  memset((char *)com2_send_buf,0,64);
	
	g_plc_msg_num++;
	
	com2_send_buf[0] = 0x68;
	com2_send_buf[1] = 0x16;
	com2_send_buf[2] = 0x00;
	com2_send_buf[3] = 0x41;
	com2_send_buf[4] = 0x00;
	com2_send_buf[5] = 0x00;
	com2_send_buf[6] = 0x5F;			//预计应答字节数
	com2_send_buf[7] = 0x00;
	com2_send_buf[8] = 0x00;
	com2_send_buf[9] = g_plc_msg_num;	//报文序列号
	com2_send_buf[10] = 0x11;
	com2_send_buf[11] = 0x02;
	com2_send_buf[12] = 0x00;
	com2_send_buf[13] = 0x01;
	com2_send_buf[14] = pbuf[0];
	com2_send_buf[15] = pbuf[1];
	com2_send_buf[16] = pbuf[2];	
	com2_send_buf[17] = pbuf[3];
	com2_send_buf[18] = pbuf[4];
	com2_send_buf[19] = pbuf[5];
	
	m_sum = checksum((unsigned char *)(&com2_send_buf[3]),17);
	com2_send_buf[20] = m_sum;
	com2_send_buf[21] = 0x16;
	Add_bpd_to_PLC_RingBuf(1, com2_send_buf, 22,PLC_MSG_TYPE_8);
//	COM2_Send_BDP(com2_send_buf,22);
}


// //查询从节点信息
// void send_query_damp_info_bdp(void)
// {
// 	unsigned char m_sum = 0;
// 	memset((char *)com2_send_buf,0,64);

// 	g_plc_msg_num++;
// 	
// 	com2_send_buf[0] = 0x68;
// 	com2_send_buf[1] = 0x12;
// 	com2_send_buf[2] = 0x00;
// 	com2_send_buf[3] = 0x41;
// 	com2_send_buf[4] = 0x00;
// 	com2_send_buf[5] = 0x00;
// 	com2_send_buf[6] = 0x5F;			//预计应答字节数
// 	com2_send_buf[7] = 0x00;
// 	com2_send_buf[8] = 0x00;
// 	com2_send_buf[9] = g_plc_msg_num;	//报文序列号
// 	com2_send_buf[10] = 0x10;
// 	com2_send_buf[11] = 0x02;
// 	com2_send_buf[12] = 0x00;
// 	com2_send_buf[13] = 0x04;
// 	com2_send_buf[14] = 0x00;
// 	com2_send_buf[15] = 0x01;
// 	
// 	m_sum = checksum(&com2_send_buf[3],13);
// 	com2_send_buf[16] = m_sum;
// 	com2_send_buf[17] = 0x16;
// 	Add_bpd_to_PLC_RingBuf(1, com2_send_buf, 18);
// //	COM2_Send_BDP(com2_send_buf,18);
// }

//监控从节点 对单个从节点进行点抄
void send_monitor_damp_bdp(unsigned char *damp_addr)
{
	//68 2F 00 41 04 00 04 00 00 0B 01 00 00 00 00 00 00 00 00 00 00 01 13 01 00 02 00 00 10 \
	68 00 00 00 00 00 01 68 11 04 33 33 34 33 B3 16 F8 16
	unsigned char m_sum = 0;
  memset((char *)com2_send_buf,0,64);

	g_plc_msg_num++;

	com2_send_buf[0] = 0x68;
	com2_send_buf[1] = 0x2F;
	com2_send_buf[2] = 0x00;
	com2_send_buf[3] = 0x41;
	com2_send_buf[4] = 0x04;
	com2_send_buf[5] = 0x00;
	com2_send_buf[6] = 0x04;				//预计应答字节数
	com2_send_buf[7] = 0x00;
	com2_send_buf[8] = 0x00;
	com2_send_buf[9] = g_plc_msg_num;		//报文序列号
	com2_send_buf[10] = g_router_addr[0];
	com2_send_buf[11] = g_router_addr[1];
	com2_send_buf[12] = g_router_addr[2];
	com2_send_buf[13] = g_router_addr[3];
	com2_send_buf[14] = g_router_addr[4];
	com2_send_buf[15] = g_router_addr[5];	
	com2_send_buf[16] = damp_addr[0];
	com2_send_buf[17] = damp_addr[1];
	com2_send_buf[18] = damp_addr[2];
	com2_send_buf[19] = damp_addr[3];
	com2_send_buf[20] = damp_addr[4];
	com2_send_buf[21] = damp_addr[5];
	com2_send_buf[22] = 0x13;
	com2_send_buf[23] = 0x01;
	com2_send_buf[24] = 0x00;	
	com2_send_buf[25] = 0x02;
	com2_send_buf[26] = 0x00;
	com2_send_buf[27] = 0x00;
	com2_send_buf[28] = 0x10;
	
	com2_send_buf[29] = 0x68;
	com2_send_buf[30] = damp_addr[0];
	com2_send_buf[31] = damp_addr[1];
	com2_send_buf[32] = damp_addr[2];
	com2_send_buf[33] = damp_addr[3];	
	com2_send_buf[34] = damp_addr[4];	
	com2_send_buf[35] = damp_addr[5];
	com2_send_buf[36] = 0x68;
	com2_send_buf[37] = 0x11;		
	com2_send_buf[38] = 0x04;
	com2_send_buf[39] = 0x33;
	com2_send_buf[40] = 0x33;	
	com2_send_buf[41] = 0x34;
 	com2_send_buf[42] = 0x33;
		
	m_sum = checksum((unsigned char *)(&com2_send_buf[29]),14);
	com2_send_buf[43] = m_sum;
	com2_send_buf[44] = 0x16;

	m_sum = checksum((unsigned char *)(&com2_send_buf[3]),42);
	com2_send_buf[45] = m_sum;
	com2_send_buf[46] = 0x16;
	Add_bpd_to_PLC_RingBuf(1, com2_send_buf, 47,PLC_MSG_TYPE_9);
//	COM2_Send_BDP(com2_send_buf,47);	
}

//回告需要抄读帧
void send_read_need_bdp(unsigned char *pbuf)
{
	//68 23 00 01 00 00 04 00 00 03 14 01 00 02 00 10 68 00 00 00 00 00 05 68 11 04 33 33 34 33 B7 16 00 B3 16
	
	unsigned char m_sum = 0;

	com2_send_buf[0] = 0x68;
	com2_send_buf[1] = 0x23;
	com2_send_buf[2] = 0x00;
	com2_send_buf[3] = 0x01;
	com2_send_buf[4] = 0x00;
	com2_send_buf[5] = 0x00;
	com2_send_buf[6] = 0x04;				//预计应答字节数
	com2_send_buf[7] = 0x00;
	com2_send_buf[8] = 0x00;
	com2_send_buf[9] = g_plc_msg_num;		//报文序列号
	com2_send_buf[10] = 0x14;
	com2_send_buf[11] = 0x01;
	com2_send_buf[12] = 0x00;
	com2_send_buf[13] = 0x02;  
	com2_send_buf[14] = 0x00;
	com2_send_buf[15] = 0x10;	
	com2_send_buf[16] = 0x68;
	com2_send_buf[17] = pbuf[0];
	com2_send_buf[18] = pbuf[1];
	com2_send_buf[19] = pbuf[2];
	com2_send_buf[20] = pbuf[3];
	com2_send_buf[21] = pbuf[4];
	com2_send_buf[22] = pbuf[5];
	com2_send_buf[23] = 0x68;
	com2_send_buf[24] = 0x11;	
	com2_send_buf[25] = 0x04;
	com2_send_buf[26] = 0x33;
	com2_send_buf[27] = 0x33;
	com2_send_buf[28] = 0x34;
	com2_send_buf[29] = 0x33;
	
	m_sum = checksum((unsigned char *)(&com2_send_buf[16]),14);
	com2_send_buf[30] = m_sum;
	com2_send_buf[31] = 0x16;
	com2_send_buf[32] = 0x00;
	m_sum = checksum((unsigned char *)(&com2_send_buf[3]),29);
	com2_send_buf[33] = m_sum;
	com2_send_buf[34] = 0x16;
//	Add_bpd_to_PLC_RingBuf(1, com2_send_buf, 35);
	COM2_Send_BDP(com2_send_buf,35);
}

//回复抄读成功帧
void send_read_reply_bdp(unsigned char read_flag)
{
	//68 13 00 01 00 00 04 00 00 04 14 01 00 01 00 00 00 1F 16 
	unsigned char m_sum = 0;
	
	com2_send_buf[0] = 0x68;
	com2_send_buf[1] = 0x13;
	com2_send_buf[2] = 0x00;
	com2_send_buf[3] = 0x01;
	com2_send_buf[4] = 0x00;
	com2_send_buf[5] = 0x00;
	com2_send_buf[6] = 0x04;			//预计应答字节数
	com2_send_buf[7] = 0x00;
	com2_send_buf[8] = 0x00;
	com2_send_buf[9] = g_plc_msg_num;	//报文序列号
	com2_send_buf[10] = 0x14;
	com2_send_buf[11] = 0x01;
	com2_send_buf[12] = 0x00;
	com2_send_buf[13] = read_flag;
	com2_send_buf[14] = 0x00;
	com2_send_buf[15] = 0x00;
	com2_send_buf[16] = 0x00;	
	
	m_sum = checksum((unsigned char *)(&com2_send_buf[3]),14);
	com2_send_buf[17] = m_sum;
	com2_send_buf[18] = 0x16;
//	Add_bpd_to_PLC_RingBuf(1, com2_send_buf, 19);
	COM2_Send_BDP(com2_send_buf,19);
}

//回复确认帧
void send_plc_ack_bdp(void)
{
	//68 15 00 01 00 00 00 00 00 03 00 01 00 01 00 00 00 05 00 0B 16 
	unsigned char m_sum = 0;
	
	com2_send_buf[0] = 0x68;
	com2_send_buf[1] = 0x15;
	com2_send_buf[2] = 0x00;
	com2_send_buf[3] = 0x01;
	com2_send_buf[4] = 0x00;
	com2_send_buf[5] = 0x00;
	com2_send_buf[6] = 0x00;			//预计应答字节数
	com2_send_buf[7] = 0x00;
	com2_send_buf[8] = 0x00;
	com2_send_buf[9] = g_plc_msg_num;	//报文序列号
	com2_send_buf[10] = 0x00;
	com2_send_buf[11] = 0x01;
	com2_send_buf[12] = 0x00;
	com2_send_buf[13] = 0x01;
	com2_send_buf[14] = 0x00;
	com2_send_buf[15] = 0x00;
	com2_send_buf[16] = 0x00;	
	com2_send_buf[17] = 0x05;
	com2_send_buf[18] = 0x00;
	
	m_sum = checksum((unsigned char *)(&com2_send_buf[3]),16);
	com2_send_buf[19] = m_sum;
	com2_send_buf[20] = 0x16;
//	Add_bpd_to_PLC_RingBuf(1, com2_send_buf, 21);
	COM2_Send_BDP(com2_send_buf,21);
}



//设置节点地址
void send_set_damp_addr_bdp(unsigned char* old_addr,unsigned char* new_addr,unsigned char loops)
{
	unsigned char m_sum = 0;
	memset((char *)com2_send_buf,0,64);

	g_plc_msg_num++;
	
	com2_send_buf[0] = 0x68;
	com2_send_buf[1] = 0x37;
	com2_send_buf[2] = 0x00;
	com2_send_buf[3] = 0x41;
	com2_send_buf[4] = 0x04;
	com2_send_buf[5] = 0x00;
	com2_send_buf[6] = 0x5F;				//预计应答字节数
	com2_send_buf[7] = 0x00;
	com2_send_buf[8] = 0x00;
	com2_send_buf[9] = g_plc_msg_num;		//报文序列号
	com2_send_buf[10] = g_router_addr[0];
	com2_send_buf[11] = g_router_addr[1];
	com2_send_buf[12] = g_router_addr[2];
	com2_send_buf[13] = g_router_addr[3];
	com2_send_buf[14] = g_router_addr[4];
	com2_send_buf[15] = g_router_addr[5];	
	com2_send_buf[16] = 0x99;
	com2_send_buf[17] = 0x99;
	com2_send_buf[18] = 0x99;
	com2_send_buf[19] = 0x99;
	com2_send_buf[20] = 0x99;
	com2_send_buf[21] = 0x99;
	com2_send_buf[22] = 0x05;
	com2_send_buf[23] = 0x04;
	com2_send_buf[24] = 0x00;	
	com2_send_buf[25] = 0x02;
	com2_send_buf[26] = 0x1A;
	
	com2_send_buf[27] = 0x68;
	com2_send_buf[28] = 0x99;
	com2_send_buf[29] = 0x99;
	com2_send_buf[30] = 0x99;
	com2_send_buf[31] = 0x99;
	com2_send_buf[32] = 0x99;
	com2_send_buf[33] = 0x99;	
	com2_send_buf[34] = 0x68;	
	com2_send_buf[35] = 0x08;
	com2_send_buf[36] = 0x0E;
	com2_send_buf[37] = 0x01;		//表示设置节点地址
	com2_send_buf[38] = old_addr[0];			//6字节原地址
	com2_send_buf[39] = old_addr[1];
	com2_send_buf[40] = old_addr[2];
	com2_send_buf[41] = old_addr[3];
	com2_send_buf[42] = old_addr[4];
	com2_send_buf[43] = old_addr[5];
	com2_send_buf[44] = new_addr[0];	//6字节新地址
	com2_send_buf[45] = new_addr[1];
	com2_send_buf[46] = new_addr[2];
	com2_send_buf[47] = new_addr[3];
	com2_send_buf[48] = new_addr[4];
	com2_send_buf[49] = new_addr[5];
	com2_send_buf[50] = loops;
		
	m_sum = checksum((unsigned char *)(&com2_send_buf[27]),24);
	com2_send_buf[51] = m_sum;
	com2_send_buf[52] = 0x16;

	m_sum = checksum((unsigned char *)(&com2_send_buf[3]),50);
	com2_send_buf[53] = m_sum;
	com2_send_buf[54] = 0x16;
	Add_bpd_to_PLC_RingBuf(1, com2_send_buf, 55,PLC_MSG_TYPE_10);
	
//	COM2_Send_BDP(com2_send_buf,21);
}

//设置节点分组
void send_set_damp_group_bdp(unsigned char *pbuf,unsigned char group)
{
	unsigned char m_sum = 0;
  memset((char *)com2_send_buf,0,64);

	g_plc_msg_num++;
	
	com2_send_buf[0] = 0x68;
	com2_send_buf[1] = 0x31;
	com2_send_buf[2] = 0x00;
	com2_send_buf[3] = 0x41;
	com2_send_buf[4] = 0x04;
	com2_send_buf[5] = 0x00;
	com2_send_buf[6] = 0x5F;				//预计应答字节数
	com2_send_buf[7] = 0x00;
	com2_send_buf[8] = 0x00;
	com2_send_buf[9] = g_plc_msg_num;		//报文序列号
	com2_send_buf[10] = g_router_addr[0];
	com2_send_buf[11] = g_router_addr[1];
	com2_send_buf[12] = g_router_addr[2];
	com2_send_buf[13] = g_router_addr[3];
	com2_send_buf[14] = g_router_addr[4];
	com2_send_buf[15] = g_router_addr[5];	
	com2_send_buf[16] = 0x99;
	com2_send_buf[17] = 0x99;
	com2_send_buf[18] = 0x99;
	com2_send_buf[19] = 0x99;
	com2_send_buf[20] = 0x99;
	com2_send_buf[21] = 0x99;
	com2_send_buf[22] = 0x05;
	com2_send_buf[23] = 0x04;
	com2_send_buf[24] = 0x00;	
	com2_send_buf[25] = 0x02;
	com2_send_buf[26] = 0x14;
	
	com2_send_buf[27] = 0x68;
	com2_send_buf[28] = 0x99;
	com2_send_buf[29] = 0x99;
	com2_send_buf[30] = 0x99;
	com2_send_buf[31] = 0x99;
	com2_send_buf[32] = 0x99;
	com2_send_buf[33] = 0x99;	
	com2_send_buf[34] = 0x68;	
	com2_send_buf[35] = 0x08;
	com2_send_buf[36] = 0x08;
	com2_send_buf[37] = 0x02;		//表示分组命令
	com2_send_buf[38] = pbuf[0];	
	com2_send_buf[39] = pbuf[1];
	com2_send_buf[40] = pbuf[2];
	com2_send_buf[41] = pbuf[3];
	com2_send_buf[42] = pbuf[4];
	com2_send_buf[43] = pbuf[5];
	com2_send_buf[44] = group;
		
	m_sum = checksum((unsigned char *)(&com2_send_buf[27]),18);
	com2_send_buf[45] = m_sum;
	com2_send_buf[46] = 0x16;

	m_sum = checksum((unsigned char *)(&com2_send_buf[3]),44);
	com2_send_buf[47] = m_sum;
	com2_send_buf[48] = 0x16;
	Add_bpd_to_PLC_RingBuf(1, com2_send_buf, 49,PLC_MSG_TYPE_11);
//	COM2_Send_BDP(com2_send_buf,49);
}

//广播开关命令报文
void send_plc_cmd_bdp(unsigned char *addr,unsigned short group,unsigned char *contents)
{
	//68 35 00 41 04 00 5F 00 00 20 01 00 00 00 00 00 99 99 99 99 99 99 05 04 00 02 18 \
	68 99 99 99 99 99 99 68 08 0C 00 00 00 00 00 09 01 00 01 00 00 00 85 16 9E 16 
	unsigned char i = 0;
	unsigned char m_sum = 0;
	memset((char *)com2_send_buf,0,70);

	g_plc_msg_num++;
	
	com2_send_buf[0] = 0x68;
	com2_send_buf[1] = 0x44;
	com2_send_buf[2] = 0x00;
	com2_send_buf[3] = 0x41;
	com2_send_buf[4] = 0x04;
	com2_send_buf[5] = 0x00;
	com2_send_buf[6] = 0x5F;				//预计应答字节数
	com2_send_buf[7] = 0x00;
	com2_send_buf[8] = 0x00;
	com2_send_buf[9] = g_plc_msg_num;		//报文序列号
	com2_send_buf[10] = g_router_addr[0];
	com2_send_buf[11] = g_router_addr[1];
	com2_send_buf[12] = g_router_addr[2];
	com2_send_buf[13] = g_router_addr[3];
	com2_send_buf[14] = g_router_addr[4];
	com2_send_buf[15] = g_router_addr[5];	
	com2_send_buf[16] = 0x99;
	com2_send_buf[17] = 0x99;
	com2_send_buf[18] = 0x99;
	com2_send_buf[19] = 0x99;
	com2_send_buf[20] = 0x99;
	com2_send_buf[21] = 0x99;
	com2_send_buf[22] = 0x05;
	com2_send_buf[23] = 0x04;
	com2_send_buf[24] = 0x00;	
	com2_send_buf[25] = 0x02;
	com2_send_buf[26] = 0x27;
	
	com2_send_buf[27] = 0x68;
	com2_send_buf[28] = 0x99;
	com2_send_buf[29] = 0x99;
	com2_send_buf[30] = 0x99;
	com2_send_buf[31] = 0x99;
	com2_send_buf[32] = 0x99;
	com2_send_buf[33] = 0x99;	
	com2_send_buf[34] = 0x68;	
	com2_send_buf[35] = 0x08;
	com2_send_buf[36] = 0x1B;
	
	com2_send_buf[37] = 0x03;		//表示为开关灯命令

	com2_send_buf[38] = addr[0];	
	com2_send_buf[39] = addr[1];
	com2_send_buf[40] = addr[2];
	com2_send_buf[41] = addr[3];
	com2_send_buf[42] = addr[4];
	com2_send_buf[43] = addr[5];
	
	com2_send_buf[44] = group/256;
	com2_send_buf[45] = group%256;
	
	for(i = 0;i < 18;i++)
	{
		com2_send_buf[46+i] = contents[i];
	}
	
	m_sum = checksum((unsigned char *)(&com2_send_buf[27]),37);
	com2_send_buf[64] = m_sum;
	com2_send_buf[65] = 0x16;

	m_sum = checksum((unsigned char *)(&com2_send_buf[3]),63);
	com2_send_buf[66] = m_sum;
	com2_send_buf[67] = 0x16;

	if((com2_send_buf[61] != 0) || ((com2_send_buf[62] != 0)) || ((com2_send_buf[63] != 0)))
		{
			Add_bpd_to_PLC_RingBuf(3, com2_send_buf, 68,PLC_MSG_TYPE_12);  //开灯发三次
		}
	else
		{
		    Add_bpd_to_PLC_RingBuf(1, com2_send_buf, 68,PLC_MSG_TYPE_12);  //关灯关一次
		}
//	COM2_Send_BDP(com2_send_buf,68);
}

//广播下发系统时间报文
void send_system_time_bdp(void)
{
	unsigned char m_sum = 0;
	memset((char *)com2_send_buf,0,64);

	g_plc_msg_num++;
	
	com2_send_buf[0] = 0x68;
	com2_send_buf[1] = 0x30;
	com2_send_buf[2] = 0x00;
	com2_send_buf[3] = 0x41;
	com2_send_buf[4] = 0x04;
	com2_send_buf[5] = 0x00;
	com2_send_buf[6] = 0x5F;				//预计应答字节数
	com2_send_buf[7] = 0x00;
	com2_send_buf[8] = 0x00;
	com2_send_buf[9] = g_plc_msg_num;		//报文序列号
	com2_send_buf[10] = g_router_addr[0];
	com2_send_buf[11] = g_router_addr[1];
	com2_send_buf[12] = g_router_addr[2];
	com2_send_buf[13] = g_router_addr[3];
	com2_send_buf[14] = g_router_addr[4];
	com2_send_buf[15] = g_router_addr[5];	
	com2_send_buf[16] = 0x99;
	com2_send_buf[17] = 0x99;
	com2_send_buf[18] = 0x99;
	com2_send_buf[19] = 0x99;
	com2_send_buf[20] = 0x99;
	com2_send_buf[21] = 0x99;
	com2_send_buf[22] = 0x05;
	com2_send_buf[23] = 0x04;
	com2_send_buf[24] = 0x00;	
	com2_send_buf[25] = 0x02;
	com2_send_buf[26] = 0x13;
	
	com2_send_buf[27] = 0x68;
	com2_send_buf[28] = 0x99;
	com2_send_buf[29] = 0x99;
	com2_send_buf[30] = 0x99;
	com2_send_buf[31] = 0x99;
	com2_send_buf[32] = 0x99;
	com2_send_buf[33] = 0x99;	
	com2_send_buf[34] = 0x68;	
	com2_send_buf[35] = 0x08;
	com2_send_buf[36] = 0x07;
	com2_send_buf[37] = 0x04;		//表示为下发系统时间命令
	
	com2_send_buf[38] = g_now_year;	
	com2_send_buf[39] = g_now_month;
	com2_send_buf[40] = g_now_day;
	com2_send_buf[41] = g_now_hour;
	com2_send_buf[42] = g_now_min;
	com2_send_buf[43] = g_now_sec;
	
	m_sum = checksum((unsigned char *)(&com2_send_buf[27]),17);
	com2_send_buf[44] = m_sum;
	com2_send_buf[45] = 0x16;

	m_sum = checksum((unsigned char *)(&com2_send_buf[3]),43);
	com2_send_buf[46] = m_sum;
	com2_send_buf[47] = 0x16;
	Add_bpd_to_PLC_RingBuf(1, com2_send_buf, 48,PLC_MSG_TYPE_13);
//	COM2_Send_BDP(com2_send_buf,48);
}


//广播下发工作时间报文
void send_working_time_bdp(void)
{
	unsigned char i = 0;
	unsigned char m_sum = 0;
	memset((char *)com2_send_buf,0,256);

	g_plc_msg_num++;
	
	com2_send_buf[0] = 0x68;
	com2_send_buf[1] = 0x8A;
	com2_send_buf[2] = 0x00;
	com2_send_buf[3] = 0x41;
	com2_send_buf[4] = 0x04;
	com2_send_buf[5] = 0x00;
	com2_send_buf[6] = 0x5F;				//预计应答字节数
	com2_send_buf[7] = 0x00;
	com2_send_buf[8] = 0x00;
	com2_send_buf[9] = g_plc_msg_num;		//报文序列号
	com2_send_buf[10] = g_router_addr[0];
	com2_send_buf[11] = g_router_addr[1];
	com2_send_buf[12] = g_router_addr[2];
	com2_send_buf[13] = g_router_addr[3];
	com2_send_buf[14] = g_router_addr[4];
	com2_send_buf[15] = g_router_addr[5];	
	com2_send_buf[16] = 0x99;
	com2_send_buf[17] = 0x99;
	com2_send_buf[18] = 0x99;
	com2_send_buf[19] = 0x99;
	com2_send_buf[20] = 0x99;
	com2_send_buf[21] = 0x99;
	com2_send_buf[22] = 0x05;
	com2_send_buf[23] = 0x04;
	com2_send_buf[24] = 0x00;	
	com2_send_buf[25] = 0x02;
	com2_send_buf[26] = 0x6D;
	
	com2_send_buf[27] = 0x68;
	com2_send_buf[28] = 0x99;
	com2_send_buf[29] = 0x99;
	com2_send_buf[30] = 0x99;
	com2_send_buf[31] = 0x99;
	com2_send_buf[32] = 0x99;
	com2_send_buf[33] = 0x99;	
	com2_send_buf[34] = 0x68;	
	com2_send_buf[35] = 0x08;
	com2_send_buf[36] = 0x61;
	com2_send_buf[37] = 0x05;		//表示为下发工作时间命令
	
		
	if(g_lamp_working_time_num > 0)
	{
		com2_send_buf[38] = g_Light_Time_Info.s_time[g_lamp_working_time_index[0]]/256;	
		com2_send_buf[39] = g_Light_Time_Info.s_time[g_lamp_working_time_index[0]]%256;
		com2_send_buf[40] = g_Light_Time_Info.e_time[g_lamp_working_time_index[0]]/256;
		com2_send_buf[41] = g_Light_Time_Info.e_time[g_lamp_working_time_index[0]]%256;
		com2_send_buf[42] = g_Light_Time_Info.group[g_lamp_working_time_index[0]]/256;
		com2_send_buf[43] = g_Light_Time_Info.group[g_lamp_working_time_index[0]]%256;
		com2_send_buf[44] = g_Light_Time_Info.loop[g_lamp_working_time_index[0]]/256;
		com2_send_buf[45] = g_Light_Time_Info.loop[g_lamp_working_time_index[0]]%256;
		for(i = 0;i < 13;i++)
		{
			com2_send_buf[46+i] = 0;
		}		
		com2_send_buf[59] = g_Light_Time_Info.ratio[g_lamp_working_time_index[0]][2];
		com2_send_buf[60] = g_Light_Time_Info.ratio[g_lamp_working_time_index[0]][1];
		com2_send_buf[61] = g_Light_Time_Info.ratio[g_lamp_working_time_index[0]][0];
	}
	if(g_lamp_working_time_num > 1)
	{
		com2_send_buf[62] = g_Light_Time_Info.s_time[g_lamp_working_time_index[1]]/256;		
		com2_send_buf[63] = g_Light_Time_Info.s_time[g_lamp_working_time_index[1]]%256;
		com2_send_buf[64] = g_Light_Time_Info.e_time[g_lamp_working_time_index[1]]/256;
		com2_send_buf[65] = g_Light_Time_Info.e_time[g_lamp_working_time_index[1]]%256;
		com2_send_buf[66] = g_Light_Time_Info.group[g_lamp_working_time_index[1]]/256;
		com2_send_buf[67] = g_Light_Time_Info.group[g_lamp_working_time_index[1]]%256;
		com2_send_buf[68] = g_Light_Time_Info.loop[g_lamp_working_time_index[1]]/256;
		com2_send_buf[69] = g_Light_Time_Info.loop[g_lamp_working_time_index[1]]%256;
		for(i = 0;i < 13;i++)
		{
			com2_send_buf[70+i] = 0;
		}
		com2_send_buf[83] = g_Light_Time_Info.ratio[g_lamp_working_time_index[1]][2];
		com2_send_buf[84] = g_Light_Time_Info.ratio[g_lamp_working_time_index[1]][1];
		com2_send_buf[85] = g_Light_Time_Info.ratio[g_lamp_working_time_index[1]][0];
	}
	if(g_lamp_working_time_num > 2)
	{
		com2_send_buf[86] = g_Light_Time_Info.s_time[g_lamp_working_time_index[2]]/256;	
		com2_send_buf[87] = g_Light_Time_Info.s_time[g_lamp_working_time_index[2]]%256;
		com2_send_buf[88] = g_Light_Time_Info.e_time[g_lamp_working_time_index[2]]/256;
		com2_send_buf[89] = g_Light_Time_Info.e_time[g_lamp_working_time_index[2]]%256;
		com2_send_buf[90] = g_Light_Time_Info.group[g_lamp_working_time_index[2]]/256;
		com2_send_buf[91] = g_Light_Time_Info.group[g_lamp_working_time_index[2]]%256;
		com2_send_buf[92] = g_Light_Time_Info.loop[g_lamp_working_time_index[2]]/256;
		com2_send_buf[93] = g_Light_Time_Info.loop[g_lamp_working_time_index[2]]%256;
		for(i = 0;i < 13;i++)
		{
			com2_send_buf[94+i] = 0;
		}
		com2_send_buf[107] = g_Light_Time_Info.ratio[g_lamp_working_time_index[2]][2];
		com2_send_buf[108] = g_Light_Time_Info.ratio[g_lamp_working_time_index[2]][1];
		com2_send_buf[109] = g_Light_Time_Info.ratio[g_lamp_working_time_index[2]][0];
	}
	if(g_lamp_working_time_num == 4)
	{
		com2_send_buf[110] = g_Light_Time_Info.s_time[g_lamp_working_time_index[3]]/256;	
		com2_send_buf[111] = g_Light_Time_Info.s_time[g_lamp_working_time_index[3]]%256;
		com2_send_buf[112] = g_Light_Time_Info.e_time[g_lamp_working_time_index[3]]/256;
		com2_send_buf[113] = g_Light_Time_Info.e_time[g_lamp_working_time_index[3]]%256;
		com2_send_buf[114] = g_Light_Time_Info.group[g_lamp_working_time_index[3]]/256;
		com2_send_buf[115] = g_Light_Time_Info.group[g_lamp_working_time_index[3]]%256;
		com2_send_buf[116] = g_Light_Time_Info.loop[g_lamp_working_time_index[3]]/256;
		com2_send_buf[117] = g_Light_Time_Info.loop[g_lamp_working_time_index[3]]%256;
		for(i = 0;i < 13;i++)
		{
			com2_send_buf[118+i] = 0;
		}
		com2_send_buf[131] = g_Light_Time_Info.ratio[g_lamp_working_time_index[3]][2];
		com2_send_buf[132] = g_Light_Time_Info.ratio[g_lamp_working_time_index[3]][1];
		com2_send_buf[133] = g_Light_Time_Info.ratio[g_lamp_working_time_index[3]][0];
	}
	
	m_sum = checksum((unsigned char *)(&com2_send_buf[27]),107);
	com2_send_buf[134] = m_sum;
	com2_send_buf[135] = 0x16;

	m_sum = checksum((unsigned char *)(&com2_send_buf[3]),133);
	com2_send_buf[136] = m_sum;
	com2_send_buf[137] = 0x16;
	Add_bpd_to_PLC_RingBuf(1, com2_send_buf, 138,PLC_MSG_TYPE_14);
//	COM2_Send_BDP(com2_send_buf,138);
}

//广播设置工作模式报文
void send_working_mode_bdp(void)
{
	unsigned char m_sum = 0;
	memset((char *)com2_send_buf,0,64);

	g_plc_msg_num++;
	
	com2_send_buf[0] = 0x68;
	com2_send_buf[1] = 0x2B;
	com2_send_buf[2] = 0x00;
	com2_send_buf[3] = 0x41;
	com2_send_buf[4] = 0x04;
	com2_send_buf[5] = 0x00;
	com2_send_buf[6] = 0x5F;				//预计应答字节数
	com2_send_buf[7] = 0x00;
	com2_send_buf[8] = 0x00;
	com2_send_buf[9] = g_plc_msg_num;		//报文序列号
	com2_send_buf[10] = g_router_addr[0];
	com2_send_buf[11] = g_router_addr[1];
	com2_send_buf[12] = g_router_addr[2];
	com2_send_buf[13] = g_router_addr[3];
	com2_send_buf[14] = g_router_addr[4];
	com2_send_buf[15] = g_router_addr[5];	
	com2_send_buf[16] = 0x99;
	com2_send_buf[17] = 0x99;
	com2_send_buf[18] = 0x99;
	com2_send_buf[19] = 0x99;
	com2_send_buf[20] = 0x99;
	com2_send_buf[21] = 0x99;
	com2_send_buf[22] = 0x05;
	com2_send_buf[23] = 0x04;
	com2_send_buf[24] = 0x00;	
	com2_send_buf[25] = 0x02;
	com2_send_buf[26] = 0x0E;
	
	com2_send_buf[27] = 0x68;
	com2_send_buf[28] = 0x99;
	com2_send_buf[29] = 0x99;
	com2_send_buf[30] = 0x99;
	com2_send_buf[31] = 0x99;
	com2_send_buf[32] = 0x99;
	com2_send_buf[33] = 0x99;	
	com2_send_buf[34] = 0x68;	
	com2_send_buf[35] = 0x08;
	com2_send_buf[36] = 0x02;
	com2_send_buf[37] = 0x06;		//表示为设置工作模式命令
	
	com2_send_buf[38] = g_lamp_working_mode;	
	
	m_sum = checksum((unsigned char *)(&com2_send_buf[27]),12);
	com2_send_buf[39] = m_sum;
	com2_send_buf[40] = 0x16;

	m_sum = checksum((unsigned char *)(&com2_send_buf[3]),38);
	com2_send_buf[41] = m_sum;
	com2_send_buf[42] = 0x16;
	Add_bpd_to_PLC_RingBuf(1, com2_send_buf, 43,PLC_MSG_TYPE_15);
//	COM2_Send_BDP(com2_send_buf,43);
}

//广播设置单灯回路顺序
void send_lamp_loop_order_bdp(unsigned char *contents)
{
	unsigned char m_sum = 0;
	memset((char *)com2_send_buf,0,64);

	g_plc_msg_num++;
	
	com2_send_buf[0] = 0x68;
	com2_send_buf[1] = 0x33;
	com2_send_buf[2] = 0x00;
	com2_send_buf[3] = 0x41;
	com2_send_buf[4] = 0x04;
	com2_send_buf[5] = 0x00;
	com2_send_buf[6] = 0x5F;				//预计应答字节数
	com2_send_buf[7] = 0x00;
	com2_send_buf[8] = 0x00;
	com2_send_buf[9] = g_plc_msg_num;		//报文序列号
	com2_send_buf[10] = g_router_addr[0];
	com2_send_buf[11] = g_router_addr[1];
	com2_send_buf[12] = g_router_addr[2];
	com2_send_buf[13] = g_router_addr[3];
	com2_send_buf[14] = g_router_addr[4];
	com2_send_buf[15] = g_router_addr[5];	
	com2_send_buf[16] = 0x99;
	com2_send_buf[17] = 0x99;
	com2_send_buf[18] = 0x99;
	com2_send_buf[19] = 0x99;
	com2_send_buf[20] = 0x99;
	com2_send_buf[21] = 0x99;
	com2_send_buf[22] = 0x05;
	com2_send_buf[23] = 0x04;
	com2_send_buf[24] = 0x00;	
	com2_send_buf[25] = 0x02;
	com2_send_buf[26] = 0x16;
	
	com2_send_buf[27] = 0x68;
	com2_send_buf[28] = 0x99;
	com2_send_buf[29] = 0x99;
	com2_send_buf[30] = 0x99;
	com2_send_buf[31] = 0x99;
	com2_send_buf[32] = 0x99;
	com2_send_buf[33] = 0x99;	
	com2_send_buf[34] = 0x68;	
	com2_send_buf[35] = 0x08;
	com2_send_buf[36] = 0x0A;
	com2_send_buf[37] = 0x07;		//表示为设置单灯回路顺序命令
	
	memcpy(&com2_send_buf[38],contents,9);
	
	m_sum = checksum((unsigned char *)(&com2_send_buf[27]),20);
	com2_send_buf[47] = m_sum;
	com2_send_buf[48] = 0x16;

	m_sum = checksum((unsigned char *)(&com2_send_buf[3]),46);
	com2_send_buf[49] = m_sum;
	com2_send_buf[50] = 0x16;
	Add_bpd_to_PLC_RingBuf(1, com2_send_buf, 51,PLC_MSG_TYPE_16);
//	COM2_Send_BDP(com2_send_buf,51);
}

//抄读数据处理函数
void data_handle_func(unsigned char *pbuf)
{
	unsigned short i = 0,j = 0,m = 0;
	unsigned char fault = 0;	//前一次故障位 
	unsigned char online = 0;	//前一次在线状态
	unsigned char addr[6] = {0};
//	unsigned char m_show_addr[12] = {0};
    unsigned char m_send_para_flag = 0;	//上报单灯参数标志
	
	
 	BCDaddr_to_HEXaddr(pbuf,addr);
// 	for(i = 0; i < 6; i++)
// 	{
// 	   m_show_addr[2*i] = i_to_a((addr[i] & 0xf0)/16);
// 	   m_show_addr[2*i + 1] = i_to_a((addr[i] & 0x0F));
// 	}
	
	for(i = 0;i < DAMP_MAX_NUM;i++)
	{
		if(((g_Damp_Info[i].SN[0] == addr[0])&&(g_Damp_Info[i].SN[1] == addr[1])&&(g_Damp_Info[i].SN[2] == addr[2])\
			&&(g_Damp_Info[i].SN[3] == addr[3])&&(g_Damp_Info[i].SN[4] == addr[4])&&(g_Damp_Info[i].SN[5] == addr[5])))
		{	
			//保存上次抄读数据作判断用
			fault = g_Damp_Info[i].fault_flag;
			online = g_Damp_Info[i].m_bit_status.lamp_online_flag;
			
			//判断是否上报单灯上线
			if( g_Damp_Info[i].m_bit_status.lamp_online_flag != LAMP_STATE_ONLINE	//上次不在线
				|| (g_Damp_Info[i].m_bit_status.send_online_bdp_flag == 0x00))		//未上报过
			{
				//只有E3000在线的前提下才上报
				if(g_GPRS_connect_OK == 1)
			   	{
					send_lamp_online_bdp(i);
					g_Damp_Info[i].m_bit_status.send_online_bdp_flag = 1;
					m_send_para_flag = 1;	//重新上线后，上报参数
					
// 					memcpy((char *)g_print_buf,(char *)m_show_addr,8);
// 					memcpy((char *)&g_print_buf[8],"  单灯上线",strlen("  单灯上线"));
// 					show_text(DGUS_STATE_SHOW_ADDR,27,g_print_buf,64);
				}
			}
			
			g_Damp_Info[i].m_bit_status.read_flag = 0x02;	//标记抄读成功
			g_Damp_Info[i].m_bit_status.lamp_online_flag = LAMP_STATE_ONLINE; //在线			
			
			//数据转换ASCLL---->HEX
			for(j = 0; j < pbuf[8]; j++)	
			{
				pbuf[9+j] = pbuf[9+j] - 0x33;  //抄读数据-33H处理
			}		
			
			/////////////////////////////////////////////////////////////////////////
			//判断单灯组别与本地数据是否相同
            if( pbuf[13] != g_Damp_Info[i].m_bit_status.group )
			{
				m_send_para_flag = 1;	//单灯组别与本地数据不同则上报服务器
			}
			//判断单灯工作模式与本地数据是否相同
			if( pbuf[14] != g_lamp_working_mode )
			{
				send_working_mode_bdp();		//同步单灯工作模式
			}
			/////////////////////////////////////////////////////////////////////////
            g_Damp_Info[i].fault_flag = pbuf[22];
			
			//单灯故障(包含离线)状态检测
			if(g_Damp_Info[i].fault_flag != 0)  //当前有故障  
			{
				if( (fault == 0)&&(online == LAMP_STATE_ONLINE) )	//之前没有故障且在线
				{
					g_damp_fault_num++;		//单灯故障数量+1
				}		
			}
			else								//当前无故障  
			{			
				if( (fault != 0)||(online == LAMP_STATE_OFFLINE))	//之前有故障或离线
				{
					if(g_damp_fault_num > 0)
					{
						g_damp_fault_num--;		//单灯故障数量-1
					}
				}
			}
						
			//////////////////////////////////////////////////////////////////////////////////
			//漏电状态变化上报
			if( (g_Damp_Info[i].fault_flag & 0x01) == 0x01)	//现在有漏电
			{
				if( ((fault & 0x01) == 0)	//之前没有漏电,
					|| (g_Damp_Info[i].m_bit_status.send_status_bdp_flag == 0) )				//还未向服务器上报
				{
					//只有E3000在线的前提下才上报
					if(g_GPRS_connect_OK == 1)
					{
						send_lamp_fault_bdp(i,&pbuf[22]);		//上报漏电故障
						g_Damp_Info[i].m_bit_status.send_status_bdp_flag = 1;
						
// 						memcpy((char *)g_print_buf,(char *)m_show_addr,8);
// 						memcpy((char *)&g_print_buf[8],"  漏电上报",strlen("  漏电上报"));
// 						show_text(DGUS_STATE_SHOW_ADDR,27,g_print_buf,64);
					}
				}
			}
						
			if( (g_Damp_Info[i].fault_flag & 0x01) == 0x00)	//现在无漏电
			{	
				if( ((fault&0x01) == 0x01)	//之前漏电 
					|| (g_Damp_Info[i].m_bit_status.send_status_bdp_flag == 0) )				//还未向服务器上报
				{
					//只有E3000在线的前提下才上报
					if(g_GPRS_connect_OK == 1)
					{
						send_lamp_fault_release_bdp(i,0x01);	//解除漏电故障
						g_Damp_Info[i].m_bit_status.send_status_bdp_flag = 1;
						
// 						memcpy((char *)g_print_buf,(char *)m_show_addr,8);
// 						memcpy((char *)&g_print_buf[8],"  解除漏电",strlen("  解除漏电"));
// 						show_text(DGUS_STATE_SHOW_ADDR,27,g_print_buf,64);
					}
				}
			}
					
			//////////////////////////////////////////////////////////////////////////////////			
			//保存抄读数据		
			g_Damp_Info[i].voltage = pbuf[29]*256+pbuf[30];
			g_Damp_Info[i].loop = pbuf[31]*256+pbuf[32];
			
			for(j = 0;j < 3;j++)
			{
				if( ((g_Damp_Info[i].loop>>j) & 0x01) == 1)
				{
					g_Damp_Info[i].ratio[j] = pbuf[33+7*m];
					if(g_Damp_Info[i].ratio[j] == 0)
					{
						g_Damp_Info[i].ratio[j] = 0xFF;
					}
					g_Damp_Info[i].current[j]        = pbuf[34+7*m]*256+pbuf[35+7*m];
					g_Damp_Info[i].active_power[j]   = pbuf[36+7*m]*256+pbuf[37+7*m];
					g_Damp_Info[i].reactive_power[j] = pbuf[38+7*m]*256+pbuf[39+7*m];
					m++;
				}
			}
			
			////////////////////////////////////////////////////////////////////////////
			//单灯参数上报
			//判断是否电流变动上传单灯参数			
			if(pbuf[21] == 0x01)
			{
				m_send_para_flag = 1;
			}
			
			//判断是否是第一次向服务器发送
			if(g_Damp_Info[i].m_bit_status.send_para_bdp_flag == 0)  //还没有向服务器上报
			{
				m_send_para_flag = 1;
			}
			
			if(m_send_para_flag == 1)
			{
				//只有E3000在线的前提下才上报
			   if(g_GPRS_connect_OK == 1)
			   	{
				   send_single_lamp_para_bdp(i);
				   g_Damp_Info[i].m_bit_status.send_para_bdp_flag = 1;
					
// 	 			   memcpy((char *)g_print_buf,(char *)m_show_addr,8);			   
// 	 			   memcpy((char *)&g_print_buf[8],"  单灯数据上报",strlen("  单灯数据上报"));
// 	 			   show_text(DGUS_STATE_SHOW_ADDR,27,g_print_buf,64);
			   	}
			}
			break;
		}
	}
}

//节点抄读失败判断函数
void read_fail_judge_func(unsigned char *last_addr)
{
	unsigned short i = 0,j = 0;
	
//	unsigned char m_show_addr[12] = {0};
	
	if(last_addr[0] != 0xFA)	//地址不合法
	{
		return;
	}
		
// 	for(i = 0; i < 6; i++)
// 	{
// 	   m_show_addr[2*i] = i_to_a((last_addr[i] & 0xf0)/16);
// 	   m_show_addr[2*i + 1] = i_to_a((last_addr[i] & 0x0F));
// 	}
	
	//判断上一节点是否抄读失败
	for(i = 0;i < DAMP_MAX_NUM;i++)
	{
		if((g_Damp_Info[i].SN[0] == last_addr[0])&&(g_Damp_Info[i].SN[1] == last_addr[1])&&(g_Damp_Info[i].SN[2] == last_addr[2])\
			&&(g_Damp_Info[i].SN[3] == last_addr[3])&&(g_Damp_Info[i].SN[4] == last_addr[4])&&(g_Damp_Info[i].SN[5] == last_addr[5]))
		{				
			if( g_Damp_Info[i].m_bit_status.read_flag == 0x01 )	//节点抄读失败
			{	
				if( (g_Damp_Info[i].fault_flag == 0x00)										//之前没有故障
					&&(g_Damp_Info[i].m_bit_status.lamp_online_flag != LAMP_STATE_OFFLINE))	//之前不是离线
				{
					g_damp_fault_num++;		//单灯故障数量+1
				}
				
				if((g_Damp_Info[i].m_bit_status.lamp_online_flag != LAMP_STATE_OFFLINE)	//之前不是离线状态
					|| (g_Damp_Info[i].m_bit_status.send_online_bdp_flag == 0x00) )		//未上报过
				{					
					//只有E3000在线的前提下才上报
					if(g_GPRS_connect_OK == 1)
					{
						send_lamp_offline_bdp(i);	//上报单灯下线
						g_Damp_Info[i].m_bit_status.send_online_bdp_flag = 1;
						
// 						memcpy((char *)g_print_buf,(char *)m_show_addr,8);
// 						memcpy((char *)&g_print_buf[8],"  单灯下线",strlen("  单灯下线"));
// 						show_text(DGUS_STATE_SHOW_ADDR,27,g_print_buf,64);
					}				
				}
				g_Damp_Info[i].m_bit_status.lamp_online_flag = LAMP_STATE_OFFLINE; //离线
				
				//参数清零
				g_Damp_Info[i].voltage = 0;
				for(j = 0;j < 3;j++)
				{
					g_Damp_Info[i].ratio[j] = 0;
					g_Damp_Info[i].current[j] = 0;
					g_Damp_Info[i].active_power[j] = 0;
					g_Damp_Info[i].reactive_power[j] = 0;
				}
			}
			break;
		}
	}
		
}

