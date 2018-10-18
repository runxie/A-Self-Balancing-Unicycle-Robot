#ifndef __Gyro_h__
#define __Gyro_h__


#define ENDURANCE_L 0x00
#define ENDURANCE_H 0x01
#define SUPPLY_OUT_L 0x02
#define SUPPLY_OUT_H 0x03
#define GYRO_OUT_L 0x04
#define GYRO_OUT_H 0x05
#define AUX_ADC_L 0x0A
#define AUX_ADC_H 0x0B
#define TEMP_OUT_L 0x0C
#define TEMP_OUT_H 0x0D
#define ANGL_OUT_L 0x0E
#define ANGL_OUT_H 0x0F

#define GYRO_OFF_L 0x14
#define GYRO_OFF_H 0x15
#define GYRO_SCALE_L 0x16
#define GYRO_SCALE_H 0x17
#define ALM_MAG1_L 0x20
#define ALM_MAG1_H 0x21 
#define ALM_MAG2_L 0x22
#define ALM_MAG2_H 0x23
#define ALM_SMPL1_L 0x24
#define ALM_SMPL1_H 0x25
#define ALM_SMPL2_L 0x26
#define ALM_SMPL2_H 0x27
#define ALM_CTRL_L 0x28
#define ALM_CTRL_H 0x29
#define AUX_DAC_L 0x30
#define AUX_DAC_H 0x31
#define GPIO_CTRL_L 0x32
#define GPIO_CTRL_H 0x33
#define MSC_CTRL_L 0x34
#define MSC_CTRK_H 0x35
#define SMPL_PRD_L 0x36
#define SMPL_PRD_H 0x37
#define SENS_AVG_L 0x38
#define SENS_AVG_H 0x39
#define SLP_CNT_L 0x3A
#define SLP_CNT_H 0x3B
#define STATUS_L 0x3C
#define STATUS_H 0x3D
#define COMMAND_L 0x3E
#define COMMAND_H 0x3F

void GyroInit(void);
void Gyroscope(void);
void SPI_Writebyte(u16 addr,u8 data);
u16 SPI_Readbyte(u16 addr);
u16 ReadAngl(void);
u16 ReadGyro(void);
u16 ReadTemp(void);

void Auto_Null();
void GyroSet(void);

typedef struct GyroOut{
  u16 ENDURANCE;
  u16 SUPPLY_OUT;
  s16 GYRO_OUT;
  u16 AUX_ADC;
  s16 TEMP_OUT;
  s16 ANGL_OUT;
}GyroOut;

extern struct GyroOut GyroOutX1;
#endif

