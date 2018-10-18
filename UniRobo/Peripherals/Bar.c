#include "stm32f10x.h"
#include "Bar.h"
#include "LCD_Dis.h"
#include "Motor.h"
#include "MotorCoder.h"
#define Timer TIM2
#define TimerCLK RCC_APB1Periph_TIM2


#define BarPort	GPIOE
#define RCC_APB2Periph_BarPort (RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOC)
#define BarPinIn	(GPIO_Pin_7)

struct BarOut BarData;

void BarInit(void){
        NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
        
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_BarPort,ENABLE);

	//GPIO_DeInit(BarPortIn);
	GPIO_InitStructure.GPIO_Pin = BarPinIn;                      
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BarPort, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12);                      
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
        
        
	RCC_APB1PeriphClockCmd(TimerCLK,ENABLE);
		//timer configuration
	TIM_BaseInitStructure.TIM_Period = 3600-1;
	TIM_BaseInitStructure.TIM_Prescaler = 1000-1;  //中断频率10Hz
	TIM_BaseInitStructure.TIM_ClockDivision = 0;
	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_BaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(Timer,&TIM_BaseInitStructure);


	TIM_ClearFlag(Timer,TIM_IT_Update);

	TIM_ITConfig(Timer,TIM_IT_Update,ENABLE);

	TIM_Cmd(Timer,ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; //更新事件
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; //响应优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //允许中断
	NVIC_Init(&NVIC_InitStructure); //写入设置

}

void TIM2_IRQHandler(void){//Bar MototCounterRead KeyScan
  if(TIM_GetITStatus(Timer,TIM_IT_Update)!=RESET){
    TIM_ClearITPendingBit(Timer,TIM_IT_Update);
    //BarRead
    if(!GPIO_ReadInputDataBit(BarPort,BarPinIn)){
     BarData.Bar1=0;
     if(speed_drc==-1){
       speed=80;
       GPIO_SetBits(GPIOC,GPIO_Pin_10);
       GPIO_ResetBits(GPIOC,GPIO_Pin_11);
       GPIO_ResetBits(GPIOC,GPIO_Pin_12);
     }
     if(speed_drc==1){
       speed=-80;
       GPIO_SetBits(GPIOC,GPIO_Pin_12);
       GPIO_ResetBits(GPIOC,GPIO_Pin_11);
       GPIO_ResetBits(GPIOC,GPIO_Pin_10);
     }
    }
    else{
     BarData.Bar1=1;
     if(speed==80){
       speed_drc=1;
     }
     if(speed==-80){
       speed_drc=-1;
     }
     speed=0;
       GPIO_SetBits(GPIOC,GPIO_Pin_11);
       GPIO_ResetBits(GPIOC,GPIO_Pin_10);
       GPIO_ResetBits(GPIOC,GPIO_Pin_12);
    }
  }
}

u8 GetBarValue(){
    return GPIO_ReadInputDataBit(BarPort,BarPinIn);
}
