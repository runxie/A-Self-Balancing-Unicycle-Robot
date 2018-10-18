#ifndef	__LCD_DIS_h__
#define	__LCD_DIS_h__
//	write your header here
typedef struct typFNT_GB16		// 汉字字模数据结构
{
	unsigned char Index[2];		//国标码的两个字节
	unsigned char Msk[32];
}FNT_GB16;

typedef struct typFNT_GB12		// 汉字字模数据结构
{
	unsigned char Index[2];		////国标码的两个字节
	unsigned char Msk[24];
}FNT_GB12;

//字库
extern FNT_GB12 const HZK12[];			 //12*12的汉字库
extern FNT_GB16 const HZK16[];			 //16*16的汉字库
extern void LCD_Config(void);
extern void TimeDelay(unsigned int Timers);
extern void us_Delay(unsigned int Timers);
extern void LCD_Init(void);
extern void SPI_SSSet(unsigned char Status);
extern void SPI_Send(unsigned char Data);

extern void FontSet(unsigned char Font_NUM,unsigned char Color);
extern void FontSet_cn(unsigned char Font_NUM,unsigned char Color);
extern void PutChar(unsigned char x,unsigned char y,unsigned char a);
extern void PutString(unsigned char x,unsigned char y,unsigned char *p);
extern void PutChar_cn(unsigned char x,unsigned char y,unsigned char * GB);
extern void PutString_cn(unsigned char x,unsigned char y,unsigned char *p);
extern void SetPaintMode(unsigned char Mode,unsigned char Color);
extern void PutPixel(unsigned char x,unsigned char y);
extern void Line(unsigned char s_x,unsigned char  s_y,unsigned char  e_x,unsigned char  e_y);
extern void Circle(unsigned char x,unsigned char y,unsigned char r,unsigned char mode);
extern void Rectangle(unsigned char left, unsigned char top, unsigned char right,
				 unsigned char bottom, unsigned char mode);
extern void ClrScreen(void);
extern void PutBitmap(unsigned char x,unsigned char y,unsigned char width,unsigned char high,unsigned char const *p);
extern void FontMode(unsigned char Cover,unsigned char Color);
extern void ShowChar(unsigned char x,unsigned char y,unsigned char a,unsigned char type) ;
extern void ShowShort(unsigned char x,unsigned char y,unsigned short a,unsigned char type) ;
extern void SetBackLight(unsigned char Deg);

extern void Showu16(unsigned char x,unsigned char y, u16 num);
extern void ShowFloat(unsigned char x,unsigned char y, float num);
extern void PutString_8(unsigned char x,unsigned char y,unsigned char *p);
extern void Show_Variable_2Num(unsigned char x,unsigned char y,unsigned char Show_D);
extern void Show_Variable_4Num(unsigned char x,unsigned char y,unsigned int Show_D);
extern void Show_LongInt_Mzlh04(unsigned char x,unsigned char y, unsigned long Show_D);
extern void PutString_cn_Matrix(unsigned char x,unsigned char y,unsigned char *s,unsigned char type);
extern void PutChar_cn_Matrix(unsigned char x,unsigned char y,unsigned char const * P,unsigned char type);
extern void fill_s(unsigned char x,unsigned char y,unsigned char const * String,unsigned int Number);

extern FNT_GB16 const *GetHzk16(unsigned char c1,unsigned char c2);
extern FNT_GB12 const *GetHzk12(unsigned char c1,unsigned char c2);


extern unsigned char Bitmap01[];
#define Dis_X_MAX		128-1
#define Dis_Y_MAX		64-1

#endif
