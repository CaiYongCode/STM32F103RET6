#ifndef __IAP_H__
#define __IAP_H__
#include "sys.h"  

typedef  void (*iapfun)(void);				//定义一个函数类型的参数.

//保留0X08000000~0X0800FFFF的空间为IAP使用
//启动相关标志1页
#define APP_START_FLAG_ADDR			0x807F800			//APP启动标志	1为1区,2为2区
#define APP_RUN_FLAG_ADDR			(0x807F800+2)		//APP运行标志,0xAA为失败,0xBB为成功
#define APP_UPDATE_FLAG_ADDR		(0x807F800+4)			//升级标志，1标识需要升级

#define FLASH_APP1_ADDR		0x08005000  	//第一个应用程序起始地址(存放在FLASH)
											
#define FLASH_APP2_ADDR		0x0801F000  	//第一个应用程序起始地址(存放在FLASH)
																						
void iap_load_app(u32 appxaddr);			//执行flash里面的app程序

//void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 applen);	//在指定地址开始,写入bin

#endif







































