//========================================================================
// �ļ���:  LCD_Dis.c
// ��  ��: xinqiang Zhang(С��)  (email: xinqiang@Mzdesign.com.cn)
// ��  ��: 2009/02/10
// ��  ��: MzLH04-12864 LCD��ʾģ������V1.0��
//			�������ܽӿں�����
//
// ��  ��: ��
// ��  ��:
//      2009/02/10      First version    Mz Design
//========================================================================
#include"stm32f10x.h"
#include"LCD_Dis.h"
#include<string.h>       //��strlen�������ַ������Ⱥ���
#include<stdio.h>    //�������е�sprintf

#define rst  GPIO_Pin_13
#define cs   GPIO_Pin_15
#define clk  GPIO_Pin_13
#define data GPIO_Pin_15
#define SPIPORT GPIOB  //clk��data�Ķ˿�
#define gport GPIOE   //rst �� cs�Ķ˿�
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
  //GPIOB and spi clock enable : rst��pd.8), cs(pd.9), clk(pb.13) and data(pb.15)
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1PORT|RCC_APB2Periph_SPI1, ENABLE);
  //spiʱ����APB1
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
// ����: void LCD_Init(void)
// ����: LCD��ʼ��������Ҫ��������ɶ˿ڳ�ʼ���Լ�LCDģ��ĸ�λ
// ����: �� 
// ����: ��
// ��ע:
// �汾:
//      2009/02/10      First version    Mz Design
//========================================================================
void TimeDelay(unsigned int Timers)//0.1ms��ʱ
{
	unsigned int i;
	while(Timers)
	{
		Timers--;
		for(i=0;i<36000;i++) ;
	}
}
/*
void us_Delay(unsigned int Timers)//0.001ms��ʱ
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
	//SS��SCKԤ������Ϊ�ߵ�ƽ
GPIO_SetBits(gport, cs);   
//GPIO_SetBits(GPIOA, clk);
	
	//��λLCDģ��
 GPIO_ResetBits(gport, rst);
	TimeDelay(10);			
	//���ֵ͵�ƽ���2ms����
GPIO_SetBits(gport, rst);
	TimeDelay(100);				    	//��ʱ���10ms����
}
//========================================================================
// ����: void SPI_SSSet(unsigned char Status)
// ����: ��SS��״̬
// ����: Status   =1���øߵ�ƽ��=0���õ͵�ƽ  
// ����: ��
// �汾:
//      2009/02/10      First version    Mz Design
//========================================================================
void SPI_SSSet(unsigned char Status)
{
	if(Status)	//�ж���Ҫ��SSΪ�ͻ��Ǹߵ�ƽ��//SS�øߵ�ƽ
    GPIO_SetBits(gport, cs);
  else   //SS�õ͵�ƽ
    GPIO_ResetBits(gport, cs);
}
//========================================================================
// ����: void SPI_Send(unsigned char Data)
// ����: ͨ������SPI������һ��byte��������ģ��
// ����: Data Ҫ���͵����� 
// ����: ��
// �汾:
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
// ����: void FontSet(unsigned char Font_NUM,unsigned char Color)
// ����: ASCII�ַ���������
// ����: Font_NUM ����ѡ��,�������������ֿ�Ϊ׼
//		 Color  �ı���ɫ,��������ASCII�ֿ�  
// ����: ��
// ��ע: 
// �汾:
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
	//����ASCII�ַ�������
	SPI_SSSet(0);					//SS�õ͵�ƽ			
	SPI_Send(0x81);					//����ָ��0x81
	SPI_Send(ucTemp);				//ѡ��8X16��ASCII����,�ַ�ɫΪ��ɫ
	SPI_SSSet(1);					//��ɲ�����SS�ߵ�ƽ	
}
//========================================================================
// ����: void FontMode(unsigned char Cover,unsigned char Color)
// ����: �����ַ���ʾ����ģʽ
// ����: Cover  �ַ�����ģʽ���ã�0��1
//		 Color ����ģʽΪ1ʱ�ַ���ʾʱ�ı�������ɫ 
// ����: ��
// ��ע: 
// �汾:
//      2008/11/27      First version
//========================================================================
void FontMode(unsigned char Cover,unsigned char Color)
{
	unsigned char ucTemp=0;
	ucTemp = (Cover<<4)|Color;
	//����ASCII�ַ�������
	SPI_SSSet(0);					//SS�õ͵�ƽ			
	SPI_Send(0x89);					//����ָ��0x81
	SPI_Send(ucTemp);				//ѡ��8X16��ASCII����,�ַ�ɫΪ��ɫ
	SPI_SSSet(1);					//��ɲ�����SS�ߵ�ƽ	
}
//========================================================================
// ����: void FontSet_cn(unsigned char Font_NUM,unsigned char Color)
// ����: ���ֿ��ַ���������
// ����: Font_NUM ����ѡ��,�������������ֿ�Ϊ׼
//		 Color  �ı���ɫ,�������ں��ֿ�  
// ����: ��
// ��ע: 
// �汾:
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
	//����ASCII�ַ�������
	SPI_SSSet(0);					//SS�õ͵�ƽ			
	SPI_Send(0x82);					//����ָ��0x81
	SPI_Send(ucTemp);				//ѡ��8X16��ASCII����,�ַ�ɫΪ��ɫ
	SPI_SSSet(1);					//��ɲ�����SS�ߵ�ƽ	
}
//========================================================================
// ����: void PutChar(unsigned char x,unsigned char y,unsigned char a) 
// ����: д��һ����׼ASCII�ַ�
// ����: x  X������     y  Y������
//		 a  Ҫ��ʾ���ַ����ֿ��е�ƫ����  
// ����: ��
// ��ע: ASCII�ַ���ֱ������ASCII�뼴��
// �汾:
//      2007/07/19      First version
//========================================================================
void PutChar(unsigned char x,unsigned char y,unsigned char a) 
{
	//��ʾASCII�ַ�
	SPI_SSSet(0);					//SS�õ͵�ƽ	
	SPI_Send(7);					//����ָ��0x07
	SPI_Send(x);					//Ҫ��ʾ�ַ������Ͻǵ�X��λ��
	SPI_Send(y);					//Ҫ��ʾ�ַ������Ͻǵ�Y��λ��
	SPI_Send(a);					//Ҫ��ʾ�ַ�ASCII�ַ���ASCII��ֵ
	SPI_SSSet(1);					//��ɲ�����SS�ߵ�ƽ
}
//========================================================================
// ����: void PutString(int x,int y,char *p)
// ����: ��x��yΪ��ʼ���괦д��һ����׼ASCII�ַ�
// ����: x  X������     y  Y������
//		 p  Ҫ��ʾ���ַ���  
// ����: ��
// ��ע: ���������Դ���ASCII�ַ�����ʾ
// �汾:
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
// ����: void PutChar_cn(unsigned char x,unsigned char y,unsigned short * GB) 
// ����: д��һ���������ֿ⺺��
// ����: x  X������     y  Y������
//		 a  GB��  
// ����: ��
// ��ע: 
// �汾:
//      2007/07/19      First version
//		2007/07/24		V1.2 for MCS51 Keil C
//========================================================================
void PutChar_cn(unsigned char x,unsigned char y,unsigned char * GB) 
{
	//��ʾASCII�ַ�
	SPI_SSSet(0);					//SS�õ͵�ƽ	
	SPI_Send(8);					//����ָ��0x08
	SPI_Send(x);					//Ҫ��ʾ�ַ������Ͻǵ�X��λ��
	SPI_Send(y);					//Ҫ��ʾ�ַ������Ͻǵ�Y��λ��
	
	SPI_Send(*(GB++));	//���Ͷ����ֿ��к���GB��ĸ߰�λֵ
	SPI_Send(*GB);		//���Ͷ����ֿ��к���GB��ĵͰ�λֵ
	SPI_SSSet(1);					//��ɲ�����SS�ߵ�ƽ
}
//========================================================================
// ����: void PutString_cn(unsigned char x,unsigned char y,unsigned short *p)
// ����: ��x��yΪ��ʼ���괦д��һ�������ַ�
// ����: x  X������     y  Y������
//		 p  Ҫ��ʾ���ַ���  
// ����: ��
// ��ע: ͬPutChar_cn�еĽ���
// �汾:
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
// ����: void SetPaintMode(unsigned char Mode,unsigned char Color)
// ����: ��ͼģʽ����
// ����: Mode ��ͼģʽ    Color  ���ص����ɫ,�൱��ǰ��ɫ  
// ����: ��
// ��ע: Mode��Ч
// �汾:
//      2007/07/19      First version
//========================================================================
void SetPaintMode(unsigned char Mode,unsigned char Color)
{
	unsigned char ucTemp=0;
	ucTemp = (Mode<<4)|Color;
	//���û�ͼģʽ
	SPI_SSSet(0);					//SS�õ͵�ƽ			
	SPI_Send(0x83);					//����ָ��0x83
	SPI_Send(ucTemp);				//ѡ��8X16��ASCII����,�ַ�ɫΪ��ɫ
	SPI_SSSet(1);					//��ɲ�����SS�ߵ�ƽ
}
//========================================================================
// ����: void PutPixel(unsigned char x,unsigned char y)
// ����: ��x��y���ϻ���һ��ǰ��ɫ�ĵ�
// ����: x  X������     y  Y������
// ����: ��
// ��ע: ʹ��ǰ��ɫ
// �汾:
//      2007/07/19      First version
//========================================================================
void PutPixel(unsigned char x,unsigned char y)
{
	//������	
	SPI_SSSet(0);					//SS�õ͵�ƽ
	SPI_Send(1);					//��ָ��0x01
	SPI_Send(x);					//�͵�һ������,�����õ��X��λ��
	SPI_Send(y);					//���Y��λ��
	SPI_SSSet(1);					//��ɲ�����SS�ߵ�ƽ
}
//========================================================================
// ����: void Line(unsigned char s_x,unsigned char  s_y,
//					unsigned char  e_x,unsigned char  e_y)
// ����: ��s_x��s_yΪ��ʼ���꣬e_x��e_yΪ�����������һ��ֱ��
// ����: x  X������     y  Y������
// ����: ��
// ��ע: ʹ��ǰ��ɫ
// �汾:
//      2007/07/19      First version
//========================================================================
void Line(unsigned char s_x,unsigned char  s_y,unsigned char  e_x,unsigned char  e_y)
{  
	//����ֱ��
	SPI_SSSet(0);					//SS�õ͵�ƽ
	SPI_Send(2);					//��ָ��0x02
	SPI_Send(s_x);					//���X������
	SPI_Send(s_y);					//���Y������
	SPI_Send(e_x);					//�յ�X������
	SPI_Send(e_y);					//�յ�Y������
	SPI_SSSet(1);					//��ɲ�����SS�ߵ�ƽ
}
//========================================================================
// ����: void Circle(unsigned char x,unsigned char y,
//					unsigned char r,unsigned char mode)
// ����: ��x,yΪԲ��RΪ�뾶��һ��Բ(mode = 0) or Բ��(mode = 1)
// ����: 
// ����: ��
// ��ע: ��Բ����ִ�н��������MCU�п��Ź����������幷�Ĳ���
// �汾:
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
// ����: void Rectangle(unsigned char left, unsigned char top, unsigned char right,
//				 unsigned char bottom, unsigned char mode)
// ����: ��x,yΪԲ��RΪ�뾶��һ��Բ(mode = 0) or Բ��(mode = 1)
// ����: left - ���ε����ϽǺ����꣬��Χ0��126
//		 top - ���ε����Ͻ������꣬��Χ0��62
//		 right - ���ε����½Ǻ����꣬��Χ1��127
//		 bottom - ���ε����½������꣬��Χ1��63
//		 Mode - ����ģʽ��������������ֵ֮һ��
//				0:	���ο򣨿��ľ��Σ�
//				1:	�����棨ʵ�ľ��Σ�
// ����: ��
// ��ע: ��Բ����ִ�н��������MCU�п��Ź����������幷�Ĳ���
// �汾:
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
// ����: void ClrScreen(void)
// ����: ����������ִ��ȫ��Ļ���
// ����: 
// ����: ��
// ��ע: 
// �汾:
//      2007/07/19      First version
//========================================================================
void ClrScreen(void)
{
	//��������
	SPI_SSSet(0);					//SS�õ͵�ƽ
	SPI_Send(0x80);					//��ָ��0x80
         TimeDelay(5);
	SPI_SSSet(1);					//��ɲ�����SS�ߵ�ƽ
}
//========================================================================
// ����: void PutBitmap(unsigned char x,unsigned char y,unsigned char width,
//						unsigned char high,unsigned char *p)
// ����: ��x��yΪ��ʼ������ʾһ����ɫλͼ
// ����: x  X������     y  Y������
//		 width λͼ�Ŀ��  
//		 high  λͼ�ĸ߶�
//		 p  Ҫ��ʾ��λͼ����ģ�׵�ַ  
// ����: ��
// ��ע: ��
// �汾:
//      2008/11/27      First version
//========================================================================
void PutBitmap(unsigned char x,unsigned char y,unsigned char width,unsigned char high,unsigned char const *p)
{
	unsigned short Dat_Num;
	//unsigned short i;
	unsigned char ucTemp=0;
	SPI_SSSet(0);					//SS�õ͵�ƽ
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
		if(ucTemp>250)				//����Ļ����MCUʱ,���������Ҫ�ʵ��ĵ���
		{
			//TimeDelay(28);			//�Լ��˴�,�Ա�֤ǰ�洫�͵�LCDģ���е������ѱ�������
			ucTemp = 0;
		}
		p++;
	}
	SPI_SSSet(1);					//��ɲ�����SS�ߵ�ƽ
}

//========================================================================
// ����: void ShowChar(unsigned char x,unsigned char y,unsigned char Num,unsigned char type)
// ����: ��x��y��λ��Ϊ��ʾ��������Ͻǣ���ʾһ��unsigned char����
// ����: x  X������     y  Y������    Num  Ҫ��ʾ��8λ��ȵ�����  type  ��ʾ����(0,1,2)
// ����: ��
// ��ע: 
// �汾:
//      2008/11/27      First version
//========================================================================
void ShowChar(unsigned char x,unsigned char y,unsigned char a,unsigned char type) 
{
	//��ʾASCII�ַ�
	SPI_SSSet(0);					//SS�õ͵�ƽ	
	SPI_Send(11);					//����ָ��0x0B
	SPI_Send(x);					//Ҫ��ʾ�ַ������Ͻǵ�X��λ��
	SPI_Send(y);					//Ҫ��ʾ�ַ������Ͻǵ�Y��λ��
	SPI_Send(a);					//Ҫ��ʾ������
	SPI_Send(type);
	SPI_SSSet(1);					//��ɲ�����SS�ߵ�ƽ
}
//========================================================================
// ����: void ShowShort(unsigned char x,unsigned char y,unsigned short Num,unsigned char type)
// ����: ��x��y��λ��Ϊ��ʾ��������Ͻǣ���ʾһ��unsigned short������16λ��ȣ�
// ����: x  X������     y  Y������    Num  Ҫ��ʾ��16λ��ȵ�����  type  ��ʾ����(0,1,2)
// ����: ��
// ��ע: 
// �汾:
//      2008/11/27      First version
//========================================================================
void ShowShort(unsigned char x,unsigned char y,unsigned short a,unsigned char type) 
{
	//��ʾASCII�ַ�
	SPI_SSSet(0);					//SS�õ͵�ƽ	
	SPI_Send(12);					//����ָ��0x0C
	SPI_Send(x);					//Ҫ��ʾ�ַ������Ͻǵ�X��λ��
	SPI_Send(y);					//Ҫ��ʾ�ַ������Ͻǵ�Y��λ��
	SPI_Send((unsigned char)(a>>8));
	SPI_Send((unsigned char)a);
	SPI_Send(type);					//Ҫ��ʾ�ַ�ASCII�ַ���ASCII��ֵ
	SPI_SSSet(1);					//��ɲ�����SS�ߵ�ƽ
}
//========================================================================
// ����: void SetBackLight(unsigned char Deg)
// ����: ���ñ������ȵȼ�
// ����: Grade  0~127�ĵȼ�
// ����: ��
// ��ע: 
// �汾:
//      2008/11/27      First version
//========================================================================
void SetBackLight(unsigned char Deg) 
{
	//��ʾASCII�ַ�
	SPI_SSSet(0);					//SS�õ͵�ƽ	
	SPI_Send(0x8a);					//����ָ��0x07
	 //TimeDelay(2);
        SPI_Send(Deg);					//Ҫ��ʾ�ַ������Ͻǵ�X��λ��
	 //TimeDelay(2); 
        SPI_SSSet(1);					//��ɲ�����SS�ߵ�ƽ
}
//========================================================================
// ����: void PutString_10(int x,int y,char *p)
// ����: ��x��yΪ��ʼ���괦д��10����׼ASCII�ַ�
// ����: x  X������     y  Y������
//		 p  Ҫ��ʾ���ַ���  
// ����: ��
// ��ע: ���������Դ���ASCII�ַ�����ʾ
// �汾:
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
// ����: void ShowFloat(unsigned char x,unsigned char y,float num)
// ����: ��x��yΪ��ʼ���괦д��һ��9λ���ĸ�����
// ����: x  X������     y  Y������
//		numҪ��ʾ�ĸ�����  
// ����: ��
// ��ע: 
// �汾:
//      2009/05/23     zhoujun
//========================================================================
void ShowFloat(unsigned char x,unsigned char y, float num)
{
	char chFloat[20];
	sprintf(chFloat,"%f",num);	 //��������numת�����ַ�����chFloat
	PutString_8(x,y,(unsigned char *)chFloat);     //����ʾ7λ
}


void Showu16(unsigned char x,unsigned char y, u16 num)
{
	char chFloat[20];
	sprintf(chFloat,"%d",num);	 //��������numת�����ַ�����chFloat
	PutString_8(x,y,(unsigned char *)chFloat);     //����ʾ7λ
}

/*******************************************************************************
* Function Name  : Show_Variable_2Num
* Description    : ��ָ����λ����ʾһ��0����99����,
* parameter 	 : FontΪ����ѡ��Font=6Ϊ6*10���壬Font=8Ϊ8*16
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
* Description    : ��ָ����λ����ʾһ��0����9999����,
* parameter 	 : FontΪ����ѡ��Font=6Ϊ6*10���壬Font=8Ϊ8*16
* Input          : X,Y,Show_D
* Output         : None
* Return         : None
*******************************************************************************/
void Show_Variable_4Num(unsigned char x,unsigned char y,unsigned int Show_D)
{							    
	unsigned char ucTemp;
	unsigned int ucTemp1;		//�м���� 
	unsigned char Font=6;		//�����СΪ6*10����Ҫѡ�ô�ľ��޸�Font����
    if(Show_D>=10000) return;
    ucTemp = Show_D/1000;  					
	if(ucTemp>0)						//Ϊ4λ������9999
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
	else if(Show_D/100>0)				//Ϊ3λ������999	    
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
		else if(Show_D/10>0)					   //��99
			{
			x = x+2*Font;
			ucTemp=	Show_D/10;				 
		 	PutChar(x,y,'0'+ucTemp);
			x = x+Font;
    		ucTemp1 = Show_D%10;			//ucTemp1=9
    		PutChar(x,y,'0'+ucTemp1);
			}
			else						  //Ϊ1λ��
				{
					x = x+3*Font;
					PutChar(x,y,'0'+Show_D);
				}
					
}

/*******************************************************************************
* Function Name  : Show_LongInt_Mzlh04
* Description    : ��ָ����λ����ʾһ������������,
* parameter 	 : Font_NUMΪ����ѡ��Font_NUM=6Ϊ6*10���壬Font_NUM=8Ϊ8*16
* Input          : X,Y,Show_D
* Output         : None
* Return         : None
*******************************************************************************/
void Show_LongInt_Mzlh04(unsigned char x,unsigned char y, unsigned long Show_D)
{
	int s16Temp1, s16Temp2, s16Temp3;		//�������α�����10λ�ֳ�3�ݣ�4λ��3λ��3λ
	long s32med1;						   	//�м����
	unsigned char Font_NUM;	
	Font_NUM = 6;	
	if(Show_D > 4000000000u)
	{
		Show_D = 0;
	}				
	s16Temp1 = Show_D / 1000000000;					//ȡ��һλ�����λ
	s32med1 = (Show_D % 1000000000);
	s16Temp1 = s16Temp1*10 + s32med1 / 100000000;	//ȡǰ��λ
        
	s32med1 = s32med1 % 100000000;	   		//�ڶ�����ȡ��λ
	s16Temp2 = s32med1 / 10000000;  			 
	s32med1 = s32med1 % 10000000;
	s16Temp2 = s16Temp2*10 + s32med1 / 1000000;	  			 
	s32med1 = s32med1 % 1000000;			
	s16Temp2 =s16Temp2*10 + s32med1 / 100000;
	s32med1 = s32med1 % 100000;			 
	s16Temp2 = s16Temp2*10+s32med1 / 10000;

	s32med1 = s32med1 % 10000;			   //��������ȡ��λ
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
	{					        	//��ʾ����λ
		if(s16Temp3<10)
		{	
			Show_Variable_4Num(x-4*Font_NUM,y,s16Temp3);
			PutString(x-4*Font_NUM,y,"000");						 //���ֻ��һλǰ�油0������3λ
			
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
				
		if(s16Temp2<10)							//��ʾ�м���λ
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
							
		Show_Variable_2Num(x-10*Font_NUM,y,s16Temp1);		   //��ʾ����λ		
	}

	else if(s16Temp2!=0)											//�������λΪ0���м���λ��Ϊ0
	{
		if(s16Temp3<10)
		{	
			Show_Variable_4Num(x-4*Font_NUM,y,s16Temp3);
			PutString(x-4*Font_NUM,y,"000");						 //���ֻ��һλǰ�油0������3λ			
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
	else 										 //�����6λ������0
	{
		Show_Variable_4Num(x-4*Font_NUM,y,s16Temp3);
	}
}

/*******************************************************************************
* Function Name  : *GetHzk16
* Description    : ������Ƿ���16*16�ĺ��ֿ��У��ǵĻ���ȡ��������ģ�����飬û���򷵻�0
* parameter 	 : 
* Input          : c1,c2Ϊ������ĸߵ͵�ַ
* Output         : FNT_GB16,һ���ṹ������
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
* Description    : ������Ƿ���12*12�ĺ��ֿ��У��ǵĻ���ȡ��������ģ�����飬û���򷵻�0
* parameter 	 : 
* Input          : c1,c2Ϊ������ĸߵ͵�ַ
* Output         : FNT_GB12,һ���ṹ������
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
* Description    : ��ָ����λ����ʾ����ģ��ȡ�������ַ������м仹������ASCII
* parameter 	 : �ڵ���ǰ����ASCII����Ҫ�趨�������С��6*10��̫С��һ�����8*16��
* Input          : typeѡ�������С��type=0��ʾ16*16��type=1��ʾ12*12
* Output         : None
* Return         : None
*******************************************************************************/
void PutString_cn_Matrix(unsigned char x,unsigned char y,unsigned char *s,unsigned char type)
{
	unsigned char c1,c2;
	unsigned char Row_Num;

	if(type)				//С����
	{
		Row_Num=12;
		X_Witch_cn=12;
		Y_Witch_cn=12;
		X_Witch=6;
		Y_Witch=10;
	}
	else			   //������
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
		else if( c1 > 0x80)		   //��ʾ�Ǻ��֣���Ϊ������ĸ߰�λ���Ǵ���0x80��
			{
				if((x + X_Witch_cn) > Dis_X_MAX)
				{
					x = Dis_Zero;
					if((Dis_Y_MAX - y) < Y_Witch) break;
					else y += Y_Witch_cn;
				}
				c2=*s++;
				if(type)			  								//12*12����
				PutChar_cn_Matrix(x,y,GetHzk12(c1,c2)->Msk,1);
				else												//16*16����
				PutChar_cn_Matrix(x,y,GetHzk16(c1,c2)->Msk,0);
				x += Row_Num;	
			}
			else		//��ASCII����ֱ�ӵ���PutChar���������ַ���ʱҪ��FontSet�������ַ�����
			{
				if((x + X_Witch) > Dis_X_MAX)
				{
					x = Dis_Zero;
					if((Dis_Y_MAX - y) < Y_Witch) break;
					else y += Y_Witch;
				}
			//	FontSet(type,1);      //type=1��ʾ6*10���ַ���type=0��ʾ8*16���ַ�
				PutChar(x,y-4,c1);
				x +=Row_Num/2;
			}
	}
}
/*******************************************************************************
* Function Name  : PutChar_cn_Matrix
* Description    : ��ָ����λ����ʾһ������ģ��ȡ�����ĺ���,
* parameter 	 : typeѡ�������С��type=0��ʾ16*16��type=1��ʾ12*12
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


