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
   TIM_BaseInitStructure.TIM_Period = 10-1; //�ж�Ƶ��200Hz
   TIM_BaseInitStructure.TIM_Prescaler = (36000-1);
   TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
   TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
   TIM_BaseInitStructure.TIM_RepetitionCounter = 0;
   TIM_TimeBaseInit(Timer,&TIM_BaseInitStructure);


   TIM_ClearFlag(Timer,TIM_IT_Update);

   TIM_ITConfig(Timer,TIM_IT_Update,ENABLE);

   TIM_Cmd(Timer,ENABLE);
   
   
   SetADXL345(); //����ADXL345����
		

   NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn; //�����¼�
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //��ռ���ȼ�0
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //��Ӧ���ȼ�4
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //�����ж�
   NVIC_Init(&NVIC_InitStructure); //д������
  
}

void SetADXL345(void){
  
    u8 MSub;
    u8 byData[13];

    //����ͨ�Ÿ�ʽ;����Ϊ�Լ칦�ܽ���,4����SPI�ӿ�,�͵�ƽ�ж����,13λȫ�ֱ���,��������Ҷ���,16g����
    MSub = 0x31;
    byData[0] = 0x2B;
    I2C_SequentialWrite(ADXL345Addr_W,byData,MSub,1);


    MSub = 0x1E;

    byData[0] = 0x00; //X������; (15.6mg/LSB)
    byData[1] = 0x00; //Y������; (15.6mg/LSB)
    byData[2] = 0x00;  //Z������; (15.6mg/LSB)
    byData[3] = 0x00;  //�û���ʱ0:����; (1.25ms/LSB)
    byData[4] = 0x00;  //����һ���û������ʱ0:����; (1.25ms/LSB)
    byData[5] = 0x00; //�û�����0:����; (1.25ms/LSB)
    byData[6] = 0x01;  //��������ֵ; (62.5mg/LSB)
    byData[7] = 0x01;  //�����⾲ֹ��ֵ; (62.5mg/LSB)
    byData[8] = 0x2B;  //���ʱ�䷧ֵ; (1s/LSB)
    byData[9] = 0x00;
    byData[10] = 0x09; //�����������Ƽ���ֵ; (62.5mg/LSB)
    byData[11] =  0xFF;  //����������ʱ�䷧ֵ,����Ϊ���ʱ��; (5ms/LSB)
    byData[12] =  0x80;
    I2C_SequentialWrite(ADXL345Addr_W,byData,MSub,13);

    MSub = 0x2C;

    byData[0] = 0x0A;
    byData[1] = 0x28;  //����Link,��������;�ر��Զ�����,����,���ѹ���
    byData[2] = 0x00;  //���о��ر�
    byData[3] = 0x00;  //�жϹ����趨,��ʹ���ж�
    I2C_SequentialWrite(ADXL345Addr_W,byData,MSub,4);
    
    //FIFOģʽ�趨,Streamģʽ����������INT1,31����������
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
////////////////////////////////һ�����Իع鷢//////////////////////////
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
///////////////////////ƽ������/////////////////
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