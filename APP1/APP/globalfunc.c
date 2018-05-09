#include "globalfunc.h"
#include "globaldata.h"
#include "dacai_touch.h"
#include "GPRS.h"
#include "PLC.h"
#include "IOctl.h"

void SendMessage(unsigned short msg_type,unsigned int msg_len,int msg_addr)
{
	R_msg_Array[msg_index%QSIZE].msg_type = msg_type;    //消息类型
	R_msg_Array[msg_index%QSIZE].msg_len  =  msg_len;    //报文长度
	R_msg_Array[msg_index%QSIZE].msg_addr = msg_addr;    //报文数据首地址
													
	OSQPost(QMsg,&R_msg_Array[msg_index%QSIZE]);
	msg_index++;
}

void Enable_RS485_Send(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_3);
}

void Enable_RS485_Recv(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_3);
}


void Enable_RS485_1_Send(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_15);
}

void Enable_RS485_1_Recv(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_15);
}

void uart_send_char(unsigned char * str)
{
	while(*str != '\0')
	{
		USART_SendData(USART1,*str);  
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
		{
		}
		str++;
	} 
}

void gprs_data_send(unsigned char * pbuf,unsigned short len)
{
	unsigned short i = 0;
	memcpy((char *)com1_send_buf,(char *)pbuf,len); // 
	com1_send_len = len;
	
	for(i = 0; i < len; i++)
	{
		USART_SendData(USART1,pbuf[i]); 
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
		{
		}
	}
}


void COM1_Send_BDP(unsigned char * buf,unsigned char len)
{
	unsigned char i = 0;
	for(i = 0; i < len; i++)
	{
		USART_SendData(USART1,buf[i]); 
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
		{
		}
	}
}

void COM2_Send_BDP(unsigned char * buf,unsigned char len)
{	
	unsigned char i = 0;
	
	for(i = 0; i < len; i++)
	{
		USART_SendData(USART2,buf[i]); 
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
		{
		}
	}
	plc_comm_flag = 1;
	
}

void COM3_Send_BDP(unsigned char * buf,unsigned char len)	//JP3
{
	unsigned char i = 0;
	Enable_RS485_1_Send();
	for(i = 0; i < len; i++)
	{
		USART_SendData(USART3,buf[i]); 
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
		{
		}
	}
	OSTimeDly(5);
	Enable_RS485_1_Recv();
}


void COM4_Send_BDP(unsigned char * buf,unsigned char len)	//JP2
{
	unsigned char i = 0;
	Enable_RS485_Send();
	for(i = 0; i < len; i++)
	{
		USART_SendData(UART4,buf[i]); 
		while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET)
		{
		}
	}
	OSTimeDly(5);
	Enable_RS485_Recv();
}

void COM5_Send_BDP(unsigned char * buf,unsigned char len)
{
	unsigned char i = 0;
	for(i = 0; i < len; i++)
	{
		USART_SendData(UART5,buf[i]); 
		while(USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET)
		{
		}
	}
}


unsigned char AsciiToHex(unsigned char asc_hi, unsigned char asc_lo)
{
 	return ((a_to_i(asc_hi)<<4)+a_to_i(asc_lo));
}

unsigned char i_to_a(unsigned char val)
{
	unsigned char value = 0;
	if( (val >= 0) && (val <= 9) )
	{
		value = val + '0';
	}
	else
	{
		value = val + 'A' - 10;
	}
	return value;
}

unsigned char a_to_i(unsigned char val)
{
  /* compute decimal value */
	unsigned char value = 0;
	if(val >= '0' && val <= '9')
		value = val - '0';
	else if(val >= 'A' && val <= 'F')
		value = val - 'A' + 10;
	else if(val >= 'a' && val <= 'f')
		value = val - 'a' + 10;
	else
		value = 0;
  return value;
}

unsigned char h_to_i(unsigned char val)
{
	unsigned char value = 0;
	value = val/16*10+val%16;
	return value;
}

//补码转换
unsigned short complement_conversion(short value)
{
	unsigned short temp = 0;
    if(value >= 0) 
	{
       return value;
	}
    else
    { 
        temp = value;
        return temp;
    }
}

//16进制节点地址转换成BCD码地址
void HEXaddr_to_BCDaddr(unsigned char *hexaddr,unsigned char *bcdaddr)
{
	unsigned char i = 0;
	unsigned int addr = 0;

	for(i = 0; i < 4; i++)
	{
		addr = (addr<<8)|hexaddr[i]; 
	}
	for(i = 5; i > 0; )
	{
		i--;
		bcdaddr[i] = (addr%100)/10*16+(addr%100)%10; 
		addr = addr/100;
	}
}

//BCD码节点地址转换成16进制地址
void BCDaddr_to_HEXaddr(unsigned char *bcdaddr,unsigned char *hexaddr)
{
	unsigned char i = 0;
	unsigned int addr = 0;

	for(i = 0; i < 5; i++)
	{
		addr = addr*100+(bcdaddr[i]/16*10 + bcdaddr[i]%16); 
	}
	for(i = 4; i > 0; )
	{
		i--;
		hexaddr[i] = addr&0xFF; 
		addr = addr>>8;
	}
}


unsigned char checksum(unsigned  char * str,unsigned short len)
{
	unsigned char data = 0;
	unsigned short i = 0;
	for(i = 0; i < len; i++)
	{
		data += str[i];
	}
	return data;
}

//读取单片机ID
void Read_STM32_CHIP_ID(void)
{
	STM32_CHIP_ID[0]  = *(__IO u8*)(0x1FFFF7E8); 
    STM32_CHIP_ID[1]  = *(__IO u8*)(0x1FFFF7E9); 
    STM32_CHIP_ID[2]  = *(__IO u8*)(0x1FFFF7EA); 
    STM32_CHIP_ID[3]  = *(__IO u8*)(0x1FFFF7EB); 
    STM32_CHIP_ID[4]  = *(__IO u8*)(0x1FFFF7EC); 
    STM32_CHIP_ID[5]  = *(__IO u8*)(0x1FFFF7ED); 
    STM32_CHIP_ID[6]  = *(__IO u8*)(0x1FFFF7EE); 
    STM32_CHIP_ID[7]  = *(__IO u8*)(0x1FFFF7EF); 
    STM32_CHIP_ID[8]  = *(__IO u8*)(0x1FFFF7F0); 
    STM32_CHIP_ID[9]  = *(__IO u8*)(0x1FFFF7F1); 
    STM32_CHIP_ID[10] = *(__IO u8*)(0x1FFFF7F2); 
    STM32_CHIP_ID[11] = *(__IO u8*)(0x1FFFF7F3);       	
}

void Read_Flash_RN8302_Config_Info(void)
{
     ReadData_From_Flash(RTU_RN8302_INFO_START_ADDR,sizeof(EffectParSetToSave)/2,(uint16_t *)&G_EffectPar_Info);	
}

void Write_RN8302_Config_to_Flash(unsigned short * buf)
{
    FLASH_HALFWORD_Write(RTU_RN8302_INFO_START_ADDR, buf, sizeof(EffectParSetToSave)/2);
}

void Read_Flash_RTU_Info(void)
{
	unsigned char i = 0;
	g_E3000_Info.config_flag = FLASH_ReadHalfWord(RTU_CONFIG_FLAG_ADDR);
	for(i = 0; i < 8; i++)		//主控端SN
	{
		g_E3000_Info.SN[i] = FLASH_ReadHalfWord(RTU_SN_START_ADDR + 2*i);
	}
	for(i = 0; i < 4; i++)		//软件版本号
	{
		g_E3000_Info.soft_ersion[i] = FLASH_ReadHalfWord(RTU_SOFTWARE_VERSION_START_ADDR + 2*i);
	}
	for(i = 0; i < 14; i++)		//SIM卡号
	{
		g_E3000_Info.simcard_num[i] = FLASH_ReadHalfWord(RTU_SIMCARD_NUM_START_ADDR + 2*i);
	}
	for(i = 0; i < 6; i++)		//路由地址
	{
		g_E3000_Info.route_addr[i] = FLASH_ReadHalfWord(RTU_ROUTE_ADDR_START_ADDR + 2*i);
	}
	g_E3000_Info.ip_set = FLASH_ReadHalfWord(RTU_IP_SET_ADDR_START_ADDR);
	g_E3000_Info.ip_len = FLASH_ReadHalfWord(RTU_IP_LEN_ADDR_START_ADDR);
	for(i = 0;i < 32;i++)
	{
		g_E3000_Info.ip[i] = FLASH_ReadHalfWord(RTU_IP_ADDR_START_ADDR + 2*i);
	}
	g_E3000_Info.port_set = FLASH_ReadHalfWord(RTU_PORT_SET_ADDR_START_ADDR);
	g_E3000_Info.port_len = FLASH_ReadHalfWord(RTU_PORT_LEN_ADDR_START_ADDR);
	for(i = 0;i < 5;i++)
	{
		g_E3000_Info.port[i] = FLASH_ReadHalfWord(RTU_PORT_ADDR_START_ADDR + 2*i);
	}
	g_working_mode = FLASH_ReadHalfWord(RTU_WORKING_MODE_ADDR);
	g_lamp_working_mode = FLASH_ReadHalfWord(DAMP_WORKING_MODE_ADDR);;
}


void Write_RTU_info_to_Flash(void)
{
	unsigned char i = 0;
	unsigned short buf[80] = {0};

	buf[0] = g_E3000_Info.config_flag;
	for(i = 0;i < 8;i++)	//主控端SN
	{
		buf[1+i] = g_E3000_Info.SN[i];
	}
	for(i = 0;i < 4;i++)	//软件版本号
	{
		buf[9+i] = g_E3000_Info.soft_ersion[i];
	}
	for(i = 0;i < 14;i++)	//SIM卡号
	{
		buf[13+i] = g_E3000_Info.simcard_num[i];
	}
	for(i = 0;i < 6;i++)	//路由地址
	{
		buf[27+i] = g_E3000_Info.route_addr[i];
	}
	buf[33] = g_E3000_Info.ip_set;
	buf[34] = g_E3000_Info.ip_len;
	for(i = 0;i < 32;i++)
	{
		buf[35+i] = g_E3000_Info.ip[i];
	}
	buf[67] = g_E3000_Info.port_set;
	buf[68] = g_E3000_Info.port_len;
	for(i = 0;i < 5;i++)
	{
		buf[69+i] = g_E3000_Info.port[i];
	}
	
	buf[74] = g_working_mode;
	buf[75] = g_lamp_working_mode;
	FLASH_HALFWORD_Write(RTU_CONFIG_INFO_START_ADDR,buf,80);
}

//读取周工作时间
void Read_Week_Light_Time_Info(void)
{
	unsigned char j = 0;
	
	if((g_now_week < 1)||(g_now_week > 7))
	{
		return;
	}
	
	for(j = 0;j < 8;j++)
	{
		g_Light_Time_Info.s_time[j]   = FLASH_ReadHalfWord(RTU_WEEK_LIGHT_TIME_START_ADDR+112*(g_now_week-1)+j*14);
		g_Light_Time_Info.e_time[j]   = FLASH_ReadHalfWord(RTU_WEEK_LIGHT_TIME_START_ADDR+112*(g_now_week-1)+j*14+2);
		g_Light_Time_Info.group[j]    = FLASH_ReadHalfWord(RTU_WEEK_LIGHT_TIME_START_ADDR+112*(g_now_week-1)+j*14+4);
		g_Light_Time_Info.loop[j]     = FLASH_ReadHalfWord(RTU_WEEK_LIGHT_TIME_START_ADDR+112*(g_now_week-1)+j*14+6);
		g_Light_Time_Info.ratio[j][0] = FLASH_ReadHalfWord(RTU_WEEK_LIGHT_TIME_START_ADDR+112*(g_now_week-1)+j*14+8);
		g_Light_Time_Info.ratio[j][1] = FLASH_ReadHalfWord(RTU_WEEK_LIGHT_TIME_START_ADDR+112*(g_now_week-1)+j*14+10);
		g_Light_Time_Info.ratio[j][2] = FLASH_ReadHalfWord(RTU_WEEK_LIGHT_TIME_START_ADDR+112*(g_now_week-1)+j*14+12);
		
	}
}
//保存周/月工作数据
unsigned short week_month_buf[16*56] = {0};

//保存周工作时间

void Write_Week_Light_Time_Info(void)
{
	unsigned char i = 0,j = 0;
	
	for(i = 0;i < 7;i++)
	{
		for(j = 0;j < 8;j++)
		{
			week_month_buf[0+7*j+i*56] = g_Week_Light_Time_Info[i].s_time[j];
			week_month_buf[1+7*j+i*56] = g_Week_Light_Time_Info[i].e_time[j];
			week_month_buf[2+7*j+i*56] = g_Week_Light_Time_Info[i].group[j];
			week_month_buf[3+7*j+i*56] = g_Week_Light_Time_Info[i].loop[j];
			week_month_buf[4+7*j+i*56] = g_Week_Light_Time_Info[i].ratio[j][0];
			week_month_buf[5+7*j+i*56] = g_Week_Light_Time_Info[i].ratio[j][1];
			week_month_buf[6+7*j+i*56] = g_Week_Light_Time_Info[i].ratio[j][2];
		}
	}	
	FLASH_HALFWORD_Write(RTU_WEEK_LIGHT_TIME_START_ADDR,week_month_buf,7*56);	
}

//读取月工作数据
void Read_Month_Light_Time_Info(void)
{
	unsigned char j = 0;
	
	if((g_now_day < 1)||(g_now_day > 31)||(g_now_month < 1)||(g_now_month > 12))
	{
		return;
	}
	//前15天
	if(g_now_day <= 15)
	{
		for(j = 0;j < 8;j++)
		{
			g_Light_Time_Info.s_time[j]   = FLASH_ReadHalfWord(RTU_MONTH_LIGHT_TIME_START_ADDR+4096*(g_now_month-1)+112*(g_now_day-1)+j*14);
			g_Light_Time_Info.e_time[j]   = FLASH_ReadHalfWord(RTU_MONTH_LIGHT_TIME_START_ADDR+4096*(g_now_month-1)+112*(g_now_day-1)+j*14+2);
			g_Light_Time_Info.group[j]    = FLASH_ReadHalfWord(RTU_MONTH_LIGHT_TIME_START_ADDR+4096*(g_now_month-1)+112*(g_now_day-1)+j*14+4);
			g_Light_Time_Info.loop[j]     = FLASH_ReadHalfWord(RTU_MONTH_LIGHT_TIME_START_ADDR+4096*(g_now_month-1)+112*(g_now_day-1)+j*14+6);
			g_Light_Time_Info.ratio[j][0] = FLASH_ReadHalfWord(RTU_MONTH_LIGHT_TIME_START_ADDR+4096*(g_now_month-1)+112*(g_now_day-1)+j*14+8);
			g_Light_Time_Info.ratio[j][1] = FLASH_ReadHalfWord(RTU_MONTH_LIGHT_TIME_START_ADDR+4096*(g_now_month-1)+112*(g_now_day-1)+j*14+10);
			g_Light_Time_Info.ratio[j][2] = FLASH_ReadHalfWord(RTU_MONTH_LIGHT_TIME_START_ADDR+4096*(g_now_month-1)+112*(g_now_day-1)+j*14+12);
		
		}
	}
	else	//后15天
	{
		for(j = 0;j < 8;j++)
		{		
			g_Light_Time_Info.s_time[j]   = FLASH_ReadHalfWord(RTU_MONTH_LIGHT_TIME_START_ADDR+4096*(g_now_month-1)+2048+112*(g_now_day-15-1)+j*14);
			g_Light_Time_Info.e_time[j]   = FLASH_ReadHalfWord(RTU_MONTH_LIGHT_TIME_START_ADDR+4096*(g_now_month-1)+2048+112*(g_now_day-15-1)+j*14+2);
			g_Light_Time_Info.group[j]    = FLASH_ReadHalfWord(RTU_MONTH_LIGHT_TIME_START_ADDR+4096*(g_now_month-1)+2048+112*(g_now_day-15-1)+j*14+4);
			g_Light_Time_Info.loop[j]     = FLASH_ReadHalfWord(RTU_MONTH_LIGHT_TIME_START_ADDR+4096*(g_now_month-1)+2048+112*(g_now_day-15-1)+j*14+6);
			g_Light_Time_Info.ratio[j][0] = FLASH_ReadHalfWord(RTU_MONTH_LIGHT_TIME_START_ADDR+4096*(g_now_month-1)+2048+112*(g_now_day-15-1)+j*14+8);
			g_Light_Time_Info.ratio[j][1] = FLASH_ReadHalfWord(RTU_MONTH_LIGHT_TIME_START_ADDR+4096*(g_now_month-1)+2048+112*(g_now_day-15-1)+j*14+10);
			g_Light_Time_Info.ratio[j][2] = FLASH_ReadHalfWord(RTU_MONTH_LIGHT_TIME_START_ADDR+4096*(g_now_month-1)+2048+112*(g_now_day-15-1)+j*14+12);
		
		}
	}
}


void Write_Month_Light_Time_Info(unsigned char month,unsigned char days)
{
	unsigned char i = 0,j = 0;
	//前15天
	for(i = 0;i < 15;i++)
	{
		for(j = 0;j < 8;j++)
		{
			week_month_buf[0+7*j+i*56] = g_Month_Light_Time_Info[i].s_time[j];
			week_month_buf[1+7*j+i*56] = g_Month_Light_Time_Info[i].e_time[j];
			week_month_buf[2+7*j+i*56] = g_Month_Light_Time_Info[i].group[j];
			week_month_buf[3+7*j+i*56] = g_Month_Light_Time_Info[i].loop[j];
			week_month_buf[4+7*j+i*56] = g_Month_Light_Time_Info[i].ratio[j][0];
			week_month_buf[5+7*j+i*56] = g_Month_Light_Time_Info[i].ratio[j][1];
			week_month_buf[6+7*j+i*56] = g_Month_Light_Time_Info[i].ratio[j][2];
		}
	}
	FLASH_HALFWORD_Write(RTU_MONTH_LIGHT_TIME_START_ADDR+4096*(month-1),week_month_buf,15*56);	
	//后15天
	for(i = 0;i < 16;i++)
	{
		for(j = 0;j < 8;j++)
		{
			week_month_buf[0+7*j+i*56] = g_Month_Light_Time_Info[15+i].s_time[j];
			week_month_buf[1+7*j+i*56] = g_Month_Light_Time_Info[15+i].e_time[j];
			week_month_buf[2+7*j+i*56] = g_Month_Light_Time_Info[15+i].group[j];
			week_month_buf[3+7*j+i*56] = g_Month_Light_Time_Info[15+i].loop[j];
			week_month_buf[4+7*j+i*56] = g_Month_Light_Time_Info[15+i].ratio[j][0];
			week_month_buf[5+7*j+i*56] = g_Month_Light_Time_Info[15+i].ratio[j][1];
			week_month_buf[6+7*j+i*56] = g_Month_Light_Time_Info[15+i].ratio[j][2];
		}
	}
	FLASH_HALFWORD_Write(RTU_MONTH_LIGHT_TIME_START_ADDR+4096*(month-1)+2048,week_month_buf,16*56);
}

void Read_Damp_Info(void)
{
	unsigned short m_val = 0;
	unsigned short i = 0,j = 0;
	g_lamp_total_num = FLASH_ReadHalfWord(DAMP_NUM_ADDR);
	if(g_lamp_total_num == 0xFFFF)
	{
		g_lamp_total_num = 0;
		return;
	}
	if(g_lamp_total_num > DAMP_MAX_NUM)
	{
		g_lamp_total_num = DAMP_MAX_NUM;
	}
	for(i = 0; i < 200; i++)
	{
		
		m_val = FLASH_ReadHalfWord(DAMP_INFO_DATA_START_ADDR + 8 * i);
		g_Damp_Info[i].SN[0] = m_val/256;
		g_Damp_Info[i].SN[1] = m_val%256;
		m_val = FLASH_ReadHalfWord(DAMP_INFO_DATA_START_ADDR + 8 * i + 2);
		g_Damp_Info[i].SN[2] = m_val/256;
		g_Damp_Info[i].SN[3] = m_val%256;
		m_val = FLASH_ReadHalfWord(DAMP_INFO_DATA_START_ADDR + 8 * i + 4);
		g_Damp_Info[i].SN[4] = m_val/256;
		g_Damp_Info[i].SN[5] = m_val%256;
		m_val = FLASH_ReadHalfWord(DAMP_INFO_DATA_START_ADDR + 8 * i + 6);
		g_Damp_Info[i].m_bit_status.group = m_val/256;
		g_Damp_Info[i].m_bit_status.status = m_val%256;
		
		if(g_Damp_Info[i].m_bit_status.group == 0xFF)	//组属未设置过则置0
		{
			g_Damp_Info[i].m_bit_status.group = 0;
		}
		
		g_Damp_Info[i].m_bit_status.send_para_bdp_flag = 0; //初始化没有向服务器发送单灯信息报文
		g_Damp_Info[i].m_bit_status.send_online_bdp_flag = 0;
		g_Damp_Info[i].m_bit_status.send_status_bdp_flag = 0;
		g_Damp_Info[i].m_bit_status.lamp_online_flag = LAMP_STATE_UNKNOW;
	}   
	
	for(i = 200; i < DAMP_MAX_NUM; i++)
	{	
		m_val = FLASH_ReadHalfWord(DAMP_INFO_DATA_START_ADDR2 + 8 * j);
		g_Damp_Info[i].SN[0] = m_val/256;
		g_Damp_Info[i].SN[1] = m_val%256;
		m_val = FLASH_ReadHalfWord(DAMP_INFO_DATA_START_ADDR2 + 8 * j + 2);
		g_Damp_Info[i].SN[2] = m_val/256;
		g_Damp_Info[i].SN[3] = m_val%256;
		m_val = FLASH_ReadHalfWord(DAMP_INFO_DATA_START_ADDR2 + 8 * j + 4);
		g_Damp_Info[i].SN[4] = m_val/256;
		g_Damp_Info[i].SN[5] = m_val%256;
		m_val = FLASH_ReadHalfWord(DAMP_INFO_DATA_START_ADDR2 + 8 * j + 6);
		g_Damp_Info[i].m_bit_status.group = m_val/256;
		g_Damp_Info[i].m_bit_status.status = m_val%256;
		
		if(g_Damp_Info[i].m_bit_status.group == 0xFF)	//组属未设置过则置0
		{
			g_Damp_Info[i].m_bit_status.group = 0;
		}
		
		g_Damp_Info[i].m_bit_status.send_para_bdp_flag = 0; //初始化没有向服务器发送单灯信息报文
		g_Damp_Info[i].m_bit_status.send_online_bdp_flag = 0;
		g_Damp_Info[i].m_bit_status.send_status_bdp_flag = 0;
		g_Damp_Info[i].m_bit_status.lamp_online_flag = LAMP_STATE_UNKNOW;
		
		j++;
	}   
}
//设置下位单灯信息
unsigned short lampbuf[1024] = {0};
void Write_Damp_Info(void)
{	
	unsigned short i = 0,j = 0;
	//第1页保存200个节点信息
	lampbuf[0] = g_lamp_total_num;
	for(i = 0;i < 200;i++)
	{
		lampbuf[1 + i*4]     = g_Damp_Info[i].SN[0]*256 + g_Damp_Info[i].SN[1];
		lampbuf[1 + i*4 + 1] = g_Damp_Info[i].SN[2]*256 + g_Damp_Info[i].SN[3];
		lampbuf[1 + i*4 + 2] = g_Damp_Info[i].SN[4]*256 + g_Damp_Info[i].SN[5];
		lampbuf[1 + i*4 + 3] = g_Damp_Info[i].m_bit_status.group*256+g_Damp_Info[i].m_bit_status.status;
	}
	FLASH_HALFWORD_Write(DAMP_NUM_ADDR,lampbuf,801);
	//第2页保存200个节点信息
	for(i = 200;i < DAMP_MAX_NUM;i++)
	{
		lampbuf[j*4]     = g_Damp_Info[i].SN[0]*256 + g_Damp_Info[i].SN[1];
		lampbuf[j*4 + 1] = g_Damp_Info[i].SN[2]*256 + g_Damp_Info[i].SN[3];
		lampbuf[j*4 + 2] = g_Damp_Info[i].SN[4]*256 + g_Damp_Info[i].SN[5];
		lampbuf[j*4 + 3] = g_Damp_Info[i].m_bit_status.group*256+g_Damp_Info[i].m_bit_status.status;
		j++;
	}
	FLASH_HALFWORD_Write(DAMP_INFO_DATA_START_ADDR2,lampbuf,800);
}

void Read_Concentrator_Config_Info(void)
{
	unsigned char i = 0,j = 0;
	
	g_Concentrator_Para.e3000_config_flag = FLASH_ReadHalfWord(RTU_CONCENTRATOR_PARA_START_ADDR);
	g_Concentrator_Para.e3000_enable = FLASH_ReadHalfWord(RTU_CONCENTRATOR_PARA_START_ADDR + 2);
	g_Concentrator_Para.e3000_relay_enable[0] = FLASH_ReadHalfWord(RTU_CONCENTRATOR_PARA_START_ADDR + 4);
	g_Concentrator_Para.e3000_relay_enable[1] = FLASH_ReadHalfWord(RTU_CONCENTRATOR_PARA_START_ADDR + 6);
	g_Concentrator_Para.e3000_relay_enable[2] = FLASH_ReadHalfWord(RTU_CONCENTRATOR_PARA_START_ADDR + 8);
	g_Concentrator_Para.e3000_V_enable[0] = FLASH_ReadHalfWord(RTU_CONCENTRATOR_PARA_START_ADDR + 10);
	g_Concentrator_Para.e3000_V_enable[1] = FLASH_ReadHalfWord(RTU_CONCENTRATOR_PARA_START_ADDR + 12);
	g_Concentrator_Para.e3000_V_enable[2] = FLASH_ReadHalfWord(RTU_CONCENTRATOR_PARA_START_ADDR + 14);
	g_Concentrator_Para.e3000_A_enable[0] = FLASH_ReadHalfWord(RTU_CONCENTRATOR_PARA_START_ADDR + 16);
	g_Concentrator_Para.e3000_A_enable[1] = FLASH_ReadHalfWord(RTU_CONCENTRATOR_PARA_START_ADDR + 18);
	g_Concentrator_Para.e3000_A_enable[2] = FLASH_ReadHalfWord(RTU_CONCENTRATOR_PARA_START_ADDR + 20);
	
	g_Concentrator_Para.ext_enable[0] = FLASH_ReadHalfWord(RTU_CONCENTRATOR_PARA_START_ADDR + 22);
	g_Concentrator_Para.ext_enable[1] = FLASH_ReadHalfWord(RTU_CONCENTRATOR_PARA_START_ADDR + 24);
	g_Concentrator_Para.ext_enable[2] = FLASH_ReadHalfWord(RTU_CONCENTRATOR_PARA_START_ADDR + 26);
	for(i = 0;i < MAX_EXT_MODULE_NUM;i++)
	{
		for(j = 0;j < 4;j++)
		{
			g_Concentrator_Para.ext_relay_enable[i][j] = FLASH_ReadHalfWord(RTU_CONCENTRATOR_PARA_START_ADDR + 28 + (i*4+j)*2 );
		}
	}
	for(i = 0;i < MAX_EXT_MODULE_NUM;i++)
	{
		for(j = 0;j < 12;j++)
		{
			g_Concentrator_Para.ext_phase_enable[i][j] = FLASH_ReadHalfWord(RTU_CONCENTRATOR_PARA_START_ADDR + 52 + (i*12+j)*2 );
		}
	}
	

}


void Write_Concentrator_Config_Info(void)
{
	unsigned char i = 0,j = 0;
	unsigned short buf[64] = {0};

	buf[0] = g_Concentrator_Para.e3000_config_flag;
	buf[1] = g_Concentrator_Para.e3000_enable;
	buf[2] = g_Concentrator_Para.e3000_relay_enable[0];
	buf[3] = g_Concentrator_Para.e3000_relay_enable[1];
	buf[4] = g_Concentrator_Para.e3000_relay_enable[2];
	buf[5] = g_Concentrator_Para.e3000_V_enable[0];
	buf[6] = g_Concentrator_Para.e3000_V_enable[1];
	buf[7] = g_Concentrator_Para.e3000_V_enable[2];
	buf[8] = g_Concentrator_Para.e3000_A_enable[0];
	buf[9] = g_Concentrator_Para.e3000_A_enable[1];
	buf[10] = g_Concentrator_Para.e3000_A_enable[2];

	buf[11] = g_Concentrator_Para.ext_enable[0];
	buf[12] = g_Concentrator_Para.ext_enable[1];
	buf[13] = g_Concentrator_Para.ext_enable[2];
	for(i = 0;i < MAX_EXT_MODULE_NUM;i++)
	{
		for(j = 0;j < 4;j++)
		{
			buf[14+i*4+j] = g_Concentrator_Para.ext_relay_enable[i][j];
		}
	}
	
	for(i = 0;i < MAX_EXT_MODULE_NUM;i++)
	{
		for(j = 0;j < 12;j++)
		{
			buf[26+i*12+j] = g_Concentrator_Para.ext_phase_enable[i][j];
		}
	}
	
	FLASH_HALFWORD_Write(RTU_CONCENTRATOR_PARA_START_ADDR,buf,64);
}
//保存控制命令
unsigned char Read_CMD_Info(unsigned short hour,unsigned short min,unsigned short sec)
{
	unsigned char i = 0,j = 0;
	unsigned char t_hh,t_mm,t_ss;
	unsigned char m_ret = 0;
	
	
	//读取保存的系统时间
	t_hh = FLASH_ReadHalfWord(CMD_SAVE_ADDR+26);
	t_mm = FLASH_ReadHalfWord(CMD_SAVE_ADDR+28);
	t_ss = FLASH_ReadHalfWord(CMD_SAVE_ADDR+30);

	if( abs((min*60+sec) - (t_mm*60+t_ss)) < 60 )  //60S内
		{
		  //读取回路命令信息
			g_Concentrator_Para.e3000_relay_enable[0] = FLASH_ReadHalfWord(CMD_SAVE_ADDR);
			for(i = 0;i < MAX_EXT_MODULE_NUM;i++)
			{
				for(j = 0;j< 4;j++)
				{
					g_extmodule_relay[i][j] = FLASH_ReadHalfWord(CMD_SAVE_ADDR + 2 + (i*4+j)*2 );
				}
			}
			m_ret = 1;
		}
	else
		{
		   m_ret = 0;
		}
	return m_ret;
}
//保存控制命令
void Write_CMD_Info(void)
{
	unsigned char i = 0,j = 0;
	unsigned short buf[20] = {0};

	//保存回路命令信息
	buf[0] = g_Concentrator_Para.e3000_relay_enable[0];
	for(i = 0;i < MAX_EXT_MODULE_NUM;i++)
	{
		for(j = 0;j< 4;j++)
		{
			buf[i*4+j+1] = g_extmodule_relay[i][j];
		}
	}
	
	//保存系统时间
	buf[13] = g_now_hour;
	buf[14] = g_now_min;
	buf[15] = g_now_sec;
	FLASH_HALFWORD_Write(CMD_SAVE_ADDR,buf,20);
}

void Write_APP_Flag_to_Flash(unsigned char workSpace)
{
	unsigned short buf[3] = {0};
	buf[0] = workSpace;
	buf[1] = 0xBB;
	buf[2] = g_update_flag;
	FLASH_HALFWORD_Write(APP_START_FLAG_ADDR,buf,3);
}

//数据转换成ASCLL值，返回十进制位数
unsigned char data_conversion(unsigned char* buf,unsigned short data,unsigned char decimal)
{
	unsigned char len = 0;
	
	if(10000 <= data)	//五位十进制数
	{
		buf[0] = i_to_a(data/10000);
		buf[1] = i_to_a(data%10000/1000);
		buf[2] = i_to_a(data%1000/100);
		if(DECIMAL0 == decimal)
		{
			buf[3] = i_to_a(data%100/10);
			buf[4] = i_to_a(data%10);
			len = 5;
		}
		else if(DECIMAL1 == decimal)
		{
			buf[3] = i_to_a(data%100/10);
			buf[4] = 0x2E;
			buf[5] = i_to_a(data%10);
			len = 6;
		}
		else if(DECIMAL2 == decimal)
		{
			buf[3] = 0x2E;
			buf[4] = i_to_a(data%100/10);
			buf[5] = i_to_a(data%10);	
			len = 6;
		}
	}
	if((1000 <= data)&&(10000 > data))	//四位十进制数
	{
		buf[0] = i_to_a(data/1000);
		buf[1] = i_to_a(data%1000/100);
		
		if(DECIMAL0 == decimal)
		{
			buf[2] = i_to_a(data%100/10);
			buf[3] = i_to_a(data%10);
			len = 4;
		}
		else if(DECIMAL1 == decimal)
		{
			buf[2] = i_to_a(data%100/10);
			buf[3] = 0x2E;
			buf[4] = i_to_a(data%10);
			len = 5;
		}
		else if(DECIMAL2 == decimal)
		{
			buf[2] = 0x2E;
			buf[3] = i_to_a(data%100/10);
			buf[4] = i_to_a(data%10);	
			len = 5;
		}
	}
	if((100 <= data)&&(1000 > data))	//三位十进制数
	{
		buf[0] = i_to_a(data/100);
		if(DECIMAL0 == decimal)
		{
			buf[1] = i_to_a(data%100/10);
			buf[2] = i_to_a(data%10);
			len = 3;
		}
		else if(DECIMAL1 == decimal)
		{
			buf[1] = i_to_a(data%100/10);
			buf[2] = 0x2E;
			buf[3] = i_to_a(data%10);
			len = 4;
		}
		else if(DECIMAL2 == decimal)
		{
			buf[1] = 0x2E;
			buf[2] = i_to_a(data%100/10);
			buf[3] = i_to_a(data%10);
			len = 4;
		}
	}
	if((10 <= data)&&(100 > data))	//两位十进制数
	{
		if(DECIMAL0 == decimal)
		{
			buf[0] = i_to_a(data/10);
			buf[1] = i_to_a(data%10);
			len = 2;
		}
		else if(DECIMAL1 == decimal)
		{
			buf[0] = i_to_a(data/10);
			buf[1] = 0x2E;
			buf[2] = i_to_a(data%10);
			len = 3;
		}
		else if(DECIMAL2 == decimal)
		{
			buf[0] = 0x30;
			buf[1] = 0x2E;
			buf[2] = i_to_a(data/10);
			buf[3] = i_to_a(data%10);	
			len = 4;			
		}
	}
	if((0 < data)&&(10 > data))	//一位十进制非0数
	{
		if(DECIMAL0 == decimal)
		{
			buf[0] = i_to_a(data);
			len = 1;
		}
		else if(DECIMAL1 == decimal)
		{
			buf[0] = 0x30;
			buf[1] = 0x2E;
			buf[2] = i_to_a(data);
			len = 3;
		}
		else if(DECIMAL2 == decimal)
		{
			buf[0] = 0x30;
			buf[1] = 0x2E;
			buf[2] = 0x30;
			buf[3] = i_to_a(data);	
			len = 4;			
		}
	}
	else if(0 == data)
	{
		buf[0] = 0x30;
		len = 1;
	}
	
	return len;
}

//返回当前处在第几段设置段内  ，0代表不在任何时间段内
unsigned char judge_light_time_stage(unsigned char hour,unsigned char  min)
{
   unsigned char i = 0;
   unsigned char m_ret = 0;
   unsigned short ss,ee;

   unsigned short m_val = 0;	            //实时时间
   m_val = g_now_hour * 60 + g_now_min;
   //依次检测8段工作时间
	for(i = 0; i < 8; i++)
	{	
		if(t_v[i] == 1)                      //当前时间段有效
		{ 
		    ss = t_s[i];
			ee = t_e[i];
			if(ss > ee)  //跨零点
				{
				   ee = ee + 24*60;
					if( (m_val >= 0)&&(m_val < t_e[i]))
					{
						m_val = m_val + 24*60;
					}
				}
		    if((m_val >= ss) && (m_val < ee))  //当前时间在开灯时间段内
		    	{
		    	   m_ret = i + 1;
				   break;
		    	}			
		}
	}
	return m_ret;
	
}

