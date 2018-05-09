#include "Key.h"
#include "globaldata.h"
#include "globalfunc.h"
#include "IOctl.h"

unsigned char m_keydown_flag = 0;
unsigned char m_key_back = 0;
unsigned char	m_last_keyval = 0;
void Key_Scan(void)
{
	unsigned char m_keyval = 0,m_keyval2 = 0;

	if(OPEN == Get_Key_OK_Status())
	{
		m_keyval = KEY_OK_BTN; //获取KEY_OK_BTN 的值 1
	}
	if(OPEN == Get_Key_RETURN_Status())
	{
		m_keyval = KEY_RETURN_BTN;  //KEY_RETURN_BTN 的值 2
	}
	if(OPEN == Get_Key_SET_Status())
	{
		m_keyval = KEY_SET_BTN; //KEY_SET_BTN 的值 3
	}
	if(OPEN == Get_Key_FUNCTION_Status())
	{
		m_keyval = KEY_FUNCTION_BTN; //获取KEY_FUNCTION_BTN 的值 4
	}

	//和之前状态比较  按键防抖动
	OSTimeDly(10);  //20ms延时 防抖动
	//确认当前是否有按键响应，并获取按键值
   
	if(OPEN == Get_Key_OK_Status())
	{
		m_keyval2 = KEY_OK_BTN; //获取KEY_OK_BTN 的值 1
	}
	if(OPEN == Get_Key_RETURN_Status())
	{
		m_keyval2 = KEY_RETURN_BTN;  //KEY_RETURN_BTN 的值 2
	}
	if(OPEN == Get_Key_SET_Status())
	{
		m_keyval2 = KEY_SET_BTN; //KEY_SET_BTN 的值 3
	}
	if(OPEN == Get_Key_FUNCTION_Status())
	{
		m_keyval2 = KEY_FUNCTION_BTN; //获取KEY_FUNCTION_BTN 的值 4
	}
	if((m_keyval2 == m_keyval)&&(m_keyval > 0))
	{
		m_keydown_flag = 1; 
		m_last_keyval = m_keyval;   //获取确实按下的键值
	}
   

	//当按下的键弹起的时候
	// m_key_back = 0;  //按键弹起标志
	if(m_keydown_flag == 1)
	{
		switch(m_last_keyval)
		{
			case KEY_OK_BTN:
				if(CLOSE == Get_Key_OK_Status())
				{	
					m_key_back = 1;
				}
				break;
			case KEY_RETURN_BTN:
				if(CLOSE == Get_Key_RETURN_Status())
				{	
					m_key_back = 1;
				}
				break;
			case KEY_SET_BTN:
				if(CLOSE == Get_Key_SET_Status())
				{	
					m_key_back = 1;
				}
				break;
			case KEY_FUNCTION_BTN:
				if(CLOSE == Get_Key_FUNCTION_Status())
				{	
					m_key_back = 1;
				}
				break;
			default:
				break;
		}
	}
   
	//如果按键响应发送消息到主消息队列
	if((m_last_keyval > 0) && (m_key_back  == 1))
 	{		
		SendMessage(KEY_MESSAGE_TYPE,1,(INT32U)m_last_keyval);
		m_last_keyval = 0;
		m_keyval2 = 0;
		m_keyval = 0;
		m_keydown_flag = 0;
		m_key_back = 0; 	  
 	}
}

/********************key IO口 状态读取***************************/

//确定
unsigned char Get_Key_OK_Status(void)
{
	return GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5);
}
//返回
unsigned char Get_Key_RETURN_Status(void)
{
	return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1);
}
//设置
unsigned char Get_Key_SET_Status(void)
{
	return	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);
}
//功能
unsigned char Get_Key_FUNCTION_Status(void)
{
	return GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3);
}


