#include "globaldata.h"
#include "LM75A.h"

uint16_t Temp_Mid;  //从LM75A读取的数据


void LM75A_I2C_GPIO_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

	/* 使能与 I2C1 有关的时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);


  /* PB6-I2C1_SCL、PB7-I2C1_SDA*/
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	       // 开漏输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);
// 	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
//   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	       // 上拉输出
//   GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	
}

void LM75A_I2C_Mode_Config(void)
{
  I2C_InitTypeDef I2C_InitStructure;

  /* I2C 配置 */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 =I2C1_OWN_ADDRESS7;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;

   /* I2C1 初始化 */
  I2C_Init(I2C1, &I2C_InitStructure);

   /* 使能 I2C1 */
  I2C_Cmd(I2C1, ENABLE);
}

/*------------往LM75A中写数据-----------------------*/
/*
 * 函数名：LM75A_Write
 * 描述  ：向LM75A发送功能命令字
 * 输入  ：-I2CAddr      接收命令的LTC2990的地址
 *         -RegAddr      LTC2990控制寄存器地址
 *         -RegData      LTC2990控制寄存器中的功能
 * 输出  ：无
 * 返回  ：无
 * 调用  ：外部调用
 */
void LM75A_Write(uint8_t I2CAddr,uint8_t RegAddr, uint8_t RegData)
{
	/* Send STRAT condition */
  I2C_GenerateSTART(I2C1, ENABLE);

  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

  /* Send LM75A address for write */
  I2C_Send7bitAddress(I2C1, I2CAddr, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  /* Send the LM75A internal address to write to */
  I2C_SendData(I2C1, RegAddr);

  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  /* Send the byte to be written */
  I2C_SendData(I2C1, RegData);

  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  /* Send STOP condition */
  I2C_GenerateSTOP(I2C1, ENABLE);
}


/*------------从LM75A中读温度-----------------------*/
/*
 * 函数名：LM75A_Read
 * 描述  ：从LM75A里面读取数据。
 * 输入  ：-*pBuffer 存放从LTC2990读取的数据的缓冲区指针。
 *         -ReadAddr 读取温度数据的寄存器地址
 *         -NumByteToWrite 要从EEPROM读取的字节数。
 * 输出  ：无
 * 返回  ：无
 * 调用  ：外部调用
 */
void LM75A_Read(uint8_t I2CAddr, uint8_t ReadAddr,uint8_t *pBuffer,uint8_t NumByteToRead)
{
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

  /* Send START condition */
  I2C_GenerateSTART(I2C1, ENABLE);

  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

  /* Send LM75A address for write */
  I2C_Send7bitAddress(I2C1, I2CAddr, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(I2C1, ENABLE);

  /* Send the LTC2990's internal address to write to */
  I2C_SendData(I2C1, ReadAddr);

  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  /* Send STRAT condition a second time */
  I2C_GenerateSTART(I2C1, ENABLE);

  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

  /* Send LTC2990 address for read */
  I2C_Send7bitAddress(I2C1, I2CAddr, I2C_Direction_Receiver);

  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

  /* While there is data to be read */
  while(NumByteToRead)
  {
    if(NumByteToRead == 1)
    {
      /* Disable Acknowledgement */
      I2C_AcknowledgeConfig(I2C1, DISABLE);

      /* Send STOP Condition */
      I2C_GenerateSTOP(I2C1, ENABLE);
    }

    /* Test on EV7 and clear it */
    if(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
    {
      /* Read a byte from the EEPROM */
      *pBuffer = I2C_ReceiveData(I2C1);

      /* Point to the next location where the byte read will be saved */
      pBuffer++;

      /* Decrement the read bytes counter */
      NumByteToRead--;
    }
  }

  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(I2C1, ENABLE);
}

void I2C_LM75A_Init(void)
{

  LM75A_I2C_GPIO_Config();

  LM75A_I2C_Mode_Config();

  LM75A_Write(I2C1_OWN_ADDRESS7,LM75A_CONF, 0x00);
}



unsigned short Get_LM75A_Temp_Value(void)  
{
	uint8_t x,y;
	unsigned short value = 0;
	unsigned short temp = 0;
	unsigned char flag = 0;
    
	LM75A_Read(I2C1_OWN_ADDRESS7, LM75A_TEMP,(uint8_t *)&(Temp_Mid),2);
	value = Temp_Mid;
	
	x = (uint8_t)value;
	y = (uint8_t)(value>>8);
	value = x;
	value = (value<<8)|y;
	value = value>>5;
	
	if( value & 0x0400 )    //负温度
	{
		flag = 1;
		value = (~value+1)&0x3FF;
	}
	
	temp = value*0.125*10;
	
	if(flag == 1)
	{
		temp = temp|0x8000;
	}
	
	return temp;
} 

