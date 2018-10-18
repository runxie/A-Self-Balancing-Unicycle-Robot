#include "stm32f10x.h"


#include "Motor.h"


#define Timer TIM5
#define TimerCLK RCC_APB1Periph_TIM5
#define RefreshTimer TIM8
#define RefreshCLK RCC_APB2Periph_TIM8


#define MotorPort	GPIOA
#define RCC_APB2Periph_Motor1Port (RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC |RCC_APB2Periph_AFIO)
#define MotorPin	(GPIO_Pin_0 | GPIO_Pin_1) 


#define INPort GPIOC
#define INPin (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3)

struct Control ControlData;
float Torque1;
float Torque2;
float Torque1_temp;
float Torque2_temp;
s32 calculate1_temp;
s32 calculate2_temp;
float angleX=0; 
float angleX_temp=0;
float angleY=0;
float angleY_temp=0;
s32 angleX_sum=0;
s32 angleY_sum=0;

float ANGLX_Setoff=0;
float ANGLY_Setoff=0;

u16 a_drc1=0;
u16 a_drc2=0;
u16 c_drc1=0;
u16 c_drc2=0;
u16 a_add=0;
u16 c_add=0;
u16 count=0;


u16 TIMCompare1=0;
u16 TIMCompare2=0;

u16 MotorDown=0;                                                                                                                   
s16 speed=0;
s16 speed_drc=-1;


void MotorInit(void){
  
        NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;
        TIM_OCInitTypeDef TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_Motor1Port,ENABLE);
        
	GPIO_InitStructure.GPIO_Pin = MotorPin;                      
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MotorPort, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin = INPin;                      
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(INPort, &GPIO_InitStructure);

	RCC_APB1PeriphClockCmd(TimerCLK,ENABLE);
		//timer configuration
	TIM_BaseInitStructure.TIM_Period = (3600-1); //PWM频率10KHz(36MHz/3.6K)
	TIM_BaseInitStructure.TIM_Prescaler = 0;
	TIM_BaseInitStructure.TIM_ClockDivision = 0;
	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_BaseInitStructure.TIM_RepetitionCounter = 0; //周期计数器值,RCR向下计数器每次计数至0，会产生一个更新事件且计数器重新由RCR值（N）开始计数。
	TIM_TimeBaseInit(Timer,&TIM_BaseInitStructure);
        
      
        //通道OC1
        TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
        TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable; //选择输出比较状态
        TIM_OCInitStructure.TIM_OutputNState=TIM_OutputState_Disable;//反向输出关闭
        TIM_OCInitStructure.TIM_Pulse=0; //设置了待装入捕获比较寄存器的脉冲值。
        TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;//输出极性
        TIM_OC1Init(Timer,&TIM_OCInitStructure);
        //通道OC2
        TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
        TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable; //选择输出比较状态
        TIM_OCInitStructure.TIM_OutputNState=TIM_OutputState_Disable;//反向输出关闭
        TIM_OCInitStructure.TIM_Pulse=0; //设置了待装入捕获比较寄存器的脉冲值。
        TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;//输出极性
        TIM_OC2Init(Timer,&TIM_OCInitStructure);
        
        TIM_OC1PreloadConfig(Timer,TIM_OCPreload_Enable);
        TIM_OC2PreloadConfig(Timer,TIM_OCPreload_Enable);
        TIM_ARRPreloadConfig(Timer,ENABLE);

	TIM_PrescalerConfig(Timer, 8, TIM_PSCReloadMode_Immediate); //?
	TIM_ClearFlag(Timer,TIM_IT_Update);
	TIM_ITConfig(Timer,TIM_IT_Update,ENABLE); //使能指定的TIM中断

	TIM_Cmd(Timer,ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn ; //更新事件:TIM5 Update Interrupt 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5; //响应优先级5
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //允许中断
	NVIC_Init(&NVIC_InitStructure); //写入设置
        
        
          
        RCC_APB2PeriphClockCmd(RefreshCLK,ENABLE);
        //timer configuration
	TIM_BaseInitStructure.TIM_Period = 200-1; //10Hz
	TIM_BaseInitStructure.TIM_Prescaler = (36000-1);
	TIM_BaseInitStructure.TIM_ClockDivision = 0;
	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_BaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(RefreshTimer,&TIM_BaseInitStructure);

	TIM_ClearFlag(RefreshTimer,TIM_IT_Update);

	TIM_ITConfig(RefreshTimer,TIM_IT_Update,ENABLE);

	TIM_Cmd(RefreshTimer,ENABLE);
           
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_IRQn; //更新事件
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0 ; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //响应优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //允许中断
	NVIC_Init(&NVIC_InitStructure); //写入设置

       
}

void Pin_A_R(void){
  GPIO_SetBits(GPIOC,GPIO_Pin_0);
  GPIO_ResetBits(GPIOC,GPIO_Pin_1);
}

void Pin_A_L(void){
  GPIO_SetBits(GPIOC,GPIO_Pin_1);
  GPIO_ResetBits(GPIOC,GPIO_Pin_0);
}

void Pin_B_R(void){
  GPIO_SetBits(GPIOC,GPIO_Pin_2);
  GPIO_ResetBits(GPIOC,GPIO_Pin_3);
}

void Pin_B_L(void){
  GPIO_SetBits(GPIOC,GPIO_Pin_3);
  GPIO_ResetBits(GPIOC,GPIO_Pin_2);
}
u16 PWM1calculator(void){
  
  float kd=0.00;//0.022;//0.068;
  float kp=0.7;//0.79;//0.67;//0.622;
  float ki=-2.5;//0.0006;
  float a=0.01;
  
  u16 PWM=0;
  s32 calculate=0; 
   
  angleX=a*13*ADXL345_Out.X_ANGL_OUT+(1-a)*(angleX_temp+0.022*GyroOutX3.Y_GYRO_OUT); //2.85,0.04375
  calculate=kd*GyroOutX3.Y_GYRO_OUT+kp*(angleX-ANGLX_Setoff)+ki*(MotorCoderData.dCounter+speed);

  /*
   if(angleX>300){
    calculate=kd*GyroOutX3.Y_GYRO_OUT+kp*angleX+ki*(MotorCoderData.dCounter+speed)+1*(angleX-300);
  }
  if(angleX<-300){
   calculate=kd*GyroOutX3.Y_GYRO_OUT+kp*angleX+ki*(MotorCoderData.dCounter+speed)+1*(angleX+300);
  }
   if(angleX<=300&&angleX>=0){
   calculate=kd*GyroOutX3.Y_GYRO_OUT+kp*angleX+ki*(MotorCoderData.dCounter+speed);
  }
  if(angleX>=-300&&angleX<-0){
   calculate=kd*GyroOutX3.Y_GYRO_OUT+kp*angleX+ki*(MotorCoderData.dCounter+speed);
  }
 */
  angleX_temp=angleX;
  
  if(calculate>3600){
    calculate=3600;
  }
  if(calculate<-3600){
    calculate=-3600;
  } 
  
   
  if(calculate<0){
    PWM=-calculate+121;//171
    
    Pin_A_R();
    
  }
  if(calculate>0){
    PWM=calculate+121;//176
    Pin_A_L(); 
  }
  
  /*
  if(calculate<20){
    PWM=-calculate+121;//171
    
    Pin_A_R();
    
  }
  if(calculate>20){
    PWM=calculate+126;//176
    Pin_A_L(); 
  }
  */
  
/*  
  angleX=a*11.1*ADXL345_Out.X_ANGL_OUT+(1-a)*(angleX_temp+0.061*GyroOutX3.Y_GYRO_OUT);
  calculate=kd*GyroOutX3.Y_GYRO_OUT+kp*angleX+ki*calculate1_temp;   

  
  ////////////////////////////大角度校准//////////////////////////////
  if(angleX>500){
  
   calculate=kd*GyroOutX3.Y_GYRO_OUT+kp*angleX+ki*calculate1_temp+2*(angleX-500);
  }
  if(angleX<-500){
  
   calculate=kd*GyroOutX3.Y_GYRO_OUT+kp*angleX+ki*calculate1_temp+2*(angleX+500);
  }
  else{
    //angleX_sum+=0.9*angleX;
    
  
    calculate=kd*GyroOutX3.Y_GYRO_OUT+kp*angleX+ki*calculate1_temp;   
    
  }

  
  if(calculate<0){
    PWM=-calculate;//+161;
    
    Pin_A_R();
    
  }
  else{
    PWM=calculate;//+166;
    Pin_A_L(); 
  }
  */
  /*
  /////////////////////////DRY函数///////////////////////
 
  
  if( angleX>300){
     a_drc2++;
     a_drc1=0;
  }
  if(angleX<-300){
    a_drc1++;
    a_drc2=0;
  }
  if(a_drc1==50){
      a_add=50;
      a_drc1=0;  
  }
  if(a_drc2==50){
     
      a_add=50;
      a_drc2=0;
  }
  if(a_add>=25){
      a_add--;
      PWM=PWM+8*(50-a_add);
  }
  if(0<a_add&&a_add<25){
      a_add--;
      PWM=PWM+8*a_add;
  }
  */
 
 // calculate1_temp=0.998*(calculate+calculate1_temp);
  angleX_temp=angleX;
  count++;
  return PWM;
}


u16 PWM2calculator(void){
  //float a=63.323;
  //float b=0.93495;
  //float c=0.60345;
  //float d=0.2;
  //u16 f=210;//300;
  
  float kd=0.34;
  float kp=0.85;//0.29;
  float ki=-11;
  float a=0.01;
    
  s32 calculate=0; 
  u16 PWM=0;
  
  angleY=-a*14*ADXL345_Out.Y_ANGL_OUT+(1-a)*(angleY_temp+0.025*GyroOutX3.X_GYRO_OUT);

   if(angleY>250){
   calculate=kd*GyroOutX3.X_GYRO_OUT+kp*(angleY-ANGLY_Setoff)+ki*MotorCoderData2.dCounter+1.8*(angleY-250);
  }
  if(angleY<-250){
   calculate=kd*GyroOutX3.X_GYRO_OUT+kp*(angleY-ANGLY_Setoff)+ki*MotorCoderData2.dCounter+1.8*(angleY+250);
  }
   if(angleY<=250&&angleY>=0){
   calculate=kd*GyroOutX3.X_GYRO_OUT+kp*(angleY-ANGLY_Setoff)+ki*MotorCoderData2.dCounter;
  }
  if(angleY>=-250&&angleY<-0){
   calculate=kd*GyroOutX3.X_GYRO_OUT+kp*(angleY-ANGLY_Setoff)+ki*MotorCoderData2.dCounter;
  }

  angleY_temp=angleY;
  
  if(calculate>2900){
    calculate=2900;
  }
  if(calculate<-2900){
    calculate=-2900;
  }
  
  if(calculate<0){
    PWM=(-calculate)+700;
    Pin_B_L();
   
  }
  else{
    PWM=calculate+700;
    Pin_B_R(); 
    
  }
 //////////////////dry函数////////////////
/*
  if(c_drc1==150){
    if(PWM>400){
      PWM+=1550;
    }
    c_drc1=0;
  }
  if(c_drc2==150){
    if(PWM>400){
      PWM+=1550;
    }
    c_drc2=0;
  }
  if(GyroOutX3.Y_ANGL_OUT&0x8000){
     a_drc2++;
     a_drc1=0;
  }
  if(!(GyroOutX3.Y_ANGL_OUT&0x8000)){
    a_drc1++;
    a_drc2==0;
  }
  if(a_drc1==100){
    if(GyroOutX3.Y_ANGL_OUT>10000){
      GyroOutX3.Y_ANGL_OUT=0;
      PWM+=400;
      a_drc1-=100;
    }
    else{
      a_drc1=0;  
    }
  }
  if(a_drc2==100){
     if(GyroOutX3.Y_ANGL_OUT<-10000){
      GyroOutX3.Y_ANGL_OUT=0;
      PWM+=400;
       a_drc2-=100;
    }
     else{
       a_drc2=0;
     }
  }
 */
///////////////////////////////
  calculate2_temp=calculate;
  return PWM;
}

void TORQUEcalculator(void){
    float a1=-5.8510;
    float b1=-7.608;
    float c1=-27.1693;
    float d1=-4.0543;
    float a2=-5.2333;
    float b2=-92.7146;
    float c2=-3033.5;
    float d2=-471.5936;
    Torque1_temp=Torque1;
    Torque2_temp=Torque2;
    Torque1=a1*MotorCoderData.Counter+b1*MotorCoderData.dCounter;
     
    Torque2=a1*MotorCoderData2.Counter+b1*MotorCoderData2.dCounter
      +GyroOutX3.Y_ANGL_OUT+GyroOutX3.Y_GYRO_OUT;
}



void TIM5_IRQHandler(void){
   if(TIM_GetITStatus(Timer,TIM_IT_Update)!=RESET){  
    TIM_ClearITPendingBit(Timer,TIM_IT_Update);
    if(!MotorDown){
      MotorControl();
    }
   }
}


void MotorControl(void){  
    //TORQUEcalculator();
    TIM_SetCompare1(Timer,TIMCompare1);
    TIM_SetCompare2(Timer,TIMCompare2); 
}


void TIM8_UP_IRQHandler(void){
  while(TIM_GetITStatus(RefreshTimer,TIM_IT_Update)!=RESET){     
    TIM_ClearITPendingBit(RefreshTimer,TIM_FLAG_Update);
    Counter_Getter();
    Counter_Getter2();
  }
    
}