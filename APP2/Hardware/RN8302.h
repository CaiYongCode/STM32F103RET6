#ifndef  _RN8302_H
#define  _RN8302_H

#include "stm32f10x.h"
#include "includes.h"
#include "Initialize.h"

#include "globaldata.h"




#define  Ustand     17600000		   //220V电压输入  理论值为17112760
#define  Istand     15000000	   //5A电流输入      理论值为16777216    
#define  Pstand     15735625       //0.5L时，功率寄存器的标准值 =Ustand*Istand/2^24
#define  Uk         80000 	       //Ustand/220
#define  Ik         3000000        //Istand/5
#define  COSk         8388608        //(1<<23)      P25



#define  Read_Measure     0x00    //读计量寄存器
#define  Write_Measure    0x80	  //写计量寄存器
#define  Read_Configure   0x10 	  //读配置寄存器
#define  Write_Configure  0x90 	  //写配置寄存器



#define X_NORMAL     0x00   //总谐波畸变为正常
#define X_MORE_MAX   0x01    //总谐波畸变超过设定的最大值
#define X_LESS_MIX   0x02    //总谐波畸变低于设定的最小值
//投切保护位
#define NO_PROTECT      0x00000000

#define UA_PROTECT		0x00000001
#define UB_PROTECT		0x00000002
#define UC_PROTECT		0x00000004
#define UT_PROTECT      0x00000008

#define IA_PROTECT		0x00000010
#define IB_PROTECT		0x00000020
#define IC_PROTECT		0x00000040
#define IT_PROTECT      0x00000080

#define THDUA_PROTECT	0x00000100
#define THDUB_PROTECT	0x00000200
#define THDUC_PROTECT	0x00000400
#define THDIA_PROTECT	0x00000800
#define THDIB_PROTECT	0x00001000
#define THDIC_PROTECT	0x00002000
extern 



void Select_RN8302(void);
void NotSelect_RN8302(void);
void RN8302_Init(void);
extern void RN8302_Adjust(void);
void Read_PhasePower_Info(void);
#endif

