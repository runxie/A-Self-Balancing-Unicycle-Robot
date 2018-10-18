#include "stm32f10x.h"
#include "ADIS16255.h"
#include "Delay.h"
#include "LCD_Dis.h"

#define Timer TIM6
#define TimerCLK RCC_APB1Periph_TIM6

#define GyroPort GPIOA
#define SPIPort SPI1
#define NSS GPIO_Pin_4
#define SCK GPIO_Pin_5
#define MISO GPIO_Pin_6
#define MOSI GPIO_Pin_7
#define RCC_APB2Periph_GyroPort RCC_APB2Periph_GPIOA
#define RCC_APB2Periph_SPIPort RCC_APB2Periph_SPI1
#define SPI_NSS_LOW()        GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define SPI_NSS_HIGH()       GPIO_SetBits(GPIOA,GPIO_Pin_4)


struct GyroOut GyroOutX1;

void GyroInit(void){
  
   
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GyroPort | RCC_APB2Periph_AFIO,ENABLE);

   GPIO_InitTypeDef GPIO_InitStructure;
   SPI_InitTypeDef SPI_InitStructure;
   NVIC_InitTypeDef NVIC_InitStructure; 
   TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;
  
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
   GPIO_InitStructure.GPIO_Pin=SCK | MOSI | NSS |MISO;
   GPIO_Init(GyroPort,&GPIO_InitStructure);
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
   GPIO_InitStructure.GPIO_Pin=NSS;
   GPIO_Init(GyroPort,&GPIO_InitStructure);
   
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPIPort,ENABLE);

   SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
   SPI_InitStructure.SPI_Mode=SPI_Mode_Master;
   SPI_InitStructure.SPI_DataSize=SPI_DataSize_16b;
   SPI_InitStructure.SPI_CPOL=SPI_CPOL_High;  // 空闲时为高电平
   SPI_InitStructure.SPI_CPHA=SPI_CPHA_2Edge;   //第二个边沿采样
   SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;
  
   /*时钟SCK=72MHz/36*/
   SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_64;
   SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;
   SPI_InitStructure.SPI_CRCPolynomial=7;
   SPI_Init(SPIPort,&SPI_InitStructure);
   SPI_Cmd(SPIPort,ENABLE);
   
   GyroSet();
  
   RCC_APB1PeriphClockCmd(TimerCLK,ENABLE);
		//timer configuration
   TIM_BaseInitStructure.TIM_Period = 36000-1; //中断频率250Hz
   TIM_BaseInitStructure.TIM_Prescaler = 4-1;
   TIM_BaseInitStructure.TIM_ClockDivision = 0;
   TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
   TIM_BaseInitStructure.TIM_RepetitionCounter = 0;
   TIM_TimeBaseInit(Timer,&TIM_BaseInitStructure);

   TIM_PrescalerConfig(Timer, 0, TIM_PSCReloadMode_Immediate);
   TIM_ClearFlag(Timer,TIM_IT_Update);
   TIM_ITConfig(Timer,TIM_IT_Update,ENABLE);

   TIM_Cmd(Timer,ENABLE);

   NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn; //更新事件
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级0
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //响应优先级1
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //允许中断
   NVIC_Init(&NVIC_InitStructure); //写入设置
  
}



void SPI_Writebyte(u16 addr,u8 data){
  while(SPI_I2S_GetFlagStatus(SPIPort,SPI_I2S_FLAG_TXE) == RESET);
  SPI_I2S_SendData(SPIPort,0x8000|(addr<<8)|data);
  while (SPI_I2S_GetFlagStatus(SPIPort, SPI_I2S_FLAG_RXNE) == RESET);  
  SPI_I2S_ReceiveData(SPIPort);  

}

u16 SPI_Readbyte(u16 addr){
  while(SPI_I2S_GetFlagStatus(SPIPort,SPI_I2S_FLAG_TXE) == RESET);
  SPI_I2S_SendData(SPIPort,(addr<<8)&0x3fff);
  while (SPI_I2S_GetFlagStatus(SPIPort, SPI_I2S_FLAG_RXNE) == RESET);
  us_Delay(10);  
  return SPI_I2S_ReceiveData(SPIPort);  
}


void Gyro_Null(){
  s16 temp=0;
  u8 temp_L,temp_H=0;
  u16 i=0;
  u16 N=256;
  
  SPI_Writebyte(SENS_AVG_L,0x0F); //滤波器开至最大
  ms_Delay(20);

  SPI_Writebyte(COMMAND_L,0x02); //出厂校准
  ms_Delay(20);
 
  SPI_Writebyte(COMMAND_L,0x01); //自动校准
  ms_Delay(100);
  
  SPI_Writebyte(SENS_AVG_L,0x05);  //滤波器抽头数 :2^5=32
  ms_Delay(20);
  
  ReadGyro();
  for(i=0;i<N;i++){
    temp+= ReadGyro();
    ms_Delay(10);
  }
  temp=temp/256;
  temp_L=temp&0x00ff;
  temp_H=temp>>8;
  SPI_Writebyte(GYRO_OFF_L,temp_L);
  ms_Delay(20);
  SPI_Writebyte(GYRO_OFF_H,temp_H);
  ms_Delay(20);
 
  return;
}


void Auto_Null(){
 //u16 angle=1;
  SPI_Writebyte(SENS_AVG_L,0x0F); //滤波器开至最大
  ms_Delay(20);
 // angle=SPI_Readbyte(SENS_AVG_L);
 // angle=1;
  SPI_Writebyte(COMMAND_L,0x02); //出厂校准
  ms_Delay(20);
  //angle=SPI_Readbyte(COMMAND_L);
 // angle=1;
  SPI_Writebyte(COMMAND_L,0x01); //自动校准
  ms_Delay(100);
  SPI_Writebyte(COMMAND_L,0x01); //自动校准
  ms_Delay(100);
  SPI_Writebyte(COMMAND_L,0x01); //自动校准
  ms_Delay(100);
  SPI_Writebyte(COMMAND_L,0x01); //自动校准
  ms_Delay(100);
  SPI_Writebyte(COMMAND_L,0x01); //自动校准
  ms_Delay(100);
  SPI_Writebyte(COMMAND_L,0x01); //自动校准
  ms_Delay(100);
 // angle=SPI_Readbyte(COMMAND_L);
 //  angle=SPI_Readbyte(0x14);
  return;
}





void GyroSet(void){
  SPI_NSS_LOW(); 
  ms_Delay(20);
  //Auto_Null();
  Gyro_Null();
  SPI_Writebyte(SMPL_PRD_L,0x01);  //ADC采样周期  256Hz
  ms_Delay(20);
  SPI_Writebyte(SENS_AVG_L,0x05);  //滤波器抽头数 :2^5=32
  ms_Delay(20);
  SPI_Writebyte(SENS_AVG_H,0x01);  //80度每秒
  ms_Delay(20);
  SPI_NSS_HIGH();
}






u16 ReadAngl(){
  u16 angle=0;
  angle=SPI_Readbyte(ANGL_OUT_L);    

  angle=angle<<2;
  if(angle&0x8000){
    angle=angle|0x0003;
  }
  return angle;
 
}

u16 ReadGyro(){
  u16 velocity=0;
  velocity=SPI_Readbyte(GYRO_OUT_L); 
  
 
  if(velocity&0x2000){
   
    velocity=velocity&0x1fff;
    velocity=(~velocity)+1;
  }
 
/*
  if(velocity&0x2000){
    velocity=(~velocity)+1;
    velocity=velocity&0x3fff;
    velocity=(~velocity)+1;
  }
 */
  return velocity;  
    
/*  
  velocity=velocity<<2;
  if(velocity&0x8000){
    velocity=velocity|0x0003;
  }
  return velocity;
  */
  
}

u16 ReadTemp(){
  u16 temp=0;
  temp=SPI_Readbyte(TEMP_OUT_L);
  temp=temp<<4;
  if(temp&0x8000){
    temp=temp|0x000f;
  }
  return temp;
}


void Gyroscope(void){
  SPI_NSS_LOW(); 
  //ms_Delay(10);
  GyroOutX1.GYRO_OUT=ReadGyro();
  GyroOutX1.GYRO_OUT=ReadGyro();
  //ms_Delay(10);
  GyroOutX1.ANGL_OUT=ReadAngl();
  GyroOutX1.ANGL_OUT=ReadAngl();
  //ms_Delay(10);
  GyroOutX1.TEMP_OUT=ReadTemp();
  GyroOutX1.TEMP_OUT=ReadTemp();
  //ms_Delay(10);
  SPI_NSS_HIGH();
}


void TIM6_IRQHandler(void){
  if(TIM_GetITStatus(Timer,TIM_IT_Update)!=RESET){
    TIM_ClearITPendingBit(Timer,TIM_IT_Update);
    Gyroscope();
  }
}