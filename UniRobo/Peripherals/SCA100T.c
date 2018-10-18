#include "stm32f10x.h"
#include "SCA100T.h"
#include "Delay.h"
#include "LCD_Dis.h"

#define Timer TIM6
#define TimerCLK RCC_APB1Periph_TIM6

#define InclinometerPort GPIOA
#define SPIPort SPI1
#define NSS GPIO_Pin_4
#define SCK GPIO_Pin_5
#define MISO GPIO_Pin_6
#define MOSI GPIO_Pin_7
#define RCC_APB2Periph_GyroPort RCC_APB2Periph_GPIOA
#define RCC_APB2Periph_SPIPort RCC_APB2Periph_SPI1
#define SPI_NSS_LOW()        GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define SPI_NSS_HIGH()       GPIO_SetBits(GPIOA,GPIO_Pin_4)


struct InclinometerOut InclinometerX2Out;

void InclinometerInit(void){
  
   
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GyroPort | RCC_APB2Periph_AFIO,ENABLE);

   GPIO_InitTypeDef GPIO_InitStructure;
   SPI_InitTypeDef SPI_InitStructure;
   NVIC_InitTypeDef NVIC_InitStructure; 
   TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;
  
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
   GPIO_InitStructure.GPIO_Pin=SCK | MOSI | NSS |MISO;
   GPIO_Init(InclinometerPort,&GPIO_InitStructure);
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
   GPIO_InitStructure.GPIO_Pin=NSS;
   GPIO_Init(InclinometerPort,&GPIO_InitStructure);
   
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPIPort,ENABLE);

   SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
   SPI_InitStructure.SPI_Mode=SPI_Mode_Master;
   SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;
   SPI_InitStructure.SPI_CPOL=SPI_CPOL_High;  // 空闲时为高电平
   SPI_InitStructure.SPI_CPHA=SPI_CPHA_2Edge;   //第二个边沿采样
   SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;
  
   /*时钟SCK=72MHz/36*/
   SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_64;
   SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;
   SPI_InitStructure.SPI_CRCPolynomial=7;
   SPI_Init(SPIPort,&SPI_InitStructure);
   SPI_Cmd(SPIPort,ENABLE);
   
  
   RCC_APB1PeriphClockCmd(TimerCLK,ENABLE);
		//timer configuration
   TIM_BaseInitStructure.TIM_Period = 36000-1; //中断频率10Hz
   TIM_BaseInitStructure.TIM_Prescaler = 100-1;
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



void SPI_Writebyte(u8 addr,u8 data){
  SPI_NSS_LOW(); 
  while(SPI_I2S_GetFlagStatus(SPIPort,SPI_I2S_FLAG_TXE) == RESET);
  SPI_I2S_SendData(SPIPort,addr);
  while (SPI_I2S_GetFlagStatus(SPIPort, SPI_I2S_FLAG_RXNE) == RESET);  
  SPI_I2S_ReceiveData(SPIPort);  
  while(SPI_I2S_GetFlagStatus(SPIPort,SPI_I2S_FLAG_TXE) == RESET);
  SPI_I2S_SendData(SPIPort,data);
  while (SPI_I2S_GetFlagStatus(SPIPort, SPI_I2S_FLAG_RXNE) == RESET);  
  SPI_I2S_ReceiveData(SPIPort);  
  SPI_NSS_HIGH();

}

u8 SPI_Readbyte(u8 addr){
  u16 readtemp=0;
  SPI_NSS_LOW(); 
  while(SPI_I2S_GetFlagStatus(SPIPort,SPI_I2S_FLAG_TXE) == RESET);
  SPI_I2S_SendData(SPIPort,addr);
  us_Delay(10);  
  while (SPI_I2S_GetFlagStatus(SPIPort, SPI_I2S_FLAG_RXNE) == RESET);
  readtemp=SPI_I2S_ReceiveData(SPIPort)<<8;  
  us_Delay(10);  
  //while (SPI_I2S_GetFlagStatus(SPIPort, SPI_I2S_FLAG_RXNE) == RESET);
  readtemp|=SPI_I2S_ReceiveData(SPIPort);  
  SPI_NSS_HIGH();
 
  return readtemp;  
}







u16 ReadAnglX(){
  u16 angle=0;
  angle=SPI_Readbyte(RADX);    

  angle=angle<<2;
  if(angle&0x8000){
    angle=angle|0x0003;
  }
  return angle;
 
}
u16 ReadAnglY(){
  u16 angle=0;
  angle=SPI_Readbyte(RADY);    

  angle=angle<<2;
  if(angle&0x8000){
    angle=angle|0x0003;
  }
  return angle;
 
}

void InclinometerReader(void){
 
  //ms_Delay(10);
  InclinometerX2Out.ANGLX_OUT=ReadAnglX();
  InclinometerX2Out.ANGLX_OUT=ReadAnglX();
  //ms_Delay(10);
  InclinometerX2Out.ANGLY_OUT=ReadAnglY();
  InclinometerX2Out.ANGLY_OUT=ReadAnglY();
  //ms_Delay(10);

  SPI_NSS_HIGH();
}


void TIM6_IRQHandler(void){
  if(TIM_GetITStatus(Timer,TIM_IT_Update)!=RESET){
    TIM_ClearITPendingBit(Timer,TIM_IT_Update);
    InclinometerReader();
  }
}