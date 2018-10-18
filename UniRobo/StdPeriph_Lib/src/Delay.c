#include "stm32f10x.h"
#include "Delay.h"
//��ʱNms
//ע��Nms�ķ�Χ
//Nms<=0xffffff*8/SYSCLK
//��72M������,Nms<=1864 
void ms_Delay(u16 Nms)
{    
 SysTick->LOAD=(u32)Nms*9000; //����װ�ؼĴ�����ֵ��9000ʱ��������1ms��ʱ�� 
 SysTick->CTRL|=0x01;               //��ʼ����    
 while(!(SysTick->CTRL&(1<<16)));   //�ȴ�ʱ�䵽�� 
 SysTick->CTRL=0X00000000;         //�رռ�����
 SysTick->VAL=0X00000000;           //��ռ�����     
}   
//��ʱus           
void us_Delay(u32 Nus)
{ 
 SysTick->LOAD=Nus*9;       //ʱ�����      
 SysTick->CTRL|=0x01;            //��ʼ����    
 while(!(SysTick->CTRL&(1<<16)));//�ȴ�ʱ�䵽�� 
 SysTick->CTRL=0X00000000;       //�رռ�����
 SysTick->VAL=0X00000000;        //��ռ�����     
}  


void s_Delay(unsigned int Timers)//1s��ʱ
{
	unsigned int i=1000;
	while(i)
	{
		i--;
	         ms_Delay(Timers) ;
	}
}


/*
void ms_Delay(unsigned int Timers)//1ms��ʱ
{
	unsigned int i;
	while(Timers)
	{
		Timers--;
		for(i=0;i<36000;i++) ;
	}
}

void us_Delay(unsigned int Timers)//1ms��ʱ
{
	unsigned int i;
	while(Timers)
	{
		Timers--;
		for(i=0;i<36;i++) ;
	}
}

void s_Delay(unsigned int Timers)//1ms��ʱ
{
	unsigned int i=1000;
	while(i)
	{
		i--;
	         ms_Delay(Timers) ;
	}
}
*/