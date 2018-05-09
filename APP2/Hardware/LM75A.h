#ifndef _LM75A_H
#define _LM75A_H

#include "stm32f10x.h"
#include "includes.h"
#include "Initialize.h"
#include "struct.h"
#include "const.h"
#include "stdio.h"
#include <string.h>


#define I2C1_OWN_ADDRESS7    0x90
#define I2C_Speed              100000

#define LM75A_TEMP  0x00
#define LM75A_CONF  0x01
#define LM75A_THYS  0x02
#define LM75A_TOS   0x01




void LM75A_Read(uint8_t I2CAddr, uint8_t ReadAddr,uint8_t *pBuffer,uint8_t NumByteToRead);
void I2C_LM75A_Init(void);
unsigned short Get_LM75A_Temp_Value(void);

#endif
