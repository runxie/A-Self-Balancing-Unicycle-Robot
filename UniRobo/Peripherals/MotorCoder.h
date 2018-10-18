

#ifndef __Coder_H_
#define __Coder_H_
typedef struct CoderOut{
  s32 Counter;
  s32 dCounter;
  s32 ddCounter;
}CoderOut;

void CoderInit(void);
void Counter_Getter(void);
extern struct CoderOut MotorCoderData;

#endif