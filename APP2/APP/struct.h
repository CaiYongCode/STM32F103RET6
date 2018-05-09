#ifndef _STRUCT_H
#define _STRUCT_H
#include "stm32f10x.h"
#include "includes.h"
#include "Initialize.h"
#include "Const.h"

typedef struct R_msg
{
	unsigned short msg_type;
	unsigned short msg_len;
	int msg_addr;
}R_msg;


typedef struct RTU_CONFIG_INFO
{
    unsigned short config_flag;  //0x0D  已注册
	unsigned char SN[8];
	unsigned char soft_ersion[4];
	unsigned char simcard_num[14];	
	unsigned char route_addr[6];
	unsigned char ip_set;	
	unsigned char ip_len;
	unsigned char ip[32];
	unsigned char port_set;
	unsigned char port_len;
	unsigned char port[5];
	unsigned char SQ;
	unsigned int longitude;			//经度
	unsigned int latitude;			//纬度
}RTU_CONFIG_INFO;

typedef struct Concentrator_parameter
{
	unsigned short Voltage[3];
	unsigned short e3000_current[3];        //集諫器萯路电流采集
	unsigned char ext_enable[3];             // 扩展模块状态
	unsigned short ext_current[MAX_EXT_MODULE_NUM][12];      //扩展模块电流
	unsigned char power_supply;				//供电方式
	unsigned char e3000_enable;
	unsigned char e3000_config_flag;		//集中器配置标志位
	unsigned char e3000_relay_enable[3];		//集中器继电器启用状态：0xFF未启用，0~100通，0xFF断
	unsigned char e3000_V_enable[3];		//集中器3相电压启用状态：0xFF未启用
	unsigned char e3000_A_enable[3];		//集中器3相电流启用状态：0xFF未启用
	unsigned char ext_relay_enable[MAX_EXT_MODULE_NUM][4];	//3个扩展模块各4个继电器启用状态：0xFF未启用，0~100通，0xFF断
	unsigned char  ext_phase_enable[MAX_EXT_MODULE_NUM][12];   	//3个扩展模块各12相启用状态：0xFF未启用
}CONCENTRATOR_PARAMETER;


////////////////////////////////////////////////////////////////////////

//PLC

typedef struct  Lamp_bit_status
{
	unsigned short send_para_bdp_flag:1;    //发送到服务器参数报文 1 已发送
	unsigned short send_online_bdp_flag:1;  //发送到服务器上下线报文 1 已发送
	unsigned short send_status_bdp_flag:1;  //发送到服务器故障报文 1 已发送
	unsigned short read_flag:2;             //当前节点抄读标志位:0未抄读;1已抄读;2抄读成功
	unsigned short lamp_read_status:1;      //节点已经抄读成功过标志
	unsigned short switch_flag:1;           //开关标志位
	unsigned short phase:2;                 //所属相位
	unsigned short status:1;                //存储状态 有无存储节点
	unsigned short group:4;                 //所属组别
	unsigned short lamp_online_flag:2;      //当前节点在线 离线，未知 标志  和GPRS 离线重连后，默认所有单灯都是离线的	
}Lamp_bit_status;



typedef struct  Damp_Info
{
	unsigned char SN[6];	      
	unsigned short loop;			      //3回路状态
	unsigned char ratio[3];         	  //亮度--0xFF表示回路断开			//
	unsigned short voltage;			      //电压
	unsigned short current[3];		      //3回路电流值
	unsigned short active_power[3];	      //3回路有功功率
	unsigned short reactive_power[3];	  //3回路无功功率         
	unsigned char fault_flag;	          //故障标志位 
	Lamp_bit_status m_bit_status;
}Damp_Info;


typedef struct  Send_bdp_FIFO
{
	unsigned char send_counter; //当前报文发送次数
	unsigned char buf[156];   	//缓存发送报文内容
	unsigned char len;          //发送报文长度
	unsigned char type;			//报文类型
}Send_bdp_FIFO;


typedef struct  Send_GPRS_bdp_FIFO
{
   unsigned char send_counter;  //当前报文发送次数
   unsigned char buf[128];   //缓存发送报文内容
   unsigned char len;           //发送报文长度
}Send_GPRS_bdp_FIFO;

///////////////////////////////////////////////

typedef struct Light_Time_Info
{
	unsigned short s_time[8];
	unsigned short e_time[8];
	unsigned short group[8];
	unsigned short loop[8];  
	unsigned char ratio[8][3];
}Light_Time_Info;





//RN8302 

typedef struct  EffectParSetToSave
{

    uint16_t adjust_valid_flag;       //校正系数有效标志   1有效
	uint16_t voltage_adjust_ratio[3]; //三相电压校正系数
	uint16_t current_adjust_ratio[3]; //三相电流校正系数

	
	uint16_t CosH;			//功率因子
	uint16_t CosL;			//功率因子
	uint16_t U_Max;			//电压最大门限
	uint16_t U_Mix;			//电压最小门限
	uint16_t I_Mix;			//电流最小门限
	uint16_t THDu;          //电压谐波总畸变率门限
	uint16_t THDi;          //电流谐波总畸变率门限
	uint16_t CT;            //电流变比
	uint16_t DelayTime;		//延时时间
	uint16_t SystemPassword; //密码		
	uint16_t Addr;       //终端地址码
	uint16_t PW;          //终端密码
	uint16_t BAUD ;       //通信波特率
	uint16_t IP1[4];     //主IP   IP1[0].IP1[1].IP1[2].IP1[3]
	uint16_t Port1;      //主端口
	uint16_t IP2[4];     //辅IP   IP2[0].IP2[1].IP2[2].IP2[3]
	uint16_t Port2;      //辅端口
	uint16_t GSUA;			//电压通道和电流通道的增益调整
	uint16_t GSUB;
	uint16_t GSUC;
	uint16_t GSIA;
	uint16_t GSIB;
	uint16_t GSIC;	
	uint16_t PHSUA;
	uint16_t PHSUB;
	uint16_t PHSUC;
	uint16_t Work_Mode;   //工作模式 0为混补，1为共补

	
}EffectParSetToSave;

typedef struct typPHASE_POWER_ITEM
{
	uint16_t Ux;		//电压值Ua,Ub,Uc
	int16_t Cosx;      //功率因子--补码
	uint32_t Ix;		//电流值Ia,Ib,Ic	
	uint32_t Px;        //有功功率 >   1L<<31   为负数
	uint32_t Qx;        //无功功率 >   1L<<31   为负数
	uint32_t Px_WP_Pos;   //正向
	uint32_t Px_WP_Neg;   //负向
	uint32_t Qx_WP_Pos;
	uint32_t Qx_WP_Neg;
	uint8_t  Ux_Over;   //电压溢出
	uint8_t  Ix_Over;   //电流溢出
	uint8_t  THDu_Over; //电压总畸变率溢出
	uint8_t  THDi_Over; //电流总畸变率溢出
}PHASE_POWER_ITEM;

typedef struct Time_Typedef //  bit7        bit6          bit5          bit4          bit3          bit2          bit1          bit0        
{		
	uint8_t second; //秒钟 ,  =0,晶振工作            bit6-bit4  10second                               bit3-bit0  second
	uint8_t minute; //分钟       无效                bit6-bit4  10minter                               bit3-bit0  minter
	uint8_t hour;   //小时   =0 24小时模式    无效  =1选择20-23小时段 =1选择10-20小时段                bit3-bit0  hour
	uint8_t date;   //日         无效         无效           bit5-bit4  10data                         bit3-bit0  data
	uint8_t month;  //月         无效         无效          无效         10month                       bit3-bit0  month
	uint8_t week;   //星期       无效         无效          无效          无效          无效                    bit2-bit0  month 
	uint8_t  year;   //年	                     bit7-bit4  10year                                        bit3-bit0  year 
}Time_Typedef;


#endif
