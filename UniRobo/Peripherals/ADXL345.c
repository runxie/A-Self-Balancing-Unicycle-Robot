#include "stm32f10x.h"
#include "L3G4200D.h"
#include "ADXL345.h"
#include "Delay.h"
#include "LCD_Dis.h"
#include "Motor.h"
#include "I2C.h"

#define Timer TIM6
#define TimerCLK RCC_APB1Periph_TIM6

#define I2CPort I2C1
#define ADXL345Port GPIOB
#define SCL GPIO_Pin_6
#define SDA GPIO_Pin_7
#define Power GPIO_Pin_8
#define GND GPIO_Pin_9

//#define ADXL345Addr 0x1D //0xD0
//#define ADXL345Addr_W 0x1D+0
//#define ADXL345Addr_R 0x1D+1

struct ADXL345Out ADXL345_Out={0,0,0};

s16 ADXL345_X_SetOff=0;
s16 ADXL345_Y_SetOff=0;
s16 ADXL345_Z_SetOff=0;

void ADXL345Init(void)
{ 
   NVIC_InitTypeDef NVIC_InitStructure;
   TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;
      
   RCC_APB1PeriphClockCmd(TimerCLK,ENABLE);
		//timer configuration
   TIM_BaseInitStructure.TIM_Period = 10-1; //中断频率200Hz
   TIM_BaseInitStructure.TIM_Prescaler = (36000-1);
   TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
   TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
   TIM_BaseInitStructure.TIM_RepetitionCounter = 0;
   TIM_TimeBaseInit(Timer,&TIM_BaseInitStructure);


   TIM_ClearFlag(Timer,TIM_IT_Update);

   TIM_ITConfig(Timer,TIM_IT_Update,ENABLE);

   TIM_Cmd(Timer,ENABLE);
   
   
   SetADXL345(); //设置ADXL345参数
		

   NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn; //更新事件
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级0
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //响应优先级4
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //允许中断
   NVIC_Init(&NVIC_InitStructure); //写入设置
  
}

void SetADXL345(void){
  
    u8 MSub;
    u8 byData[13];

    //数据通信格式;设置为自检功能禁用,4线制SPI接口,低电平中断输出,13位全分辨率,输出数据右对齐,16g量程
    MSub = 0x31;
    byData[0] = 0x2B;
    I2C_SequentialWrite(ADXL345Addr_W,byData,MSub,1);


    MSub = 0x1E;

    byData[0] = 0x00; //X轴误差补偿; (15.6mg/LSB)
    byData[1] = 0x00; //Y轴误差补偿; (15.6mg/LSB)
    byData[2] = 0x00;  //Z轴误差补偿; (15.6mg/LSB)
    byData[3] = 0x00;  //敲击延时0:禁用; (1.25ms/LSB)
    byData[4] = 0x00;  //检测第一次敲击后的延时0:禁用; (1.25ms/LSB)
    byData[5] = 0x00; //敲击窗口0:禁用; (1.25ms/LSB)
    byData[6] = 0x01;  //保存检测活动阀值; (62.5mg/LSB)
    byData[7] = 0x01;  //保存检测静止阀值; (62.5mg/LSB)
    byData[8] = 0x2B;  //检测活动时间阀值; (1s/LSB)
    byData[9] = 0x00;
    byData[10] = 0x09; //自由落体检测推荐阀值; (62.5mg/LSB)
    byData[11] =  0xFF;  //自由落体检测时间阀值,设置为最大时间; (5ms/LSB)
    byData[12] =  0x80;
    I2C_SequentialWrite(ADXL345Addr_W,byData,MSub,13);

    MSub = 0x2C;

    byData[0] = 0x0A;
    byData[1] = 0x28;  //开启Link,测量功能;关闭自动休眠,休眠,唤醒功能
    byData[2] = 0x00;  //所有均关闭
    byData[3] = 0x00;  //中断功能设定,不使用中断
    I2C_SequentialWrite(ADXL345Addr_W,byData,MSub,4);
    
    //FIFO模式设定,Stream模式，触发连接INT1,31级样本缓冲
    MSub = 0x38;
    byData[0] = 0x9F;
    I2C_SequentialWrite(ADXL345Addr_W,byData,MSub,1);
}


void ReadADXL345(void){
   u8 ADXL345temp[6];
   I2C_SequentialRead(ADXL345Addr_R,ADXL345temp,DATA_X_L,6);
    ADXL345_Out.X_ANGL_OUT=((ADXL345temp[0]|ADXL345temp[1]<<8)<<3)-ADXL345_X_SetOff;
    ADXL345_Out.Y_ANGL_OUT=((ADXL345temp[2]|ADXL345temp[3]<<8)<<3)-ADXL345_Y_SetOff; 
    ADXL345_Out.Z_ANGL_OUT=((ADXL345temp[4]|ADXL345temp[5]<<8)<<3)-ADXL345_Z_SetOff; 
}




void TIM6_IRQHandler(void){
  if(TIM_GetITStatus(Timer,TIM_IT_Update)!=RESET){ 
    TIM_ClearITPendingBit(Timer,TIM_IT_Update);
    ReadADXL345();
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