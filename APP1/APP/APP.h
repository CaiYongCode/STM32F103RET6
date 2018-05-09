#ifndef _APP_H
#define _APP_H
#include "stm32f10x.h"
#include "includes.h"
#include "Initialize.h"
#include "globaldata.h"
/************************************************************************************************
**任务的优先级声明
************************************************************************************************/
#define START_TASK_Prio              18       //起始任务
#define RTU_DEMO_TASK_Prio           50       //RTU任务


#define Timer_1000ms_TASK_Prio       20       	//1000ms定时任务
#define Timer_2000ms_TASK_Prio       21       	//2000ms定时任务
#define MsgProc_TASK_Prio            22       	//消息队列响应任务
#define TCP_RECONNETC_TASK_Prio      23     	//GPRS注册任务
#define Key_Scan_TASK_Prio           24     	//按键扫描任务
/************************************************************************************************
**任务堆栈容量声明（字节）
************************************************************************************************/
#define TIMER_2000MS_STK_SIZE        256      //2000MS定时任务
#define TIMER_1000MS_STK_SIZE        512      //1000MS定时任务
#define START_STK_SIZE               256       //起始任务
#define MSG_PROC_SIZE             	 512        //消息队列任务
#define KEY_SCAN_STK_SIZE            128      //按键扫描定时任务
#define TCP_RECONNECT_STK_SIZE       256      //tcp定时检查重连任务
////////////////////////////////////////////////////////////////////////////////////////


/************************************************************************************************
**任务声明
************************************************************************************************/
void TaskStart(void * pdata);                 //起始任务


void Task_COM_PROC(void *pdata);              //串口处理任务


void System_App_Init(void);



#endif
