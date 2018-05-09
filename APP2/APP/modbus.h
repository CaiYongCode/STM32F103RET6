#ifndef _MODBUS_
#define _MODBUS_


#include "globaldata.h"


uint16_t crc16(unsigned char *puchMsg, uint16_t usDataLen) ;
void checkCommModbus(unsigned char * pbuf,unsigned char receCount);
void readCoil(void);
void readRegisters(void);
void forceSingleCoil(unsigned char receCount);
void presetSingleRegister(void);
void presetMultipleRegisters(void);
void forceMultipleCoils(void);
uint16_t getRegisterVal(uint16_t addr,uint16_t *tempData);
uint16_t setRegisterVal(uint16_t addr,uint16_t tempData);
uint16_t getCoilVal(uint16_t addr,uint16_t *tempData);
uint16_t setCoilVal(uint16_t addr,uint16_t tempData);

#endif  //_MODBUS_