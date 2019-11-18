#include "2450addr.h"
#include "option.h"
#include "my_lib.h"
//#include "macro.h"
#include "buzzer.h"

#define BASE10	10000

#define DURATION_5SEC   1000
#define DURATION_4SEC   500
#define DURATION_3SEC   250
#define DURATION_1SEC   100

#define Z        0         // Mute
#define C1      261.626     // Do
#define C1_    277.183
#define D1      293.665     // Re
#define D1_    311.127
#define E1       29.628    // Mi
#define F1      349.228     // Pa
#define F1_    369.994
#define G1      391.995     // Sol
#define G1_    415.305
#define A1      440     // La
#define A1_    466.164
#define B1      493.883     // Si
#define C2      523.251    // high Do
#define C2_    554.365
#define D2      587.330    // high Re
#define D2_     622.254
#define E2       659.255    // high Mi
#define F2       698.456    // high Pa
#define F2_     739.989
#define G2       783.991    // high Sol
#define G2_     830.609
#define A2       880        // high La
#define A2_     932.328
#define B2       987.767      // high Si
#define C3       1046.502   // double high Do
#define C3_     1108.730
#define D3       1174.659  // double high Si
#define D3_      1244.508 
#define E3        1318.510    // double high Mi
#define F3         1396.913    // double high Pa
#define F3_       1479.978
#define G3          1567.982   // double high Sol
#define G3_       1661.219
#define A3           1760     //  double high La
#define A3_       1864.655
#define B3         1975.533    // double high Si


void Buzzer_Init(void)
{
	// Buzzer = GPB1
	rGPBDAT |= (0x1<<1);
	rGPBCON &= ~(0x3 << 2);
	rGPBCON |= (0x1<<2);
}

void Buzzer_Beep(int tone, int duration)
{
	unsigned int temp;

	//Uart_Printf("Buzzer_Beep\n");

	for( ;(unsigned)duration > 0; duration--)
	{
		rGPBDAT &= ~(0x1<<1);
		for(temp = 0 ; temp < (unsigned)tone; temp++);
		rGPBDAT |= (0x1<<1);
		for(temp = 0 ; temp < (unsigned)tone; temp++);
	}
}

/*
void Beep(int tone, int duration)
{
	Uart_Printf("Beep\n");

	rTCFG1 = (rTCFG1 &~ (0xf<<20))|(3<<16); 
	
	/ TCON¼³Á¤ :Dead zone disable,  auto reload on, output inverter off
	  manual update no operation, timer0 stop, TCNTB0=0, TCMPB0 =0
	 /
	rTCNTB4 = 16.113*duration;
	rTCON &=~  (1<<22);
	rTCON |=  (1<<21);
	rTCON &= ~(1<<21);
	rTCON |=  (1<<20);
	
	Uart_Printf("rTCNTO4=%d\n", rTCNTO4);
	while(rTCNTO4 !=0) 
	{
		rGPBDAT &= ~(0x1<<1);
		DelayForPlay2(BASE10/tone);
		rGPBDAT |= (0x1<<1);
		DelayForPlay2(BASE10/tone);
	}
	rTCON &= ~(1<<20);
}

void DelayForPlay2(unsigned short time)	// resolution=0.1ms
{
	//Prescaler value : 39  
	//Clock Select    : 128 
	rWTCON=(37<<8)|(3<<3);			// resolution=0.1ms
	rWTDAT=time+10;					// Using WDT
	rWTCNT=time+10;
	rWTCON|=(1<<5);

	while(rWTCNT>10);
	rWTCON = 0;
}

void BuzzerTest(void) 
{
	Uart_Printf("BuzzerTest\n");

	Buzzer_Init();
//	Beep(TONE_BEEP, DURATION_5SEC);
	Beep(C1, DURATION_5SEC);
	Beep(D1, DURATION_5SEC);
	Beep(E1, DURATION_5SEC);
	Beep(F1, DURATION_5SEC);
	Beep(G1, DURATION_5SEC);
	Beep(A1, DURATION_5SEC);
	Beep(B1, DURATION_5SEC);
	Beep(C2, DURATION_5SEC);
}

void  FailBeep(void)
{
   Beep(Z, DURATION_3SEC);
   Beep(B1, DURATION_1SEC);
   Beep(F2, DURATION_1SEC);
   Beep(Z, DURATION_1SEC);
   Beep(F2, DURATION_1SEC);
   Beep(Z, DURATION_1SEC);
   Beep(F2, DURATION_1SEC);
   Beep(E2, DURATION_1SEC);
   Beep(D2, DURATION_1SEC);
   Beep(C2, DURATION_3SEC);
   Beep(Z, DURATION_3SEC); 
}

void  VictoryBeep(void)
{
   Beep(Z, DURATION_1SEC);
   Beep(G1, DURATION_1SEC);
   Beep(A1, DURATION_1SEC);
   Beep(B1, DURATION_1SEC);
   
   Beep(C2, DURATION_1SEC);
   Beep(G1, DURATION_3SEC);
   Beep(Z, DURATION_1SEC);
   
   Beep(C2, DURATION_1SEC);
   Beep(B1, DURATION_1SEC);
   Beep(C2, DURATION_1SEC);
   Beep(D2, DURATION_1SEC);
   Beep(A1, DURATION_3SEC);
   Beep(Z, DURATION_1SEC);
   
   Beep(A1, DURATION_1SEC);
   Beep(B1, DURATION_1SEC);
   Beep(C2, DURATION_1SEC);

   Beep(E2, DURATION_1SEC);
   Beep(D2, DURATION_1SEC);
   Beep(D2, DURATION_1SEC);
   Beep(C2, DURATION_1SEC);
   Beep(C2, DURATION_1SEC);
   Beep(B1, DURATION_1SEC);
   Beep(A1, DURATION_1SEC);
   Beep(B1, DURATION_1SEC);
   Beep(G1, DURATION_3SEC);
   Beep(Z, DURATION_1SEC);
}

void  FirstBeep(void)
{
	Beep(Z, DURATION_1SEC);
   	Beep(C2_, DURATION_3SEC);
   	Beep(Z, DURATION_1SEC);
}

void  SecondBeep(void)
{
	Beep(Z, DURATION_1SEC);
   	Beep(G2_, DURATION_3SEC);
   	Beep(Z, DURATION_1SEC);
}

void StartBeep(void)
{
    Beep(Z, DURATION_3SEC);
   Beep(G1, DURATION_1SEC);
   Beep(A1, DURATION_1SEC);   
   Beep(B1, DURATION_1SEC);
   Beep(C2, DURATION_3SEC);
   Beep(Z, DURATION_1SEC);
   Beep(G1, DURATION_1SEC);
   Beep(C2, DURATION_3SEC);
   Beep(Z, DURATION_1SEC);
}
*/

