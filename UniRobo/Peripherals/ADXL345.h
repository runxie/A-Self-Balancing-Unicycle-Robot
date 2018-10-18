#ifndef  __ADXL345_H__
#define  __ADXL345_H__


#define ADXL345Addr 0xA6//0x3A //0xA6
#define ADXL345Addr_W (ADXL345Addr+0)
#define ADXL345Addr_R (ADXL345Addr+1)

// ¼Ä´æÆ÷µØÖ·¶¨Òå
#define OFSX 0x1E
#define OFSY 0x1F
#define OFSZ 0x20
#define BW_RATE        0x25
#define DATA_FOEMAT     0x31
#define DATA_X_L         0x32
#define DATA_X_H         0x33
#define DATA_Y_L         0x34
#define DATA_Y_H         0x35
#define DATA_Z_L         0x36
#define DATA_Z_H         0x37

void ADXL345Init(void);

void SetADXL345(void);


typedef struct ADXL345Out{
  s16 X_ANGL_OUT;
  s16 Y_ANGL_OUT;
  s16 Z_ANGL_OUT;
}ADXL345Out;

extern struct ADXL345Out ADXL345_Out;

extern s16 ADXL345_X_SetOff;
extern s16 ADXL345_Y_SetOff;
extern s16 ADXL345_Z_SetOff;

#endif
