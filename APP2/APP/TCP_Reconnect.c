#include "TCP_Reconnect.h"
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
#include "Update.h"

#define DELAY  1000

//打印信息
//SIM900A
unsigned char g_CGCLASS1[32]   = "YDT类别设置成功   "; 
unsigned char g_CGCLASS2[32]   = "YDT类别设置失败   ";

unsigned char g_CIPMODE0[32]   = "开始传输模式设置  ";
unsigned char g_CIPMODE1[32]   = "传输模式设置成功  "; 
unsigned char g_CIPMODE2[32]   = "传输模式设置失败  ";
 
unsigned char g_CLPORT0[32]    = "开始本地端口设置  ";
unsigned char g_CLPORT1[32]    = "本地端口设置成功  "; 
unsigned char g_CLPORT2[32]    = "本地端口设置失败  "; 

unsigned char g_CIPCCFG0[32]   = "开始设置缓冲区    "; 
unsigned char g_CIPCCFG1[32]   = "缓冲区设置成功    "; 
unsigned char g_CIPCCFG2[32]   = "缓冲区设置失败    "; 

unsigned char g_CIPSTART0[32]  = "开始网络连接设置  ";
unsigned char g_CIPSTART1[32]  = "网络连接设置成功  "; 
unsigned char g_CIPSTART2[32]  = "网络连接设置失败  ";

unsigned char g_REG0[32]       = "开始服务器注册    "; 
unsigned char g_REG1[32]       = "服务器注册成功    "; 
unsigned char g_REG2[32]       = "服务器注册失败    ";

//EC20
unsigned char g_QICSGP0[32]   = "开始配置TCP/IP      ";
unsigned char g_QICSGP1[32]   = "TCP/IP配置成功      "; 
unsigned char g_QICSGP2[32]   = "TCP/IP配置失败      ";
 
unsigned char g_CPIN0[32]     = "开始查询PIN状态       ";
unsigned char g_CPIN1[32]     = "PIN已解锁             "; 
unsigned char g_CPIN2[32]     = "PIN已锁定             ";

unsigned char g_CREG0[32]     = "开始查询CS服务      "; 
unsigned char g_CREG1[32]     = "CS服务注册成功      "; 
unsigned char g_CREG2[32]     = "CS服务注册失败      ";

unsigned char g_CCID0[32]     = "开始请求CCID      	 "; 
unsigned char g_CCID1[32]     = "请求CCID成功        "; 
unsigned char g_CCID2[32]     = "请求CCID失败        ";

unsigned char g_IMSI0[32]     = "开始请求IMSI        "; 
unsigned char g_IMSI1[32]     = "请求IMSI成功        "; 
unsigned char g_IMSI2[32]     = "请求IMSI失败        ";

unsigned char g_QIACT0[32]    = "开始激活PDP 		 ";
unsigned char g_QIACT1[32]    = "PDP激活成功         "; 
unsigned char g_QIACT2[32]    = "PDP激活失败         "; 

unsigned char g_QIOPEN0[32]   = "开始网络连接        ";
unsigned char g_QIOPEN1[32]   = "网络连接成功        "; 
unsigned char g_QIOPEN2[32]   = "网络连接失败        ";
/////////////////////////////////////////////////////////////////////

unsigned char CIPSTART_timer = 0;

//unsigned char debug_buf1[128] = {0};
/********************************************************************************************************** 
**********************************************************************************************************/
extern OS_EVENT * QMsg; 


/********************************************************************************************************** 
** Function name:      Task_2000ms_Timer
** Descriptions:       TCP 离线后实现重连过程
** input parameters:   void *pdata
** output parameters:  无
** Returned value:     无
**********************************************************************************************************/
unsigned char m_timeout_counter = 0;
void Tcp_Reconnect_proc(void *pdata)
{
   
	while(1)
	{ 	
		g_gprs_timeout_2000ms_counter++;
// 		//打印GPRS超时计数
// 		print_len = data_conversion(print_buf,g_gprs_timeout_2000ms_counter,DECIMAL0);
// 		show_text(DGUS_PRINT_SHOW_ADDR,9,print_buf,print_len);
        //////////////////////////////////////////////////////////////////////////////
        //识别 过程
		if(g_identify_module_model_flag == 1)  //识别GPRS模块
		{
			identify_gprs_module_model();
		}
		//////////////////////////////////////////////////////////////////////////////
		//TCP 连接 过程   如果 TCP 重连 从此过程 开始 即置 g_GPRS_in_Reg_stage = 1
	    if( g_GPRS_in_Reg_stage == 1)
		{
		   
			if(g_gprs_module_model == SIM900)	//SIM900A
			{

				SIM900A_connect_Step_by_Step();
			}
			else if(g_gprs_module_model == EC20)	//EC20
			{
				EC20_connect_Step_by_Step();
			} 
							
		}
      //////////////////////////////////////////////////////////////////////////////////////////
      //注意    参数初始上报的时序在此函数中实现

		//联网完成进行信息上报
		if(g_report_state_flag == 1) //当前处于参数上报状态
		{
			Report_Func();	
		}

       //////////////////////////////////////////////////////////////////////////////////////////
	   //TCP 离线判断
	   //根据当前GPRS所在的状态(注册/工作) 设置离线判断时间
		
		if(g_GPRS_in_Reg_stage == 0)  //不在注册状态
		{
			//不在注册状态 60秒后重连
			m_timeout_counter = 30;
		}
		else
		{
			//在注册状态 180秒后重连  防止出现在注册过程中发出一条报文而收不到回告从而出不来不能充气问题
			m_timeout_counter = 90;
		}
		//如果离线计数器(g_gprs_timeout_2000ms_counter)的计数值超过设定的时间，则判定为离线  
		//走TCP离线重连流程  状态到 TCP 连接 过程  g_GPRS_in_Reg_stage = 1  
		if(g_gprs_timeout_2000ms_counter >= m_timeout_counter)  //重连
		{				    
			show_text(DGUS_STATE_SHOW_ADDR,9,"超时重连          ",strlen("超时重连          "));
			Reset_parameter_to_Reconnect();				
		}

		
		OSTimeDly(1000);		
	}			
}

void Reset_parameter_to_Reconnect(void)
{
	g_update_flag = 0;				//退出升级状态
	
	Close_Connect();                  //关闭TCP连接
	Reset_GPRS_Module();              //重新复位模块
	g_connect_step = 0;               //连接步骤清零
	g_GPRS_connect_OK = 0;            //GPRS离线
	Set_GPRS_LED_Status(CLOSE);       //面板LED显示GPRS离线
	Set_GPRS_Status_to_TC(OFFLINE);   //触摸屏显示GPRS离线   
	m_err_counter = 0;                //之前正常连接m_err_counter=16,如果掉线必须置0,否则不会执行Connect_Server
	g_identify_module_model_flag = 1; //设置到识别状态
	g_GPRS_in_Reg_stage = 0;          //清零注册状态
	g_gprs_timeout_2000ms_counter = 0;//清零离线计数器 保证只执行一次	
		
// 	//心跳发送接收计数清零
// 	g_send_heart_num = 0;
// 	print_len = data_conversion(print_buf,g_send_heart_num,DECIMAL0);
// 	show_text(DGUS_PRINT_SHOW_ADDR,5,print_buf,print_len);
// 	g_recv_heart_num = 0;
// 	print_len = data_conversion(print_buf,g_recv_heart_num,DECIMAL0);
// 	show_text(DGUS_PRINT_SHOW_ADDR,7,print_buf,print_len);
}
//连接服务器
void Connect_Server(void)
{
	unsigned char buf[96] = "AT+QIOPEN=1,1,\"TCP\",\"";  
	
	memcpy(&buf[21],g_E3000_Info.ip,g_E3000_Info.ip_len);
	memcpy(&buf[21+g_E3000_Info.ip_len],"\",",2);
	memcpy(&buf[21+g_E3000_Info.ip_len+2],g_E3000_Info.port,g_E3000_Info.port_len);
	memcpy(&buf[21+g_E3000_Info.ip_len+2+g_E3000_Info.port_len],",0,2\r\n",6);
	
	uart_send_char(buf);
}
//第一步 识别是EC20 还是SIM900A  g_identify_module_model_flag 为标志
//第二步 TCP连接初始化   g_GPRS_in_Reg_stage 为标志
void ActOn_TCP_Connect_Bdp(unsigned char * pbuf, unsigned char len)
{

    g_gprs_timeout_2000ms_counter = 0;  //收到GPRS报文，把超时计数器清零

   //第一步 识别过程
	if(g_identify_module_model_flag == 1)  //获取GPRS模块型号
	{
		if(strstr(pbuf,"SIM900") != NULL )
		{
			g_gprs_module_model = SIM900;
			g_identify_module_model_flag = 0;
			g_GPRS_in_Reg_stage = 1;  //置连接状态  注册状态 
		}
		else if(strstr(pbuf,"EC20") != NULL )
		{
			g_gprs_module_model = EC20;
			g_identify_module_model_flag = 0;
			g_GPRS_in_Reg_stage = 1;  //置连接状态  注册状态 
		}
		else
		{
			show_text(DGUS_STATE_SHOW_ADDR,9,"版本180209 模块识别中...      ",strlen("版本180209 模块识别中...     "));
		}
		
	}
	//第二步 响应TCP连接过程
	if(g_GPRS_in_Reg_stage == 1)  //GPRS在注册阶段
	{
		if(g_gprs_module_model == SIM900)	//SIM900A
		{
			SIM900A_connect_to_gprs_net_proc(pbuf,len);
		}
		else if(g_gprs_module_model == EC20)	//EC20
		{
			EC20_connect_to_gprs_net_proc(pbuf);
		} 
	}
	
}

//关闭GPRS连接
void Close_Connect(void)
{
	OSTimeDly(DELAY);
	uart_send_char("+++");            //退出透传模式
	OSTimeDly(DELAY);
	if(g_gprs_module_model == SIM900)
	{
		uart_send_char("AT+CIPSHUT\r\n");	
		OSTimeDly(DELAY);
		uart_send_char("AT+CIPCLOSE=1\r\n");  //关闭TCP连接
	}
	else if(g_gprs_module_model == EC20)
	{
		uart_send_char("AT+QICLOSE=1\r\n");  //关闭TCP连接
		OSTimeDly(DELAY);
		uart_send_char("AT+QIDEACT=1\r\n");    //停用PDP
	} 
	OSTimeDly(DELAY);
}

//识别模块型号
void identify_gprs_module_model(void)
{
// 	uart_send_char("ATE0\r\n");	//关闭GPRS模块回显
// 	OSTimeDly(DELAY);
	uart_send_char("ATI\r\n");	//识别模块型号
}

//SIM900A注册服务器步骤
void SIM900A_connect_Step_by_Step(void)
{
	if(m_err_counter < 10)  //单次错误连接次数不超过10次
	{
		switch(g_connect_step)
		{
			case 0:
				uart_send_char("AT+CSQ\r\n");
				break;
			case 1:
				uart_send_char("AT+CGCLASS=\"B\"\r\n");
				break;
			case 2:
				uart_send_char("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n");  //
				break;
			case 3:
				uart_send_char("AT+CIPCSGP=1,\"CMNET\"\r\n"); 
				break;
			case 4:
				uart_send_char("AT+CCID\r\n");
				break;
			case 5:
				uart_send_char("AT+CIMI\r\n");
				break;
			case 6:
				uart_send_char("AT+CIPMODE=1\r\n");    //透传模式设置
				show_text(DGUS_STATE_SHOW_ADDR,9,g_CIPMODE0,strlen(g_CIPMODE0));
				break;
			case 7:
				uart_send_char("AT+CLPORT=\"TCP\",\"20002\"\r\n");  //TCP  本地端口号设置成功
				show_text(DGUS_STATE_SHOW_ADDR,9,g_CLPORT0,strlen(g_CLPORT0));
				break;
			case 8:
				uart_send_char("AT+CGATT=1\r\n");  //附着测试
				break;
			case 9:
				uart_send_char("AT+CIPCCFG=5,2,1024,1\r\n");  //缓冲区
				show_text(DGUS_STATE_SHOW_ADDR,9,g_CIPCCFG0,strlen(g_CIPCCFG0));
				CIPSTART_timer = 0;
				break;
			case 10:
				if(CIPSTART_timer == 0)
				{
#ifdef  WORKING_SERVER_IP
					if(g_E3000_Info.ip_set == 0)
					{
						uart_send_char("AT+CIPSTART=\"TCP\",\"kuitun.daominglight.com\",\"24680\"\r\n");  //网络连接	正式服务器
					}
					else
					{
						Connect_Server();
					}
#else
					uart_send_char("AT+CIPSTART=\"TCP\",\"dev.daominglight.com\",\"24680\"\r\n");  //网络连接	测试服务器
#endif
					show_text(DGUS_STATE_SHOW_ADDR,9,g_CIPSTART0,strlen(g_CIPSTART0));
				}
				CIPSTART_timer++;
				break;
			case 11:  //发送注册报文 
				send_registered();
				show_text(DGUS_STATE_SHOW_ADDR,9,g_REG0,strlen(g_REG0));
				break;
			default:
				break;	  	
		}
	}
	else     //单次错误连接次数超过10次
	{
		show_text(DGUS_STATE_SHOW_ADDR,9,"重连超次数--停止  ",strlen("重连超次数--停止  "));
		g_gprs_timeout_2000ms_counter = 0;  //超过10次,则把超时计数器置零
		g_GPRS_in_Reg_stage = 0; //退出连接状态   退出注册状态
	}
}

//EC20注册服务器步骤
void EC20_connect_Step_by_Step(void)
{
	if(m_err_counter < 10)  //单次错误连接次数不超过10次
   	{
		switch(g_connect_step)
		{
			case 0:
				uart_send_char("AT+CSQ\r\n");
				break;
			case 1:	
				uart_send_char("AT+QCCID\r\n");	//查询CCID
				show_text(DGUS_STATE_SHOW_ADDR,9,g_CCID0,strlen(g_CCID0));
				break;
			case 2:
				uart_send_char("AT+CIMI\r\n");	//查询IMSI
				show_text(DGUS_STATE_SHOW_ADDR,9,g_IMSI0,strlen(g_IMSI0));
				break;	
			case 3:
				uart_send_char("AT+QICSGP=1,1,\"CMIOT\",\"\",\"\",1\r\n");	//配置TCP/IP
				show_text(DGUS_STATE_SHOW_ADDR,9,g_QICSGP0,strlen(g_QICSGP0));
				break;
			case 4: 
				uart_send_char("AT+CREG?\r\n");  //查询CS服务
				show_text(DGUS_STATE_SHOW_ADDR,9,g_CREG0,strlen(g_CREG0));
				break;
			case 5:				
				uart_send_char("AT+QIACT=1\r\n");    //激活PDP
				show_text(DGUS_STATE_SHOW_ADDR,9,g_QIACT0,strlen(g_QIACT0));
				break;
			case 6:	
#ifdef  WORKING_SERVER_IP

//				uart_send_char("AT+CIPSTART=\"TCP\",\"kuitun.daominglight.com\",\"24680\"\r\n");  //网络连接	正式服务器

				Connect_Server();
#else
//				uart_send_char("AT+QIOPEN=1,1,\"TCP\",\"dev.daominglight.com\",24680,0,2\r\n");  //连接服务器			//测试服务器
				Connect_Server();
#endif
				show_text(DGUS_STATE_SHOW_ADDR,9,g_QIOPEN0,strlen(g_QIOPEN0));	
				break;
			case 7:  //发送注册报文 
				send_registered();
				show_text(DGUS_STATE_SHOW_ADDR,9,g_REG0,strlen(g_REG0));
				break;
			default:
				break;	  	
		}
   	}
	else     //单次错误连接次数超过10次
   	{
   	    show_text(DGUS_STATE_SHOW_ADDR,9,"重连超次数--停止  ",strlen("重连超次数--停止  "));
		g_gprs_timeout_2000ms_counter = 0;  //超过10次,则把超时计数器置零
		g_GPRS_in_Reg_stage = 0; //退出连接状态   退出注册状态
		g_connect_step = 0;
   	}
}

//SIM900A注册服务器步骤返回报文处理函数
void SIM900A_connect_to_gprs_net_proc(unsigned char* pbuf,unsigned short len)
{
    unsigned char ret;
	char *buf = NULL;
	unsigned char i = 0,j = 0;
	unsigned char CSQ = 0;
	
	switch(g_connect_step)  //当前回告报文属于什么命令
	{
		case 0:
			if( strstr(pbuf,"CSQ") == NULL )  //获取信号强度
			{
				m_err_counter++;
				show_text(DGUS_STATE_SHOW_ADDR,9,"版本E3000 V1.2      ",strlen("版本E3000 V1.2     "));
			}
			else
			{
				if((pbuf[32]>='1')&&(pbuf[32]<='3')&&(pbuf[33]>='0')&&(pbuf[33]<='9'))
				{
					CSQ = a_to_i(pbuf[32])*10+a_to_i(pbuf[33]);
					if(CSQ >= 20)
					{
						g_GPRS_signal_strength = 3;
					}
					else
					{
						g_GPRS_signal_strength = 2;
					}
				}
				else
				{
					g_GPRS_signal_strength = 1;
				}
				g_E3000_Info.SQ = CSQ;
				g_connect_step ++;
				m_err_counter = 0;
			}
			break;
		case 1:
			if( strstr(pbuf,"OK") == NULL )  
			{
				m_err_counter++;
				show_text(DGUS_STATE_SHOW_ADDR,9,g_CGCLASS2,strlen(g_CGCLASS2));
			}
			else    //移动台类别设置成功
			{
				g_connect_step ++;
				m_err_counter = 0;
				show_text(DGUS_STATE_SHOW_ADDR,9,g_CGCLASS1,strlen(g_CGCLASS1));
			}
			break;
		case 2:
			if( strstr(pbuf,"OK") == NULL )
			{
				m_err_counter++;
			}
			else
			{
				g_connect_step ++;
				m_err_counter = 0;
			}
			break;
		case 3:
			if( strstr(pbuf,"OK") == NULL )
			{
				m_err_counter++;
			}
			else
			{
				g_connect_step ++;
				m_err_counter = 0;
			}
			break;
		case 4:
			if(strstr(pbuf,"8986") == NULL )
			{
				m_err_counter++;
			}
			else
			{
				j = 0;
				buf = strstr(pbuf,"8986");
				for( i = 0;i <= 9;i++)
				{
					ICCID_and_IMSI_buf[i] = a_to_i( buf[j] )*16+a_to_i( buf[j+1] );
					j = j+2;
				}
				g_connect_step ++;
				m_err_counter = 0;
			}
			break;
		case 5:
			if(strstr(pbuf,"460") == NULL )
			{
				m_err_counter++;
			}
			else
			{
				j = 1;					
				buf = strstr(pbuf,"460");
				ICCID_and_IMSI_buf[10] = a_to_i(buf[0]);
				for(i = 11;i <= 17;i++)
				{
					ICCID_and_IMSI_buf[i] = a_to_i( buf[j] )*16+a_to_i( buf[j+1] );
					j = j+2;
				}
				g_connect_step ++;
				m_err_counter = 0;
			}	
			break;
		case 6:    //透传模式设置
			if( strstr(pbuf,"OK") == NULL )
			{
				m_err_counter++;
				show_text(DGUS_STATE_SHOW_ADDR,9,g_CIPMODE2,strlen(g_CIPMODE2));
			}
			else
			{
				show_text(DGUS_STATE_SHOW_ADDR,9,g_CIPMODE1,strlen(g_CIPMODE1));
				g_connect_step ++;
				m_err_counter = 0;
			}
			break;
		case 7:  //本地端口号设置成功
			if( strstr(pbuf,"OK") == NULL )
			{
				m_err_counter++;
				show_text(DGUS_STATE_SHOW_ADDR,9,g_CLPORT2,strlen(g_CLPORT2));
			}
			else
			{
				g_connect_step ++;
				m_err_counter = 0;
				show_text(DGUS_STATE_SHOW_ADDR,9,g_CLPORT1,strlen(g_CLPORT1));
			}
			break;
		case 8:  ////附着测试
			if( strstr(pbuf,"OK") == NULL )
			{
				m_err_counter++;
			}
			else
			{
				g_connect_step ++;
				m_err_counter = 0;
			}
			break;
		case 9:    //缓冲区
			if( strstr(pbuf,"OK") == NULL )
			{
				m_err_counter++;
				show_text(DGUS_STATE_SHOW_ADDR,9,g_CIPCCFG2,strlen(g_CIPCCFG2));
			}
			else
			{
				show_text(DGUS_STATE_SHOW_ADDR,9,g_CIPCCFG1,strlen(g_CIPCCFG1));
				g_connect_step ++;
				m_err_counter = 0;
			}
			break;
		case 10:  //网络连接
			if( strstr(pbuf,"CONNECT") == NULL )
			{	 
				show_text(DGUS_STATE_SHOW_ADDR,9,g_CIPSTART2,strlen(g_CIPSTART2));
			}
			else
			{
				show_text(DGUS_STATE_SHOW_ADDR,9,g_CIPSTART1,strlen(g_CIPSTART1));	
				g_connect_step ++;
				m_err_counter = 0;
			}
			if(CIPSTART_timer >= 10)
			{
				m_err_counter++;   
				CIPSTART_timer = 0;
			}			
			break;
		case 11:   //注册服务器步骤
			ret = Server_reg_bak_info(pbuf);
			if(ret == 1)  //服务器成功认证
			{
				g_connect_step ++;
				m_err_counter = 0;
				show_text(DGUS_STATE_SHOW_ADDR,9,g_REG1,strlen(g_REG1));					
			}
			else
			{
				m_err_counter++;
			}
			break;
		case 12:
			g_GPRS_in_Reg_stage = 0;  //注册完成 进入正常工作模式 接收服务器报文
			send_gprs_heart_bdp();     //连上之后立马发一帧心跳			
			g_connect_step ++;
		
// 			g_send_heart_num++;
// 			//打印重连次数，初次连接不算				
// 			print_len = data_conversion(print_buf,g_GPRS_reconnect_num,DECIMAL0);
// 			show_text(DGUS_PRINT_SHOW_ADDR,15,print_buf,print_len);
// 			g_GPRS_reconnect_num++;
		
		   //参数上报 相关参数设置
			g_report_state_flag = 1; //进入参数上报状态	
			g_report_step = 0;
			show_text(DGUS_STATE_SHOW_ADDR,9,"进入参数上报流程    ",strlen("进入参数上报流程    "));
			break;
		default:
			break;		
	}
}

//EC20注册服务器步骤返回报文处理函数
void EC20_connect_to_gprs_net_proc(unsigned char *pbuf)
{
	unsigned char ret = 0;
	unsigned char *buf = NULL;
	unsigned char i = 0,j = 0;
	unsigned char CSQ = 0;	//信号强度
	
	switch(g_connect_step)  
	{
		case 0:		//查询信号强度
			if( strstr(pbuf,"OK") == NULL )
			{
			  show_text(DGUS_STATE_SHOW_ADDR,9,"版本E3000 V1.3      ",strlen("版本CE3000 V1.3     "));
			}
			else
			{
				if(pbuf[16] == 0x2C) //字符','
				{
					CSQ = a_to_i(pbuf[15]);
					g_GPRS_signal_strength = 1;
				}
				else
				{
					CSQ = a_to_i(pbuf[15])*10+a_to_i(pbuf[16]);
					g_GPRS_signal_strength = 2;
				}							
				if(CSQ == 99)  //如果一直是99 代表检测不到强度  需要一直检测
				{
					show_text(DGUS_STATE_SHOW_ADDR,9,"信号无强度",strlen("信号无强度"));
				}
				g_E3000_Info.SQ = CSQ;
				
				g_connect_step++;
			}						
			break;
		case 1:		//查询CCID
			if(strstr(pbuf,"8986") == NULL )
			{
				m_err_counter++;
				show_text(DGUS_STATE_SHOW_ADDR,9,g_CCID2,strlen(g_CCID2));
			}
			else
			{
				buf = strstr(pbuf,"8986");
				for( i = 0;i <= 9;i++)
				{
					ICCID_and_IMSI_buf[i] = a_to_i( buf[j] )*16+a_to_i( buf[j+1] );
					j = j+2;
				}
				show_text(DGUS_STATE_SHOW_ADDR,9,g_CCID1,strlen(g_CCID1));
				g_connect_step ++;
				m_err_counter = 0;
			}
			break;
		case 2:		//查询IMSI
			if(strstr(pbuf,"460") == NULL )
			{
				m_err_counter++;
				show_text(DGUS_STATE_SHOW_ADDR,9,g_IMSI2,strlen(g_IMSI2));
			}
			else
			{
				j = 1;					
				buf = strstr(pbuf,"460");
				ICCID_and_IMSI_buf[10] = a_to_i(buf[0]);
				for(i = 11;i <= 17;i++)
				{
					ICCID_and_IMSI_buf[i] = a_to_i( buf[j] )*16+a_to_i( buf[j+1] );
					j = j+2;
				}					
				show_text(DGUS_STATE_SHOW_ADDR,9,g_IMSI1,strlen(g_IMSI1));
				g_connect_step ++;
				m_err_counter = 0;
			}			
			break;
		case 3:		//配置TCP/IP
			if( strstr(pbuf,"OK") == NULL )
			{
				m_err_counter++;
				show_text(DGUS_STATE_SHOW_ADDR,9,g_QICSGP2,strlen(g_QICSGP2));
			}
			else
			{				
				show_text(DGUS_STATE_SHOW_ADDR,9,g_QICSGP1,strlen(g_QICSGP1));
				g_connect_step ++;
				m_err_counter = 0;
			}
			break;
		case 4:		//查询CS服务
			buf = strstr(pbuf,"CREG:");
			if( (buf == NULL)||((buf[8] != '1')&&(buf[8] != '5')) )  
			{
				m_err_counter++;
				show_text(DGUS_STATE_SHOW_ADDR,9,g_CREG2,strlen(g_CREG2));
			}
			else    
			{
				show_text(DGUS_STATE_SHOW_ADDR,9,g_CREG1,strlen(g_CREG1));
				g_connect_step ++;
				m_err_counter = 0;
			}
			break;
		case 5:		//激活PDP
			if( (strstr(pbuf,"OK") == NULL)||(strstr(pbuf,"ERROR") != NULL) )
			{
				m_err_counter++;
				show_text(DGUS_STATE_SHOW_ADDR,9,g_QIACT2,strlen(g_QIACT2));
			}
			else
			{
				show_text(DGUS_STATE_SHOW_ADDR,9,g_QIACT1,strlen(g_QIACT1));
				g_connect_step ++;
				m_err_counter = 0;
			}
			break;
		case 6:  //网络连接
			if( strstr(pbuf,"CONNECT") == NULL )
			{
				m_err_counter++;
				show_text(DGUS_STATE_SHOW_ADDR,9,g_QIOPEN2,strlen(g_QIOPEN2));
			}
			else
			{
				show_text(DGUS_STATE_SHOW_ADDR,9,g_QIOPEN1,strlen(g_QIOPEN1));
				g_connect_step ++;
				m_err_counter = 0;
			}
			break;
		case 7:   //注册服务器步骤
			ret = Server_reg_bak_info(pbuf);
			if(ret == 1)  //服务器成功认证
			{
			   g_GPRS_connect_OK = 1;  //收到服务器的报文 证明连接上
			   Set_GPRS_LED_Status(OPEN);
			   Set_GPRS_Status_to_TC(g_GPRS_signal_strength);  //显示GPRS在线
			   g_gprs_timeout_2000ms_counter = 0;  //收到GPRS报文，把超时计数器清零
		
			   g_connect_step ++;
			   m_err_counter = 0;
			   show_text(DGUS_STATE_SHOW_ADDR,9,g_REG1,strlen(g_REG1));
			}
			else
			{
			   m_err_counter++;
			   show_text(DGUS_STATE_SHOW_ADDR,9,"服务器认证失败",strlen("服务器认证失败"));
			}
			break;
		case 8:
			g_GPRS_in_Reg_stage = 0;  //注册完成 进入参数上报流程  接收服务器报文
			send_gprs_heart_bdp();     //连上之后立马发一帧心跳			
			g_connect_step ++;
		
// 			g_send_heart_num++;
// 			//打印重连次数，初次连接不算				
// 			print_len = data_conversion(print_buf,g_GPRS_reconnect_num,DECIMAL0);
// 			show_text(DGUS_PRINT_SHOW_ADDR,15,print_buf,print_len);
// 			g_GPRS_reconnect_num++;
		
		   //参数上报 相关参数设置
			g_report_state_flag = 1; //进入参数上报状态	
			g_report_step = 0;
			show_text(DGUS_STATE_SHOW_ADDR,9,"进入参数上报流程    ",strlen("进入参数上报流程    "));
			break;
		default:
			break;				
	}
}

//上报信息功能函数
void Report_Func(void)
{
	unsigned short i = 0;
	
	switch(g_report_step)
		{
			case 0:
				if(g_terminal_reboot_flag == 1)
				{
					send_terminal_reboot_bdp();	//发送终端重启信息
					g_terminal_reboot_flag = 0;
				}					
				g_report_step++;
				break;
			case 1:		//上报集中器参数
				send_concentrator_parameter_bdp();
				g_report_step++;	
				show_text(DGUS_STATE_SHOW_ADDR,9,"完成上报集中器参数    ",strlen("完成上报集中器参数    "));
				break;
			case 2:		//上报版本号
				send_software_version_bdp();
				g_report_step++;
				show_text(DGUS_STATE_SHOW_ADDR,9,"完成上报版本号    ",strlen("完成上报版本号    "));
				break;
			case 3:		//上报CCID和IMSI
				send_CCID_and_IMSI_bdp();		
				g_report_step++;	
				show_text(DGUS_STATE_SHOW_ADDR,9,"完成上报CCID和IMSI    ",strlen("完成上报CCID和IMSI    "));
				break;
			case 4:		//上报集中器总用电量
				g_report_step++;
				break;
			case 5:		//上报STM序列号
				send_STM32_CHIP_ID_bdp();
				show_text(DGUS_STATE_SHOW_ADDR,9,"完成上报STM序列号  ",strlen("完成上报STM序列号  "));
				g_report_step++;	
				break;
			case 6:		//上报集中器温度
				send_concentrator_temperature_bdp();
				show_text(DGUS_STATE_SHOW_ADDR,9,"完成上报E3000温度  ",strlen("完成上报E3000温度  "));
				g_report_step++;
				break;
			case 7:		//上报路由状态
				if(g_router_fault_flag == 0)
				{
					send_concentrator_fault_release_bdp(CONCENTRATOR_FAULT_1,0);//解除路由通信故障
					show_text(DGUS_STATE_SHOW_ADDR,9,"载波通信正常  ",strlen("载波通信正常  "));
				}
				else
				{
					send_concentrator_fault_bdp(CONCENTRATOR_FAULT_1,0);	//上报集中器与路由通信故障
					show_text(DGUS_STATE_SHOW_ADDR,9,"载波通信故障  ",strlen("载波通信故障  "));
				}
				g_report_step++;	
				break;
			case 8:		//上报扩展模块状态
				for(i = 0;i < 3;i++)
				{
					if(g_Concentrator_Para.ext_enable[i] != 0xFF)		//扩展模块未启用
					{
						if(g_ext_online_flag[i] == ONLINE)
						{
							send_concentrator_fault_release_bdp(CONCENTRATOR_FAULT_2,i+1);	//解除扩展模块通信失败
						}
						else
						{
							send_concentrator_fault_bdp(CONCENTRATOR_FAULT_2,i+1);			//上报扩展模块通信失败
						}
					}
				}				
				g_report_step++;
				show_text(DGUS_STATE_SHOW_ADDR,9,"完成扩展模块状态上报 ",strlen("完成扩展模块状态上报 "));
				break;
			case 9:		//上报供电方式
				if(g_Concentrator_Para.power_supply == AC_POWER_SUPPLY)
				{
					send_concentrator_fault_release_bdp(CONCENTRATOR_FAULT_4,0);	//上报服务器市电供电
				}
				else if(g_Concentrator_Para.power_supply == BATTERY_POWER_SUPPLY)
				{	
					send_concentrator_fault_bdp(CONCENTRATOR_FAULT_4,0);	//上报服务器电池供电
				}
				g_report_step++;
				show_text(DGUS_STATE_SHOW_ADDR,9,"完成供电方式上报 ",strlen("完成供电方式上报 "));
				break;
			case 10:												
				send_local_time_bdp(0);					//上报本地时间
				g_report_step++;
				show_text(DGUS_STATE_SHOW_ADDR,9,"完成本地时间上报 ",strlen("完成本地时间上报 "));
				break;
			case 11:			
				send_local_working_time_bdp();		//上报工作时间			 
                g_report_step++;
				show_text(DGUS_STATE_SHOW_ADDR,9,"完成工作时间上报 ",strlen("完成工作时间上报 "));
				break;
             case 12:	
				g_report_step++;				 
				break;
			case 13:
				send_singnal_quality_bdp();
				show_text(DGUS_STATE_SHOW_ADDR,9,"完成信号强度上报 ",strlen("完成信号强度上报 "));
				g_report_step++;
				break;
			case 14:
				//初始化单灯状态
				for(i = 0 ; i < DAMP_MAX_NUM ; i++ )
				{
					if(g_Damp_Info[i].m_bit_status.status == 1)
					{
						g_Damp_Info[i].m_bit_status.send_para_bdp_flag = 0;
						g_Damp_Info[i].m_bit_status.send_online_bdp_flag = 0;
						g_Damp_Info[i].m_bit_status.send_status_bdp_flag = 0;		
					}
				}
				//参数上报过程结束
				g_report_state_flag = 0;
			    g_report_step = 0;	
				
				show_text(DGUS_STATE_SHOW_ADDR,9,"完成初始化信息上报 ",strlen("完成初始化信息上报 "));
				break;
			default:
				break;
		}
// 	//打印参数上报步骤
// 	print_len = data_conversion(print_buf,g_report_step,DECIMAL0);
// 	show_text(DGUS_PRINT_SHOW_ADDR,11,print_buf,print_len);
}
