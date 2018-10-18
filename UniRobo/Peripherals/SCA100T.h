#ifndef __SCA100T__
#define __SCA100T__


#define MEAS 0x00
#define RWTR 0x08
#define RDSR 0x0A
#define RLOAD 0x0B
#define STX 0x0E
#define STY 0x0F
#define RADX 0x10
#define RADY 0x11


void InclinometerInit(void);
void InclinometerReader(void);
void SPI_Writebyte(u8 addr,u8 data);
u8 SPI_Readbyte(u8 addr);
u16 ReadAnglX(void);
u16 ReadAnglY(void);


typedef struct InclinometerOut{
  s16 ANGLX_OUT;
  s16 ANGLY_OUT;
}InclinometerOut;

extern struct InclinometerOut InclinometerX2Out;
#endif

