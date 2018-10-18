#ifndef  __L3G4200D_H__
#define  __L3G4200D_H__

#define GyroX3Addr 0xD2  //0xD0
#define GyroX3Addr_W 0xD2+0
#define GyroX3Addr_R 0xD2+1
//#define GyroX3Addr 0xD0  //0xD0
//#define GyroX3Addr_W 0xD0+0
//#define GyroX3Addr_R 0xD0+1
#define L3G_IIC_ADDR    0x68

#define READ_L3G        0x1
#define WRITE_L3G       0x0

// ¼Ä´æÆ÷µØÖ·¶¨Òå
#define WHO_AM_I        0x0f
#define CTRL_REG1       0x20
#define CTRL_REG2       0x21
#define CTRL_REG3       0x22
#define CTRL_REG4       0x23
#define CTRL_REG5       0x24
#define REFRENCE        0x25
#define STATUS_REG      0x27
#define OUT_X_L         0x28
#define OUT_X_H         0x29
#define OUT_Y_L         0x2A
#define OUT_Y_H         0x2B
#define OUT_Z_L         0x2C
#define OUT_Z_H         0x2D
#define INT_CFG         0x30
#define INT_SRC         0x31
#define INT_TSH_XH      0x32
#define INT_TSH_XL      0x33
#define INT_TSH_YH      0x34
#define INT_TSH_YL      0x35
#define INT_TSH_ZH      0x36
#define INT_TSH_ZL      0x37
#define INT_DURATION    0x38

void GyroX3Init(void);
void SetGyroX3(void);
void Gyro3X_Null(void);
void ReadGyroX3_X(void);
void ReadGyroX3_Y(void);
void ReadGyroX3_Z(void);

extern u16 GyroX3_SetOff_Flag;
extern u16 GyroX3_Null_Flag;
extern s16 GyroX3_X_SetOff1;
extern s16 GyroX3_Y_SetOff1;
extern s16 GyroX3_Z_SetOff1;
extern s16 GyroX3_X_SetOff2;
extern s16 GyroX3_Y_SetOff2;
extern s16 GyroX3_Z_SetOff2;
extern u32 GyroX3_SetOff_Loop;

extern u16 GyroX3_X_Scale_S;
extern u16 GyroX3_Y_Scale_S;
extern u16 GyroX3_Z_Scale_S;
extern u16 GyroX3_X_Scale_L;
extern u16 GyroX3_Y_Scale_L;
extern u16 GyroX3_Z_Scale_L;

extern u16 SimpleNULL;

typedef struct GyroX3Out{
  s16 X_GYRO_OUT;
  s16 Y_GYRO_OUT;
  s16 Z_GYRO_OUT;
  s32 X_ANGL_OUT;
  s32 Y_ANGL_OUT;
  s32 Z_ANGL_OUT;
}GyroX3Out;

extern struct GyroX3Out GyroOutX3;

#endif
