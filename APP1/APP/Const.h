#ifndef _CONST_H
#define _CONST_H



//注释 DEBUG_MODE  不执行
//进入调试模式 则要释放
//#define DEBUG_MODE  1


//使用工作服务器  如果不定义 则使用调试服务器 
//#define WORKING_SERVER_IP

#define  TRUE     1
#define  FALSE    0

#define  OPEN     1
#define  CLOSE    0

#define  SIM900   1
#define  EC20     2

#define  ONLINE      1
#define  OFFLINE     0

#define  LAMP_STATE_ONLINE      2
#define  LAMP_STATE_OFFLINE     1
#define  LAMP_STATE_UNKNOW      0

//小数位数
#define	 DECIMAL0	0
#define	 DECIMAL1	1
#define	 DECIMAL2	2
///////////////////////////////////////////////
#define	SOFTWARE_VERSION	0x180209		//软件版本
///////////////////////////////////////////////
#define  AC_POWER_SUPPLY          0
#define  BATTERY_POWER_SUPPLY     1
///////////////////////////////////////////////
#define VOLTAGE_ALW  20     //电压容差范围 故障判断用

#define VOLTAGE_THRESHOLD  36     //电压有无判断门槛
#define CURRENT_THRESHOLD  10     //电流有无判断门槛
/////////////////////////////////////////////////////////////////
//工作模式
#define WEEK_MODE	    1
#define MONTH_MODE	    2
#define OFFLINE_MODE	3   //服务器下发离线工作模式
////////////////////////////////////////////////////////////////
#define  KEY_OK_BTN          1  //按键序号
#define  KEY_RETURN_BTN      2
#define  KEY_SET_BTN         3
#define  KEY_FUNCTION_BTN    4
////////////////////////////////////////////////////////////////
#define DGUS_unregistered_SHOW_ADDR     1	//未注册界面
#define DGUS_STATE_SHOW_ADDR          	2 	//状态显示界面id
#define DGUS_DAMP_MANAGE_SHOW_ADDR      3 	//节点管理界面id
#define DGUS_FUNCTION_SET_SHOW_ADDR		4	//功能设置界面id
#define DGUS_TIME_SET_SHOW_ADDR			5	//设置时间界面id
#define DGUS_LAMP_SET_SHOW_ADDR      	6	//单灯设置界面id
#define DGUS_DAMP_READ_SHOW_ADDR      	7	//节点抄读界面id
#define DGUS_MANUAL_CONTROL_SHOW_ADDR   8 	//手动控制界面id
#define DGUS_EXPANSION_SET_SHOW_ADDR    9	//扩展设置界面id
#define DGUS_EXPANSION1_SET_SHOW_ADDR   10	//扩展1设置界面id
#define DGUS_EXPANSION2_SET_SHOW_ADDR   11	//扩展2设置界面id
#define DGUS_EXPANSION3_SET_SHOW_ADDR   12	//扩展3设置界面id
#define DGUS_DAMP_SHOW_ADDR         	13	//节点显示界面id
#define DGUS_PROMPT_SHOW_ADDR         	14	//提示界面id
#define DGUS_CONCENTRATOR_SET_SHOW_ADDR 15	//集中器设置界面id
#define DGUS_PRINT_SHOW_ADDR   			16	//打印界面id
#define DGUS_SET_DAMP_ADDR_SHOW_ADDR    17	//设置单灯地址界面id
#define DGUS_PASSWORD_INPUT_SHOW_ADDR	19	//密码输入界面id
#define DGUS_LOOP_SHOW_ADDR				20  //回路信息显示界面
///////////////////////////////////////////////////////////////
//单灯 载波相关
#define INVALID_LAMP_ADDR    1
#define LAMP_ADDR_FULL       2
#define ADD_LAMP_ADDR_SUCESS 3
#define DEL_LAMP_ADDR_SUCESS 4

#define PLC_MSG_TYPE_1		1	//载波报文类型:	硬件初始化
#define PLC_MSG_TYPE_2		2	//载波报文类型:	参数区初始化
#define PLC_MSG_TYPE_3		3	//载波报文类型: 重启抄读
#define PLC_MSG_TYPE_4		4	//载波报文类型:	暂停抄读
#define PLC_MSG_TYPE_5		5	//载波报文类型: 恢复抄读
#define PLC_MSG_TYPE_6		6	//载波报文类型: 设置主节点地址
#define PLC_MSG_TYPE_7		7	//载波报文类型：添加从节点
#define PLC_MSG_TYPE_8		8	//载波报文类型：删除从节点
#define PLC_MSG_TYPE_9		9	//载波报文类型:	监控从节点
#define PLC_MSG_TYPE_10		10	//载波报文类型: 设置单灯地址
#define PLC_MSG_TYPE_11		11	//载波报文类型: 设置单灯组别
#define PLC_MSG_TYPE_12		12	//载波报文类型: 单灯开关
#define PLC_MSG_TYPE_13		13	//载波报文类型: 同步系统时间
#define PLC_MSG_TYPE_14		14	//载波报文类型: 同步工作时间
#define PLC_MSG_TYPE_15		15	//载波报文类型: 设置单灯工作模式
#define PLC_MSG_TYPE_16		16	//载波报文类型: 设置单灯回路顺序
///////////////////////////////////////////////////////////////
//GPRS自定义报文类型
#define CUSTOM_MSG_TYPE_1	1	//上报回路开关命令
#define CUSTOM_MSG_TYPE_2	2	//上报单灯开关命令
#define CUSTOM_MSG_TYPE_3	3	//时钟超差，发送系统时间到服务器
#define CUSTOM_MSG_TYPE_4   4   //上报工作时间
///////////////////////////////////////////////////////////////
#define MAX_EXT_MODULE_NUM    3   //e3000带的最大扩展模块个数
///////////////////////////////////////////////////////////////
#define PWM_Counter     59999


#define RTU_ALREADY_CONFIG_FLAG        0xD5   //RTU已注册标志

///////////////////////////////////////////////////////////////
//A区程序地址范围0x08010000~0x08037FFF,共160K
//B区程序地址范围0x08038000~0x0805FFFF,共160K
#define SPACE1			1
#define SPACE2			2
#define FLASH_APP1_ADDR		0x08005000  	//第1个应用程序起始地址(存放在FLASH)
											
#define FLASH_APP2_ADDR		0x0801F000  	//第2个应用程序起始地址(存放在FLASH)
///////////////////////////////////////////////////////////////

//数据区地址范围0x08060000~0x0807FFFF,从后向前存储

//启动相关标志1页
#define APP_START_FLAG_ADDR			0x807F800			//APP启动标志	1为1区,2为2区
#define APP_RUN_FLAG_ADDR			(0x807F800+2)		//APP运行标志,0xAA为失败,0xBB为成功
#define APP_UPDATE_FLAG_ADDR		(0x807F800+4)			//升级标志，1标识需要升级                                                                                                                 

//8302信息1页
#define RTU_RN8302_INFO_START_ADDR  			0x807F000
//E3000配置参数1页
#define RTU_CONFIG_INFO_START_ADDR				0x807E800
#define RTU_CONFIG_FLAG_ADDR					0x807E800
#define RTU_SN_START_ADDR						(0x807E800+2)
#define RTU_SOFTWARE_VERSION_START_ADDR			(0x807E800+18)
#define RTU_SIMCARD_NUM_START_ADDR				(0x807E800+26)
#define RTU_ROUTE_ADDR_START_ADDR				(0x807E800+54)
#define RTU_IP_SET_ADDR_START_ADDR				(0x807E800+66)
#define RTU_IP_LEN_ADDR_START_ADDR				(0x807E800+68)
#define RTU_IP_ADDR_START_ADDR					(0x807E800+70)
#define RTU_PORT_SET_ADDR_START_ADDR			(0x807E800+134)
#define RTU_PORT_LEN_ADDR_START_ADDR			(0x807E800+136)
#define RTU_PORT_ADDR_START_ADDR				(0x807E800+138)
#define RTU_WORKING_MODE_ADDR					(0x807E800+148)
#define DAMP_WORKING_MODE_ADDR					(0x807E800+150)
//回路启用配置信息1页
#define RTU_CONCENTRATOR_PARA_START_ADDR		0x807E000
//周工作数据1页
#define RTU_WEEK_LIGHT_TIME_START_ADDR          0x807D800
//月工作数据24页
#define RTU_MONTH_LIGHT_TIME_START_ADDR       	0x807D000
//单灯信息2页
#define DAMP_NUM_ADDR                      		0x807C800
#define DAMP_INFO_DATA_START_ADDR          		(0x807C800+2)
#define DAMP_INFO_DATA_START_ADDR2          	0x807C000	

//命令保存地址1页
#define CMD_SAVE_ADDR							0x807B8000



#endif
