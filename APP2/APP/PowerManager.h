#ifndef _POWERMANGER_H
#define _POWERMANGER_H
#include "stm32f10x.h"
#include "includes.h"
#include "Initialize.h"

#include "globaldata.h"
#include "globalfunc.h"



//初始化 电源管理管脚设置
void Init_powermanager_pin_config(void);
//电池状态  充电过程中   充满

unsigned char get_battery_status(void);

//得到供电通道信息   交流供电   电池供电  周期调用 供检测
unsigned char get_power_supply_channel(void);

//电池供电下相关设置
void set_battery_supply_mode(void);

//交流供电模式下相关设置 
void set_AC_supply_mode(void);

//检查供电方式 并处理
void check_power_supply(void);

#endif
