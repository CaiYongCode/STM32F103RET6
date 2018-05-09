#include "IOctl.h"
#include "globaldata.h"

void  Init_GPIO_config(void)
{
   //初始化  初始化时调用
   // PC9--SWITCH CHECK    PA8--SWITCHCTL
    GPIO_InitTypeDef GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA ,ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;     //       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //
	GPIO_Init(GPIOC, &GPIO_InitStructure);  
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;       //    
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

////////////////////////////////////////////////////////////////////
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;   //RS485 控制管脚        
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_3);	 //使能接收 485

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;   //RS485_1 控制管脚    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_SetBits(GPIOA, GPIO_Pin_15);   //使能接收 RS485_1

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;   //GPRS_RST      
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA, GPIO_Pin_11);   //关

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;   //8302_RST      
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_14);   //置高
	////////////////////////////////////////////////////////////////////
	//KEY
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;   //KEY 4   确定
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;   //KEY 3   返回
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;   //KEY 2   设置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;   //KEY 1	 功能
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

////////////////////////////////////////////////////////////////////
	//LED
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;   //LED7 	 正常
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_2);   //关
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;   //LED6   亮灯
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_1);   //关
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;   //LED5     故障
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_0);   //关
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;   //LED4    调试
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_15);   //关
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;   //LED3  	GPS 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_14);   //关
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;   //LED2    通信
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_13);   //关

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;   //LED1   运行   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_4);   //关
	
//////////////////////////////////////////////////////////////////////////////
	//继电器
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;   //C_K1   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_8);   //关

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;   //C_K2	 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_9);   //关
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;   //C_K3   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_5);   //关
}
//GPRS复位
void Reset_GPRS_Module(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_11);
	OSTimeDly(200);
	GPIO_SetBits(GPIOA, GPIO_Pin_11);
	OSTimeDly(200);
}
//8302复位
void Reset_8302_Module(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_14);
	OSTimeDly(200);
	GPIO_SetBits(GPIOB, GPIO_Pin_14);
	OSTimeDly(100);
}

//运行
void Set_RUN_LED_Status(unsigned char status)
{
	if(status == CLOSE)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_4);
	}
	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_4);
	}
}

//通信
void Set_GPRS_LED_Status(unsigned char status)
{
	if(status == CLOSE)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_13);
	}
	else
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	}
}

//PLC
void Set_PLC_LED_Status(unsigned char status)
{
	if(status == CLOSE)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_14);
	}
	else
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_14);
	}
}

//调试	
void Set_DEBUG_LED_Status(unsigned char status)
{
	if(status == CLOSE)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_15);
	}
	else
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_15);
	}
}

//正常	//单灯全部正常时亮
void Set_NORMAL_LED_Status(unsigned char status)
{
	if(status == CLOSE)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_2);
	}
	else
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_2);
	}
}

//亮灯  有单灯亮时亮
void Set_BRIGNT_LED_Status(unsigned char status)
{
	if(status == CLOSE)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_1);
	}
	else
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_1);
	}
}

//故障	有单灯故障时亮
void Set_FAULT_LED_Status(unsigned char status)
{
	if(status == CLOSE)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_0);
	}
	else
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_0);
	}
}

///////////////////////////////////////////////////////////////////////////////
// //继电器1
// void Fhase_A_Ctl(unsigned char state)
// {
// 	if(state == CLOSE)
// 	{
// 		GPIO_SetBits(GPIOB, GPIO_Pin_8);   //继电器响应		   
// 	}
// 	else
// 	{
// 		GPIO_ResetBits(GPIOB, GPIO_Pin_8);
// 	}
// }

// //继电器2
// void Fhase_B_Ctl(unsigned char state)
// {
// 	if(state == CLOSE)
// 	{
// 		GPIO_SetBits(GPIOB, GPIO_Pin_9);   //继电器响应	   
// 	}
// 	else
// 	{
// 		GPIO_ResetBits(GPIOB, GPIO_Pin_9);
// 	}
// }

// //继电器3
// void Fhase_C_Ctl(unsigned char state)
// {
// 	if(state == CLOSE)
// 	{
// 		GPIO_SetBits(GPIOB, GPIO_Pin_5);    //继电器响应
// 	}
// 	else
// 	{
// 		GPIO_ResetBits(GPIOB, GPIO_Pin_5);
// 	}
// }
//E3000自身3路继电器统一控制
void E3000_Relay_Ctl(unsigned char state)
{
	if(state == CLOSE)	//关
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_8);	//A
		GPIO_SetBits(GPIOB, GPIO_Pin_9);	//B
		GPIO_SetBits(GPIOB, GPIO_Pin_5);    //C
	}
	else	//开
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_8);	//A
		GPIO_ResetBits(GPIOB, GPIO_Pin_9);	//B
		GPIO_ResetBits(GPIOB, GPIO_Pin_5);    //C
	}
}

// //获取限位开关信息  注意开关是常闭还是常开 ，状态不一样
// unsigned char get_limit_switch_status(void)
// {
//     unsigned char val = 0;
//     val = GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9);
//     return val;
// }

// //检查箱门状态 并处理
// void check_box_door_status()
// {
//     unsigned char ret = 0;
// 	ret = get_limit_switch_status();
// 	if(OPEN == ret)  //箱门已打开
// 	{
// 	   
// 	}
// 	else
// 	{
// 		
// 	}
// }


/********************************************************************************************************** 
** Function name:      Plc_Led_Ctl
** Descriptions:       载波通信指示灯 发送亮灭, 接收亮灭交替  
** input parameters:   void *pdata
** output parameters:  无
** Returned value:     无
**********************************************************************************************************/
unsigned char counter = 0;
void Plc_Led_Ctl(void)
{
	if(plc_comm_flag == 0)			//无通信,灭
	{
		Set_PLC_LED_Status(CLOSE);  
	}
	else if(plc_comm_flag == 1)			//发送,亮一次
	{
		Set_PLC_LED_Status(OPEN);  
		plc_comm_flag = 0;
	}
	else if(plc_comm_flag == 2)		//接收,亮灭交替2次
	{		
		for(counter = 0;counter<2;counter++)
		{
			Set_PLC_LED_Status(OPEN);
			OSTimeDly(100);
			Set_PLC_LED_Status(CLOSE);
			OSTimeDly(100);
		}
		plc_comm_flag = 0;
	}
}
