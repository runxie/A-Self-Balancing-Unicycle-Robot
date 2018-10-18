#include "stm32f10x.h"
#include "Delay.h"
#include "key.h"


#define KeyPort	GPIOD
#define RCC_APB2Periph_KeyPort RCC_APB2Periph_GPIOD
#define KeyPinOut  (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3) //(GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11)
#define KeyPinIn   (GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7)//(GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15)
                       


//键盘编码表


char const Key_Tab[4][4]=
{
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};


void KeyInit(void){
 
	GPIO_InitTypeDef GPIO_InitStructure;
        
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_KeyPort, ENABLE);
    
	//GPIO_DeInit(KeyPortOut);
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = KeyPinOut;                      
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KeyPort, &GPIO_InitStructure);
	
	//GPIO_DeInit(KeyPortIn);
	GPIO_InitStructure.GPIO_Pin = KeyPinIn;                      
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KeyPort, &GPIO_InitStructure);
}



char GetKeyValue(){
    u8 i,j=0;
    u16 keyscan=0x0001;
    for(i=0;i<3;i++){
      GPIO_Write(KeyPort,keyscan);
      if((GPIO_ReadInputData(KeyPort)&0x00f0)!=0x00f0){
        switch(GPIO_ReadInputData(KeyPort)&0x00f0){
        case 0x0010:j=0; return (Key_Tab[i][j]);
        case 0x0020:j=1; return (Key_Tab[i][j]);
        case 0x0040:j=2; return (Key_Tab[i][j]);
        case 0x0080:j=3; return (Key_Tab[i][j]);
        default:keyscan=keyscan<<1;
        }
      }
    //  else 
    //    keyscan=keyscan<<1;
    }
   return 'x';
}
    
   




/*
char GetKeyvalue(){

  while(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET){ 
    
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_KeyPort, ENABLE);
   
   
   u8 i=5;
   u8 j=5;
   u16 p=0;
   u16 q=0;
   GPIO_InitTypeDef GPIO_InitStructure;
  
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_KeyPort, ENABLE);
  

   //GPIO_DeInit(KeyPortOut);
   GPIO_InitStructure.GPIO_Pin=KeyPinOut;
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_Init(KeyPort,&GPIO_InitStructure);
   
   //GPIO_DeInit(KeyPortIn);
   GPIO_InitStructure.GPIO_Pin=KeyPinIn;
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_Init(KeyPort,&GPIO_InitStructure);
  
  
   GPIO_Write(KeyPort,0x0f00);//扫描列值
   if((GPIO_ReadInputData(KeyPort)&0xff00)==0x0f00)
     return 'x';
   else
   {
     ms_Delay(1);//按键消抖
     if((GPIO_ReadInputData(KeyPort)&0xff00)==0x0f00)
        return 'x';
     else
       p=(GPIO_ReadInputData(KeyPort)&0xff00)^0x0f00;
   }
  
   switch(p)
   {
   case 0x1000:j=0;break;
   case 0x2000:j=1;break;
   case 0x4000:j=2;break;
   case 0x8000:j=3;break;
   default:break;
   }
   //GPIO_DeInit(KeyPortOut<>KeyPinIn);
   GPIO_InitStructure.GPIO_Pin=KeyPinIn;
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_Init(KeyPort,&GPIO_InitStructure);
   
   //GPIO_DeInit(KeyPortIn<>KeyPortOut);
   GPIO_InitStructure.GPIO_Pin=KeyPinOut;
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_Init(KeyPort,&GPIO_InitStructure);
  
   GPIO_Write(KeyPort,0xf000);//扫描行值
   if((GPIO_ReadInputData(KeyPort)&0xff00)==0xf000)
      return 'x';
   else
   {//这里不再延时再扫描，因为已经确定了不是抖动才会进入本步操作   
     q=(GPIO_ReadInputData(KeyPort)&0xff00)^0xf000;
   }
  
   switch(q)
   {
   case 0x0100:i=0;break;
   case 0x0200:i=1;break;
   case 0x0400:i=2;break;
   case 0x0800:i=3;break;
   default:break;
   }
   
   if((i==5)||(j==5))
      return 'x';
   else
     return (Key_Tab[i][j]);
  } 
}
  */