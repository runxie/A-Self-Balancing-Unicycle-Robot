#include "stm32f10x.h"
#include "L3G4200D.h"
#include "Delay.h"
#include "LCD_Dis.h"
#include "Motor.h"
#include "I2C.h"

#define Timer TIM7
#define TimerCLK RCC_APB1Periph_TIM7

#define I2CPort I2C1
#define GyroX3Port GPIOB
#define SCL GPIO_Pin_6
#define SDA GPIO_Pin_7
#define Power GPIO_Pin_8
#define GND GPIO_Pin_9


struct GyroX3Out GyroOutX3={0,0,0,0,0,0};
u8 Gyrotemp[6];

u16 GyroX3_X_Counter=0;
u16 GyroX3_Y_Counter=0;
u16 GyroX3_Z_Counter=0;

s16 GyroX3_X_SetOff1=0;
s16 GyroX3_Y_SetOff1=0;
s16 GyroX3_Z_SetOff1=0;


s16 GyroX3_X_SetOff2=0;
s16 GyroX3_Y_SetOff2=0;
s16 GyroX3_Z_SetOff2=0;


u16 GyroX3_X_Scale_S=1;
u16 GyroX3_Y_Scale_S=1;
u16 GyroX3_Z_Scale_S=1;
u16 GyroX3_X_Scale_L=1;
u16 GyroX3_Y_Scale_L=1;
u16 GyroX3_Z_Scale_L=1;

s16 GyroX3_X_Test_Flag=2;
s16 GyroX3_Y_Test_Flag=2;
s16 GyroX3_Z_Test_Flag=2;

u16 GyroX3_SetOff_Flag=0;
u16 GyroX3_Null_Flag=0;

u16 SimpleNULL=0;
u32 GyroX3_SetOff_Loop=0;
u16 LoopValue=1024;
s16 TestNull_P=2000;
s16 TestNull_N=-2000;

void GyroX3Init(void)
{ 
   NVIC_InitTypeDef NVIC_InitStructure;
   TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;
   
  
   SetGyroX3(); //设置陀螺仪参数
   
   
   RCC_APB1PeriphClockCmd(TimerCLK,ENABLE);
		//timer configuration
   TIM_BaseInitStructure.TIM_Period = 10-1; //中断频率200Hz
   TIM_BaseInitStructure.TIM_Prescaler = (36000-1);
   TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
   TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
   TIM_BaseInitStructure.TIM_RepetitionCounter = 0;
   TIM_TimeBaseInit(Timer,&TIM_BaseInitStructure);

   //TIM_PrescalerConfig(Timer, 8, TIM_PSCReloadMode_Immediate);

   TIM_ClearFlag(Timer,TIM_IT_Update);

   TIM_ITConfig(Timer,TIM_IT_Update,ENABLE);

   TIM_Cmd(Timer,ENABLE);

   NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn; //更新事件
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级0
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //响应优先级4
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //允许中断
   NVIC_Init(&NVIC_InitStructure); //写入设置
  
}

void SetGyroX3(void){
  u8 GyroX3Set[6];
  GyroX3Set[0]=0xEF;// CTRL_REG1[ODR:800Hz/ Cut-off:50/ normalmode/ Xen/ Yen/ Zen]
  GyroX3Set[1]=0x00;// CTRL_REG2 Default [InterruptDisabble /PPout /FIFODisabble] 
  GyroX3Set[2]=0x00;// CTRL_REG3 Default
  GyroX3Set[3]=0x00;// CTRL_REG4 Default [continous update/ 250 dps]
  GyroX3Set[4]=0x00;// CTRL_REG5 Defualt [HighPassFilter Off/ FIFOdisabble]
  GyroX3Set[5]=0x00;// REFERENCE Defualt 0x00
  
  I2C_SequentialWrite(GyroX3Addr,GyroX3Set,CTRL_REG1,6);
  //Gyro3X_Null();
}

/*
void ReadGyroX3(void){
  
   u8 Gyrotemp[6];
   I2C_SequentialRead(Gyrotemp,OUT_X_L,6);
    GyroOutX3.X_GYRO_OUT=Gyrotemp[0]|Gyrotemp[1]<<8;
    GyroOutX3.Y_GYRO_OUT=(Gyrotemp[2]|Gyrotemp[3]<<8); 
    GyroOutX3.Z_GYRO_OUT=Gyrotemp[4]|Gyrotemp[5]<<8;
    GyroOutX3.X_ANGL_OUT+=GyroOutX3.X_GYRO_OUT;
    GyroOutX3.Y_ANGL_OUT+=(GyroOutX3.Y_GYRO_OUT);
    GyroOutX3.Z_ANGL_OUT+=GyroOutX3.Z_GYRO_OUT;
    n++;
}
*/ 
void ReadGyroX3_X(void){
   if(GyroX3_X_Counter<=GyroX3_X_Scale_S){
    GyroOutX3.X_GYRO_OUT=(Gyrotemp[0]|Gyrotemp[1]<<8);
    GyroOutX3.X_ANGL_OUT+=(GyroOutX3.X_GYRO_OUT-GyroX3_X_SetOff1);
    GyroX3_X_Counter++;
    return;
   }
   if((GyroX3_X_Scale_S<GyroX3_X_Counter)&&(GyroX3_X_Counter<(GyroX3_X_Scale_L+GyroX3_X_Scale_S))){
    GyroOutX3.X_GYRO_OUT=(Gyrotemp[0]|Gyrotemp[1]<<8);
    GyroOutX3.X_ANGL_OUT+=(GyroOutX3.X_GYRO_OUT-GyroX3_X_SetOff2);
    GyroX3_X_Counter++;
    return;
   }
   if(GyroX3_X_Counter==(GyroX3_X_Scale_L+GyroX3_X_Scale_S)){
    GyroOutX3.X_GYRO_OUT=(Gyrotemp[0]|Gyrotemp[1]<<8);
    GyroOutX3.X_ANGL_OUT+=(GyroOutX3.X_GYRO_OUT-GyroX3_X_SetOff2);
    GyroX3_X_Counter=0;
    return;
   }
}

void ReadGyroX3_Y(void){
   if(GyroX3_Y_Counter<=GyroX3_Y_Scale_S){
    GyroOutX3.Y_GYRO_OUT=(Gyrotemp[2]|Gyrotemp[3]<<8);
    GyroOutX3.Y_ANGL_OUT+=(GyroOutX3.Y_GYRO_OUT-GyroX3_Y_SetOff1);
    GyroX3_Y_Counter++;
    return;
   }
   if((GyroX3_Y_Scale_S<GyroX3_Y_Counter)&&(GyroX3_Y_Counter<(GyroX3_Y_Scale_S+GyroX3_Y_Scale_L))){
    GyroOutX3.Y_GYRO_OUT=(Gyrotemp[2]|Gyrotemp[3]<<8);
    GyroOutX3.Y_ANGL_OUT+=(GyroOutX3.Y_GYRO_OUT-GyroX3_Y_SetOff2);
    GyroX3_Y_Counter++;
    return;
   }
   if(GyroX3_Y_Counter=(GyroX3_Y_Scale_S+GyroX3_Y_Scale_L)){
    GyroOutX3.Y_GYRO_OUT=(Gyrotemp[2]|Gyrotemp[3]<<8);
    GyroOutX3.Y_ANGL_OUT+=(GyroOutX3.Y_GYRO_OUT-GyroX3_Y_SetOff2);
    GyroX3_Y_Counter=0;
    return;
   }
}
void ReadGyroX3_Z(void){
   if(GyroX3_Z_Counter<=GyroX3_Z_Scale_S){
    GyroOutX3.Z_GYRO_OUT=(Gyrotemp[4]|Gyrotemp[5]<<8);
    GyroOutX3.Z_ANGL_OUT+=(GyroOutX3.Z_GYRO_OUT-GyroX3_Z_SetOff1);
    GyroX3_Z_Counter++;
    return;
   }
   if((GyroX3_Z_Scale_S<GyroX3_Z_Counter)&&(GyroX3_Z_Counter<(GyroX3_Z_Scale_S+GyroX3_Z_Scale_L))){
    GyroOutX3.Z_GYRO_OUT=(Gyrotemp[4]|Gyrotemp[5]<<8);
    GyroOutX3.Z_ANGL_OUT+=(GyroOutX3.Z_GYRO_OUT-GyroX3_Z_SetOff2);
    GyroX3_Z_Counter++;
    return;
   }
   if(GyroX3_Z_Counter=(GyroX3_Z_Scale_S+GyroX3_Z_Scale_L)){
    GyroOutX3.Z_GYRO_OUT=(Gyrotemp[4]|Gyrotemp[5]<<8);
    GyroOutX3.Z_ANGL_OUT+=(GyroOutX3.Z_GYRO_OUT-GyroX3_Z_SetOff2);
    GyroX3_Z_Counter=0;
    return;
   }
}

void ReadGyroX3(void){
  
   
   I2C_SequentialRead(GyroX3Addr_R,Gyrotemp,OUT_X_L,6);
   ReadGyroX3_X();
   ReadGyroX3_Y();
   ReadGyroX3_Z();
   
  
}


void TIM7_IRQHandler(void){
  if(TIM_GetITStatus(Timer,TIM_IT_Update)!=RESET){ 
    if(GyroX3_SetOff_Flag){
      ReadGyroX3();
      TIM_ClearITPendingBit(Timer,TIM_IT_Update);
      //第一次清零
      if(GyroX3_SetOff_Loop==0){
        GyroOutX3.X_ANGL_OUT=0;
        GyroOutX3.Y_ANGL_OUT=0;
        GyroOutX3.Z_ANGL_OUT=0;
        GyroX3_SetOff_Loop++;
        return;
      }
      //第一遍求平均
      if(GyroX3_SetOff_Loop<LoopValue){
        GyroX3_SetOff_Loop++;
        return;
      }
      if(GyroX3_SetOff_Loop==LoopValue){
        GyroX3_X_SetOff1=GyroX3_X_SetOff2=( GyroOutX3.X_ANGL_OUT/LoopValue);
        GyroX3_Y_SetOff1=GyroX3_Y_SetOff2=( GyroOutX3.Y_ANGL_OUT/LoopValue);
        GyroX3_Z_SetOff1=GyroX3_Z_SetOff2=( GyroOutX3.Z_ANGL_OUT/LoopValue);
        GyroOutX3.X_ANGL_OUT=0;
        GyroOutX3.Y_ANGL_OUT=0;
        GyroOutX3.Z_ANGL_OUT=0;
        GyroX3_SetOff_Loop++;
        return;
      }
      //第二遍验证校准
      if(GyroX3_SetOff_Loop<(LoopValue<<1)){
         GyroX3_SetOff_Loop++;
         return;
      }
      if(GyroX3_SetOff_Loop==LoopValue*2){
        GyroX3_Null_Flag=1;
        if(GyroOutX3.X_ANGL_OUT>TestNull_P){
            GyroX3_X_SetOff1++;
            GyroX3_X_SetOff2++;
            GyroX3_X_Test_Flag=1; 
            GyroX3_Null_Flag=0; 
        }
       
      if(GyroOutX3.X_ANGL_OUT<TestNull_N){
            GyroX3_X_SetOff1--;
            GyroX3_X_SetOff2--;
            GyroX3_X_Test_Flag=-1;
            GyroX3_Null_Flag=0; 
        }
        //Y:
        if(GyroOutX3.Y_ANGL_OUT>TestNull_P){
            GyroX3_Y_Test_Flag=1;
            GyroX3_Y_SetOff1++;
            GyroX3_Y_SetOff2++;
            GyroX3_Null_Flag=0;    
        }
       
       if(GyroOutX3.Y_ANGL_OUT<TestNull_N){
            GyroX3_Y_SetOff1--;
            GyroX3_Y_SetOff2--;
            GyroX3_Y_Test_Flag=-1;
            GyroX3_Null_Flag=0;    
        }
        //Z:
        if(GyroOutX3.Z_ANGL_OUT>TestNull_P){
            GyroX3_Z_SetOff1++;
            GyroX3_Z_SetOff2++;
            GyroX3_Z_Test_Flag=1;
            GyroX3_Null_Flag=0; 
        }
       
        if(GyroOutX3.Z_ANGL_OUT<TestNull_N){
            GyroX3_Z_SetOff1--;
            GyroX3_Z_SetOff2--;
            GyroX3_Z_Test_Flag=-1;
            GyroX3_Null_Flag=0;    
        }
        if(GyroX3_Null_Flag==1){
          GyroOutX3.X_ANGL_OUT=0;
          GyroOutX3.Y_ANGL_OUT=0;
          GyroOutX3.Z_ANGL_OUT=0;
          GyroX3_SetOff_Loop=0;
          GyroX3_SetOff_Flag=0;
          return;        
        }
       
        if(GyroX3_Null_Flag==0){
          GyroOutX3.X_ANGL_OUT=0;
          GyroOutX3.Y_ANGL_OUT=0;
          GyroOutX3.Z_ANGL_OUT=0;
          GyroX3_SetOff_Loop++;
          return;          
        }
      }
      ////////////////////////////////////
      //第三遍至第n遍不断校验
       if(GyroX3_SetOff_Loop<LoopValue*3){
         GyroX3_SetOff_Loop++;
         return;
       }
      //X:
      if(GyroX3_SetOff_Loop==LoopValue*3){
        GyroX3_Null_Flag=1;
        if(GyroOutX3.X_ANGL_OUT>TestNull_P){
          if(GyroX3_X_Test_Flag==0){
             GyroX3_X_Scale_S++;
             GyroX3_Null_Flag=0; 
          }
          if(GyroX3_X_Test_Flag==1){
            GyroX3_X_SetOff1++;
            GyroX3_X_SetOff2++;
            GyroX3_X_Test_Flag=1;
            GyroX3_Null_Flag=0;    
          }
          if(GyroX3_X_Test_Flag==-1){
            GyroX3_X_SetOff2=GyroX3_X_SetOff1;
            GyroX3_X_SetOff1++;
            GyroX3_X_Test_Flag=0;
            GyroX3_Null_Flag=0;
          }  
        }
       
        if(GyroOutX3.X_ANGL_OUT<TestNull_N){
          if(GyroX3_X_Test_Flag==0){
             GyroX3_X_Scale_L++;
             GyroX3_Null_Flag=0; 
          }
          if(GyroX3_X_Test_Flag==-1){
            GyroX3_X_SetOff1--;
            GyroX3_X_SetOff2--;
            GyroX3_X_Test_Flag=-1;
            GyroX3_Null_Flag=0;    
          }
          if(GyroX3_X_Test_Flag==1){
            GyroX3_X_SetOff2=GyroX3_X_SetOff1-1;
            GyroX3_X_Test_Flag=0;
            GyroX3_Null_Flag=0;
          }  
        }
        //Y:
        if(GyroOutX3.Y_ANGL_OUT>TestNull_P){
           if(GyroX3_Y_Test_Flag==0){
             GyroX3_Y_Scale_S++;
             GyroX3_Null_Flag=0; 
          }
          if(GyroX3_Y_Test_Flag==1){
            GyroX3_Y_SetOff1++;
            GyroX3_Y_SetOff2++;
            GyroX3_Y_Test_Flag=1;
            GyroX3_Null_Flag=0;    
          }
          if(GyroX3_Y_Test_Flag==-1){
            GyroX3_Y_SetOff2=GyroX3_Y_SetOff1;
            GyroX3_Y_SetOff1++;
            GyroX3_Y_Test_Flag=0;
            GyroX3_Null_Flag=0;
          }  
        }
       
        if(GyroOutX3.Y_ANGL_OUT<TestNull_N){
          if(GyroX3_Y_Test_Flag==0){
             GyroX3_Y_Scale_L++;
             GyroX3_Null_Flag=0; 
          }
          if(GyroX3_Y_Test_Flag==-1){
            GyroX3_Y_SetOff1--;
            GyroX3_Y_SetOff2--;
            GyroX3_Y_Test_Flag=-1;
            GyroX3_Null_Flag=0;    
          }
          if(GyroX3_Y_Test_Flag==1){
            GyroX3_Y_SetOff2=GyroX3_Y_SetOff1-1;
            GyroX3_Y_Test_Flag=0;
            GyroX3_Null_Flag=0;
          }  
        }
        //Z:
        if(GyroOutX3.Z_ANGL_OUT>TestNull_P){
           if(GyroX3_Z_Test_Flag==0){
             GyroX3_Z_Scale_S++;
             GyroX3_Null_Flag=0; 
          }
          if(GyroX3_Z_Test_Flag==1){
            GyroX3_Z_SetOff1++;
            GyroX3_Z_SetOff2++;
            GyroX3_Z_Test_Flag=1;
            GyroX3_Null_Flag=0;    
          }
          if(GyroX3_Z_Test_Flag==-1){
            GyroX3_Z_SetOff2=GyroX3_Z_SetOff1;
            GyroX3_Z_SetOff1++;
            GyroX3_Z_Test_Flag=0;
            GyroX3_Null_Flag=0;
          }  
        }
       
        if(GyroOutX3.Z_ANGL_OUT<TestNull_N){
          if(GyroX3_Z_Test_Flag==0){
             GyroX3_Z_Scale_L++;
             GyroX3_Null_Flag=0; 
          }
          if(GyroX3_Z_Test_Flag==-1){
            GyroX3_Z_SetOff1--;
            GyroX3_Z_SetOff2--;
            GyroX3_Z_Test_Flag=-1;
            GyroX3_Null_Flag=0;    
          }
          if(GyroX3_Z_Test_Flag==1){
            GyroX3_Z_SetOff2=GyroX3_Z_SetOff1-1;
            GyroX3_Z_Test_Flag=0;
            GyroX3_Null_Flag=0;
          }  
        }
        if(GyroX3_Null_Flag==0){
          GyroOutX3.X_ANGL_OUT=0;
          GyroOutX3.Y_ANGL_OUT=0;
          GyroOutX3.Z_ANGL_OUT=0;
          GyroX3_SetOff_Loop=(LoopValue*2+1);
          return;
        }
         if(GyroX3_Null_Flag==1){
          GyroOutX3.X_ANGL_OUT=0;
          GyroOutX3.Y_ANGL_OUT=0;
          GyroOutX3.Z_ANGL_OUT=0;
          GyroX3_SetOff_Loop=0;
          GyroX3_SetOff_Flag=0;
          return;          
        }
      }
      
    }
    else{
      ReadGyroX3();
      TIMCompare1=PWM1calculator();
      TIMCompare2=PWM2calculator();
      TIM_ClearITPendingBit(Timer,TIM_IT_Update);
    }
  }
}



/*
////////////////////////////////一阶线性回归发//////////////////////////
float k_null=0;
s16 b_null=0;
s32 sum_null=0;
s32 s1=0;
s32 s2=0;
s32 s3=0;
s32 s4=0;
s32 s5=0;
void ReadGyroX3(void){
  
  
  u8 Gyrotemp[6];
  I2C_SequentialRead(Gyrotemp,OUT_X_L,6);
  if(n<=0x01ff){
    
    GyroOutX3.Y_GYRO_OUT=(Gyrotemp[2]|Gyrotemp[3]<<8);
    s1+=(n*n);
    s4+=n;
    s5=n;
    s2+=GyroOutX3.Y_GYRO_OUT;
    s3+=(GyroOutX3.Y_GYRO_OUT*n);
    n++;
    return;
  }
  else
  {
    if(n==0x0200){
    k_null=((s5*s3-s2*s4)/(s5*s1-s4*s4));
    b_null=((s2/s5)-k_null*s4/s5);
    
    //sum_null=k_null*s5+b_null;
    GyroOutX3.Y_ANGL_OUT=0;
    n++;
    return;
  }

  else{
    GyroOutX3.X_GYRO_OUT=Gyrotemp[0]|Gyrotemp[1]<<8;
    GyroOutX3.Y_GYRO_OUT=(Gyrotemp[2]|Gyrotemp[3]<<8-sum_null);
    
    GyroOutX3.Z_GYRO_OUT=Gyrotemp[4]|Gyrotemp[5]<<8;
    GyroOutX3.X_ANGL_OUT+=GyroOutX3.X_GYRO_OUT;
    GyroOutX3.Y_ANGL_OUT+=(GyroOutX3.Y_GYRO_OUT);
    sum_null+=k_null;
    GyroOutX3.Z_ANGL_OUT+=GyroOutX3.Z_GYRO_OUT;
  }
  }
}
*/

/*
///////////////////////平均数法/////////////////
void Gyro3X_Null(void){
  s16 n=2048;
  s16 i=0;
  s32 temp=0;
  u8 Gyrotemp[6];
  s16 temp2=0;
  
  for(i=0;i<n;i++){
    
    I2C_SequentialRead(Gyrotemp,OUT_X_L,6);
    temp2=(Gyrotemp[2]|Gyrotemp[3]<<8);
    temp+=temp2;
    ms_Delay(1);
  }
  sum_null=temp/n;
    
  GyroOutX3.Y_ANGL_OUT=0;

}
*/