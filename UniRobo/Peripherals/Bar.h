

#ifndef __Bar_H_
#define __Bar_H_

void BarInit(void);
u8 GetBarValue(void);
void TIM3_IRQHandler(void);


typedef struct BarOut{
  u8 Bar1;
  u8 Bar2;
}BarOut;


extern struct BarOut BarData;
#endif