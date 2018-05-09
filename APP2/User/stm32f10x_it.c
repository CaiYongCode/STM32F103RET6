/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "globaldata.h"
#include "globalfunc.h"
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//系统时钟中断服务函数
void SysTick_Handler(void)
{
//   OS_CPU_SR  cpu_sr;
	OSIntEnter(); 
//   OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
//   OSIntNesting++;
//   OS_EXIT_CRITICAL();
  OSTimeTick();        /* Call uC/OS-II's OSTimeTick()               */
  OSIntExit();         /* Tell uC/OS-II that we are leaving the ISR  */
}
/************************************************************************
//串口1接收报文拼接处理
*************************************************************************/
void USART1_IRQHandler(void)
{
	unsigned char data;
	OSIntEnter(); 
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
		{
			data = USART_ReceiveData(USART1);
			com1_timeout_count = 10;
			com1_buf[com1_recvbyte_count] = data;
			com1_recvbyte_count++;
			
			if(com1_recvbyte_count > (6*1024))
			{
				com1_recvbyte_count = 0;
			}
	   }
	OSIntExit(); 
}

/************************************************************************
//串口2接收报文拼接处理
*************************************************************************/
void USART2_IRQHandler(void)
{
	unsigned char data;
	OSIntEnter(); 
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 
		{
			USART_ClearITPendingBit(USART2, USART_IT_RXNE);
			data = USART_ReceiveData(USART2);
			com2_timeout_count = 10;
			com2_buf[com2_recvbyte_count] = data;
			com2_recvbyte_count++;
			com2_recvbyte_count &= 0x7f;  //只去127个数，防止缓存溢出
			
	}
	OSIntExit(); 
}


/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : This function handles UART4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART3_IRQHandler(void)  //配置软件232
{
    unsigned char data;
    /*处理接收到的数据*/ 
	OSIntEnter(); 
    if(USART_GetFlagStatus(USART3,USART_IT_RXNE) != RESET) 
    {  
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
        data = USART_ReceiveData(USART3);
        com3_timeout_count = 10;
		com3_buf[com3_recvbyte_count] = data;
		com3_recvbyte_count++;
		com3_recvbyte_count &= 0x7f;  //只去256个数，防止缓存溢出   
    } 

	OSIntExit(); 
}

/*******************************************************************************
* Function Name  : UART4_IRQHandler
* Description    : This function handles UART5 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART4_IRQHandler(void)
{
	 unsigned char data;
    /*处理接收到的数据*/ 
	OSIntEnter(); 
    if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET) 
    { 
        /* Clear the USART1 Receive interrupt */ 
        USART_ClearITPendingBit(UART4, USART_IT_RXNE); 
		data = USART_ReceiveData(UART4);
		com4_timeout_count = 10;
		com4_buf[com4_recvbyte_count] = data;
		com4_recvbyte_count++;
		com4_recvbyte_count &= 0x7f;  //只去256个数，防止缓存溢出   

    } 

	OSIntExit(); 
}



/*******************************************************************************
* Function Name  : UART5_IRQHandler
* Description    : This function handles UART5 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART5_IRQHandler(void)
{
	 unsigned char data;
    /*处理接收到的数据*/ 
	OSIntEnter(); 
    if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) 
    { 
        /* Clear the USART1 Receive interrupt */ 
        USART_ClearITPendingBit(UART5, USART_IT_RXNE); 
		data = USART_ReceiveData(UART5);
		com5_timeout_count = 10;
		com5_buf[com5_recvbyte_count] = data;
		com5_recvbyte_count++;
		com5_recvbyte_count &= 0x7f;  //只去256个数，防止缓存溢出   

    } 

	OSIntExit(); 
}



 /*
********************************************************************************
** 函数名称 ： TIM1_UP_IRQHandler(void)
** 函数功能 ： 定时器1中断
** 输    入	： 10MS 定时器  参数设置在 TIM1_Configuration函数中
** 输    出	： 无
** 返    回	： 无
********************************************************************************
*/ 
//串口1，串口2 缓存
//unsigned char tem_buf1[USART_REC_LEN];
unsigned char tem_buf2[USART_REC_LEN];
unsigned char tem_buf3[USART_REC_LEN];
unsigned char tem_buf4[USART_REC_LEN];
unsigned char tem_buf5[USART_REC_LEN];
void TIM1_UP_IRQHandler(void)
{	
	OSIntEnter(); 
  TIM_ClearITPendingBit(TIM1, TIM_FLAG_Update); //清中断	
/************************************************************************
//确认串口1单帧报文完整后发送消息队列处理
*************************************************************************/
	if(com1_timeout_count > 0)
	{
		com1_timeout_count--;
	}
	if((com1_timeout_count == 0) && (com1_recvbyte_count > 0))
	{
//  		memcpy(tem_buf1,com1_buf,com1_recvbyte_count);
//  		SendMessage(COM_1_MESSAGE_TYPE,com1_recvbyte_count,(INT32U)&tem_buf1[0]);
		SendMessage(COM_1_MESSAGE_TYPE,com1_recvbyte_count,(INT32U)&com1_buf[0]);
		com1_recvbyte_count = 0;
	}
/************************************************************************
//确认串口2单帧报文完整后发送消息队列处理
*************************************************************************/
	if(com2_timeout_count > 0)
	{
		com2_timeout_count--;
	}
	if((com2_timeout_count == 0) && (com2_recvbyte_count > 0))
	{
		memcpy(tem_buf2,com2_buf,com2_recvbyte_count);
		SendMessage(COM_2_MESSAGE_TYPE,com2_recvbyte_count,(INT32U)&tem_buf2[0]);

		com2_recvbyte_count = 0;
	}
/************************************************************************
//确认串口3单帧报文完整后发送消息队列处理
*************************************************************************/
	if(com3_timeout_count > 0)
	{
		com3_timeout_count--;
	}
	if((com3_timeout_count == 0) && (com3_recvbyte_count > 0))
	{
		memcpy(tem_buf3,com3_buf,com3_recvbyte_count);
		SendMessage(COM_3_MESSAGE_TYPE,com3_recvbyte_count,(INT32U)&tem_buf3[0]);

		com3_recvbyte_count = 0;
	}

	/*************************************************************************/
//确认串口4单帧报文完整后发送消息队列处理
/**************************************************************************/
 	if(com4_timeout_count > 0)
 	{
 		com4_timeout_count--;
 	}
 	if((com4_timeout_count == 0) && (com4_recvbyte_count > 0))
 	{
 		memcpy(tem_buf4,com4_buf,com4_recvbyte_count);
 		SendMessage(COM_4_MESSAGE_TYPE,com4_recvbyte_count,(INT32U)&tem_buf4[0]);

 		com4_recvbyte_count = 0;
 	}

	
	
/*************************************************************************/
//确认串口5单帧报文完整后发送消息队列处理
/**************************************************************************/
 	if(com5_timeout_count > 0)
 	{
 		com5_timeout_count--;
 	}
 	if((com5_timeout_count == 0) && (com5_recvbyte_count > 0))
 	{
 		memcpy(tem_buf5,com5_buf,com5_recvbyte_count);
 		SendMessage(COM_5_MESSAGE_TYPE,com5_recvbyte_count,(INT32U)&tem_buf5[0]);

 		com5_recvbyte_count = 0;
 	}
/*************************************************************************/	
	OSIntExit(); 
	
	
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
