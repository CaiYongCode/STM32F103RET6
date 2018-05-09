#include "Lamp.h"
#include "BIT.h"
#include "struct.h"
#include "IOctl.h"
#include "dacai_touch.h"
#include "GPRS.h"
#include "PLC.h"

//由集中器SN转换为路由地址
void Get_Router_Addr(void)
{
	unsigned char buf[4] = {0};
	
    
    buf[0] = a_to_i(g_E3000_Info.SN[0])*16 + a_to_i(g_E3000_Info.SN[1]);
 	buf[1] = a_to_i(g_E3000_Info.SN[2])*16 + a_to_i(g_E3000_Info.SN[3]);
 	buf[2] = a_to_i(g_E3000_Info.SN[4])*16 + a_to_i(g_E3000_Info.SN[5]);
 	buf[3] = a_to_i(g_E3000_Info.SN[6])*16 + a_to_i(g_E3000_Info.SN[7]);
	
	HEXaddr_to_BCDaddr(buf,g_router_addr);	
}

////////////////////////////////////////////////////////////
//增加一个节点 
unsigned char Add_One_Lamp_Info(unsigned char * pbuf)
{
   unsigned char ret = ADD_LAMP_ADDR_SUCESS;
   if( (pbuf[0] != 0xFA)||((pbuf[1]&0xF0) != 0x30) )	//无效地址
   	{
   	   ret = INVALID_LAMP_ADDR;
   	}
   if(g_lamp_total_num >= DAMP_MAX_NUM)
   	{
   	   ret = LAMP_ADDR_FULL;
   	}
   return ret;
   
}

////////////////////////////////////////////////////////////
//删除一个节点 
unsigned char Del_One_Lamp_Info(unsigned char * pbuf)
{
   unsigned char ret = DEL_LAMP_ADDR_SUCESS;
   if( (pbuf[0] != 0xFA)||((pbuf[1]&0xF0) != 0x30) )	//无效地址
   	{
   	   ret = INVALID_LAMP_ADDR;
   	}
   return ret;
   
}

////////////////////////////////////////////////////////////
//清空所有单灯节点信息
void Clear_All_Lamp_Info(void)
{
    unsigned short i = 0;

	for(i = 0;i < DAMP_MAX_NUM;i++)
	{
	    //已存储
		if(g_Damp_Info[i].m_bit_status.status == 1)
		{
		   memset((char *)&g_Damp_Info[i],0,sizeof(Damp_Info));
		}
	}
	//下发路由 参数区初始化 报文
	send_parameter_init_bdp();
	//总数清零
	g_lamp_total_num = 0;
	//故障数清零
	g_damp_fault_num = 0;
	show_text(DGUS_DAMP_READ_SHOW_ADDR,9,NULL,0);
	//写 FLASH
	Write_Damp_Info();

			
}

void add_damp_addr(unsigned char *pbuf)
{
	unsigned short i = 0,j = 0;
	
	for(i = 0;i < DAMP_MAX_NUM;i++)
	{
		if((g_Damp_Info[i].SN[0] == pbuf[0])&&(g_Damp_Info[i].SN[1] == pbuf[1])\
			&&(g_Damp_Info[i].SN[2] == pbuf[2])&&(g_Damp_Info[i].SN[3] == pbuf[3]))
		{
			return;
		}
	}
	
	for(i = 0; i < DAMP_MAX_NUM ; i++)	
	{
		if(g_Damp_Info[i].m_bit_status.status != 1)
		{
			g_Damp_Info[i].m_bit_status.status = 1;
			for(j = 0;j < 4;j++)
			{
				g_Damp_Info[i].SN[j] = pbuf[j];
			}	
			g_lamp_total_num++;	
			Write_Damp_Info();
			break;
		}
	}
}

void del_damp_addr(unsigned char *pbuf)
{
	unsigned short i = 0;
	
	for(i = 0;i < DAMP_MAX_NUM;i++)
	{
		if((g_Damp_Info[i].SN[0] == pbuf[0])&&(g_Damp_Info[i].SN[1] == pbuf[1])\
			&&(g_Damp_Info[i].SN[2] == pbuf[2])&&(g_Damp_Info[i].SN[3] == pbuf[3]))
		{
			//单灯总数减1
			g_lamp_total_num--;
			//如果单灯故障或离线，单灯故障数量减1
			if((g_Damp_Info[i].fault_flag != 0)||(g_Damp_Info[i].m_bit_status.lamp_online_flag == LAMP_STATE_OFFLINE))
			{
				g_damp_fault_num--;
			}
			memset((char *)&g_Damp_Info[i],0,sizeof(Damp_Info));
			Write_Damp_Info();			
			break;
		}
	}	
}


//清空单灯抄读标志
void clear_lamp_read_flag(void)
{
	unsigned short i = 0;
	
	g_lamp_read_num = 0;
	for(i = 0;i < DAMP_MAX_NUM;i++)
	{
		if(g_Damp_Info[i].m_bit_status.status == 1)
		{
			g_Damp_Info[i].m_bit_status.read_flag = 0;
		}
	}
}


//检查集中器的从节点档案是否为空
unsigned char check_damp_file(void)
{
	unsigned short i = 0;
	for(i = 0;i < DAMP_MAX_NUM;i++)
	{
		if(g_Damp_Info[i].m_bit_status.status == 1)
			return 1;
	}
	show_text(DGUS_PROMPT_SHOW_ADDR,9,"节点档案为空",strlen("节点档案为空"));
	Icon_Show(DGUS_PROMPT_SHOW_ADDR,3,2);
	Change_Page(DGUS_PROMPT_SHOW_ADDR);
	return 0;
}



unsigned short m_lamp_total_num = 0;
unsigned short m_lamp_fault_num = 0;
void check_lamp_status(void)
{
	unsigned short i = 0;
	unsigned char m_swicth = 0;
	
	if(g_lamp_total_num == 0)
	{
		g_damp_fault_num = 0;
		show_text(DGUS_STATE_SHOW_ADDR,26,NULL,0);	//清空抄读进度显示
		show_text(DGUS_STATE_SHOW_ADDR,27,NULL,0);	//清空单灯上报显示
	}
	
	if(g_damp_fault_num > g_lamp_total_num)
	{
		g_damp_fault_num = g_lamp_total_num;
	}
	
	//界面显示单灯统计信息
	if(m_lamp_total_num != g_lamp_total_num)
	{
		show_lamp_statistics();
		m_lamp_total_num = g_lamp_total_num;
	}
	if(m_lamp_fault_num != g_damp_fault_num)
	{
		show_lamp_statistics();
		m_lamp_fault_num = g_damp_fault_num;
	}
	
	//面板LED显示单灯亮灯和故障状态
	for(i = 0;i < DAMP_MAX_NUM;i++)
	{
		if(g_Damp_Info[i].m_bit_status.status == 1)
		{
			if(g_Damp_Info[i].m_bit_status.switch_flag == 1)
			{
				m_swicth = OPEN;
				break;
			}
		}
	}
	
	Set_BRIGNT_LED_Status(m_swicth);
	
	if(0 < g_damp_fault_num)
	{		
		Set_NORMAL_LED_Status(CLOSE);
		Set_FAULT_LED_Status(OPEN);
	}
	else
	{
		Set_NORMAL_LED_Status(OPEN);
		Set_FAULT_LED_Status(CLOSE);
	}
}


//////////////////////////////////////////////////////////////////
