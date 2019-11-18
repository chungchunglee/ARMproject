/*
 * =====================================================================
 * NAME         : Uart.c
 * Descriptions : Uart Test Driver
 * IDE          : GCC-4.1.0
 * Modification : Seonghye, Son (2015.07.10)
 *	 
 * =====================================================================
 */
#include "2450addr.h"
#include "stdarg.h"
#include "option.h"
#include "my_lib.h"


extern int vsprintf(char *, const char * , va_list);

void Uart_Init(int baud)
{
	int pclk;
	pclk = PCLK;
	
	// PORT GPIO initial
	rGPHCON &= ~(0xf<<4);
	rGPHCON |= (0xa<<4);	

	
	rUFCON1 = 0x0;
	rUMCON1 = 0x0;
	
	/* Line Control(Normal mode, No parity, One stop bit, 8bit Word length */
	rULCON1 = 0x3;

	/* Transmit & Receive Mode is polling mode  */
	rUCON1  = (1<<2)|(1);

	/* Baud rate 설정  */		
	rUBRDIV1= ((unsigned int)(PCLK/16./baud+0.5)-1 );
}


void Uart_Printf(const char *fmt,...)
{
	/* Todo #1 : PC의 터미널 창으로 메시지 출력*/
	// 1 : 가변인수함수 작성법 참조
	// 2 : va_list 변수 선언 및 문자열 배열 선언
	// 3 : va_start 함수를 통해 va_list변수가 첫번째 가변인수를 가리키도록 초기화
	// 4 : vsprintf를 사용 printf와 유사한 기능 수행하도록 작성
	// 5 : Uart_SendString을 사용하여 문자열 전달
	// 6 : va_end 함수 사용하여 가변 인수 읽은 후 뒷정리
	
	  va_list ap;
    char string[256];

    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    Uart_Send_String(string);
    va_end(ap);	
		
}

void Uart_Send_String(char *pt)
{

	/* Todo #2 : 문자 하나씩 증가시키면서 문자열 출력  */
	// 1 : pt를 통하여 문자열에 접근(참조) null문자를 만날때까지 문자 출력
	// 2 : Uart_SendByte함수를 이용하여 문자 출력
	while((*pt)!='\0')
	{
        	Uart_Send_Byte(*pt);
			pt++;
	}
		
}

void Uart_Send_Byte(int data)
{
	if(data=='\n')
	{
		while(!(rUTRSTAT1 & 0x2));
		WrUTXH1('\r');
	}
	
	while(!(rUTRSTAT1 & 0x2));	
	WrUTXH1(data);
}

char Uart_GetChar(void)
{
	/* UTRSTAT1의 값을 확인하여 문자열 입력   */	
	while(!(rUTRSTAT1 & 0x1));	
	return RdURXH1();
}

char Uart_GetCh(void)
{
	/* UTRSTAT1의 값을 확인하여 문자열 입력   */	
	
	//Non-blocking Function	
	return RdURXH1();
}

	/* 
	 * void Uart_TxEmpty(int ch)
	 * -----------------------------------------------------------------
	 */
void Uart_TxEmpty(int ch)
{
    while(!(rUTRSTAT1 & 0x4)); //Wait until tx shifter is empty.
}

	/* 
	 * char Uart_GetKey(void)
	 * -----------------------------------------------------------------
	 */
char Uart_GetKey(void)
{
	if(rUTRSTAT1 & 0x1)    //Receive data ready
		return RdURXH1();
	else
		return 0;
}

