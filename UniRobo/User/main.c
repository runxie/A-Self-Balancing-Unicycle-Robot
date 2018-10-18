
#include "stm32f10x.h"
#include "main.h"
#include "LCD_Dis.h"
#include "key.h"
#include "SCA100T.h"
//#include "ADIS16255.h"
#include "L3G4200D.h"
#include "Motor.h"
#include "Bar.h"
#include "MotorCoder.h"
#include "Delay.h"
#include "ADXL345.h"
#include "I2C.h"

//void Delay(vu32 nCount);
/*
void PutChar(unsigned char x,unsigned char y,unsigned char a); 
void PutString(unsigned char x,unsigned char y,unsigned char *p);
void PutString_cn_Matrix(unsigned char x,unsigned char y,unsigned char *s,unsigned char type);
void Show_Variable_4Num(unsigned char x,unsigned char y,unsigned int Show_D);
char Get_KeyValue(void);
*/
#define Timer TIM1
#define TimerCLK RCC_APB2Periph_TIM1
 

char Systemp='7';

main(void)
{ 
  System_Init();
  while(1){}
 
}
void System_Init(void){
        NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;
        
 
        LCD_Init(); 
        ClrScreen();
        SetBackLight(100);
        FontMode(1,0);
        FontSet_cn(0,1);
        PutString_cn(22,0,"欢迎");
        I2CInit();
        //外设初始化
       
       
        GyroX3Init();
        ADXL345Init();
        
        //GyroInit();
        //InclinometerInit();
        MotorInit();
        CoderInit(); 
        CoderInit2();
        BarInit();
        KeyInit();    
        
        
        HandNull();
        
           
        
        RCC_APB2PeriphClockCmd(TimerCLK,ENABLE);
        //timer configuration
	TIM_BaseInitStructure.TIM_Period = 400-1; //5Hz
	TIM_BaseInitStructure.TIM_Prescaler = (36000-1);
	TIM_BaseInitStructure.TIM_ClockDivision = 0;
	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_BaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(Timer,&TIM_BaseInitStructure);


	TIM_ClearFlag(Timer,TIM_IT_Update);

	TIM_ITConfig(Timer,TIM_IT_Update,ENABLE);

	TIM_Cmd(Timer,ENABLE);
           
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn; //更新事件
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1 ; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7; //响应优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //允许中断
	NVIC_Init(&NVIC_InitStructure); //写入设置
        
        

}

void TIM1_UP_IRQHandler(void){
  while(TIM_GetITStatus(Timer,TIM_IT_Update)!=RESET){     
    //TIM_ClearITPendingBit(Timer,TIM_FLAG_Update);
    char temp=GetKeyValue();
     
    if(temp!='x'){
      if(Systemp!=temp){
        ClrScreen();
        Systemp=temp;
      }
    }

     switch(Systemp){
     case '1':  ADXL345Monitor();break;
     case '2':  GyroX3Monitor();break;
     case '3':  MotorSpeedMonitor();break;
     case '4':  ADXL345NULL();break;
     case '5':  GyroX3NULL();break;
     case '6':  DataZore();break;
     case '7':  MotorData();break;
     case '8':  ShutMotorDown();break;
     case '9':  Restart();break;
     case 'A':  DataMenu1();break;
     case 'B':  DataMenu2();break;
     case 'C':  ControlMenu();break;
     case '#':  MainMenu();break;
     case '*':  MainMenu();break;
     default:break;
     }
    TIM_ClearITPendingBit(Timer,TIM_FLAG_Update);
  }
    
}

void HandNull(void){
  
        GyroX3_X_SetOff1=GyroX3_X_SetOff2=0;//-550;
        GyroX3_Y_SetOff1=GyroX3_Y_SetOff2=0;//200;
        GyroX3_Z_SetOff1=GyroX3_Z_SetOff2=0;
        ADXL345_X_SetOff= 0;//140;
        ADXL345_Y_SetOff= 0;//110;
        ADXL345_Z_SetOff= 0;
        ANGLX_Setoff=190;
        ANGLY_Setoff=-400;
        
        
}

void ShutMotorDown(void){
 
    MotorDown=1;
    TIM_SetCompare1(TIM5,0);
    TIM_SetCompare2(TIM5,0); 
  
  ms_Delay(20);
  Systemp='6';
} 


void Restart(void){
  
  MotorDown=0;
  Systemp='7';
} 




void MotorData(void){
  PutString(5,0,"MotorData");
  PutString(0,15,"ANGL_X");
  PutString(0,30,"ANGL_Y");

  ShowFloat(50,17, (angleX-ANGLX_Setoff));
  ShowFloat(50,32, (angleY-ANGLY_Setoff));
   Showu16(50,52, count);
  
} 
void MainMenu(void){
  //ClrScreen();
  PutString(5,0,"GyroSystem(v4.14)");
  PutString(0,15,"A.GyroX1 Monitor");
  PutString(0,30,"B.GyroX3 Monitor");
  PutString(0,45,"C.MotorSpeeed & BarValue ");
}
void DataMenu1(void){
  //ClrScreen();
  PutString(5,0,"DataMenu1");
  PutString(0,15,"1.GyroX1 Monitor");
  PutString(0,30,"2.GyroX3 Monitor");
  PutString(0,45,"3.MotorSpeeed & BarValue ");
}
void DataMenu2(void){
  //ClrScreen();
  PutString(5,0,"DataMenu");
  PutString(0,15,"4.ADXL345NULL");
  PutString(0,30,"5.GyroX3NULL");
  PutString(0,45,"6.DataZore");
}
void ControlMenu(void){
  //ClrScreen();
  PutString(5,0,"ControlMenu");
  PutString(0,15,"7.MotorData");
  PutString(0,30,"8.ShutMotorDown");

}
/*
void GyroX1Monitor(void){
  //ClrScreen();
  PutString(5,0,"GyroX1 Monitor");
  PutString(0,15,"GYRO");
  PutString(0,30,"ANGL");
  PutString(0,45,"TEMP");
  ShowFloat(50,17,GyroOutX1.GYRO_OUT*0.00458);
  ShowFloat(50,32,GyroOutX1.ANGL_OUT*0.03663);
  ShowFloat(50,47,GyroOutX1.TEMP_OUT*0.01816);
}
*/

void InclinometerMonitor(void){
  //ClrScreen();
  PutString(5,0,"Inclinometer");
  PutString(0,15,"ANGL_X");
  PutString(0,30,"ANGL_Y");
  //PutString(0,45,"TEMP");
  ShowFloat(50,17,InclinometerX2Out.ANGLX_OUT);
  ShowFloat(50,32,InclinometerX2Out.ANGLY_OUT);
  //ShowFloat(50,47,GyroOutX1.TEMP_OUT*0.01816);
}

void GyroX3Monitor(void) {
   //ClrScreen();
  GyroX3_Null_Flag=0;
  PutString(5,0,"GyroX3 Monitor");
  PutString(0,15,"ANGL_X");
  PutString(0,30,"ANGL_Y");
  PutString(0,45,"ANGL_Z");
  ShowFloat(50,17, GyroOutX3.X_GYRO_OUT);
  ShowFloat(50,32, GyroOutX3.Y_GYRO_OUT);
  ShowFloat(50,47, GyroOutX3.Z_GYRO_OUT);
 // ShowFloat(50,47, GyroOutX3.Z_GYRO_OUT*0.003815);
}

void ADXL345Monitor(void) {
   //ClrScreen();
  PutString(5,0,"ADXL345 Monitor");
  PutString(0,15,"ANGL_X");
  PutString(0,30,"ANGL_Y");
  PutString(0,45,"ANGL_Z");
  ShowFloat(50,17, ADXL345_Out.X_ANGL_OUT/8);
  ShowFloat(50,32, ADXL345_Out.Y_ANGL_OUT/8);
  ShowFloat(50,47, ADXL345_Out.Z_ANGL_OUT/8);
 // ShowFloat(50,47, GyroOutX3.Z_GYRO_OUT*0.003815);
}
void ADXL345NULL(void){
  
  
  PutString(22,0,"ADXL345NULL");
  u8 ADXL345temp[6]={0,0,0,0,0,0};
  s16 Datatemp[3]={0,0,0};
  u16 i=0;

  for(i=0;i<128;i++){
    Showu16(10,15,i);
    I2C_SequentialRead(ADXL345Addr_R,ADXL345temp,DATA_X_L,6);
    ms_Delay(3);
    Datatemp[0]+= ((ADXL345temp[0]|ADXL345temp[1]<<8)<<3);
    Datatemp[1]+= ((ADXL345temp[2]|ADXL345temp[3]<<8)<<3);
    Datatemp[2]+= ((ADXL345temp[4]|ADXL345temp[5]<<8)<<3);
  }

  ADXL345_X_SetOff= (Datatemp[0]/128);
  ADXL345_Y_SetOff= (Datatemp[1]/128);
  ADXL345_Z_SetOff= (Datatemp[2]/128);
  

  Systemp='1'; 
  
  /*
  PutString(22,0,"ADXL345NULL");
  u8 ADXL345temp[6]={0,0,0,0,0,0};
  s16 Datatemp[3]={0,0,0};
  u8 byData[3]={0,0,0};
  u16 i=0;

  I2C_SequentialWrite(ADXL345Addr_W,byData,0x1E,3); 
  for(i=0;i<128;i++){
    Showu16(10,15,i);
    I2C_SequentialRead(ADXL345Addr_R,ADXL345temp,DATA_X_L,6);
    Datatemp[0]-= (ADXL345temp[0]|ADXL345temp[1]<<8)<<3;
    Datatemp[1]-= (ADXL345temp[2]|ADXL345temp[3]<<8)<<3;
    Datatemp[2]-= (ADXL345temp[4]|ADXL345temp[5]<<8)<<3;
    ms_Delay(10);
  }
  byData[0]= (Datatemp[0]/1024);
  byData[1]= (Datatemp[1]/1024);
  byData[2]= (Datatemp[2]/1024);
  
  I2C_SequentialWrite(ADXL345Addr_W,byData,0x1E,3);

  Systemp='1';
  */
}


void MotorSpeedMonitor(void){
  //ClrScreen();
  PutString(5,0,"MotorSpeeed");
  PutString(0,15,"Motor1:");
  PutString(0,32,"Motor2:");
  PutString(0,47,"speed:");
  ShowFloat(50,17, MotorCoderData.Counter);
  ShowFloat(50,32, MotorCoderData2.Counter);
  Showu16(50,47, speed);
}
  
void GyroX3NULL(void){
  if(!GyroX3_Null_Flag){
    GyroX3_SetOff_Flag=1;
    Showu16(20,0,GyroX3_SetOff_Loop);
    Showu16(50,0,GyroX3_X_Scale_S);
    Showu16(60,0,GyroX3_X_Scale_L);
    Showu16(70,0,GyroX3_Y_Scale_S);
    Showu16(80,0,GyroX3_Y_Scale_L);
    Showu16(90,0,GyroX3_Z_Scale_S);
    Showu16(100,0,GyroX3_Z_Scale_L);
    
    PutString(0,15,"X_SetOff");
    PutString(0,30,"Y_SetOff");
    PutString(0,45,"Z_SetOff");
    Showu16(50,17, GyroX3_X_SetOff1);
    Showu16(50,32, GyroX3_Y_SetOff1);
    Showu16(50,47, GyroX3_Z_SetOff1);
    Showu16(90,17, GyroX3_X_SetOff2);
    Showu16(90,32, GyroX3_Y_SetOff2);
    Showu16(90,47, GyroX3_Z_SetOff2);
     
    
   // Systemp='2';
  }
  else{
    Systemp='2';
    ClrScreen();
  }
  return;
}

void DataZore(void){
    u16 i=0;
    u16 Datatemp[3]={0,0,0};
    u8 Gyrotemp[6];
    ms_Delay(1000);
    for(i=0;i<1024;i++){
      Showu16(10,35,i);
      I2C_SequentialRead(GyroX3Addr_R,Gyrotemp,OUT_X_L,6);
      ms_Delay(3);
      Datatemp[0]+= (Gyrotemp[0]|Gyrotemp[1]<<8);
      Datatemp[1]+= (Gyrotemp[2]|Gyrotemp[3]<<8);
      Datatemp[2]+= (Gyrotemp[4]|Gyrotemp[5]<<8);
    }
    GyroX3_X_SetOff1=GyroX3_X_SetOff2=( Datatemp[0]/1024);
    GyroX3_Y_SetOff1=GyroX3_Y_SetOff2=( Datatemp[1]/1024);
    GyroX3_Z_SetOff1=GyroX3_Z_SetOff2=( Datatemp[2]/1024);
    
    GyroOutX3.X_GYRO_OUT=0;
    GyroOutX3.Y_GYRO_OUT=0; 
    GyroOutX3.Z_GYRO_OUT=0;
    GyroOutX3.X_ANGL_OUT=0;
    GyroOutX3.Y_ANGL_OUT=0;
    GyroOutX3.Z_ANGL_OUT=0;
    
    ADXL345NULL();
  
    angleY=0;
    angleY_temp=0;
    angleX=0;
    angleX_temp=0;
    angleX_sum=0;
    angleY_sum=0;
    
    SimpleNULL=0;
    Systemp='9';
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif


