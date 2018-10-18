

#ifndef __Coder2_H_
#define __Coder2_H_

typedef struct CoderOut2{
  s32 Counter;
  s32 dCounter;
  s32 ddCounter;
}CoderOut2;

void CoderInit2(void);
void Counter_Getter2(void);
extern struct CoderOut2 MotorCoderData2;

#endif