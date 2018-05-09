#include "RN8302.h"
#include "globalfunc.h"
#include "math.h"
#include "globaldata.h"

uint8_t RN8302_WriteSPI_Byte(uint8_t byte)
{
	/* Loop while DR register in not emplty */
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	/* Send byte through the SPI2 peripheral */
	SPI_I2S_SendData(SPI1, byte);	
	/* Wait to receive a byte */
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);	
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI1);
}
uint8_t RN8302_ReadSPI_Byte(void)
{
	return(RN8302_WriteSPI_Byte(0xFF));                              //收到数据返回
}

void Select_RN8302(void)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_4);
}
void NotSelect_RN8302(void)
{
    GPIO_SetBits(GPIOA, GPIO_Pin_4);
}

/***************************************************************************************
** 函数名称: RN8302_Read_Reg
** 功能描述: 读RN8302寄存器
** 参    数: LowAdd -- 寄存器低8位地址
**           Rx_Data -- 存放读出数据的指针
**           count -- 读出数据长度
**           RegType -- 寄存器类型
** 返 回 值: None       
** 作　  者: 
** 日  　期: 20xx年xx月xx日
**--------------------------------------------------------------------------------------
** 修 改 人: 
** 日　  期: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
uint8_t Data[4];
uint8_t RN8302_Read_Reg(uint8_t LowAdd,uint8_t *Rx_Data,uint8_t count,uint8_t RegType)
{
	uint8_t i,sum;

	Select_RN8302();
	sum = LowAdd;				//累加和+addr   
	RN8302_WriteSPI_Byte(LowAdd);				//addr	

	sum += RegType; 
	RN8302_WriteSPI_Byte(RegType);
	
	for(i=0;i<count;i++)                   //num=接收长度 必须与寄存器字节一致
	{
		Rx_Data[i]=RN8302_ReadSPI_Byte();			//先读出高位
		sum+=Rx_Data[i];                     //接收累加和
		Data[i] = Rx_Data[i];
	}
	
	//校验码
	Rx_Data[i]=RN8302_ReadSPI_Byte();
	sum+=Rx_Data[i];             //接收累加和
	NotSelect_RN8302();		
	
	if(sum==0xff) 
	{
		return(1);                        //累加和正确
	}          
	else      
	{
		return(0);                        //累加和错误
	}                              
}
void RN8302_Write_Reg(uint8_t LowAdd,uint32_t dat,uint8_t count)   
{
	uint8_t i,sum;
	uint8_t Tx_Data[5];
	
	Tx_Data[0]=LowAdd;
	Tx_Data[1]=Write_Configure;
	switch(count)
	{
	case 1:
		Tx_Data[2]=dat;
		break;			
	case 2:
		Tx_Data[2]=(uint8_t)(dat>>8);
		Tx_Data[3]=dat;
		break;
	case 3:
		Tx_Data[2]=(uint8_t)(dat>>16);
		Tx_Data[3]=(uint8_t)(dat>>8);
		Tx_Data[4]=dat;
		break;		    
	}
	
	Select_RN8302();	
	for(i=0,sum=0;i<(count+2);i++)
	{
		RN8302_WriteSPI_Byte(Tx_Data[i]);                 //发送数据
		sum += Tx_Data[i];                       //计算累加和
	} 
	//校验码
	RN8302_WriteSPI_Byte(~sum);//累加和取反
	NotSelect_RN8302();
}
/***************************************************************************************
** 函数名称: RN8302_Init
** 功能描述: RN8302初始化，初始化后要加入0.5s延时
** 参    数: None
** 返 回 值: None       
** 作　  者: 
** 日  　期: 20xx年xx月xx日
**--------------------------------------------------------------------------------------
** 修 改 人: 
** 日　  期: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
void RN8302_Init(void)
{ 
	uint8_t  PHSUA,PHSUB,PHSUC;         //电压通道相位调整
	uint16_t GSUA,GSUB,GSUC;			//电压通道和电流通道的增益调整
	uint16_t GSIA,GSIB,GSIC;	
	
//  OS_ENTER_CRITICAL();
//	RN8302_Write_Reg(0x82,0xFA,1);      //软件复位
	RN8302_Write_Reg(0x80,0xe5,1);      //写使能
	RN8302_Write_Reg(0x62,0x0000ff,3);  //通道使能
	RN8302_Write_Reg(0x81,0xa2,1);      //工作模式
//	RN8302_Write_Reg(0x86,0x33,1);    //三相三线

	
	GSUA = G_EffectPar_Info.GSUA;
	GSUB = G_EffectPar_Info.GSUB;
	GSUC = G_EffectPar_Info.GSUC;
	
	RN8302_Write_Reg(0x13,GSUA,2);     //Ua通道增益  2个字节
	RN8302_Write_Reg(0x14,GSUB,2);     //Ub通道增益
	RN8302_Write_Reg(0x15,GSUC,2);     //UC通道增益
	
	GSIA = G_EffectPar_Info.GSIA;
	GSIB = G_EffectPar_Info.GSIB;
	GSIC = G_EffectPar_Info.GSIC;
	
	RN8302_Write_Reg(0x16,GSIA,2);     //Ia通道增益
	RN8302_Write_Reg(0x17,GSIB,2);     //Ib通道增益
	RN8302_Write_Reg(0x18,GSIC,2);     //Ic通道增益 
	
	PHSUA = G_EffectPar_Info.PHSUA;
	PHSUB = G_EffectPar_Info.PHSUB;
	PHSUC = G_EffectPar_Info.PHSUC;
	
	RN8302_Write_Reg(0x0C,PHSUA,1);     //Ua相位校正  1个字节
	RN8302_Write_Reg(0x0D,PHSUB,1);     //Ub相位校正  1个字节
	RN8302_Write_Reg(0x0E,PHSUC,1);     //Uc相位校正  1个字节
	
}
/***************************************************************************************
** 函数名称: RN8302_Read_U
** 功能描述: 读出三相电压参数
** 参    数: cmd -- 选择哪相电压 
** 返 回 值: None       
** 作　  者: 
** 日  　期: 20xx年xx月xx日
**--------------------------------------------------------------------------------------
** 修 改 人: 
** 日　  期: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
uint16_t  RN8302_Read_U(uint8_t cmd) //cmd -- a b c
{
	uint8_t Rx_Data[5];
	uint8_t i;
	uint32_t ReadData;
	if (cmd>3)	return 0;
	i=RN8302_Read_Reg(0x07+cmd,Rx_Data,4,Read_Measure);
	if(i) 
	{ 
		ReadData=(((uint32_t)Rx_Data[0])<<24)+(((uint32_t)Rx_Data[1])<<16)
			+(((uint32_t)Rx_Data[2])<<8)+(uint32_t)Rx_Data[3];
	}
	else    	 
	{
		ReadData=0;
	}	
	ReadData = ((float)ReadData)/Uk;
	return ((uint16_t)(ReadData));	 
} 

/***************************************************************************************
** 函数名称: RN8302_Read_I
** 功能描述: 读出三相电流参数
** 参    数: cmd -- 选择哪相电压 
** 返 回 值: None       
** 作　  者: 
** 日  　期: 20xx年xx月xx日
**--------------------------------------------------------------------------------------
** 修 改 人: 
** 日　  期: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
uint32_t RN8302_Read_I(uint8_t cmd) //cmd 0x0b-0x0d
{
	uint8_t Rx_Data[5];
	uint8_t i;
	uint32_t ReadData;
	uint16_t ct;
	if (cmd>3)	return 0;
	i=RN8302_Read_Reg(0x0b+cmd,Rx_Data,4,Read_Measure);
	if(i)	  
	{ 
		ReadData=(((uint32_t)Rx_Data[0])<<24)+(((uint32_t)Rx_Data[1])<<16)
			+(((uint32_t)Rx_Data[2])<<8)+(uint32_t)Rx_Data[0];	
	}
	else  
	{
		ReadData=0;
	}
	ct = G_EffectPar_Info.CT;//电流变比
//	ReadData = ((float)ReadData)/Ik*1000*ct;
	ReadData = ((float)ReadData*ct*100)/Ik;
	return (ReadData);
}
/***************************************************************************************
** 函数名称: RN8302_Read_Cos
** 功能描述: 读出三相功率因子参数
** 参    数: cmd -- 选择哪相电压 
** 返 回 值: None       
** 作　  者: 
** 日  　期: 20xx年xx月xx日
**--------------------------------------------------------------------------------------
** 修 改 人: 
** 日　  期: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
int16_t RN8302_Read_Cos(uint8_t cmd)
{
	uint8_t Rx_Data[5];
	uint8_t i;   ////,flag=0;
	uint32_t ReadData;
	int32_t	 ReadInt;
	if (cmd>3)	return 0;
	i=RN8302_Read_Reg(0x20+cmd,Rx_Data,3,Read_Measure);
	if(i)	  
	{ 
		ReadData=(((uint32_t)Rx_Data[0])<<16)
			+(((uint32_t)Rx_Data[1])<<8)+(uint32_t)Rx_Data[2];
	}
	else  
	{
		ReadData=0;
	}

	if (ReadData&0x00800000)
	{
		ReadData=ReadData|0xff000000;	   // 负数
	}
	else
	{
		ReadData=ReadData& (~0xff000000);
	}

	ReadInt=*((int32_t*)&ReadData);
	
	ReadInt=((double)ReadInt*1000.0/COSk);
	return((int16_t)ReadInt);
	
/*
	if (ReadData&0x00800000)
	{//负数
		flag = 1;
		ReadData = (~(ReadData-1))&0x00FFFFFF;
	}
	ReadData=(uint16)(((float)ReadData)*1000/COSk);	
	if (flag)
	{//负数以补码输出
		ReadData = (~ReadData)+1;
	}
	return (ReadData);
*/
} 

/***************************************************************************************
** 函数名称: RN8302_Read_P
** 功能描述: 读出三相有功功率参数
** 参    数: cmd -- 选择哪相 
** 返 回 值: None       
** 作　  者: 
** 日  　期: 20xx年xx月xx日
**--------------------------------------------------------------------------------------
** 修 改 人: 
** 日　  期: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
uint32_t RN8302_Read_P(uint8_t cmd) //cmd 0x14-0x17
{
	uint8_t Rx_Data[5];
	uint8_t i,flag=0;
	uint16_t ct;
	uint32_t ReadData;
	if (cmd>3)	return 0;
	i=RN8302_Read_Reg(0x14+cmd,Rx_Data,4,Read_Measure);
	if(i)	  
	{ 
		ReadData=(((uint32_t)Rx_Data[0])<<24)+(((uint32_t)Rx_Data[1])<<16)
			+(((uint32_t)Rx_Data[2])<<8)+(uint32_t)Rx_Data[0];	
	}
	else  
	{
		ReadData=0;
	}

// 	if (ReadData&0x80000000)
// 	{//负数
// 		flag = 1;
// 		ReadData = ~(ReadData-1);
// 	}
	ct = G_EffectPar_Info.CT;//电流变比
	ReadData = (uint32_t)(((float)ReadData)/28610.228/1000*100*ct);//  除1000：以Kvar表示; 乘100保存2位小数; 
//  ReadData = (uint32_t)(((float)ReadData)/42914.4/1000*100);
	if (flag)
	{//负数以补码输出
		ReadData = (~ReadData)+1;
	}
	return (ReadData);
}

/***************************************************************************************
** 函数名称: RN8302_Read_Q
** 功能描述: 读出三相无功功率参数
** 参    数: cmd -- 选择哪相 
** 返 回 值: None       
** 作　  者: 
** 日  　期: 20xx年xx月xx日
**--------------------------------------------------------------------------------------
** 修 改 人: 
** 日　  期: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
uint32_t RN8302_Read_Q(uint8_t cmd) //cmd 0x18-0x1B
{
	uint8_t Rx_Data[5];
	uint8_t i,flag=0;
	uint16_t ct;
	uint32_t ReadData;
	if (cmd>3)	return 0;
	i=RN8302_Read_Reg(0x18+cmd,Rx_Data,4,Read_Measure);
	if(i)	  
	{ 
		ReadData=(((uint32_t)Rx_Data[0])<<24)+(((uint32_t)Rx_Data[1])<<16)
			+(((uint32_t)Rx_Data[2])<<8)+(uint32_t)Rx_Data[3];	
	}
	else  
	{
		ReadData=0;
	}

	if (ReadData&0x80000000)
	{//负数
		flag = 1;
		ReadData = ~(ReadData-1);
	}
	ct = G_EffectPar_Info.CT;//电流变比
	ReadData = (uint32_t)(((float)ReadData)/28612.007*100/1000*ct);//  除1000：以Kvar表示; 乘100保存2位小数; 
// 	if (flag)
// 	{//负数以补码输出
// 		ReadData = (~ReadData)+1;
// 	}
	return (ReadData);
}
/***************************************************************************************
** 函数名称: RN8302_Read_Freq
** 功能描述: 读出频率
** 参    数: cmd -- 选择哪相 
** 返 回 值: None       
** 作　  者: 
** 日  　期: 20xx年xx月xx日
**--------------------------------------------------------------------------------------
** 修 改 人: 
** 日　  期: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
uint16_t RN8302_Read_Freq(void) 
{
	uint8_t Rx_Data[5];
	uint8_t i;
	uint32_t ReadData;
	i=RN8302_Read_Reg(0x57,Rx_Data,3,Read_Measure);
	if(i)	  
	{ 
		ReadData=(((uint32_t)Rx_Data[0])<<16)
			+(((uint32_t)Rx_Data[1])<<8)+(uint32_t)Rx_Data[2];
	}
	else  
	{
		ReadData=0;
		return (0);
	}
	ReadData = 8192000*8/((float)ReadData)*10;	
	return ((uint16_t)ReadData);
}
/***************************************************************************************
** 函数名称: Read_PhasePower_Info
** 功能描述: 读三相电电压、电流、功率。。。
** 参    数: *pdata
** 返 回 值: None       
** 作　  者: 
** 日  　期: 20xx年xx月xx日
**--------------------------------------------------------------------------------------
** 修 改 人: 
** 日　  期: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
void Read_PhasePower_Info(void)
{
	uint8_t i,offset;
	uint16_t u_max,u_mix,i_mix,ct;
	volatile PHASE_POWER_ITEM PhasePower_Info;
	//读出设置的最大值，最小值
	u_max = G_EffectPar_Info.U_Max;
	u_mix = G_EffectPar_Info.U_Mix;
	i_mix = G_EffectPar_Info.I_Mix;
	ct = G_EffectPar_Info.CT;

//	i_mix = (float)ct*5.00*((float)i_mix/100);
	for (i=0;i<4;i++)	
	{
		if (i<3) offset = i;
		else offset = 33;//大于32就行了
		//电压
		if(G_EffectPar_Info.Work_Mode==0)  //混补
		{
			PhasePower_Info.Ux = RN8302_Read_U(i);//读三相电电压、电流、功率。。。		
		}
		
		if(G_EffectPar_Info.Work_Mode==1)
		{
			PhasePower_Info.Ux = RN8302_Read_U(i)*2;//读三相电电压、电流、功率。。。
		}
		

		if (PhasePower_Info.Ux<=u_mix)
		{
			G_PhasePower_Info[i].Ux_Over = X_LESS_MIX;
			G_CastProtect |= UA_PROTECT<<offset;//设置保护位
		}
		else if (PhasePower_Info.Ux>=u_max)
		{
			G_PhasePower_Info[i].Ux_Over = X_MORE_MAX;
			G_CastProtect |= UA_PROTECT<<offset;//设置保护位
		} 
		else
		{
			G_PhasePower_Info[i].Ux_Over = X_NORMAL;
			G_CastProtect &= ~(UA_PROTECT<<offset);//清除保护位
		}
		G_PhasePower_Info[i].Ux = PhasePower_Info.Ux;

		//电流
		PhasePower_Info.Ix = RN8302_Read_I(i);
		if (PhasePower_Info.Ix/1000<i_mix)
		{
			G_PhasePower_Info[i].Ix_Over = X_LESS_MIX;
			G_CastProtect |= IA_PROTECT<<offset;//设置保护位
		} 
		else
		{
			G_PhasePower_Info[i].Ix_Over = X_NORMAL;
			G_CastProtect &= ~(IA_PROTECT<<offset);//清除保护位
		}
		G_PhasePower_Info[i].Ix = PhasePower_Info.Ix;



		//有功功率
		if(G_EffectPar_Info.Work_Mode==0)  //混补
		{
			PhasePower_Info.Px = RN8302_Read_P(i);	
			G_PhasePower_Info[i].Px = PhasePower_Info.Px;


		
    }
		if(G_EffectPar_Info.Work_Mode==1)
		{
			PhasePower_Info.Qx = RN8302_Read_P(i);   //共补情况下，8302测量的有功值实际为无功值
			if(PhasePower_Info.Qx>((uint32_t)1L<<31))
			{
				PhasePower_Info.Qx=~(PhasePower_Info.Qx-1);
				PhasePower_Info.Qx = PhasePower_Info.Qx*2*1.732;
				PhasePower_Info.Qx=~PhasePower_Info.Qx+1;
			}
			else
				PhasePower_Info.Qx = PhasePower_Info.Qx*2*1.732;   		
			G_PhasePower_Info[i].Qx = PhasePower_Info.Qx;
	

			
    }
		

		//无功功率
		if(G_EffectPar_Info.Work_Mode==0)  //混补
		{
			PhasePower_Info.Qx = RN8302_Read_Q(i);		
			G_PhasePower_Info[i].Qx = PhasePower_Info.Qx;

			
    }
		if(G_EffectPar_Info.Work_Mode==1)
		{
			PhasePower_Info.Px=RN8302_Read_Q(i);    //共补情况下，8302测量的无功值实际为有功值
			if(PhasePower_Info.Px>((uint32_t)1L<<31))
			{
				PhasePower_Info.Px=~(PhasePower_Info.Px-1);
				PhasePower_Info.Px = PhasePower_Info.Px*2*1.732;
				PhasePower_Info.Px=~PhasePower_Info.Px+1;
			}
			else
			 PhasePower_Info.Px = PhasePower_Info.Px*2*1.732;		 		
			G_PhasePower_Info[i].Px = PhasePower_Info.Px;

			
    }
		//功率因子
 		PhasePower_Info.Cosx = RN8302_Read_Cos(i); 
		if(PhasePower_Info.Cosx <0)
			PhasePower_Info.Cosx=-PhasePower_Info.Cosx;
    if(G_EffectPar_Info.Work_Mode==0)  //混补
	  {
			if (((PhasePower_Info.Px>=0x80000000)&&(PhasePower_Info.Qx<0x80000000))
				||((PhasePower_Info.Px<0x80000000)&&(PhasePower_Info.Qx>=0x80000000)))
			{
				PhasePower_Info.Cosx= -PhasePower_Info.Cosx;
			}
		 
    }
		if(G_EffectPar_Info.Work_Mode==1)
		{
			PhasePower_Info.Cosx=sqrt(1000000-(float)PhasePower_Info.Cosx*(float)PhasePower_Info.Cosx);
			if (((PhasePower_Info.Px>=0x80000000)&&(PhasePower_Info.Qx>0x80000000))
				||((PhasePower_Info.Px<0x80000000)&&(PhasePower_Info.Qx<=0x80000000)))
			{
				PhasePower_Info.Cosx= -PhasePower_Info.Cosx;
			}
			

			
    }	
		G_PhasePower_Info[i].Cosx = PhasePower_Info.Cosx;


	}

//	G_Signal_Freq = RN8302_Read_Freq();
}


/***************************************************************************************
** 函数名称: RN8302_Adjust
** 功能描述: 校表
** 参    数: None
** 返 回 值: None       
** 作　  者: 
** 日  　期: 20xx年xx月xx日
**--------------------------------------------------------------------------------------
** 修 改 人: 
** 日　  期: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
void RN8302_Adjust(void)
{
	uint16_t zero=0;
	uint32_t Data_Ua,Data_Ub,Data_Uc;
	uint32_t Data_Ia,Data_Ib,Data_Ic;
	uint32_t Data_Pa,Data_Pb,Data_Pc;
	static uint8_t  PHSUA,PHSUB,PHSUC;         //电压通道相位调整
	static uint16_t GSUA,GSUB,GSUC;			//电压通道和电流通道的增益调整
	static uint16_t GSIA,GSIB,GSIC;	
	uint8_t i;
	uint8_t Rx_Data[5];
	double ErrP,Err;
	 	
	RN8302_Write_Reg(0x13,zero,2);     //Ua通道增益  2个字节
	RN8302_Write_Reg(0x14,zero,2);     //Ub通道增益
	RN8302_Write_Reg(0x15,zero,2);     //UC通道增益
	RN8302_Write_Reg(0x16,zero,2);     //Ia通道增益
	RN8302_Write_Reg(0x17,zero,2);     //Ib通道增益
	RN8302_Write_Reg(0x18,zero,2);     //Ic通道增益 
	
	RN8302_Write_Reg(0x0C,0x80,1);     //Ua相位校正  1个字节
	RN8302_Write_Reg(0x0D,0x80,1);     //Ub相位校正  1个字节
	RN8302_Write_Reg(0x0E,0x80,1);     //Uc相位校正  1个字节		
	
// 	//Ua	
// 	i=RN8302_Read_Reg(0x07,Rx_Data,4,Read_Measure);	   //  Ua	
// 	if(i)
// 	{
// 		Data_Ua=(((uint32_t)Rx_Data[0])<<24)+(((uint32_t)Rx_Data[1])<<16)
// 			+(((uint32_t)Rx_Data[2])<<8)+(uint32_t)Rx_Data[0];
// 	}
// 	else  
// 	{
// 		Data_Ua=0;
// 	}
// 	
// 	//Ub	
// 	i=RN8302_Read_Reg(0x08,Rx_Data,4,Read_Measure); 	
// 	if(i)	  
// 	{ 
// 		Data_Ub=(((uint32_t)Rx_Data[0])<<24)+(((uint32_t)Rx_Data[1])<<16)
// 			+(((uint32_t)Rx_Data[2])<<8)+(uint32_t)Rx_Data[0];	
// 	}
// 	else  
// 	{
// 		Data_Ub=0;
// 	}

// 	//Uc
// 	i=RN8302_Read_Reg(0x09,Rx_Data,4,Read_Measure); 	
// 	if(i)	  
// 	{ 
// 		Data_Uc=(((uint32_t)Rx_Data[0])<<24)+(((uint32_t)Rx_Data[1])<<16)
// 			+(((uint32_t)Rx_Data[2])<<8)+(uint32_t)Rx_Data[0];	
// 	}
// 	else  
// 	{
// 		Data_Uc=0;
// 	}
// 	
// 	//Ia	
// 	i=RN8302_Read_Reg(0x0b,Rx_Data,4,Read_Measure); 	
// 	if(i)	  
// 	{ 
// 		Data_Ia=(((uint32_t)Rx_Data[0])<<24)+(((uint32_t)Rx_Data[1])<<16)
// 			+(((uint32_t)Rx_Data[2])<<8)+(uint32_t)Rx_Data[0];
// 	}
// 	else  
// 	{
// 		Data_Ia=0;
// 	}

// 	//Ib 	
// 	i=RN8302_Read_Reg(0x0c,Rx_Data,4,Read_Measure); 	
// 	if(i)	  
// 	{ 
// 		Data_Ib=(((uint32_t)Rx_Data[0])<<24)+(((uint32_t)Rx_Data[1])<<16)
// 			+(((uint32_t)Rx_Data[2])<<8)+(uint32_t)Rx_Data[0];
// 	}
// 	else  
// 	{
// 		Data_Ib=0;
// 	}
// 	
// 	//Ic 	
// 	i=RN8302_Read_Reg(0x0d,Rx_Data,4,Read_Measure); 	
// 	if(i)	  
// 	{ 
// 		Data_Ic=(((uint32_t)Rx_Data[0])<<24)+(((uint32_t)Rx_Data[1])<<16)
// 			+(((uint32_t)Rx_Data[2])<<8)+(uint32_t)Rx_Data[0];
// 	}
// 	else  
// 	{
// 		Data_Ic=0; 
// 	}
	
	
	Data_Ua=17112760;
	Data_Ub=17112760;
	Data_Uc=17112760;
	Data_Ia=16777216 ; 
	Data_Ib=16777216 ; 
	Data_Ic=16777216 ; 
	
	if(Ustand>=Data_Ua) GSUA=((float)Ustand/Data_Ua-1)*32768;
	else if(Ustand<Data_Ua) GSUA=((float)Ustand/Data_Ua-1)*32768+65536;
	if(Ustand>=Data_Ub) GSUB=((float)Ustand/Data_Ub-1)*32768;
	else if(Ustand<Data_Ub) GSUB=((float)Ustand/Data_Ub-1)*32768+65536;
	if(Ustand>=Data_Uc) GSUC=((float)Ustand/Data_Uc-1)*32768;
	else if(Ustand<Data_Uc) GSUC=((float)Ustand/Data_Uc-1)*32768+65536;
	 
	G_EffectPar_Info.GSUA = GSUA;
	G_EffectPar_Info.GSUB = GSUB;
	G_EffectPar_Info.GSUC = GSUC;
	
	if(Istand>=Data_Ia) GSIA=((float)Istand/Data_Ia-1)*32768;
	else if(Istand<Data_Ia) GSIA=((float)Istand/Data_Ia-1)*32768+65536;
	if(Istand>=Data_Ib) GSIB=((float)Istand/Data_Ib-1)*32768;
	else if(Istand<Data_Ib) GSIB=((float)Istand/Data_Ib-1)*32768+65536;
	if(Istand>=Data_Ic) GSIC=((float)Istand/Data_Ic-1)*32768;
	else if(Istand<Data_Ic) GSIC=((float)Istand/Data_Ic-1)*32768+65536;
	
	G_EffectPar_Info.GSIA = GSIA;
	G_EffectPar_Info.GSIB = GSIB;
	G_EffectPar_Info.GSIC = GSIC;
	

	RN8302_Write_Reg(0x13,GSUA,2);     //Ua通道增益  2个字节
	RN8302_Write_Reg(0x14,GSUB,2);     //Ub通道增益
	RN8302_Write_Reg(0x15,GSUC,2);     //UC通道增益
	RN8302_Write_Reg(0x16,GSIA,2);     //Ia通道增益
	RN8302_Write_Reg(0x17,GSIB,2);     //Ib通道增益
	RN8302_Write_Reg(0x18,GSIC,2);     //Ic通道增益 
	

	//Pa
	i=RN8302_Read_Reg(0x14,Rx_Data,4,Read_Measure);	
	if(i)	  
	{ 
		Data_Pa=(((uint32_t)Rx_Data[0])<<24)+(((uint32_t)Rx_Data[1])<<16)
			+(((uint32_t)Rx_Data[2])<<8)+(uint32_t)Rx_Data[0];
	}
	else  
	{
		Data_Pa=0;
	}
	
	//Pb	
	i=RN8302_Read_Reg(0x15,Rx_Data,4,Read_Measure); 	
	if(i)	  
	{ 
		Data_Pb=(((uint32_t)Rx_Data[0])<<24)+(((uint32_t)Rx_Data[1])<<16)
			+(((uint32_t)Rx_Data[2])<<8)+(uint32_t)Rx_Data[0];
	}
	else  
	{
		Data_Pb=0;
	}
	
	//Pc	
	i=RN8302_Read_Reg(0x16,Rx_Data,4,Read_Measure); 	
	if(i)	  
	{ 
		Data_Pc=(((uint32_t)Rx_Data[0])<<24)+(((uint32_t)Rx_Data[1])<<16)
			+(((uint32_t)Rx_Data[2])<<8)+(uint32_t)Rx_Data[0];	
	}
	else  
	{
		Data_Pc=0;
	}

	ErrP=((double)Data_Pa-Pstand)/Pstand;
	Err=asin((-ErrP)/1.73205);
	PHSUA=128.0+Err*3259.516476+0.5;
	ErrP=((float)Data_Pb-Pstand)/Pstand;
	Err=asin((-ErrP)/1.73205);
	PHSUB=128.0+Err*3259.516476+0.5;
	ErrP=((float)Data_Pc-Pstand)/Pstand;
	Err=asin((-ErrP)/1.73205);
	PHSUC=128.0+Err*3259.516476+0.5;
	
	G_EffectPar_Info.PHSUA = PHSUA;
	G_EffectPar_Info.PHSUB = PHSUB;
	G_EffectPar_Info.PHSUC = PHSUC;
	

	RN8302_Write_Reg(0x0C,PHSUA,1);     //Ua相位校正  1个字节
	RN8302_Write_Reg(0x0D,PHSUB,1);     //Ub相位校正  1个字节
	RN8302_Write_Reg(0x0E,PHSUC,1);     //Uc相位校正  1个字节
 


	
	//写入数据 
  Write_RN8302_Config_to_Flash( (uint16_t *)&G_EffectPar_Info);	
}
