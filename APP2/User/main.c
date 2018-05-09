/**
  ******************************************************************************
  * @file   main.c 
  ******************************************************************************
  * @attention
	*

  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "Initialize.h"
#include "includes.h"
#include "APP.h"




void SysTick_Configuration(void);
OS_STK  TASK_START_STK[START_STK_SIZE];

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	/* Add your application code here */
	
	SCB->VTOR = FLASH_BASE | 0x1F000; /* Vector Table Relocation in Internal FLASH. */
	
	MCU_Init();
	
	SysTick_Configuration();   //系统时钟初始化
	

	OSInit();
	
	OSTaskCreate(TaskStart,	   //task pointer
					(void *)0,	       //parameter
					(OS_STK *)&TASK_START_STK[START_STK_SIZE-1],//task stack top pointer
					START_TASK_Prio ); //task priority
	
	OSStart();                 //开始多任务执行	
	return 0;	   
}

//系统时钟配置
void SysTick_Configuration(void)
{
 	SysTick->CTRL&=~(1<<2); //SYSTICK使用外部时钟源
	SysTick->CTRL|=1<<1;    //开启SYSTICK中断
	SysTick->LOAD=10000000/OS_TICKS_PER_SEC;  //由于运行频率是80MHz，所以8分频后为10MHz
	SysTick->CTRL|=1<<0;    //开启SYSTICK
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
