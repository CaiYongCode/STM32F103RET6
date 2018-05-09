#include "ExtModule.h"
#include "BIT.h"
#include "struct.h"
#include "IOctl.h"
#include "dacai_touch.h"
#include "GPRS.h"
////////////////////////////////////////////////////////////
//相应外部扩展模块回告报文 
unsigned char m_flag[3] = {1,1,1};

void ActOn_ExtModule_BDP(unsigned char * pbuf,unsigned char len)
{
	unsigned char m_addr = 0;
	unsigned char i = 0;
	m_addr = pbuf[1];
	if((pbuf[0]== 0x7a) && (pbuf[2]== 31) && (pbuf[3] == 0x11))
	{
		if((m_addr > 0) &&( m_addr < 4))
		{
			g_ext_offline_counter[m_addr - 1] = 0;
			m_flag[m_addr - 1] = ONLINE;
			for(i = 0; i < 12; i++)
			{
				g_Concentrator_Para.ext_current[m_addr - 1][i] = pbuf[4 + 2*i]*256 + pbuf[4 + 2*i + 1];
			}
// 			if(pbuf[28] != 0)	//扩展模块电流变动，上报集中器参数
// 			{
// 			}
		}
	}
}


////////////////////////////////////////////////////////////////////
unsigned short m_send_counter = 0;
unsigned char m_send_ext_buf[16];
unsigned char m_send_rst_8302_counter = 0;
void Send_CMD_bdp_to_ExtModule()
{
	unsigned char m_val = 0;
	
	if(g_reset_ext_8302_flag == 1)
	{
		m_send_rst_8302_counter++;		
	}
	if(m_send_rst_8302_counter >= 4)
	{
		m_send_rst_8302_counter = 0;
		g_reset_ext_8302_flag = 0;
	}
	
	m_val = m_send_counter % 3;
    m_send_counter ++;
    m_send_ext_buf[0] = 0x7A;
	m_send_ext_buf[1] = m_val + 1;  //地址
	m_send_ext_buf[2] = 10;
	m_send_ext_buf[3] = 0x21;
	switch(m_val)
		{
		   case 0:
			   	m_send_ext_buf[4] = g_extmodule_relay[0][0];
			    m_send_ext_buf[5] = g_extmodule_relay[0][1];
				m_send_ext_buf[6] = g_extmodule_relay[0][2];
				m_send_ext_buf[7] = g_extmodule_relay[0][3];
				
		   	break;
		   case 1:
			   	m_send_ext_buf[4] = g_extmodule_relay[1][0];
			    m_send_ext_buf[5] = g_extmodule_relay[1][1];
				m_send_ext_buf[6] = g_extmodule_relay[1][2];
				m_send_ext_buf[7] = g_extmodule_relay[1][3];
		   	break;
           case 2:
			   	m_send_ext_buf[4] = g_extmodule_relay[2][0];
			    m_send_ext_buf[5] = g_extmodule_relay[2][1];
				m_send_ext_buf[6] = g_extmodule_relay[2][2];
				m_send_ext_buf[7] = g_extmodule_relay[2][3];
		   	break;
		   default:
		   	break;
		}
	m_send_ext_buf[8] = g_reset_ext_8302_flag;
	m_send_ext_buf[9] = 0xA7;
	COM3_Send_BDP(m_send_ext_buf,10);
}


void Ext_Module_online_judge()
{
	
	unsigned char i = 0;

	g_ext_offline_counter[0]++;
	g_ext_offline_counter[1]++;
	g_ext_offline_counter[2]++;
	//离线判断
	if(g_ext_offline_counter[0] > 10)
	{
		g_ext_offline_counter[0] = 10;
		m_flag[0] = OFFLINE;
	}
	if(g_ext_offline_counter[1] > 10)
	{
		g_ext_offline_counter[1] = 10;
		m_flag[1] = OFFLINE;
	}
	if(g_ext_offline_counter[2] > 10)
	{
		g_ext_offline_counter[2] = 10;
		m_flag[2] = OFFLINE;
	}
	//发送最新状态到显示屏
	if(m_flag[0] != g_ext_online_flag[0]) 
	{
		show_ext_status(0,1-m_flag[0]);
		if(g_Concentrator_Para.ext_enable[0] != 0xFF)		//扩展模块启用
		{
			if(OFFLINE == m_flag[0])	//上报扩展模块1通信失败
			{
				send_concentrator_fault_bdp(CONCENTRATOR_FAULT_2,1);
				for(i = 0;i < 12;i++)	//扩展模块1电流清零
				{
					g_Concentrator_Para.ext_current[0][i] = 0;
				}
			}
			else if(ONLINE == m_flag[0])	//解除扩展模块1通信失败
			{
				send_concentrator_fault_release_bdp(CONCENTRATOR_FAULT_2,1);
			}
		}
		g_ext_online_flag[0] = m_flag[0];
		
	}
	if(m_flag[1] != g_ext_online_flag[1])
	{
		show_ext_status(1,1-m_flag[1]);
		if(g_Concentrator_Para.ext_enable[1] != 0xFF)		//扩展模块启用
		{
			if(OFFLINE == m_flag[1])	//上报扩展模块2通信失败
			{
				send_concentrator_fault_bdp(CONCENTRATOR_FAULT_2,2);
				for(i = 0;i < 12;i++)	//扩展模块2电流清零
				{
					g_Concentrator_Para.ext_current[1][i] = 0;
				}
			}
			else if(ONLINE == m_flag[1])	//解除扩展模块2通信失败
			{
				send_concentrator_fault_release_bdp(CONCENTRATOR_FAULT_2,2);			
			}
		}
		g_ext_online_flag[1] = m_flag[1];
	}
	if(m_flag[2] != g_ext_online_flag[2])
	{
		show_ext_status(2,1-m_flag[2]);
		if(g_Concentrator_Para.ext_enable[2] != 0xFF)		//扩展模块启用
		{
			if(OFFLINE == m_flag[2])	//上报扩展模块3通信失败
			{
				send_concentrator_fault_bdp(CONCENTRATOR_FAULT_2,3);
				for(i = 0;i < 12;i++)	//扩展模块3电流清零
				{
					g_Concentrator_Para.ext_current[2][i] = 0;
				}
			}
			else if(ONLINE == m_flag[2])	//解除扩展模块3通信失败
			{
				send_concentrator_fault_release_bdp(CONCENTRATOR_FAULT_2,3);
			}
		}
		g_ext_online_flag[2] = m_flag[2];
	}	
}
//验证扩展模块是否启用
void Check_ext_enable(unsigned char num)
{
	unsigned char i = 0;
	
	g_Concentrator_Para.ext_enable[num] = 0xFF;	//默认未启用
	for(i = 0;i < 4;i++)
	{
		if(g_Concentrator_Para.ext_relay_enable[num][i] != 0xFF)	//任1回路继电器启用
		{
			g_Concentrator_Para.ext_enable[num] = 0;	//扩展模块启用
			break;
		}
	}
	for(i = 0;i < 12;i++)
	{
		if(g_Concentrator_Para.ext_phase_enable[num][i] != 0xFF)	//任1相位电流检测启用
		{
			g_Concentrator_Para.ext_enable[num] = 0;	//扩展模块启用
			break;
		}
	}

}

//更新扩展状态
void Update_Ext_Status(unsigned char num)
{
	
	if( g_Concentrator_Para.ext_enable[num] == 0xFF)	//扩展模块3未启用
	{				
		show_ext_status(num,2);
	}
	else	//扩展模块3启用
	{				
		if(g_ext_online_flag[num] == ONLINE)
		{
			show_ext_status(num,0);	//显示在线
		}
		else if(g_ext_online_flag[num] == OFFLINE)
		{
			show_ext_status(num,1);	//显示离线
		}
	}
}
