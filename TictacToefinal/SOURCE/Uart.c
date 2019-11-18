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

	/* Baud rate ����  */		
	rUBRDIV1= ((unsigned int)(PCLK/16./baud+0.5)-1 );
}


void Uart_Printf(const char *fmt,...)
{
	/* Todo #1 : PC�� �͹̳� â���� �޽��� ���*/
	// 1 : �����μ��Լ� �ۼ��� ����
	// 2 : va_list ���� ���� �� ���ڿ� �迭 ����
	// 3 : va_start �Լ��� ���� va_list������ ù��° �����μ��� ����Ű���� �ʱ�ȭ
	// 4 : vsprintf�� ��� printf�� ������ ��� �����ϵ��� �ۼ�
	// 5 : Uart_SendString�� ����Ͽ� ���ڿ� ����
	// 6 : va_end �Լ� ����Ͽ� ���� �μ� ���� �� ������
	
	  va_list ap;
    char string[256];

    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    Uart_Send_String(string);
    va_end(ap);	
		
}

void Uart_Send_String(char *pt)
{

	/* Todo #2 : ���� �ϳ��� ������Ű�鼭 ���ڿ� ���  */
	// 1 : pt�� ���Ͽ� ���ڿ��� ����(����) null���ڸ� ���������� ���� ���
	// 2 : Uart_SendByte�Լ��� �̿��Ͽ� ���� ���
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
	/* UTRSTAT1�� ���� Ȯ���Ͽ� ���ڿ� �Է�   */	
	while(!(rUTRSTAT1 & 0x1));	
	return RdURXH1();
}

char Uart_GetCh(void)
{
	/* UTRSTAT1�� ���� Ȯ���Ͽ� ���ڿ� �Է�   */	
	
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

