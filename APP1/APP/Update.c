#include "Update.h"
#include "globaldata.h"
#include "globalfunc.h"
#include "dacai_touch.h"
#include "TCP_Reconnect.h"
#include "PLC.h"
#include "IOctl.h"

void ActOn_Update_BDP(unsigned char * pbuf,unsigned short len)
{
	switch(pbuf[5])
	{
		case 0x09:	//停止升级
			Update_Stop();
			break;
		case 0x0B:	//下发升级包
			g_update_timeout_1000ms_counter = 0;
			
			Write_Update_Program_to_Flash(pbuf,len);
			Show_Update_Progress(pbuf[6],pbuf[7]);
			break;
		case 0x0D:	//通知升级
			Update_Ready();
			break;
		
	}
	
	
}



void Update_Ready(void)
{
// 	//如果当前时间处于工作时间内,不进行升级
// 	if(judge_light_time_stage(g_now_hour,g_now_min) > 0)
// 	{
// 		send_update_ready_bdp(0);
// 	}
// 	else
	{		
		g_update_flag = 1;
		Write_APP_Flag_to_Flash(SPACE1);
		
		send_plc_stop_bdp();	//暂停抄读
		
		send_update_ready_bdp(0);
		show_text(DGUS_STATE_SHOW_ADDR,9,"版本升级中......  ",strlen("版本升级中......  "));
	}
}

void Update_Stop(void)
{
	g_update_flag = 0;

	Write_APP_Flag_to_Flash(SPACE1);

	show_text(DGUS_STATE_SHOW_ADDR,9,"版本升级中......  ",strlen("版本升级中......  "));
}

//发送升级请求
void send_update_request_bdp(void)
{
	unsigned char buf[10] = {0}; 
	
	g_gprs_msg_num++;

	buf[0] = 0xA5;
	buf[1] = 0x00;
	buf[2] = 0x0A;
	buf[3] = g_gprs_msg_num;
	buf[4] = 0x99;
	buf[5] = 0x0A;
	buf[6] = 0x00;
	buf[7] = 0x00;
	buf[8] = 0x00;
	
	buf[9] = checksum(buf,9);
	gprs_data_send(buf,10);
}

//回复升级准备状态
void send_update_ready_bdp(unsigned char status)
{
	unsigned char buf[8] = {0}; 

	buf[0] = 0xA5;
	buf[1] = 0x00;
	buf[2] = 0x08;
	buf[3] = g_gprs_msg_num;
	buf[4] = 0x99;
	buf[5] = 0x0E;
	buf[6] = status;
	
	buf[7] = checksum(buf,7);
	gprs_data_send(buf,8);
}

//回复升级包请求
void send_update_package_request_bdp(unsigned char num)
{
	unsigned char buf[8] = {0}; 
	
	buf[0] = 0xA5;
	buf[1] = 0x00;
	buf[2] = 0x08;
	buf[3] = g_gprs_msg_num;
	buf[4] = 0x99;
	buf[5] = 0x0C;
	buf[6] = num;
	
	buf[7] = checksum(buf,8);
	gprs_data_send(buf,8);
}

//回复升级结果
void send_update_result_bdp(unsigned char result)
{
	unsigned char buf[8] = {0};; 

	buf[0] = 0xA5;
	buf[1] = 0x00;
	buf[2] = 0x08;
	buf[3] = g_gprs_msg_num;
	buf[4] = 0x99;
	buf[5] = 0x0F;
	buf[6] = result;
	
	buf[7] = checksum(buf,7);
	gprs_data_send(buf,8);
}

u32 appxaddr = FLASH_APP2_ADDR;	//升级程序写入FLASH的首地址
void Write_Update_Program_to_Flash(u8 *appbuf,u32 appsize)
{
	u16 t = 0;
	u16 i = 0;
	u16 j = 0;
	u16 temp;
	u16 temp_buf[1024] = {0};
	u8 *dfu=NULL;
	u8 xorcheck = 0;
	u8 packNum = 0;

	packNum = appbuf[6];
	
// 	if(	(appsize != (appbuf[1]*256+appbuf[2])) )	//如果发送与接收数据长度不一致，取发送数据长度
// 		appsize = (appbuf[1]*256+appbuf[2]);
	
	for(j = 8;j < (appsize-2);j++)	//异或校验
	{
		xorcheck ^= appbuf[j];
	}
	if(xorcheck == appbuf[appsize-2])		//校验成功
	{
//		show_text(DGUS_STATE_SHOW_ADDR,9,"校验成功......  ",strlen("校验成功......  "));
		
		dfu=&appbuf[8];
		appsize = appsize-10;
		
		for(t=0;t<appsize;t+=2)
		{						    
			temp=(u16)dfu[1]<<8;
			temp+=(u16)dfu[0];	  
			dfu+=2;//偏移2个字节
			temp_buf[i++]=temp;	    
			if(i==1024)
			{
				i=0;
				FLASH_HALFWORD_Write(appxaddr,temp_buf,1024);	
				appxaddr+=2048;//偏移2048  
			}
		}
		if(i)
		{
			FLASH_HALFWORD_Write(appxaddr,temp_buf,i);	//将最后的一些内容字节写进去. 
		}
		
//		show_text(DGUS_STATE_SHOW_ADDR,9,"写入FLASH完成......  ",strlen("写入FLASH完成......  "));
		
		if(appbuf[6] == appbuf[7])		//当前报文序号与总段数相等，表明接受完成
		{
			send_update_result_bdp(0);		//回复升级完成			
						
			g_update_flag = 0;
			Write_APP_Flag_to_Flash(SPACE2);	
			
			show_text(DGUS_STATE_SHOW_ADDR,9,"版本升级完成      ",strlen("版本升级完成      "));
			
			Delay_s(5);
			NVIC_SystemReset();	//系统复位
		}
		else
		{
			packNum++;
			send_update_package_request_bdp(packNum);		//请求下一段升级包
		}
	}
	else
	{
		show_text(DGUS_STATE_SHOW_ADDR,9,"校验失败,重新接收......  ",strlen("校验失败,重新接收......  "));
		send_update_package_request_bdp(packNum);	//校验失败，请求第1帧升级包
	}
}

//显示升级进度
void Show_Update_Progress(unsigned char packNum,unsigned char totalNum)
{
	unsigned char buf[32] = {0xC9,0xFD,0xBC,0xB6,0xCA,0xFD,0xBE,0xDD,0xBD,0xD3,0xCA,0xD5,0xD6,0xD0,0x2E,0x2E,0x2E,0,0,0,0x25};
	unsigned char buf1[3] = {0};	
	unsigned char ratio = 0;
	unsigned char len = 0;
	
	ratio = packNum*100/totalNum;
	len = data_conversion(buf1,ratio,0);
	
	memcpy(&buf[17],buf1,len);
	buf[17+len] = 0x25;
	
	show_text(DGUS_STATE_SHOW_ADDR,9,buf,17+len+1);
	
}

void Check_Update_Status(void)
{
	if((g_update_flag == 1)&&(g_GPRS_connect_OK == 1))
	{
		g_update_timeout_1000ms_counter++;
		if(g_update_timeout_1000ms_counter > 120)	//2分钟未收到升级包，再次申请该段升级包
		{				
			g_update_timeout_1000ms_counter = 0;
			send_update_package_request_bdp(1);
			show_text(DGUS_STATE_SHOW_ADDR,9,"重新接收数据  ",strlen("重新接受数据  "));
		}
	}
}

void Delay_s(unsigned char s)
{
	unsigned short i = 0,j = 0;
	
	j = s*1000;
	while(j--)
	{
		i=10000;
		while(i--);
	}
}
