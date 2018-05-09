#include "iFlash.h"


/********************************************************************************************************** 
** Function name:      FLASH_ReadHalfWord
** Descriptions:       读取指定地址的半字数据
** input parameters:   u32 addr：要读的地址
** output parameters:  无
** Returned value:     无
**********************************************************************************************************/
u16 FLASH_ReadHalfWord(u32 addr)
{
	return *(vu16*)addr; 
}

/********************************************************************************************************** 
** Function name:      FLASH_Write
** Descriptions:       Flash写
** input parameters:   u32 Addr：起始地址
                       u16 *p ： 要写的内容（半字为单位）
                       u16 Num： 要写的半字数目 
** output parameters:  无
** Returned value:     无
**********************************************************************************************************/
void FLASH_HALFWORD_Write(u32 Addr,u16 *p,u16 Num)
{	
	u16 i;
	uint16_t* RamSource;
	RamSource = p;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(Addr);
	for(i = 0;i < Num;i++)
	{
		FLASH_ProgramHalfWord(Addr,*(uint16_t*)RamSource);
	  
		if((*(uint16_t *)Addr != *(uint16_t *)RamSource))
		{
			return;
		}
		Addr += 2;
		RamSource += 1;
	} 
	FLASH_Lock();
}

////////////////////////////////////////////////////////////////////
void ReadData_From_Flash(uint32_t address,uint8_t num,uint16_t *Data)
{
	uint16_t *PthFlash;
	uint8_t count;
	PthFlash=(uint16_t *)(address );
	for(count=0;count<num;count++)
	{
		*(Data+count)=*(PthFlash+count);
  }
}
