#ifndef  __I2C_H__
#define  __I2C_H__


void I2CInit(void);
u8 I2C_Readbyte(u8 I2CAddr, u8 ReadAddr);
void I2C_SequentialRead(u8 I2CAddr, u8* pBuffer, u8 ReadAddr, u16 NumByteToRead);
void I2C_Writebyte(u8 I2CAddr, u8 addr, u8 data);
void I2C_SequentialWrite(u8 I2CAddr, u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite);

#endif
