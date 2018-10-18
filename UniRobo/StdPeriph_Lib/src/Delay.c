#include "stm32f10x.h"
#include "Delay.h"
//延时Nms
//注意Nms的范围
//Nms<=0xffffff*8/SYSCLK
//对72M条件下,Nms<=1864 
void ms_Delay(u16 Nms)
{    
 SysTick->LOAD=(u32)Nms*9000; //给重装载寄存器赋值，9000时，将产生1ms的时基 
 SysTick->CTRL|=0x01;               //开始倒数    
 while(!(SysTick->CTRL&(1<<16)));   //等待时间到达 
 SysTick->CTRL=0X00000000;         //关闭计数器
 SysTick->VAL=0X00000000;           //清空计数器     
}   
//延时us           
void us_Delay(u32 Nus)
{ 
 SysTick->LOAD=Nus*9;       //时间加载      
 SysTick->CTRL|=0x01;            //开始倒数    
 while(!(SysTick->CTRL&(1<<16)));//等待时间到达 
 SysTick->CTRL=0X00000000;       //关闭计数器
 SysTick->VAL=0X00000000;        //清空计数器     
}  


void s_Delay(unsigned int Timers)//1s延时
{
	unsigned int i=1000;
	while(i)
	{
		i--;
	         ms_Delay(Timers) ;
	}
}


/*
void ms_Delay(unsigned int Timers)//1ms延时
{
	unsigned int i;
	while(Timers)
	{
		Timers--;
		for(i=0;i<36000;i++) ;
	}
}

void us_Delay(unsigned int Timers)//1ms延时
{
	unsigned int i;
	while(Timers)
	{
		Timers--;
		for(i=0;i<36;i++) ;
	}
}

void s_Delay(unsigned int Timers)//1ms延时
{
	unsigned int i=1000;
	while(i)
	{
		i--;
	         ms_Delay(Timers) ;
	}
}
*/