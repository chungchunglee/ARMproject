#include "2450addr.h"
#include "option.h"
#include "timer.h"


void Timer_Init(void)
{
	/* 
	* 	Timer0 Init 
	* Prescaler value : 255, dead zone length = 0
	* Divider value : 1/16, no DMA mode
	* New frequency : (PCLK/(Prescaler value+1))*Divider value = (66Mhz/(256))*(1/16)
	*				= 16.113Khz(16113Hz)
	*/
	rTCFG0 = (0<<8)|(0xff); 
	rTCFG1 = (0<<20)|(3); 
	
	/* TCON설정 :Dead zone disable,  auto reload on, output inverter off
	*  manual update no operation, timer0 stop, TCNTB0=0, TCMPB0 =0
	*/
	rTCON  = (0<<4)|(1<<3)|(0<<2)|(0<<1)|(0);
	rTCNTB0 = 0;
	rTCMPB0 = 0;
  
}

void Timer_Delay(int msec)
{
	/*
	* 1) TCNTB0설정 : 넘겨받는 data의 단위는 msec이다.
	*                  따라서 msec가 그대로 TCNTB0값으로 설정될 수는 없다.
	* 2) manual update후에  timer0를 start시킨다. 
	* 	 note : The bit has to be cleared at next writing.
	* 3) TCNTO0값이 0이 될때까지 기다린다. 	
	*/
	/* YOUR CODE HERE */	
	rTCNTB0 = 16.113*msec;	

	rTCON |= (1<<1)|(0);
	rTCON &= ~(1<<1);
	
	rTCON |= 1;	
	
	while(rTCNTO0);
	
}


void Timer3_Init(void){
	rTCFG0 = rTCFG0 | (0xff<<8);
	rTCFG1 = rTCFG1 | (0x3<<12);
	//16.113Khz(16113hz)
	rTCON = (rTCON & ~(0xf<<16));
	rTCON = (rTCON | (1<<19));
	rTCNTB3=0;
	rTCMPB3=0;
}

void Timer3_Countup_Start(void){
	rTCNTB3=0x3ef1;//16113
	rTCMPB3=0;
	rTCON = rTCON | (1<<17);
	rTCON = rTCON & ~(1<<17);
	rTCON = rTCON | (1<<16);
	
}
void Timer3_Countup_Stop(void){
	rTCON = rTCON & ~(1<<16);
	
}

void Timer3_Isr_Init(void (*fp)(void))
{
	pISR_TIMER3 = (unsigned int)fp;	//(*(unsigned *)(_ISR_STARTADDRESS+0x54))
}

void Timer3_ISR_Enable(int enable){
	enable? (rINTMSK1 &= (unsigned)(~(1<<13))) : (rINTMSK1 |= (unsigned)(1<<13));
}

//
// Use Timer0 for PC_ElapsedStart
// Timer INPUT clock Resolution 2.0625MHz
void ElapseTimer_Start(void)
{
 rTCFG0 = (rTCFG0 & ~0xff) | 1;
 rTCFG1 = (rTCFG1 & ~0xf) | 3; 
 
 /* TCON설정 :Dead zone disable,  auto reload on, output inverter off
 *  manual update no operation, timer0 stop, TCNTB0=0, TCMPB0 =0
 */
 rTCON  = (rTCON & ~((1<<4)|(1<<2)|(1<<1)|(1))) | (1<<3);

 rTCNTB0 = 0;
 rTCMPB0 = 0;

 rTCNTB0 = 0xffff; // initial value(65535) for timer

 rTCON |= (1<<1)|(0);
 rTCON &= ~(1<<1);
 rTCON |= 1;
}

//
// Use Timer0 for PC_ElapsedStart
// Timer Resolution 1.5625MHz
unsigned int ElapseTimer_Stop(void)
{
 rTCON &= ~1;

 return (0xffff - rTCNTO0);
}

