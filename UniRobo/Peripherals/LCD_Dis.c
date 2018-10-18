//========================================================================
// 文件名:  LCD_Dis.c
// 作  者: xinqiang Zhang(小丑)  (email: xinqiang@Mzdesign.com.cn)
// 日  期: 2009/02/10
// 描  述: MzLH04-12864 LCD显示模块驱动V1.0版
//			基本功能接口函数集
//
// 参  考: 无
// 版  本:
//      2009/02/10      First version    Mz Design
//========================================================================
#include"stm32f10x.h"
#include"LCD_Dis.h"
#include<string.h>       //用strlen，计算字符串长度函数
#include<stdio.h>    //用了其中的sprintf

#define rst  GPIO_Pin_13
#define cs   GPIO_Pin_15
#define clk  GPIO_Pin_13
#define data GPIO_Pin_15
#define SPIPORT GPIOB  //clk和data的端口
#define gport GPIOE   //rst 和 cs的端口
#define spi SPI2   //
 

unsigned char X_Witch=6;
unsigned char Y_Witch=10;
unsigned char X_Witch_cn=16;
unsigned char Y_Witch_cn=16;
unsigned char Dis_Zero=0;
GPIO_InitTypeDef GPIO_InitStructure;
SPI_InitTypeDef   SPI_InitStructure;


/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : IO_Config
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Config(void)
{
  //GPIOB and spi clock enable : rst（pd.8), cs(pd.9), clk(pb.13) and data(pb.15)
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1PORT|RCC_APB2Periph_SPI1, ENABLE);
  //spi时钟是APB1
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);


  //Configure spi pins: SCK and MOSI
  GPIO_InitStructure.GPIO_Pin = clk|data;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(SPIPORT, &GPIO_InitStructure);
  //configure rst and cs
  GPIO_InitStructure.GPIO_Pin = rst|cs;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(gport, &GPIO_InitStructure);
  
  

  SPI_I2S_DeInit(spi);
   //spi Config
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(spi, &SPI_InitStructure);
  //Enable spi 
  SPI_Cmd(spi, ENABLE);
}
//========================================================================
// 函数: void LCD_Init(void)
// 描述: LCD初始化程序，主要在里面完成端口初始化以及LCD模块的复位
// 参数: 无 
// 返回: 无
// 备注:
// 版本:
//      2009/02/10      First version    Mz Design
//========================================================================
void TimeDelay(unsigned int Timers)//0.1ms延时
{
	unsigned int i;
	while(Timers)
	{
		Timers--;
		for(i=0;i<36000;i++) ;
	}
}
/*
void us_Delay(unsigned int Timers)//0.001ms延时
{
	unsigned int i;
	while(Timers)
	{
		Timers--;
		for(i=0;i<36;i++) ;
	}
}
*/
void LCD_Init(void)
{
  LCD_Config();
	//SS和SCK预先设置为高电平
GPIO_SetBits(gport, cs);   
//GPIO_SetBits(GPIOA, clk);
	
	//复位LCD模块
 GPIO_ResetBits(gport, rst);
	TimeDelay(10);			
	//保持低电平大概2ms左右
GPIO_SetBits(gport, rst);
	TimeDelay(100);				    	//延时大概10ms左右
}
//========================================================================
// 函数: void SPI_SSSet(unsigned char Status)
// 描述: 置SS线状态
// 参数: Status   =1则置高电平，=0则置低电平  
// 返回: 无
// 版本:
//      2009/02/10      First version    Mz Design
//========================================================================
void SPI_SSSet(unsigned char Status)
{
	if(Status)	//判断是要置SS为低还是高电平？//SS置高电平
    GPIO_SetBits(gport, cs);
  else   //SS置低电平
    GPIO_ResetBits(gport, cs);
}
//========================================================================
// 函数: void SPI_Send(unsigned char Data)
// 描述: 通过串行SPI口输送一个byte的数据置模组
// 参数: Data 要传送的数据 
// 返回: 无
// 版本:
//      2007/07/17      First version
//		2007/07/24		V1.2 for MCS51 Keil C
//========================================================================
void SPI_Send(unsigned char Data)
{ int i;
 //Wait for spi Tx buffer empty 
    while(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE)==RESET);
    SPI_I2S_SendData(spi, Data); 
    for(i=0;i<700;i++);

}
//========================================================================
// 函数: void FontSet(unsigned char Font_NUM,unsigned char Color)
// 描述: ASCII字符字体设置
// 参数: Font_NUM 字体选择,以驱动所带的字库为准
//		 Color  文本颜色,仅作用于ASCII字库  
// 返回: 无
// 备注: 
// 版本:
//      2007/07/19      First version
//========================================================================
void FontSet(unsigned char Font_NUM,unsigned char Color)
{
	unsigned char ucTemp=0;
	if(Font_NUM==0)
	{
		X_Witch = 6;//7;
		Y_Witch = 10;
	}
	else
	{
		X_Witch = 8;
		Y_Witch = 16;
	}
	ucTemp = (Font_NUM<<4)|Color;
	//设置ASCII字符的字型
	SPI_SSSet(0);					//SS置低电平			
	SPI_Send(0x81);					//传送指令0x81
	SPI_Send(ucTemp);				//选择8X16的ASCII字体,字符色为黑色
	SPI_SSSet(1);					//完成操作置SS高电平	
}
//========================================================================
// 函数: void FontMode(unsigned char Cover,unsigned char Color)
// 描述: 设置字符显示覆盖模式
// 参数: Cover  字符覆盖模式设置，0或1
//		 Color 覆盖模式为1时字符显示时的背景覆盖色 
// 返回: 无
// 备注: 
// 版本:
//      2008/11/27      First version
//========================================================================
void FontMode(unsigned char Cover,unsigned char Color)
{
	unsigned char ucTemp=0;
	ucTemp = (Cover<<4)|Color;
	//设置ASCII字符的字型
	SPI_SSSet(0);					//SS置低电平			
	SPI_Send(0x89);					//传送指令0x81
	SPI_Send(ucTemp);				//选择8X16的ASCII字体,字符色为黑色
	SPI_SSSet(1);					//完成操作置SS高电平	
}
//========================================================================
// 函数: void FontSet_cn(unsigned char Font_NUM,unsigned char Color)
// 描述: 汉字库字符字体设置
// 参数: Font_NUM 字体选择,以驱动所带的字库为准
//		 Color  文本颜色,仅作用于汉字库  
// 返回: 无
// 备注: 
// 版本:
//      2007/07/19      First version
//========================================================================
void FontSet_cn(unsigned char Font_NUM,unsigned char Color)
{
	unsigned char ucTemp=0;
	if(Font_NUM==0)
	{
		X_Witch_cn = 12;
		Y_Witch_cn = 12;
	}
	else
	{
		X_Witch_cn = 16;
		Y_Witch_cn = 16;
	}
	ucTemp = (Font_NUM<<4)|Color;
	//设置ASCII字符的字型
	SPI_SSSet(0);					//SS置低电平			
	SPI_Send(0x82);					//传送指令0x81
	SPI_Send(ucTemp);				//选择8X16的ASCII字体,字符色为黑色
	SPI_SSSet(1);					//完成操作置SS高电平	
}
//========================================================================
// 函数: void PutChar(unsigned char x,unsigned char y,unsigned char a) 
// 描述: 写入一个标准ASCII字符
// 参数: x  X轴坐标     y  Y轴坐标
//		 a  要显示的字符在字库中的偏移量  
// 返回: 无
// 备注: ASCII字符可直接输入ASCII码即可
// 版本:
//      2007/07/19      First version
//========================================================================
void PutChar(unsigned char x,unsigned char y,unsigned char a) 
{
	//显示ASCII字符
	SPI_SSSet(0);					//SS置低电平	
	SPI_Send(7);					//传送指令0x07
	SPI_Send(x);					//要显示字符的左上角的X轴位置
	SPI_Send(y);					//要显示字符的左上角的Y轴位置
	SPI_Send(a);					//要显示字符ASCII字符的ASCII码值
	SPI_SSSet(1);					//完成操作置SS高电平
}
//========================================================================
// 函数: void PutString(int x,int y,char *p)
// 描述: 在x、y为起始坐标处写入一串标准ASCII字符
// 参数: x  X轴坐标     y  Y轴坐标
//		 p  要显示的字符串  
// 返回: 无
// 备注: 仅能用于自带的ASCII字符串显示
// 版本:
//      2007/07/19      First version 
//========================================================================
void PutString(unsigned char x,unsigned char y,unsigned char *p)
{
	while(*p!=0)
	{
		PutChar(x,y,*p);
		x += X_Witch;
		if((x + X_Witch) > Dis_X_MAX)
		{
			x = Dis_Zero;
			if((Dis_Y_MAX - y) < Y_Witch) break;
			else y += Y_Witch;
		}
		p++;
	}
}
//========================================================================
// 函数: void PutChar_cn(unsigned char x,unsigned char y,unsigned short * GB) 
// 描述: 写入一个二级汉字库汉字
// 参数: x  X轴坐标     y  Y轴坐标
//		 a  GB码  
// 返回: 无
// 备注: 
// 版本:
//      2007/07/19      First version
//		2007/07/24		V1.2 for MCS51 Keil C
//========================================================================
void PutChar_cn(unsigned char x,unsigned char y,unsigned char * GB) 
{
	//显示ASCII字符
	SPI_SSSet(0);					//SS置低电平	
	SPI_Send(8);					//传送指令0x08
	SPI_Send(x);					//要显示字符的左上角的X轴位置
	SPI_Send(y);					//要显示字符的左上角的Y轴位置
	
	SPI_Send(*(GB++));	//传送二级字库中汉字GB码的高八位值
	SPI_Send(*GB);		//传送二级字库中汉字GB码的低八位值
	SPI_SSSet(1);					//完成操作置SS高电平
}
//========================================================================
// 函数: void PutString_cn(unsigned char x,unsigned char y,unsigned short *p)
// 描述: 在x、y为起始坐标处写入一串汉字字符
// 参数: x  X轴坐标     y  Y轴坐标
//		 p  要显示的字符串  
// 返回: 无
// 备注: 同PutChar_cn中的解释
// 版本:
//      2007/07/19      First version
//		2007/07/24		V1.2 for MCS51 Keil C
//========================================================================
void PutString_cn(unsigned char x,unsigned char y,unsigned char *p)
{
	while(*p!=0)
	{
		if(*p<128)
		{
			PutChar(x,y,*p);
			x += X_Witch+1;
		if((x/* + X_Witch*/) > Dis_X_MAX)
		{
			x = Dis_Zero;
			if((Dis_Y_MAX - y) < Y_Witch) break;
			else y += Y_Witch_cn;
		}
		p+=1;
		}
		else
		{
		PutChar_cn(x,y,p);
		x += X_Witch_cn+1;
		if((x/* + X_Witch_cn*/) > Dis_X_MAX)
		{
			x = Dis_Zero;
			if((Dis_Y_MAX - y) < Y_Witch_cn) break;
			else y += Y_Witch_cn;
		}
		p+=2;
		}
	}
}
//========================================================================
// 函数: void SetPaintMode(unsigned char Mode,unsigned char Color)
// 描述: 绘图模式设置
// 参数: Mode 绘图模式    Color  像素点的颜色,相当于前景色  
// 返回: 无
// 备注: Mode无效
// 版本:
//      2007/07/19      First version
//========================================================================
void SetPaintMode(unsigned char Mode,unsigned char Color)
{
	unsigned char ucTemp=0;
	ucTemp = (Mode<<4)|Color;
	//设置绘图模式
	SPI_SSSet(0);					//SS置低电平			
	SPI_Send(0x83);					//传送指令0x83
	SPI_Send(ucTemp);				//选择8X16的ASCII字体,字符色为黑色
	SPI_SSSet(1);					//完成操作置SS高电平
}
//========================================================================
// 函数: void PutPixel(unsigned char x,unsigned char y)
// 描述: 在x、y点上绘制一个前景色的点
// 参数: x  X轴坐标     y  Y轴坐标
// 返回: 无
// 备注: 使用前景色
// 版本:
//      2007/07/19      First version
//========================================================================
void PutPixel(unsigned char x,unsigned char y)
{
	//绘点操作	
	SPI_SSSet(0);					//SS置低电平
	SPI_Send(1);					//送指令0x01
	SPI_Send(x);					//送第一个数据,即设置点的X轴位置
	SPI_Send(y);					//点的Y轴位置
	SPI_SSSet(1);					//完成操作置SS高电平
}
//========================================================================
// 函数: void Line(unsigned char s_x,unsigned char  s_y,
//					unsigned char  e_x,unsigned char  e_y)
// 描述: 在s_x、s_y为起始坐标，e_x、e_y为结束坐标绘制一条直线
// 参数: x  X轴坐标     y  Y轴坐标
// 返回: 无
// 备注: 使用前景色
// 版本:
//      2007/07/19      First version
//========================================================================
void Line(unsigned char s_x,unsigned char  s_y,unsigned char  e_x,unsigned char  e_y)
{  
	//绘制直线
	SPI_SSSet(0);					//SS置低电平
	SPI_Send(2);					//送指令0x02
	SPI_Send(s_x);					//起点X轴坐标
	SPI_Send(s_y);					//起点Y轴坐标
	SPI_Send(e_x);					//终点X轴坐标
	SPI_Send(e_y);					//终点Y轴坐标
	SPI_SSSet(1);					//完成操作置SS高电平
}
//========================================================================
// 函数: void Circle(unsigned char x,unsigned char y,
//					unsigned char r,unsigned char mode)
// 描述: 以x,y为圆心R为半径画一个圆(mode = 0) or 圆面(mode = 1)
// 参数: 
// 返回: 无
// 备注: 画圆函数执行较慢，如果MCU有看门狗，请作好清狗的操作
// 版本:
//      2007/07/19      First version
//========================================================================
void Circle(unsigned char x,unsigned char y,unsigned char r,unsigned char mode)
{
	SPI_SSSet(0);
	if(mode)
		SPI_Send(6);
	else
		SPI_Send(5);
	SPI_Send(x);
	SPI_Send(y);
	SPI_Send(r);
	SPI_SSSet(1);
}
//========================================================================
// 函数: void Rectangle(unsigned char left, unsigned char top, unsigned char right,
//				 unsigned char bottom, unsigned char mode)
// 描述: 以x,y为圆心R为半径画一个圆(mode = 0) or 圆面(mode = 1)
// 参数: left - 矩形的左上角横坐标，范围0到126
//		 top - 矩形的左上角纵坐标，范围0到62
//		 right - 矩形的右下角横坐标，范围1到127
//		 bottom - 矩形的右下角纵坐标，范围1到63
//		 Mode - 绘制模式，可以是下列数值之一：
//				0:	矩形框（空心矩形）
//				1:	矩形面（实心矩形）
// 返回: 无
// 备注: 画圆函数执行较慢，如果MCU有看门狗，请作好清狗的操作
// 版本:
//      2007/07/19      First version
//========================================================================
void Rectangle(unsigned char left, unsigned char top, unsigned char right,
				 unsigned char bottom, unsigned char mode)
{
	SPI_SSSet(0);
	if(mode)
		SPI_Send(4);
	else
		SPI_Send(3);
	SPI_Send(left);
	SPI_Send(top);
	SPI_Send(right);
	SPI_Send(bottom);
	SPI_SSSet(1);
}
//========================================================================
// 函数: void ClrScreen(void)
// 描述: 清屏函数，执行全屏幕清除
// 参数: 
// 返回: 无
// 备注: 
// 版本:
//      2007/07/19      First version
//========================================================================
void ClrScreen(void)
{
	//清屏操作
	SPI_SSSet(0);					//SS置低电平
	SPI_Send(0x80);					//送指令0x80
         TimeDelay(5);
	SPI_SSSet(1);					//完成操作置SS高电平
}
//========================================================================
// 函数: void PutBitmap(unsigned char x,unsigned char y,unsigned char width,
//						unsigned char high,unsigned char *p)
// 描述: 在x、y为起始坐标显示一个单色位图
// 参数: x  X轴坐标     y  Y轴坐标
//		 width 位图的宽度  
//		 high  位图的高度
//		 p  要显示的位图的字模首地址  
// 返回: 无
// 备注: 无
// 版本:
//      2008/11/27      First version
//========================================================================
void PutBitmap(unsigned char x,unsigned char y,unsigned char width,unsigned char high,unsigned char const *p)
{
	unsigned short Dat_Num;
	//unsigned short i;
	unsigned char ucTemp=0;
	SPI_SSSet(0);					//SS置低电平
	SPI_Send(0x0e);	
	SPI_Send(x);
	SPI_Send(y);
	SPI_Send(width);
	SPI_Send(high);
	
	width = width+0x07;
	Dat_Num = (width>>3)*high;
	while(Dat_Num--)
	{
	  	ucTemp++;
		SPI_Send(*p);
		if(ucTemp>250)				//如果改换别的MCU时,这里可能需要适当的调整
		{
			//TimeDelay(28);			//以及此处,以保证前面传送到LCD模块中的数据已被处理完
			ucTemp = 0;
		}
		p++;
	}
	SPI_SSSet(1);					//完成操作置SS高电平
}

//========================================================================
// 函数: void ShowChar(unsigned char x,unsigned char y,unsigned char Num,unsigned char type)
// 描述: 在x、y的位置为显示区域的左上角，显示一个unsigned char型数
// 参数: x  X轴坐标     y  Y轴坐标    Num  要显示的8位宽度的数据  type  显示特性(0,1,2)
// 返回: 无
// 备注: 
// 版本:
//      2008/11/27      First version
//========================================================================
void ShowChar(unsigned char x,unsigned char y,unsigned char a,unsigned char type) 
{
	//显示ASCII字符
	SPI_SSSet(0);					//SS置低电平	
	SPI_Send(11);					//传送指令0x0B
	SPI_Send(x);					//要显示字符的左上角的X轴位置
	SPI_Send(y);					//要显示字符的左上角的Y轴位置
	SPI_Send(a);					//要显示的数据
	SPI_Send(type);
	SPI_SSSet(1);					//完成操作置SS高电平
}
//========================================================================
// 函数: void ShowShort(unsigned char x,unsigned char y,unsigned short Num,unsigned char type)
// 描述: 在x、y的位置为显示区域的左上角，显示一个unsigned short型数（16位宽度）
// 参数: x  X轴坐标     y  Y轴坐标    Num  要显示的16位宽度的数据  type  显示特性(0,1,2)
// 返回: 无
// 备注: 
// 版本:
//      2008/11/27      First version
//========================================================================
void ShowShort(unsigned char x,unsigned char y,unsigned short a,unsigned char type) 
{
	//显示ASCII字符
	SPI_SSSet(0);					//SS置低电平	
	SPI_Send(12);					//传送指令0x0C
	SPI_Send(x);					//要显示字符的左上角的X轴位置
	SPI_Send(y);					//要显示字符的左上角的Y轴位置
	SPI_Send((unsigned char)(a>>8));
	SPI_Send((unsigned char)a);
	SPI_Send(type);					//要显示字符ASCII字符的ASCII码值
	SPI_SSSet(1);					//完成操作置SS高电平
}
//========================================================================
// 函数: void SetBackLight(unsigned char Deg)
// 描述: 设置背光亮度等级
// 参数: Grade  0~127的等级
// 返回: 无
// 备注: 
// 版本:
//      2008/11/27      First version
//========================================================================
void SetBackLight(unsigned char Deg) 
{
	//显示ASCII字符
	SPI_SSSet(0);					//SS置低电平	
	SPI_Send(0x8a);					//传送指令0x07
	 //TimeDelay(2);
        SPI_Send(Deg);					//要显示字符的左上角的X轴位置
	 //TimeDelay(2); 
        SPI_SSSet(1);					//完成操作置SS高电平
}
//========================================================================
// 函数: void PutString_10(int x,int y,char *p)
// 描述: 在x、y为起始坐标处写入10个标准ASCII字符
// 参数: x  X轴坐标     y  Y轴坐标
//		 p  要显示的字符串  
// 返回: 无
// 备注: 仅能用于自带的ASCII字符串显示
// 版本:
//      2007/07/19      First version
//========================================================================
void PutString_8(unsigned char x,unsigned char y,unsigned char *p)
{
	unsigned char i;
	i=strlen((char *)p);	 
	if(i > 8)
	{
		i=8;
	}
	while(i)
	{
		PutChar(x,y,*p);
		x += X_Witch;
		if((x + X_Witch) > Dis_X_MAX)
		{
			x = Dis_Zero;
			if((Dis_Y_MAX - y) < Y_Witch) break;
			else y += Y_Witch;
		}
		i--;
		p++;
	}
}

//========================================================================
// 函数: void ShowFloat(unsigned char x,unsigned char y,float num)
// 描述: 在x、y为起始坐标处写入一个9位数的浮点数
// 参数: x  X轴坐标     y  Y轴坐标
//		num要显示的浮点数  
// 返回: 无
// 备注: 
// 版本:
//      2009/05/23     zhoujun
//========================================================================
void ShowFloat(unsigned char x,unsigned char y, float num)
{
	char chFloat[20];
	sprintf(chFloat,"%f",num);	 //将浮点型num转化成字符串到chFloat
	PutString_8(x,y,(unsigned char *)chFloat);     //共显示7位
}


void Showu16(unsigned char x,unsigned char y, u16 num)
{
	char chFloat[20];
	sprintf(chFloat,"%d",num);	 //将浮点型num转化成字符串到chFloat
	PutString_8(x,y,(unsigned char *)chFloat);     //共显示7位
}

/*******************************************************************************
* Function Name  : Show_Variable_2Num
* Description    : 在指定的位置显示一个0——99的数,
* parameter 	 : Font为字体选择，Font=6为6*10字体，Font=8为8*16
* Input          : X,Y,Show_D
* Output         : None
* Return         : None
*******************************************************************************/
void Show_Variable_2Num(unsigned char x,unsigned char y,unsigned char Show_D)
{
	unsigned char ucTemp;
	unsigned char Font=6;
    if(Show_D>=100) return;
    ucTemp = Show_D/10;
	if(ucTemp>0)
	{
    	PutChar(x,y,'0'+ucTemp);
	}
    x = x+Font;
    ucTemp = Show_D%10;
    PutChar(x,y,'0'+ucTemp); 
}

/*******************************************************************************
* Function Name  : Show_Variable_4Num
* Description    : 在指定的位置显示一个0——9999的数,
* parameter 	 : Font为字体选择，Font=6为6*10字体，Font=8为8*16
* Input          : X,Y,Show_D
* Output         : None
* Return         : None
*******************************************************************************/
void Show_Variable_4Num(unsigned char x,unsigned char y,unsigned int Show_D)
{							    
	unsigned char ucTemp;
	unsigned int ucTemp1;		//中间变量 
	unsigned char Font=6;		//字体大小为6*10，如要选用大的就修改Font就行
    if(Show_D>=10000) return;
    ucTemp = Show_D/1000;  					
	if(ucTemp>0)						//为4位数，如9999
	{
    	PutChar(x,y,'0'+ucTemp);
   		x = x+Font;
    	ucTemp1 = Show_D%1000; 			// ucTemp1=999
		ucTemp = ucTemp1/100;
    	PutChar(x,y,'0'+ucTemp);
		x = x+Font;
    	ucTemp1 = ucTemp1%100;		  //ucTemp1=99
		ucTemp = ucTemp1/10;
    	PutChar(x,y,'0'+ucTemp);
		x = x+Font;
    	ucTemp1 = ucTemp1%10;		  //ucTemp1=9
    	PutChar(x,y,'0'+ucTemp1);
	}
	else if(Show_D/100>0)				//为3位数，如999	    
		{
			x = x+Font;
			ucTemp=	Show_D/100;				 
		 	PutChar(x,y,'0'+ucTemp);
			x = x+Font;
    		ucTemp1 = Show_D%100;			//ucTemp1=99
			ucTemp = ucTemp1/10;
    		PutChar(x,y,'0'+ucTemp);
			x = x+Font;
    		ucTemp1 = ucTemp1%10;		  //ucTemp1=9
    		PutChar(x,y,'0'+ucTemp1);
		}
		else if(Show_D/10>0)					   //如99
			{
			x = x+2*Font;
			ucTemp=	Show_D/10;				 
		 	PutChar(x,y,'0'+ucTemp);
			x = x+Font;
    		ucTemp1 = Show_D%10;			//ucTemp1=9
    		PutChar(x,y,'0'+ucTemp1);
			}
			else						  //为1位数
				{
					x = x+3*Font;
					PutChar(x,y,'0'+Show_D);
				}
					
}

/*******************************************************************************
* Function Name  : Show_LongInt_Mzlh04
* Description    : 在指定的位置显示一个长整型数据,
* parameter 	 : Font_NUM为字体选择，Font_NUM=6为6*10字体，Font_NUM=8为8*16
* Input          : X,Y,Show_D
* Output         : None
* Return         : None
*******************************************************************************/
void Show_LongInt_Mzlh04(unsigned char x,unsigned char y, unsigned long Show_D)
{
	int s16Temp1, s16Temp2, s16Temp3;		//将长整形变量的10位分成3份，4位，3位，3位
	long s32med1;						   	//中间变量
	unsigned char Font_NUM;	
	Font_NUM = 6;	
	if(Show_D > 4000000000u)
	{
		Show_D = 0;
	}				
	s16Temp1 = Show_D / 1000000000;					//取第一位，最高位
	s32med1 = (Show_D % 1000000000);
	s16Temp1 = s16Temp1*10 + s32med1 / 100000000;	//取前两位
        
	s32med1 = s32med1 % 100000000;	   		//第二个数取四位
	s16Temp2 = s32med1 / 10000000;  			 
	s32med1 = s32med1 % 10000000;
	s16Temp2 = s16Temp2*10 + s32med1 / 1000000;	  			 
	s32med1 = s32med1 % 1000000;			
	s16Temp2 =s16Temp2*10 + s32med1 / 100000;
	s32med1 = s32med1 % 100000;			 
	s16Temp2 = s16Temp2*10+s32med1 / 10000;

	s32med1 = s32med1 % 10000;			   //第三个数取四位
	s16Temp3 = s32med1 / 1000;
	s32med1 = s32med1 % 1000;			 
	s16Temp3 = s16Temp3*10+s32med1 / 100;
	s32med1 = s32med1 % 100;			 
	s16Temp3 = s16Temp3*10+s32med1 / 10;
	s32med1 = s32med1 % 10;			 
	s16Temp3 = s16Temp3*10+s32med1;

	 //FontSet(0,type);
/*	if(s16Temp1!=0)
	{
		ShowShort(x,y,s16Temp1,2);
		if(s16Temp2<10)
		{
			PutString(x+5*Font_NUM,y,"00");
		//	ShowShort(x+7*6,y,s16Temp2,1);
			ShowChar(x+7*Font_NUM,y,s16Temp2,1);					
		}
		else if(s16Temp2>9 && s16Temp2<100)
			{
				PutString(x+5*Font_NUM,y,"0");
				ShowShort(x+6*Font_NUM,y,s16Temp2,1);
			}
		else
			{
				ShowShort(x+5*Font_NUM,y,s16Temp2,1);
			}

		if(s16Temp3<10)
		{
			PutString(x+8*Font_NUM,y,"00");
		//	ShowShort(x+10*6,y,s16Temp3,1);
			ShowChar(x+10*Font_NUM,y,s16Temp3,1);
		}
		else if(s16Temp3>9 && s16Temp3<100)
			{
				PutString(x+8*Font_NUM,y,"0");
				ShowShort(x+9*Font_NUM,y,s16Temp3,1);
			}
			else
			{
				ShowShort(x+8*Font_NUM,y,s16Temp3,1);
			}
		
	}
	else if(s16Temp2!=0)
		{
			ShowShort(x,y,s16Temp2,2);
			if(s16Temp3<10)
			{
				PutString(x+5*Font_NUM,y,"00");
				ShowShort(x+7*Font_NUM,y,s16Temp3,1);
			}
			else if(s16Temp3>9 && s16Temp3<100)
			{
				PutString(x+5*Font_NUM,y,"0");
				ShowShort(x+6*Font_NUM,y,s16Temp3,1);
			}
			else
			{
				ShowShort(x+5*Font_NUM,y,s16Temp3,1);
			}			
		}
		else 
		{
			ShowShort(x,y,s16Temp3,2);
		} */
	if(s16Temp1!=0)
	{					        	//显示低四位
		if(s16Temp3<10)
		{	
			Show_Variable_4Num(x-4*Font_NUM,y,s16Temp3);
			PutString(x-4*Font_NUM,y,"000");						 //如果只有一位前面补0，补足3位
			
		}
		else if(s16Temp3>9 && s16Temp3<100)
			{
				Show_Variable_4Num(x-4*Font_NUM,y,s16Temp3);
				PutString(x-4*Font_NUM,y,"00");
			}
			else if(s16Temp3>99 && s16Temp3<1000)
			{
				Show_Variable_4Num(x-4*Font_NUM,y,s16Temp3);
				PutString(x-4*Font_NUM,y,"0");
			}
				else
				{
					Show_Variable_4Num(x-4*Font_NUM,y,s16Temp3);
				}
				
		if(s16Temp2<10)							//显示中间四位
		{
			Show_Variable_4Num(x-8*Font_NUM,y,s16Temp2);
			PutString(x-8*Font_NUM,y,"000");					
		}
		else if(s16Temp2>9 && s16Temp2<100)
			{				
				Show_Variable_4Num(x-8*Font_NUM,y,s16Temp2);
				PutString(x-8*Font_NUM,y,"00");
			}
			else if(s16Temp2>99 && s16Temp2<1000)
			{
				Show_Variable_4Num(x-8*Font_NUM,y,s16Temp2);
				PutString(x-8*Font_NUM,y,"0");
			}
				else
				{
					Show_Variable_4Num(x-8*Font_NUM,y,s16Temp2);
				}
							
		Show_Variable_2Num(x-10*Font_NUM,y,s16Temp1);		   //显示高两位		
	}

	else if(s16Temp2!=0)											//如果高两位为0且中间四位不为0
	{
		if(s16Temp3<10)
		{	
			Show_Variable_4Num(x-4*Font_NUM,y,s16Temp3);
			PutString(x-4*Font_NUM,y,"000");						 //如果只有一位前面补0，补足3位			
		}
		else if(s16Temp3>9 && s16Temp3<100)
		{
			Show_Variable_4Num(x-4*Font_NUM,y,s16Temp3);
			PutString(x-4*Font_NUM,y,"00");
		}
		else if(s16Temp3>99 && s16Temp3<1000)
		{
			Show_Variable_4Num(x-4*Font_NUM,y,s16Temp3);
			PutString(x-4*Font_NUM,y,"0");
		}
		else
		{
			Show_Variable_4Num(x-4*Font_NUM,y,s16Temp3);
		}
		
		Show_Variable_4Num(x-8*Font_NUM,y,s16Temp2);		
	}
	else 										 //如果高6位都等于0
	{
		Show_Variable_4Num(x-4*Font_NUM,y,s16Temp3);
	}
}

/*******************************************************************************
* Function Name  : *GetHzk16
* Description    : 检测字是否在16*16的汉字库中，是的话就取返回其字模的数组，没有则返回0
* parameter 	 : 
* Input          : c1,c2为国标码的高低地址
* Output         : FNT_GB16,一个结构体类型
* Return         : None
*******************************************************************************/
FNT_GB16 const *GetHzk16(unsigned char c1,unsigned char c2)
{
	int i = 0;
	while(HZK16[i].Index[0] != 0)
	{
		if(c1 == HZK16[i].Index[0] && c2 == HZK16[i].Index[1])
			return &HZK16[i];
		i ++;
	}
	return 0;
}

/*******************************************************************************
* Function Name  : *GetHzk12
* Description    : 检测字是否在12*12的汉字库中，是的话就取返回其字模的数组，没有则返回0
* parameter 	 : 
* Input          : c1,c2为国标码的高低地址
* Output         : FNT_GB12,一个结构体类型
* Return         : None
*******************************************************************************/
FNT_GB12 const *GetHzk12(unsigned char c1,unsigned char c2)
{
	int i = 0;
	while(HZK12[i].Index[0] != 0)
	{
		if(c1 == HZK12[i].Index[0] && c2 == HZK12[i].Index[1])
			return &HZK12[i];
		i ++;
	}
	return 0;
}
/*******************************************************************************
* Function Name  : PutString_cn_Matrix
* Description    : 在指定的位置显示从字模中取出来的字符串，中间还可以有ASCII
* parameter 	 : 在调用前如有ASCII码先要设定起字体大小，6*10的太小，一般设成8*16的
* Input          : type选择字体大小，type=0表示16*16，type=1表示12*12
* Output         : None
* Return         : None
*******************************************************************************/
void PutString_cn_Matrix(unsigned char x,unsigned char y,unsigned char *s,unsigned char type)
{
	unsigned char c1,c2;
	unsigned char Row_Num;

	if(type)				//小字体
	{
		Row_Num=12;
		X_Witch_cn=12;
		Y_Witch_cn=12;
		X_Witch=6;
		Y_Witch=10;
	}
	else			   //大字体
	{
		Row_Num=16;
		X_Witch_cn=16;
		Y_Witch_cn=16;
		X_Witch=8;
		Y_Witch=16;
	}
	while(*s)
	{
		c1 = *s++;
		if(c1 == ' ')
		{
			x += Row_Num/2;
		}
		else if( c1 > 0x80)		   //表示是汉字，因为国标码的高八位都是大于0x80的
			{
				if((x + X_Witch_cn) > Dis_X_MAX)
				{
					x = Dis_Zero;
					if((Dis_Y_MAX - y) < Y_Witch) break;
					else y += Y_Witch_cn;
				}
				c2=*s++;
				if(type)			  								//12*12字体
				PutChar_cn_Matrix(x,y,GetHzk12(c1,c2)->Msk,1);
				else												//16*16字体
				PutChar_cn_Matrix(x,y,GetHzk16(c1,c2)->Msk,0);
				x += Row_Num;	
			}
			else		//是ASCII，则直接调用PutChar，所以有字符串时要先FontSet，设置字符类型
			{
				if((x + X_Witch) > Dis_X_MAX)
				{
					x = Dis_Zero;
					if((Dis_Y_MAX - y) < Y_Witch) break;
					else y += Y_Witch;
				}
			//	FontSet(type,1);      //type=1表示6*10的字符，type=0表示8*16的字符
				PutChar(x,y-4,c1);
				x +=Row_Num/2;
			}
	}
}
/*******************************************************************************
* Function Name  : PutChar_cn_Matrix
* Description    : 在指定的位置显示一个从字模中取出来的汉字,
* parameter 	 : type选择字体大小，type=0表示16*16，type=1表示12*12
* Input          : X,Y,P,type
* Output         : None
* Return         : None
*******************************************************************************/
void PutChar_cn_Matrix(unsigned char x,unsigned char y,unsigned char const * P,unsigned char type)
{
	unsigned char Row_Num;
	if(type)
	{
		Row_Num=12;
	}
	else
	{
		Row_Num=16;
	}
	fill_s(x,y,P,Row_Num);
	fill_s(x,y+(Row_Num/2),P+Row_Num,Row_Num);
}	


