#ifndef _DS1302_H
#define _DS1302_H

#include "stm32f10x.h"
#include "includes.h"
#include "Initialize.h"
#include "struct.h"
#include "const.h"

#define Sec_Add_W          0x80        //写秒地址
#define Sec_Add_R          0x81        //读秒地址
#define Min_Add_W          0x82        //写分地址
#define Min_Add_R          0x83        //读分地址
#define Hour_Add_W          0x84        //写小时地址
#define Hour_Add_R          0x85        //读小时地址
#define Data_Add_W          0x86        //写日地址
#define Data_Add_R          0x87        //读日地址
#define Month_Add_W          0x88        //写月地址
#define Month_Add_R          0x89        //读月地址
#define Week_Add_W          0x8a        //写星期地址
#define Week_Add_R          0x8b        //读星期地址
#define Year_Add_W          0x8c        //写年地址
#define Year_Add_R          0x8d        //读年地址
#define Protect_Add_W          0x8e        //写写保护地址
#define Protect_Add_R          0x8f        //读写保护地址
#define Charge_Add_W          0x90        //写充电地址
#define Charge_Add_R          0x91        //读充电地址

#define BeiFen_Add_W           0xc4        //备份RAM，用来检测是否掉过电
#define BeiFen_Add_R           0xc5 

#define Open_Write_Protect               0x80        //打开写保护
#define Close_Write_Protect              0x00        //关闭写保护
#define Close_Charge                     0x00        //关闭充电功能



#define Set_DS1302_RST_High()           GPIO_SetBits(GPIOB, GPIO_Pin_12)
#define Set_DS1302_RST_Low()            GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define Set_DS1302_CLK_High()           GPIO_SetBits(GPIOB, GPIO_Pin_13)
#define Set_DS1302_CLK_Low()            GPIO_ResetBits(GPIOB, GPIO_Pin_13)
#define Set_DS1302_SIO_High()           GPIO_SetBits(GPIOB, GPIO_Pin_15)
#define Set_DS1302_SIO_Low()            GPIO_ResetBits(GPIOB, GPIO_Pin_15)

#define Read_DS1302_SIO()               GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15)      



//extern Time_Typedef TimeValue_Init;     //用于掉电后的初始化

void DS1302_Init(void);
void DS1302_GetTime(Time_Typedef *PP);
void Set_RTC_Time(void);
void Get_RTC_Time(void);
unsigned char WeekDay(unsigned char year, unsigned char month, unsigned char day) ;
#endif






