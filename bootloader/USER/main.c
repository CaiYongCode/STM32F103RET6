#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "stmflash.h"
#include "iap.h"
#include "wdg.h"


int main(void)
{	
	unsigned short update_flag = 0;
	unsigned short app_start_flag = 0;
	unsigned short app_run_flag = 0;
	unsigned short flag[3] = {0};
	
// 	uart_init(9600);	//串口初始化为9600
// 	delay_init();	   	 	//延时初始化 
	
	STMFLASH_Read(APP_UPDATE_FLAG_ADDR,&update_flag,1);

	STMFLASH_Read(APP_START_FLAG_ADDR,&app_start_flag,1);
	STMFLASH_Read(APP_RUN_FLAG_ADDR,&app_run_flag,1);


	if(app_run_flag != 0xBB)		//应用程序运行失败或者初始状态
	{
		if(app_start_flag != 1)		//2区应用程序运行失败或者初始状态
		{
			if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
			{	
				flag[0] = 0x01;
				flag[1] = 0xAA;
				flag[2] = update_flag;
				STMFLASH_Write(APP_START_FLAG_ADDR,flag,3);					
				
				iap_load_app(FLASH_APP1_ADDR);//执行APP1代码			
			}
		}
		else		//1区应用程序运行失败
		{
			if(((*(vu32*)(FLASH_APP2_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
			{	 
				flag[0] = 0x02;
				flag[1] = 0xAA;
				flag[2] = update_flag;
				STMFLASH_Write(APP_START_FLAG_ADDR,flag,3);
				
				iap_load_app(FLASH_APP2_ADDR);//执行APP2代码			
			}
		}
	}
	else
	{	
		if(app_start_flag == 1)	//启动1区程序
		{
			if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
			{	 
				flag[0] = 0x01;
				flag[1] = 0xAA;
				flag[2] = update_flag;
				STMFLASH_Write(APP_START_FLAG_ADDR,flag,3);
				
				iap_load_app(FLASH_APP1_ADDR);//执行APP1代码					
			}
		}
		else if(app_start_flag == 2)	//启动2区程序
		{
			if(((*(vu32*)(FLASH_APP2_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
			{	 
				flag[0] = 0x02;
				flag[1] = 0xAA;
				flag[2] = update_flag;
				STMFLASH_Write(APP_START_FLAG_ADDR,flag,3);			
				
				iap_load_app(FLASH_APP2_ADDR);//执行APP2代码	
			}
		}
	}
}













