#include "BIT.h"
#include "struct.h"
#include "IOctl.h"
#include "PowerManager.h"
#include "dacai_touch.h"
#include "GPRS.h"
/////////////////////////////////////////////////////////////////////
//初始化 电源管理管脚设置  初始化时调用
//PC6--UPS SUPPLY     PC8--UPS CHARGE 
void Init_powermanager_pin_config(void)
{

	GPIO_InitTypeDef GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA ,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_8 ;     //       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //
	GPIO_Init(GPIOC, &GPIO_InitStructure);   

	
}
/////////////////////////////////////////////////////////////////////
//电池状态  充电过程中   充满
unsigned char get_battery_status(void)
{
     //
	return 0;
}

/////////////////////////////////////////////////////////////////////
//得到供电通道信息   交流供电   电池供电  周期调用 供检测
//1----交流供电   0---电池供电
unsigned char get_power_supply_channel(void)
{
	unsigned char m_Val = 0;
	m_Val = GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6);
	return m_Val;
}
//////////////////////////////////////////////////////////////////////
//电池供电下相关设置
void set_battery_supply_mode()
{
}
//////////////////////////////////////////////////////////////////////
//交流供电模式下相关设置 
void set_AC_supply_mode()
{
}


//检查供电方式 并处理
void check_power_supply()
{
  unsigned char ret = 0;  	
	ret = get_power_supply_channel();
	if(AC_POWER_SUPPLY == ret)
	{
		if(g_Concentrator_Para.power_supply == BATTERY_POWER_SUPPLY)
		{
			show_power_supply(0);		//供电图标显示交流
			adjust_TS_backlight(0);//打开屏的背光 
			send_concentrator_fault_release_bdp(CONCENTRATOR_FAULT_4,0);	//上报服务器
			g_Concentrator_Para.power_supply = AC_POWER_SUPPLY;
		}
	}
	else if(BATTERY_POWER_SUPPLY == ret)
	{
	//电池供电  关闭继电器(包括扩展模块)   把屏的背光关闭  
 		if(g_Concentrator_Para.power_supply == AC_POWER_SUPPLY)
		{
		    show_power_supply(1);			//供电图标显示电池
		    
#ifdef  DEBUG_MODE		
			adjust_TS_backlight(0xFF);
#endif				
			send_concentrator_fault_bdp(CONCENTRATOR_FAULT_4,0);	//上报服务器
			g_Concentrator_Para.power_supply = BATTERY_POWER_SUPPLY;
		}
	}
}
