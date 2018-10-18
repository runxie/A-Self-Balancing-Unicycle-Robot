
#ifndef __Motor_h
#define __Motor_h
#include "MotorCoder.h"

#include "stm32f10x.h"
#include "Bar.h"
//#include "ADIS16255.h"
#include "MotorCoder.h"
#include "MotorCoder2.h"
#include "LCD_Dis.h"
#include "L3G4200D.h"
#include "ADXL345.h"

//u16 PWMcalculator(struct GyroOut GyroData,struct CoderOut CoderData);
void MotorInit(void);
void MotorControl(void);
u16 PWM1calculator(void);
u16 PWM2calculator(void);
void Counter_Getter();

void Pin_A_R(void);
void Pin_A_L(void);
void Pin_B_R(void);
void Pin_B_L(void);

typedef struct Control{
   //GyroOut GyroOutX1;
   GyroX3Out *GyroOutX3;
   CoderOut MotorCoderData;
   CoderOut2 MotorCoderData2;
   BarOut BarData;
}Control;

extern u16 TIMCompare1;
extern u16 TIMCompare2;
extern struct Control ControlData;
extern float angleX;
extern float angleX_temp;
extern float angleY;
extern float angleY_temp;
extern u16 MotorDown;
extern s32 angleX_sum;
extern s32 angleY_sum;
extern u16 count;
extern s16 speed;
extern s16 speed_drc;

extern float ANGLX_Setoff;
extern float ANGLY_Setoff;
#endif