#include "globaldata.h"
#include "DS1302.h"

uint8_t Test_DiaoDian=0x00;
Time_Typedef TimeValue_Init = {0x30,0x04,0x10,0x16,0x12,0x05,0x16};

uint8_t ChargeOff=0x00;
void Ds1302_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB ,ENABLE);

  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12 | GPIO_Pin_13;   //GPIO_Pin_12为复位脚，GPIO_Pin_13为时钟脚
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;   //数据位，双向端口，输入输出时需切换模式
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOB,&GPIO_InitStructure);
  
}

void SIO_Input(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB ,ENABLE);
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	 GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOB,&GPIO_InitStructure);
}
void SIO_Output(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB ,ENABLE);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

void Delay( uint32_t t)
{
  uint8_t T=10;
  while(T--);
  while(t--);

}

void DS1302_Reset(void)
{
  Set_DS1302_RST_Low();
  Set_DS1302_CLK_Low();
  Delay(30);
  Set_DS1302_RST_High();
	Delay(30);
}
void DS1302_WriteByte(uint8_t byte)
{
  uint8_t i=8;
  uint8_t mask=0x01;
  SIO_Output();
  while(i--)
  {
		Set_DS1302_CLK_Low();
		if(byte&mask)
		{
		  Set_DS1302_SIO_High();
		}
		else
		{
		  Set_DS1302_SIO_Low() ;
		}
		mask<<=1;
		Delay(30);
		Set_DS1302_CLK_High();
		Delay(30);
  
  }
}

uint8_t DS1302_ReadByte(void)
{
	uint8_t i=8;
	uint8_t Data=0;
	SIO_Input();
	while(i--)
	{

		Data>>=1;
		Set_DS1302_CLK_High();
		Delay(30);
		Set_DS1302_CLK_Low();
		Delay(30);

		if(Read_DS1302_SIO())
		{
			Data|=0x80;
		}
	}
	Delay(30);
	Set_DS1302_CLK_High();
	return Data;
}
// void CloseCharge(void)
// {
// 	
// 	DS1302_CWP();
//   DS1302_Reset();
// 	DS1302_WriteByte(Protect_Add_W);
// 	Delay(300);
// 	DS1302_WriteByte(Open_Write_Protect);
// 	Delay(300);
// 	Set_DS1302_CLK_Low();
// 	
// 	
// }


/*----------------读取DS1302的有效数据---------------
------------------address为所要读取寄存器的地址，P为数据保存地址---*/
void DS1302_ReadData(uint8_t address,uint8_t *P)
{
	DS1302_Reset();
  DS1302_WriteByte(address);
  Delay(300);
	*P=DS1302_ReadByte();
	Delay(300);
  Set_DS1302_RST_Low() ;
}
/*----------------往DS1302写有效数据---------------
------------------address为所要写的寄存器的地址，P为写入的数据---*/
void DS1302_WriteData(uint8_t address,uint8_t *P)
{
	DS1302_Reset();
  DS1302_WriteByte(address);
  Delay(300);
  DS1302_WriteByte(*P);
	Delay(300);
  Set_DS1302_RST_Low() ;
}
/*---------------设置写保护位----------------*/
void DS1302_WP(void)
{
	DS1302_Reset();
	DS1302_WriteByte(Protect_Add_W);
	Delay(300);
	DS1302_WriteByte(Open_Write_Protect);
	Delay(300);
	Set_DS1302_RST_Low() ;
//	Set_DS1302_CLK_Low();
	
	
}
/*------------------清除写保护位---------------*/
void DS1302_CWP(void)
{
  DS1302_Reset();
  DS1302_WriteByte(Protect_Add_W);
  Delay(300);
  DS1302_WriteByte(Close_Write_Protect);
  Delay(300);
	Set_DS1302_RST_Low() ;
//  Set_DS1302_CLK_Low();

}
void DS1302_GetTime(Time_Typedef *PP)
{
	DS1302_CWP();
	Delay(300);
  DS1302_ReadData(Sec_Add_R,&(PP->second));
  Delay(300);
	DS1302_ReadData(Min_Add_R,&(PP->minute));
  Delay(300);
	DS1302_ReadData(Hour_Add_R,&(PP->hour));
  Delay(300);
	DS1302_ReadData(Data_Add_R,&(PP->date));
  Delay(300);
	DS1302_ReadData(Month_Add_R,&(PP->month));
  Delay(300);
	DS1302_ReadData(Week_Add_R,&(PP->week));
  Delay(300);
	DS1302_ReadData(Year_Add_R,&(PP->year));
  Delay(300);
	DS1302_WP();
  Delay(300);
}
void DS1302_SetTime(Time_Typedef *PP)
{
	DS1302_CWP();
	Delay(300);
  DS1302_WriteData(Sec_Add_W,&(PP->second));
	Delay(300);
	DS1302_WriteData(Min_Add_W,&(PP->minute));
	Delay(300);
	DS1302_WriteData(Hour_Add_W,&(PP->hour));
	Delay(300);
	DS1302_WriteData(Data_Add_W,&(PP->date));
	Delay(300);
	DS1302_WriteData(Month_Add_W,&(PP->month));
	Delay(300);
	DS1302_WriteData(Week_Add_W,&(PP->week));
	Delay(300);
	DS1302_WriteData(Year_Add_W,&(PP->year));
	Delay(300);		
	Test_DiaoDian=0x5a;
	DS1302_WriteData(BeiFen_Add_W,&Test_DiaoDian);
	Delay(300);
	DS1302_WP();
  Delay(300);
}
void DS1302_Init(void)
{
	Ds1302_GPIO_Config();
//	DS1302_WriteData(Charge_Add_W,&ChargeOff);
	DS1302_CWP();
	Delay(300);
	DS1302_ReadData(Year_Add_R,&(g_DS1302_TimeValue.year));
	DS1302_WP();
  Delay(300);
	DS1302_GetTime(&g_DS1302_TimeValue);
	DS1302_CWP();
	Delay(300);
	DS1302_ReadData(BeiFen_Add_R,&Test_DiaoDian);
	DS1302_WP();
  Delay(300);
//	if(Test_DiaoDian!=0x5a)    //掉过电
	{
	//	DS1302_SetTime(&TimeValue_Init);
    }
}

//year 范围2000-2100
void Set_RTC_Time(void)
{	
	TimeValue_Init.year   = (g_set_year/10)* 16 + g_set_year % 10;
	TimeValue_Init.month  = (g_set_month/10)* 16 + (g_set_month % 10);
	TimeValue_Init.date   = (g_set_day/10)* 16 + (g_set_day % 10);
	TimeValue_Init.minute = (g_set_min/10)* 16 + (g_set_min % 10);
	TimeValue_Init.hour   = (g_set_hour/10)* 16 + (g_set_hour % 10);
	TimeValue_Init.second = (g_set_sec/10)* 16 + (g_set_sec % 10);;
	TimeValue_Init.week   =  WeekDay(g_set_year,g_set_month,g_set_day);

	DS1302_SetTime(&TimeValue_Init);
}

//获取DS1302实时时间
void Get_RTC_Time(void)
{
	DS1302_GetTime(&g_DS1302_TimeValue);
	
	g_rtc_year = (g_DS1302_TimeValue.year/16)* 10 + (g_DS1302_TimeValue.year % 16);
	g_rtc_month = (g_DS1302_TimeValue.month/16)* 10 + (g_DS1302_TimeValue.month % 16);
	g_rtc_day = (g_DS1302_TimeValue.date/16)* 10 + (g_DS1302_TimeValue.date % 16);
	g_rtc_hour = (g_DS1302_TimeValue.hour/16)* 10 + (g_DS1302_TimeValue.hour % 16);
	g_rtc_min = (g_DS1302_TimeValue.minute/16)* 10 + (g_DS1302_TimeValue.minute % 16);
	g_rtc_sec = (g_DS1302_TimeValue.second/16)* 10 + (g_DS1302_TimeValue.second % 16);
	g_rtc_week = g_DS1302_TimeValue.week;

}

unsigned char WeekDay(unsigned char year, unsigned char month, unsigned char day) 
{ 
	unsigned char week;
	if (month==1||month==2)
	{
		month+=12;
		year--; 
	}
	week= (day+2*month+3* (month+1) /5+year+year/4-year/100+year/400+1) %7;
	
	if(week == 0)
	{
		week = 7;
	}
	
	return week;
}
