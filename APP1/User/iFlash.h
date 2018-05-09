#ifndef _iFlash_h
#define _iFlash_h
#include "stm32f10x.h"

u16 FLASH_ReadHalfWord(u32 addr);             //读取指定地址的半字数据
void FLASH_HALFWORD_Write(u32 Addr,u16 *p,u16 Num);

void ReadData_From_Flash(uint32_t address,uint8_t num,uint16_t *Data);
#endif
