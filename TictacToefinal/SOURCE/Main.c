/*
 * =====================================================================
 * NAME         : Main.c
 *
 * Descriptions : Main routine for S3C2450
 *
 * IDE          : GCC-4.1.0
 *
 * Modification
 *	  
 * =====================================================================
 */

#include <stdlib.h>
#include <time.h> 
#include "2450addr.h"
#include "my_lib.h"
#include "option.h"
#include "key.h"
#include "timer.h"
#include "buzzer.h"
#include "AC.h"
#include "E.h"
#include "INNN.h"

#define YELLOW	0xDB1D
#define BLACK	0x0000
#define WHITE	0xFFFF
#define BLUE	0x001F
#define GREEN	0x03E0
#define RED		0x7C00
#define w1 0x124
#define w2 0x92
#define w3 0x49
#define w4 0x1c0
#define w5 0x38
#define w6 0x7
#define w7 0x111
#define w8 0x54

#define  NonPal_Fb   ((volatile unsigned short(*)[480]) FRAME_BUFFER)



void Uart_Printf(char *fmt,...);
void Lcd_Printf(int x, int y, int color, int bkcolor, int zx, int zy, char *fmt,...);

void print(int a,int b);
void In(int *a,int *b);
int vic(int *a,int *b);
void menu(void);
void m1(void);
void m2(void);
void m3(void);
int argo1(int *a, int *b);
int argo2(int *a, int *b);
int argo0(int *a);
void __DrawingTic(void);
int __TS1(void);
int __TouchIn(void);



// Global Variables Declaration
volatile int Key_Value = 0;
volatile  int ADC_x, ADC_y;
volatile  int Touch_Pressed=0;

// ISR Declaration
static void Uart_RX1_ISR(void) __attribute__ ((interrupt ("IRQ")));;
void Key_ISR(void) __attribute__ ((interrupt ("IRQ")));;
void Touch_ISR(void) __attribute__ ((interrupt ("IRQ")));

// Reset
int resetCheck = 1;




/***************************************
 * 
 * Key_Interrupt (External Interrupt)
 * 
 ***************************************/



void Key_ISR()
{
	unsigned int temp;

	// Ìï¥Îãπ Ïù∏ÌÑ∞ÎüΩÌä∏ Masking 
	rINTMSK1 |= (0x7<<2);
	rEINTMASK |= (0x7<<4);

	// ÌÇ§Î•º ÏùΩÍ∏∞ ÏúÑÌïòÏó¨ Polling Ìï®Ïàò ÏÇ¨Ïö© 
	temp = rGPFCON;
	
	//1) GPFCON[6:2] change to Input mode 
	//2) ÏûÖÎ†•Îêú ÌÇ§Í∞íÏùÑ ÌôïÏù∏ÌïòÏó¨ Î≥ÄÏàòÏóê Ï†ÄÏû• 
     		 
	rGPFCON &= ~(0x3ff<<4);	
	Key_Value = Key_Get_Pressed();

    // Î∞òÎìúÏãú portÎ•º Î≥µÏõêÌïòÍ∏∞ ÏúÑÌïòÏó¨ Ï†ÄÏû•Ìïú Í∞íÏúºÎ°ú Î≥µÏõê 
	rGPFCON = temp;

	//KeyoutÏùÑ 0Î°ú ÌïòÏó¨ ÌÇ§ ÏûÖÎ†• ÎåÄÍ∏∞ÏÉÅÌÉúÎ°ú Î∞îÍøà 
	rGPGDAT &= ~0x1;
	rGPFDAT	&= ~(0x1<<7);
	
	//Pendng Clear on EINT2,3,4,5,6 
	rEINTPEND |= (0x7<<4);
	rSRCPND1 |= (0x7<<2);
	rINTPND1 |= (0x7<<2);
	
	
	// Key Interrupt
	
	if(Key_Value==6)
	{
		resetCheck = 1;
	}
	

	//Ìï¥Îãπ Ïù∏ÌÑ∞ÎüΩÌä∏ UnMasking 
	rINTMSK1 &= ~(0x7<<2);
	rEINTMASK &= ~(0x7<<4);	

	
}


/***************************************
 * 
 * UART_Interrupt (External Interrupt)
 * 
 ***************************************/

static void Uart_RX1_ISR(void)
{
	//UART1_RX Í∞Ä Îã§Ïãú Î∞úÏÉùÌïòÏßÄ ÏïäÎèÑÎ°ù Masking 
	rINTSUBMSK |=(0x1<<3);
	rINTMSK1 |=(0x1<<23);
	
	//Pendng Clear on UART1_RX,UART1_ERR
 
	rSUBSRCPND |= (0x1<<3); //UART1_RX
	rSRCPND1 |=(0x1<<23);
	rINTPND1 |=(0x1<<23);
	
	// ISRoutine Execution
	//Uart_Send_Byte('\n');
	//Uart_Send_String("Uart Receive data is OK..!!\n");
	
	#if 0
	if(Uart_Check_Overrun_Error()) 	Uart_Send_Byte('*'); Uart_Send_Byte('\n');
	
	#endif
	
	//UnMasking on UART1_RX//
	rINTSUBMSK &=~(0x1<<3);
	rINTMSK1 &=~(0x1<<23);

}
 /***************************************
 * 
 * Title: Lcd Touch
 * 
 ***************************************/
void Touch_ISR()
{
	//Ïù∏ÌÑ∞ÎüΩÌä∏ ÌóàÏö©ÌïòÏßÄ ÏïäÏùå on Touch
	rINTSUBMSK |= (0x1<<9);
	rINTMSK1 |= (0x1<<31);	
	
	//Pendng Clear on Touch
	rSUBSRCPND |= (0x1<<9);
	rSRCPND1 |= (0x1<<31);
	rINTPND1 |= (0x1<<31);
	
	if(rADCTSC & 0x100)
	{
		rADCTSC &= (0xff); 
		Touch_Pressed = 0;
		Uart_Send_String("Detect Stylus Up Interrupt Signal \n");
	}
	
	else
	{
		Uart_Send_String(" ISR ÎÇ¥Î∂Ä  \n");
		
		//Stylus Down, YM_out Enable, YP_out Disable, XM_out Disable, XP_out disable
		//XP Pull-up Disable, Auto Sequential measurement of X/Y, No operation mode 
	rADCTSC =(0<<8)|(1<<7)|(1<<6)|(0<<5)|(1<<4)|(1<<3)|(1<<2)|(0);	

		//ENABLE_START 	
	rADCCON |=(1);
		
		//wait until End of A/D Conversion 
	while(!(rADCCON & (1<<15)));
		
		//store X-Position & Y-Position Conversion data value to ADC_x, ADC_y
	ADC_x = (rADCDAT0 & 0x3ff);
	ADC_y = (rADCDAT1 & 0x3ff);
		
		Touch_Pressed = 1;
		
		//change to Waiting for interrupt mode 
		//Stylus Up, YM_out Enable, YP_out Disable, XM_out Disable, XP_out disable
		//XP Pull-up Disable, Normal ADC conversion, Waiting for interrupt mode	
	rADCTSC =(1<<8)|(1<<7)|(1<<6)|(1<<4)|(0<<3)|(0<<2)|(3);
	}
	
	/* Ïù∏ÌÑ∞ÎüΩÌä∏ Îã§Ïãú ÌóàÏö©  on Touch */
	rINTSUBMSK &= ~(0x1<<9);
	rINTMSK1 &= ~(0x1<<31);
	
}

void print(int a,int b) 			// ±◊∑°«¡ √‚∑¬«œ¥¬ «‘ºˆ 
{
	
	int i;
	for(i=1;i<4;i++)		
	{
			
		if((a>>(9-i)&1)==1)
			{
			Lcd_Printf(25+85*(i-1),25,BLACK,YELLOW,2,2,"O");			continue;
			}
		if((b>>(9-i)&1)==1)
			{
			Lcd_Printf(25+85*(i-1),25,BLACK,YELLOW,2,2,"X");			continue;
			}		

	}
		for(i=4;i<7;i++)		
	{
		if((a>>(9-i)&1)==1)
			{
				Lcd_Printf(25+85*(i-4),120,BLACK,YELLOW,2,2,"O");				continue;
			}
		if((b>>(9-i)&1)==1)
			{
				Lcd_Printf(25+85*(i-4),120,BLACK,YELLOW,2,2,"X");				continue;
			}		
	
	}
		for(i=7;i<10;i++)		
	{
		if((a>>(9-i)&1)==1)
			{
				Lcd_Printf(25+85*(i-7),210,BLACK,YELLOW,2,2,"O");				continue;
			}
		if((b>>(9-i)&1)==1)
			{
				Lcd_Printf(25+85*(i-7),210,BLACK,YELLOW,2,2,"X");				continue;
			}		
	}
}

void In(int *a,int *b)//∞™¿ª πﬁ∞Ì  ∫Ø»Ø«ÿº≠  a,bø° ¿˙¿Â«œ¥¬ «‘ºˆ  
{
	int input; 
	static int cnt=1;
			
	while(1)
	{
	input=__TouchIn();
	if((input>9)||(input<1)) ;
	else if(((1<<(9-input))|(*a))==(*a)) ; 
	else if(((1<<(9-input))|(*b))==(*b)) ;
	else break; // ∏∏æ‡ ø¿∑˘∞° ¿÷¥Ÿ∏È 1~9±Ó¡ˆ∏∏ «“∂ß µ«∞‘ πŸ≤Ÿ±‚ 
	}
		if(cnt%2==1)
		{
			*a|=(1<<(9-input));
					}
		else 
		{
			*b|=(1<<(9-input));
		}	
	cnt++;
}

void In2(int *a,int *b)//a∞™ πﬁ¥¬ «‘ºˆ  -æÀ∞Ì∏Æ¡Ú ¿¸øÎ 
{
	int input; 
	while(1)
	{
	input=__TouchIn();
	if((input>9)||(input<1)) ;
	else if(((1<<(9-input))|(*a))==(*a)) ;
	else if(((1<<(9-input))|(*b))==(*b)) ;	
	else break;
	}
	*a|=(1<<(9-input));
}
void In3(int *b,int input)//b∞™πﬁ¥¬«‘ºˆ  - æÀ∞Ì∏Æ¡Ú ¿¸øÎ 
{
	*b|=(1<<(9-input));
}

int vic(int *a,int *b) //Ω¬∏Æ ∆–πË ∆«∫∞ 
{
	int w[8]={w1,w2,w3,w4,w5,w6,w7,w8};
	int i,j;
	
	
	for(i=0;i<8;i++)
	{
	if((w[i]|(*a))==(*a)) 
		{
			print(*a,*b);
			Lcd_Printf(280,60,WHITE,RED,1,2,"* Player O win!!! *");
			for(j=0;j<4000000;j++) ;
			return -1;	
		}
	if(((*b)|w[i])==(*b)) 
		{
			print(*a,*b);
			Lcd_Printf(280,60,WHITE,RED,1,2,"* Player X win!!! *");
			for(j=0;j<4000000;j++) ;
			return -1;	
		}
	}	
	if((*a|*b)==0x1ff)
	{
		print(*a,*b);
		Lcd_Printf(315,60,WHITE,RED,1,2,"*  Draw!!!  *");
		for(j=0;j<4000000;j++) ;
		return -1;	
	}
	return 0;
}

int argo1(int *a, int *b)//≥≠¿Ãµµ ªÛ 
{
	int w[8]={w1,w2,w3,w4,w5,w6,w7,w8};
	int x[8][3]={{0x24,0x104,0x120},{0x12,0x82,0x90},{0x9,0x41,0x48},{0xc0,0x140,0x180},{0x18,0x28,0x60},{0x3,0x5,0x6},{0x22,0x101,0x110},{0x14,0x44,0x50}};
	//{{0b000100100,0b100000100,0b100100000},{0b000010010,0b010000010,0b010010000},{0b000001001,0b001000001,0b001001000}
	//,{0b011000000,0b101000000,0b110000000},{0b000011000,0b000101000,0b000110000},{0b000000011,0b000000101,0b000000110},{0b000010001,0b100000001,0b100010000},{0b000010100,0b001000100,0b001010000}};
	int i,j;
	int output;
	//srand(time(NULL));
	output=0;
	//≥ª≤® √§øÏ±‚ 
	for(i=0;i<8;i++)
	{
		for(j=0;j<3;j++)
		{
			if((*b&w[i])==x[i][j])
			{		
				if(((w[i]-x[i][j])&(*a))!=0){ continue;}
				else	{output=(w[i]-x[i][j]);		return output;}
			}
		}	
	}
	for(i=0;i<8;i++) // ªÛ¥Î≤® ∏∑±‚ 
	{
		for(j=0;j<3;j++)
		{
			if((*a&w[i])==x[i][j])
			{		
				if(((w[i]-x[i][j])&(*b))!=0) continue;	
				else	{output=(w[i]-x[i][j]);		return output;}
			}
		}	
	}
	while(1)
	{
		output=rand()%9+1;
		if(((1<<(9-output))&(*a))==0)
		{
			if(((1<<(9-output))&(*b))==0)
			{
			output=(1<<(9-output));
			return output;
			}
		}
	}
	//µ—¥Ÿ æ∆¥“ ∞ÊøÏ ≥ª≤® ∑£¥˝ √§øÏ±‚ 
}
int argo2(int *a, int *b)//≥≠¿Ãµµ «œ 
{
	int w[8]={w1,w2,w3,w4,w5,w6,w7,w8};
	int x[8][3]={{0x24,0x104,0x120},{0x12,0x82,0x90},{0x9,0x41,0x48},{0xc0,0x140,0x180},{0x18,0x28,0x60},{0x3,0x5,0x6},{0x22,0x101,0x110},{0x14,0x44,0x50}};
	int i,j;
	int output;
	//srand(time(NULL));
	output=0; //≥ª≤® √§øÏ±‚ 
	for(i=0;i<8;i++) // ªÛ¥Î≤® ∏∑±‚ 
	{
		for(j=0;j<3;j++)
		{
			if((*a&w[i])==x[i][j])
			{		
				if(((w[i]-x[i][j])&(*b))!=0) continue;	
				else	{output=(w[i]-x[i][j]);		return output;}
			}
		}	
	}

	for(i=0;i<5;i++) //10π¯∏∏ ≥≠ºˆ∑Œ ±‚»∏¡‹ 
	{
		output=rand()%9+1;
		if(((1<<(9-output))&(*a))==0)
		{
			if(((1<<(9-output))&(*b))==0)
			{
			output=(1<<(9-output));
			return output;
			}
		}
	}	
	for(i=0;i<8;i++)
	{
		for(j=0;j<3;j++)
		{
			if((*b&w[i])==x[i][j])
			{
				if(((w[i]-x[i][j])&(*a))!=0){ continue;}
				else	{output=(w[i]-x[i][j]);		return output;}
			}
		}	
	}
	while(1)
	{
		output=rand()%9+1;
		if(((1<<(9-output))&(*a))==0)
		{
			if(((1<<(9-output))&(*b))==0)
			{
			output=(1<<(9-output));
			return output;
			}
		}
	}//µ—¥Ÿ æ∆¥“ ∞ÊøÏ ≥ª≤® ∑£¥˝ √§øÏ±‚ 
}

int argo0(int *a) //Ω√¿€æÀ∞Ì∏Æ¡Ú1 - æÀ∞Ì∏Æ¡Ú ªÛ Ω√¿€øÎ 
{
	switch(*a)
	{
		case 0x100:
		case 0x40:
		case 0x4:
		case 0x1:	
			return 5;
		case 0x80:
			return 8;
		case 0x20:
			return 6;
		case 0x8:
			return 4;
		case 0x2:
			return 2;
		case 0x10:
			return 3;
	}
}


void menu(void)//∏ﬁ¥∫ √‚∑¬  /1 p vs p /2 p v e √ ±ﬁ / 3 pve ∞Ì±ﬁ 
{
	int x=0;

	while(1)
	{
	//Lcd_Draw_BMP(0, 0, innn);
	__DrawingTic();	
		
	x=__TS1();
	
	switch(x)
	{
		case 1:
		//	printf("Player VS Player ¿ª º±≈√«œºÃΩ¿¥œ¥Ÿ.\n");
			m1();
			break;

		case 2:
		//	printf("Player VS Computer(≥≠¿Ãµµ «œ) ¿ª º±≈√«œºÃΩ¿¥œ¥Ÿ.\n");
			m2();
			break;

		case 3:
		//	printf("Player VS Computer(≥≠¿Ãµµ ªÛ) ¿ª º±≈√«œºÃΩ¿¥œ¥Ÿ.\n");
			m3();
			break;

	}
	}
}

void m1(void)// pvp 
{
	int x=0;
	int i,j;
	int static a=0;
	int static b=0;
	a=0;
	b=0; 
	for(j=0; j<272; j++)
	{
		for(i=0; i<480; i++)
			NonPal_Put_Pixel(i,j,WHITE);
	}
	Lcd_Draw_BMP(0, 0, ac);
	Lcd_Printf(260,12,WHITE,BLACK,1,2," PLAYER VS PLAYER ");
	while(1)
	{
	print(a,b);
	In(&a,&b);
	x=vic(&a,&b);
	if(x==-1) break; 
	}
}
void m2(void)// pve ≥≠¿Ãµµ «œ 
{
	int x=0;
	int static a=0;
	int static b=0;
	int inputb;
	a=0;
	b=0; 
	int i,j;
	//Graphic_Init();
	for(j=0; j<272; j++)
	{
		for(i=0; i<480; i++)
			NonPal_Put_Pixel(i,j,WHITE);
	}
	Lcd_Draw_BMP(0, 0, ac);
		Lcd_Printf(260,12,WHITE,BLACK,1,2," PLAYER VS COMPUTER(EASY) ");
	while(1)
	{
		print(a,b);
		In2(&a,&b);
		x=vic(&a,&b);	if(x==-1) break; 
		inputb=argo2(&a,&b);
		b=b|inputb;
		x=vic(&a,&b);	if(x==-1) break; 
	}
}
void m3(void)// pve ≥≠¿Ãµµ ªÛ 
{
	int x=0;
	int static a=0;
	int static b=0;
	int inputb;
	a=0;
	b=0; 
	int i,j;
	//Graphic_Init();
	for(j=0; j<272; j++)
	{
		for(i=0; i<480; i++)
			NonPal_Put_Pixel(i,j,WHITE);
	}
	Lcd_Draw_BMP(0, 0, ac);
	Lcd_Printf(260,12,WHITE,BLACK,1,1," PLAYER VS COMPUTER(HARD) ");
	print(a,b);
	In2(&a,&b);
	inputb=argo0(&a);
	b=b|(1<<(9-inputb));
	In3(&b,inputb);
	print(a,b);
	while(1)
	{
		In2(&a,&b);
		x=vic(&a,&b);
		if(x==-1) break; 
		inputb=argo1(&a,&b);
		b=b|inputb;
		x=vic(&a,&b);
		if(x==-1) break; 
		print(a,b);
	}
}
void __DrawingTic(void) // ∆Ω≈√≈‰ »≠∏È √‚∑¬ 
{
	int i,j;
	//Graphic_Init();
	for(j=0; j<272; j++)
	{
		for(i=0; i<480; i++)
			NonPal_Put_Pixel(i,j,WHITE);
	}
	Lcd_Draw_BMP(0, 0, e);
	Lcd_Printf(350, 30, BLACK, WHITE, 1, 1, "VS PLAYER");
	Lcd_Printf(350, 98, BLACK, WHITE, 1, 1, "VS COM(EASY)");
	Lcd_Printf(350, 175, BLACK, WHITE, 1, 1, "VS COM(HARD)");
}
int __TS1(void)  //≈Õƒ° 
{
	while(1){
	if (Touch_Pressed == 1)
	{
		if ((ADC_x > 170 && ADC_x <= 480) && (ADC_y > 650 && ADC_y <= 686)) // START
			{
				return 1;
			}
		else if (((ADC_x > 170 && ADC_x <= 480) && (ADC_y > 530 && ADC_y <= 560)))// STOP
			{
				return 2;
			}
		else if (((ADC_x > 170 && ADC_x <= 480) && (ADC_y > 420 && ADC_y <= 500))) //RESET
			{
				return 3;
			}
	}
	}
}
int __TouchIn(void) // s2
{
	while(1){
		
	if (Touch_Pressed == 1)
	{
		
		if (ADC_x > 761 && ADC_x <= 864 && ADC_y > 550 && ADC_y <= 661)
		{
			//while(!(Touch_Pressed==0))
			return 1;
		}
		else if (ADC_x > 620 && ADC_x <= 761 && ADC_y > 550 && ADC_y <= 661)
		
		
		{
			// Swt
		//	while(!(Touch_Pressed==0))
			return 2;
		}
		else if (ADC_x > 518 && ADC_x <= 620 && ADC_y > 550 && ADC_y <= 661)
		{
			// Swt
		//	while(!(Touch_Pressed==0))
			return 3;
		}
		else if (ADC_x > 761 && ADC_x <= 864 && ADC_y > 416 && ADC_y <= 550)
		{
			// Swt
	//		while(!(Touch_Pressed==0))
			return 4;
		}
		else if (ADC_x > 620 && ADC_x <= 761 && ADC_y > 416 && ADC_y <= 550)
		{
			// Swt
		//	while(!(Touch_Pressed==0))
			return 5;
		}
		else if (ADC_x > 518 && ADC_x <= 620 && ADC_y > 416 && ADC_y <= 550)
		{
			// Swt
		//	while(!(Touch_Pressed==0))
			return 6;
		}
		else if (ADC_x > 761 && ADC_x <= 864 && ADC_y > 350 && ADC_y <= 416)
		{
			// Swt
		//	while(!(Touch_Pressed==0))
			return 7;
		}
		else if (ADC_x > 620 && ADC_x <= 761 && ADC_y > 350 && ADC_y <= 416)
		{
			// Swt
		//	while(!(Touch_Pressed==0))
			return 8;
		}
		else if (ADC_x > 518 && ADC_x <= 620 && ADC_y > 350 && ADC_y <= 416)
		{
			// Swt
		//	while(!(Touch_Pressed==0))
			return 9;
		}
	}
	}
}






void Main(void)
{	
	int i,j;
	
	Uart_Init(115200);	
	Lcd_Port_Init();
	NonPal_Lcd_Init();
	Key_IRQ_Port_Init();
	Timer_Init();
	Buzzer_Init();
	Touch_Init();
	
	Uart_Printf("\n");

	//LCD color Init
	/*
	for(j=0; j<272; j++)
	{
		for(i=0; i<480; i++)
			NonPal_Put_Pixel(i,j,WHITE);
	}*/
		//Key Interrupt
	//Ïù∏ÌÑ∞ÎüΩÌä∏ Î≤°ÌÑ∞Ïóê Key_ISR Ìï®Ïàò Îì±Î°ù
	//pISR_EINT3 = (unsigned int)Key_EINT3_ISR;	
	pISR_EINT2 = (unsigned int)Key_ISR;
	pISR_EINT3 = (unsigned int)Key_ISR;
	pISR_EINT4_7 = (unsigned int)Key_ISR;
	//Ïù∏ÌÑ∞ÎüΩÌä∏ ÌóàÏö© on EINT2,3,4,5,6	
	//rINTMSK &= ~(1<<3);
	rINTMSK1 &= ~(0x7<<2);
	rEINTMASK &= ~(0x7<<4);	
	//UART Interrupt
	//Ïù∏ÌÑ∞ÎüΩÌä∏ Î≤°ÌÑ∞Ïóê Uart_RX1_ISR Ìï®Ïàò Îì±Î°ù
	pISR_UART1 = (unsigned int)Uart_RX1_ISR;
		
	//Ïù∏ÌÑ∞ÎüΩÌä∏ ÌóàÏö© on UART1_RX,ERR
	rINTSUBMSK &=~(0x1<<3);  //UART1_RX
	rINTSUBMSK &=~(0x1<<5); //UART1_ERR
	rINTMSK1 &=~(0x1<<23);
	
	//Lcd Touch
	//Ïù∏ÌÑ∞ÎüΩÌä∏ Î≤°ÌÑ∞Ïóê Touch_ISR Ìï®Ïàò Îì±Î°ù 
	pISR_ADC = (unsigned int)Touch_ISR;
	//Ïù∏ÌÑ∞ÎüΩÌä∏ ÌóàÏö© on Touch 
	rINTSUBMSK &= ~(0x1<<9);
	rINTMSK1 &= ~(0x1<<31);	
	/* TO DO : ¿Œ≈Õ∑¥∆Æ ∫§≈Õø° Touch_ISR «‘ºˆ µÓ∑œ */
	pISR_ADC = (unsigned int)Touch_ISR;
	
	/* TO DO :  ¿Œ≈Õ∑¥∆Æ «„øÎ on Touch */
	rINTSUBMSK &= ~(0x1<<9);
	rINTMSK1 &= ~(0x1<<31);	
	
	
	menu();

	

	
}

