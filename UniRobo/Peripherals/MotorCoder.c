#include "stm32f10x.h"
#include "MotorCoder.h"




#define Timer TIM3
#define TimerCLK RCC_APB1Periph_TIM3


#define MotorCoderPort	GPIOC
#define RCC_APB2Periph_MotorCoderPort (RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO)
#define MotorCoderPin	(GPIO_Pin_6 | GPIO_Pin_7 )//| GPIO_Pin_8 | GPIO_Pin_9) 


u16 Counter_Low=0;
s32 Counter_High=0;
s32 Counter_Temp=0;
s32 dCounter_Temp=0;

s32 Counter=0;
s32 dCounter=0;

struct CoderOut MotorCoderData;






void CoderInit(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
   TIM_ICInitTypeDef TIM_ICInitStructure;

   /* Encoder unit connected to TIM3, 4X mode */    
   GPIO_InitTypeDef GPIO_InitStructure;
   NVIC_InitTypeDef NVIC_InitStructure;

   /* TIM3 clock source enable */
   RCC_APB1PeriphClockCmd(TimerCLK, ENABLE);
   /* Enable GPIOA, clock */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_MotorCoderPort, ENABLE);

   GPIO_StructInit(&GPIO_InitStructure);
   
   //PC6 A相 PC7 B相
   
   GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
   GPIO_InitStructure.GPIO_Pin = MotorCoderPin;                      
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(MotorCoderPort, &GPIO_InitStructure);
    
    
     /* Enable the TIM3 Update Interrupt */
   NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
   
   /* Timer configuration in Encoder mode */
   TIM_DeInit(Timer);
   //TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

   TIM_TimeBaseStructure.TIM_Prescaler = 0; // 1us一个计数
   TIM_TimeBaseStructure.TIM_Period = 0xffff; 
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
   TIM_TimeBaseInit(Timer, &TIM_TimeBaseStructure);

   TIM_EncoderInterfaceConfig(Timer, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
   TIM_ICStructInit(&TIM_ICInitStructure);
   TIM_ICInitStructure.TIM_ICFilter = 6; //6_670nsec
   TIM_ICInit(Timer, &TIM_ICInitStructure);

   // Clear all pending interrupts
   TIM_ClearFlag(Timer, TIM_FLAG_Update);
   TIM_ITConfig(Timer, TIM_IT_Update, ENABLE);
   
   //Reset counter
   TIM3->CNT = 0;

   TIM_Cmd(Timer, ENABLE); 
   
  
   /*
         RCC_APB1PeriphClockCmd(TimerCLK,ENABLE);
		//timer configuration
	TIM_BaseInitStructure.TIM_Period = 0xFFFF; //计数值最大为65535
	TIM_BaseInitStructure.TIM_Prescaler = 1000;//0:1.1K~72M 10:110Hz~7.2M 100:11Hz~720K 1000:1.1Hz~72K
	TIM_BaseInitStructure.TIM_ClockDivision = 0;
	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_BaseInitStructure.TIM_RepetitionCounter = 0; //周期计数器值,RCR向下计数器每次计数至0，会产生一个更新事件且计数器重新由RCR值（N）开始计数。
	TIM_TimeBaseInit(Timer,&TIM_BaseInitStructure);
        
        
        TIM_ICInitStructure.TIM_ICMode = TIM_ICMode_ICAP;               //配置为输入捕获
        TIM_ICInitStructure.TIM_Channel=TIM_Channel_1;                  //通道选择 
        TIM_ICInitStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;       //输入上升沿捕获  
        TIM_ICInitStructure.TIM_ICSelection=TIM_ICSelection_DirectTI;   //管脚与寄存器对应关系 
        TIM_ICInitStructure.TIM_ICPrescaler=TIM_ICPSC_DIV1;             //每次检测到捕获输入就触发一次捕获
        TIM_ICInitStructure.TIM_ICFilter=0x1;                           //滤波设置，经历几个周期跳变认定波形稳定0x0～0xF 
        TIM_ICInit(Timer,&TIM_ICInitStructure);                         //初始化 
        
        
        TIM_ICInitStructure.TIM_ICMode = TIM_ICMode_ICAP;                 //配置为输入捕获模式          
        TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;                     //选择通道2
        TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;       //输入上升沿捕获  
        TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;   // 通道方向选择   
        TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;               //每次检测到捕获输入就触发一次捕获
        TIM_ICInitStructure.TIM_ICFilter = 0x1;                            //
        TIM_ICInit(Timer, &TIM_ICInitStructure);

        
        TIM_SelectInputTrigger(Timer,TIM_TS_TI1FP1);     //TIM_TS_TI1FP1 TIM_TS_TI2FP2选择时钟触发源 
        TIM_SelectSlaveMode(Timer, TIM_SlaveMode_Reset); //复位模式-选中的触发输入（TRGI）的上升沿初始化计数器，并且产生一个更新线号
        TIM_SelectInputTrigger(Timer, TIM_TS_TI2FP2);     //参考TIM结构图选择滤波后的TI1输入作为触发源，触发下面程序的复位 
　　    TIM_SelectSlaveMode(Timer, TIM_SlaveMode_Reset);   //复位模式-选中的触发输入（TRGI）的上升沿初始化计数器，并且产生一个更新线号

        TIM_SelectMasterSlaveMode(Timer, TIM_MasterSlaveMode_Enable); //启动定时器的被动触发 
        TIM_ITConfig(Timer, TIM_IT_CC1, ENABLE);         //打开中断
        TIM_ITConfig(Timer, TIM_IT_CC2, ENABLE);
	//Reset counter
        TIM3->CNT = 0;
        TIM_Cmd(Timer,ENABLE);
   */
 
}




void TIM3_IRQHandler(void){
  if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET){
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    if(TIM3->CR1 & 0x0010)
      Counter_High-=1<<16;
    else
      Counter_High+=1<<16;
  }
}


void Counter_Getter(void){
  //Counter_Low=TIM_GetCapture2(Timer);
  Counter_Low=TIM_GetCounter(Timer);
  Counter=Counter_High+Counter_Low;
  dCounter=Counter-Counter_Temp;
 
  MotorCoderData.Counter=Counter;
  MotorCoderData.dCounter=dCounter;
  MotorCoderData.ddCounter=dCounter-dCounter_Temp;
  
  Counter_Temp=Counter;
  dCounter_Temp=dCounter;
}
 


















/*

void Encoder_Configration(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
   TIM_ICInitTypeDef TIM_ICInitStructure;

   //PC6 A相 PC7 B相
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOC,&GPIO_InitStructure);

   NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQChannel;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIMx_PRE_EMPTION_PRIORITY;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIMx_SUB_PRIORITY;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);

   
   TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // No prescaling 
   TIM_TimeBaseStructure.TIM_Period = 10000; 
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
   TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

   TIM_EncoderInterfaceConfig(TIM8, TIM_EncoderMode_TI12, 
                             TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
   TIM_ICStructInit(&TIM_ICInitStructure);
   TIM_ICInitStructure.TIM_ICFilter = 6;//ICx_FILTER;
   TIM_ICInit(TIM8, &TIM_ICInitStructure);

   // Clear all pending interrupts
   TIM_ClearFlag(TIM8, TIM_FLAG_Update);
   TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);
   //Reset counter
   TIM2->CNT = 0;

   TIM_Cmd(TIM8, ENABLE);

}

//n_Counter = TIM_GetCounter(TIM8);
//Diled_Disp_Num((float)n_Counter);


void TIM3_ETR_GetDropCounts_Configuration(void)
{ 
   GPIO_InitTypeDef GPIO_InitStructure;
   TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
   //test PA0 TIM8_ETR
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOD, &GPIO_InitStructure);
   
   TIM_TimeBaseStructure.TIM_Prescaler = 0x00;
   TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
   TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
   TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); // Time base configuration

   TIM_ETRClockMode2Config(TIM3, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0);

   TIM_SetCounter(TIM3, 0);
   
   TIM_Cmd(TIM3, ENABLE);
}

void TIM2_IRQHandler(void)

{

        if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) 

        {

                TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

                if(TIM2->CR1 & 0x0010)

                        CNT_High16_2 --;

                else

                        CNT_High16_2 ++;

        }

}


*/

/*

void MotorInit(void){
  
        NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;
        TIM_OCInitTypeDef TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_MotorCoderPort,ENABLE);
        GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
	
        GPIO_InitStructure.GPIO_Pin = MotorCoderPin;                      
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MotorCoderPort, &GPIO_InitStructure);
        
	RCC_APB1PeriphClockCmd(TimerCLK,ENABLE);
		//timer configuration
	TIM_BaseInitStructure.TIM_Period = 0xFFFF; //计数值最大为65535
	TIM_BaseInitStructure.TIM_Prescaler = 1000;//0:1.1K~72M 10:110Hz~7.2M 100:11Hz~720K 1000:1.1Hz~72K
	TIM_BaseInitStructure.TIM_ClockDivision = 0;
	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_BaseInitStructure.TIM_RepetitionCounter = 0; //周期计数器值,RCR向下计数器每次计数至0，会产生一个更新事件且计数器重新由RCR值（N）开始计数。
	TIM_TimeBaseInit(Timer,&TIM_BaseInitStructure);
        
        
        TIM_ICInitStructure.TIM_ICMode = TIM_ICMode_ICAP;               //配置为输入捕获
        TIM_ICInitStructure.TIM_Channel=TIM_Channel_1;                  //通道选择 
        TIM_ICInitStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;       //输入上升沿捕获  
        TIM_ICInitStructure.TIM_ICSelection=TIM_ICSelection_DirectTI;   //管脚与寄存器对应关系 
        TIM_ICInitStructure.TIM_ICPrescaler=TIM_ICPSC_DIV1;             //每次检测到捕获输入就触发一次捕获
        TIM_ICInitStructure.TIM_ICFilter=0x1;                           //滤波设置，经历几个周期跳变认定波形稳定0x0～0xF 
        TIM_ICInit(Timer,&TIM_ICInitStructure);                         //初始化 
        
        
        TIM_ICInitStructure.TIM_ICMode = TIM_ICMode_ICAP;                 //配置为输入捕获模式          
        TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;                     //选择通道2
        TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;       //输入上升沿捕获  
        TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;   // 通道方向选择   
        TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;               //每次检测到捕获输入就触发一次捕获
        TIM_ICInitStructure.TIM_ICFilter = 0x1;                            //
        TIM_ICInit(Timer, &TIM_ICInitStructure);

        
        TIM_SelectInputTrigger(Timer,TIM_TS_TI1FP1);     //TIM_TS_TI1FP1 TIM_TS_TI2FP2选择时钟触发源 
        TIM_SelectSlaveMode(Timer, TIM_SlaveMode_Reset); //复位模式-选中的触发输入（TRGI）的上升沿初始化计数器，并且产生一个更新线号
        TIM_SelectInputTrigger(Timer, TIM_TS_TI2FP2);     //参考TIM结构图选择滤波后的TI1输入作为触发源，触发下面程序的复位 
　　    TIM_SelectSlaveMode(Timer, TIM_SlaveMode_Reset);   //复位模式-选中的触发输入（TRGI）的上升沿初始化计数器，并且产生一个更新线号

        TIM_SelectMasterSlaveMode(Timer, TIM_MasterSlaveMode_Enable); //启动定时器的被动触发 
        TIM_ITConfig(Timer, TIM_IT_CC1, ENABLE);         //打开中断
        TIM_ITConfig(Timer, TIM_IT_CC2, ENABLE);
	TIM_Cmd(Timer,ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn ; //更新事件:TIM3 Update Interrupt 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; //响应优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //允许中断
	NVIC_Init(&NVIC_InitStructure); //写入设置
        
}






*/
