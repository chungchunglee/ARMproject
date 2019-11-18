/* Assembly for CW2.1 */
/* ROM, SDRAM �� GCS ��ȣ �ʱ�ȭ ���� */
/* Global �������� �ʱ�ȭ */
/* Stack �ʱ�ȭ */

#include "2450addr.inc"

	.equ _ISR_STARTADDRESS,		0x33ffff00

	/* The location of stacks
	 * stack layout design
	 * ����� ���Ƿ� ������ �°� �缳�� �Ǿ�� �Ѵ�
	 * ���� ����� ZI-LIMIT�� ������ ���� ���� ����
	 */
	
	.equ	STACK_BASE_ADDR, 0x33ff8000
	.equ	SVCStack,		(STACK_BASE_ADDR - 0x3800)
	.equ	UserStack,		(STACK_BASE_ADDR - 0x2800)
	.equ	UndefStack,		(STACK_BASE_ADDR - 0x2400)
	.equ	AbortStack,		(STACK_BASE_ADDR - 0x2000)
	.equ	IRQStack,		(STACK_BASE_ADDR - 0x1000)
	.equ	FIQStack,		(STACK_BASE_ADDR - 0x0)

	/* Exception ó���� ���Ͽ� �� ����� ������ �����Ͽ��� �Ѵ�
	 * �켱 �� ����� ������ �����ϱ� ���Ͽ��� ��� ���� ��ȯ�� �ʿ��ϴ�
	 * �̴� SVC������ �����Ͽ� �� ���� CPSR�� �����Ͽ� ��ȯ�Ѵ�
	 */
	.equ	Mode_USR,	0x10
	.equ	Mode_FIQ,	0x11
	.equ	Mode_IRQ,	0x12
	.equ	Mode_SVC,	0x13
	.equ	Mode_ABT,	0x17
	.equ	Mode_UND,	0x1b
	.equ	Mode_SYS,	0x1f
	.equ	Mode_MASK,	0x1f
	
	.equ	I_Bit,		0x80	/* when I bit is set, IRQ is disabled */
	.equ	F_Bit,		0x40	/* when F bit is set, FIQ is disabled */
	.equ	NOINT,		0xc0
	
	/* CONFIG_CLK_534_133_66	*/
	.equ	Startup_MDIV,		267
	.equ	Startup_PDIV,		3
	.equ	Startup_SDIV,		1
	.equ	Startup_EPLLSTOP,	0
	.equ	Startup_ARMCLKdiv,	0
	.equ	Startup_PREdiv,		0x1
	.equ	Startup_HCLKdiv,	0x1
	.equ	Startup_PCLKdiv,	1
	
	.equ	CLK_DIV_VAL,	((Startup_ARMCLKdiv<<9)|(Startup_PREdiv<<4)|(Startup_PCLKdiv<<2)|(Startup_HCLKdiv)|(1<<3))
	.equ	MPLL_VAL,	((Startup_EPLLSTOP<<24)|(Startup_MDIV<<14)|(Startup_PDIV<<5)|(Startup_SDIV))
	.equ	EPLL_VAL,	(32<<16)|(1<<8)|(2<<0)
	
	
	/* macro ���� */
	.macro HANDLER, HandlerLabel, HandleLabel
\HandlerLabel:
	sub		sp,sp,#4			/* decrement sp(to store jump address) */
	stmfd	sp!,{r0}			/* PUSH the work register to stack(lr doesn`t push because */ 
						   		/* it return to original address) */
	ldr		r0,=\HandleLabel	/* load the address of HandleXXX to r0 */
	ldr		r0,[r0]         	/* load the contents(service routine start address) of HandleXXX */
	str		r0,[sp,#4]      	/* store the contents(ISR) of HandleXXX to stack */
	ldmfd	sp!,{r0,pc}     	/* POP the work register and pc(jump to ISR) */
	.endm

	.text
	.globl _start
_start:

	/* ResetHandler�� ó������ ������ ���� �ƴ϶� 
	 * vector �ּ� �������� reset vector�� �����ؾ� �Ѵ�
	 * exception�� �߻��ϸ� ARM�� �ϵ���������� ���� �ּҷ� �б�ȴ�
	 */
	b	ResetHandler
	b	HandlerUndef			/* handler for Undefined mode */
	b	HandlerSWI				/* handler for SWI interrupt */
	b	HandlerPabort			/* handler for PAbort */
	b	HandlerDabort			/* handler for DAbort */
	b	.						/* reserved */
	b	HandlerIRQ				/* handler for IRQ interrupt */
	b	HandlerFIQ				/* handler for FIQ interrupt */

	.ltorg
HANDLER HandlerFIQ, HandleFIQ
HANDLER HandlerIRQ, HandleIRQ
HANDLER HandlerUndef, HandleUndef
HANDLER HandlerSWI, HandleSWI
HANDLER HandlerDabort, HandleDabort
HANDLER HandlerPabort, HandlePabort

	/* Reset Handler�� ���� */
	.globl ResetHandler
ResetHandler:
	ldr	r0,=WTCON			/* watch dog disable  */
	ldr	r1,=0x0         
	str	r1,[r0]

	ldr	r0,=INTMSK1
	ldr	r1,=0xffffffff		/* all interrupt disable */
	str	r1,[r0]
	ldr	r0,=INTMSK2
	ldr	r1,=0xffffffff
	str	r1,[r0]

	ldr	r0,=INTSUBMSK
	ldr	r1,=0x7fff			/* all sub interrupt disable */
	str	r1,[r0]

	/* Set clock PLL */	
	ldr	r0,=CLKDIV0CON			/* Configure Clock Divide 	for FCLK:HCLK:PCLK  */
	ldr	r1,=CLK_DIV_VAL
	str	r1,[r0]
	
	ldr	r0,=LOCKCON0		/* Set MPLL LockTime*/
	ldr	r1,=0xe10
	str	r1,[r0]

	ldr	r0,=MPLLCON			/* Configure MPLL */
	ldr	r1,=MPLL_VAL   		/* 532MHz @ 12MHz */
	str	r1,[r0]
	
	ldr	r0,=LOCKCON1		/*Set EPLL LockTime*/
	ldr	r1, =0x1780
	str	r1, [r0]
	
	ldr	r0,=EPLLCON			/*Set EPLL*/
	ldr	r1, =EPLL_VAL		
	str	r1, [r0]
	
	ldr	r0,=CLKSRCCON		/* set CLKSRC to fit MPLL and USB */
	ldr	r1, [r0]
	orr	r1, r1, #0x50
	str	r1, [r0]
	
	//	 Make LED1~4(GPG4~7) to LOW
		
	ldr r0, =GPGDAT 
	ldr r1, [r0]
	bic r1, r1, #0xF0
	str r1, [r0]

	//	Configure LED1~4(GPG4~7) as OUTPUT
	
	ldr r0, =GPGCON
	ldr r1, [r0]
	bic r1, r1, #0xFF00
	orr r1, r1, #0x5500
	str r1, [r0]
	
#if 1
	/* Set memory control registers */
    ldr	r0,=SMRDATA
	ldr	r1,=BANKCFG		/* BANKCFG Address (Start Address) */
	add	r2, r0, #20		/* End address of SMRDATA => total registers = 20/4 = 5�� */
0:
	ldr	r3, [r0], #4   	/* post increment */
	str	r3, [r1], #4  	/* post increment */
	cmp	r2, r0
	bne	0b
#endif
	/* C����� ���� ������ �����ϱ� ���� ��ƾ
	 * �⺻���� �޸� ������ �Ϸ�ǰ� �����Ͽ��� �Ѵ�
	 * stack�� ����
	 * �⺻������ C��� ���α׷����� ����ϴ� ������ �����Ǿ��
	 * �Լ�ȣ��� ���������� ����� �����ϴ�
	 */

	/* Initialize stacks */
	bl	InitStacks
	
#if 1    
	/* Copy and paste RW data zero initialized data */
	ldr	r0, =rodata_end_addr 	/* text_end_addr Get pointer to ROM data */
	ldr	r1, =data_start_addr 	/* bss_end_addr and RAM copy */
	ldr	r3, =data_end_addr  	/* bss_start_addr  */
	
	/* Zero init base => top of initialised data */
	cmp	r0, r1      		/* Check that they are different */
	beq	2f
1:       
	cmp	r1, r3      		/* Copy init data */
	ldrcc	r2, [r0], #4    /* --> LDRCC r2, [r0] + ADD r0, r0, #4 */         
	strcc	r2, [r1], #4    /* --> STRCC r2, [r1] + ADD r1, r1, #4 */
	bcc	1b
2:       
	mov	r2, #0 
    ldr r1,=bss_end_addr 
    ldr r3,=bss_start_addr
3:       
	cmp	r3, r1      /* Zero init */
	strcc	r2, [r3], #4
	bcc	3b
#endif    
	
	/* ���⼭ IRQ�� �߻��Ҷ� ������ ���� 
	 * IsrIRQ ��ƾ���� �б��ϱ� ���� ISR_VECTOR ������ �Ѵ� 
	 */

	/* Setup IRQ handler */
	ldr	r0,=HandleIRQ  
	ldr	r1,=IsrIRQ			
	str	r1,[r0]

	/* �����Ϸ��� startup ��ƾ�� ȣ���ϰų� Ȥ�� ������ Main�� ȣ��
	 * b Main�ϸ� C �Լ����� ���ƿ��� ������ �����ϴ� ���̸�
	 * bl �ϸ� �ǵ��� �ü��� ������ �����Ѵ�.
	 * ���� �ǷϾ� �� �� �� ���� ������ b %B0 �ϸ� ���� ������ �����ȴ�.
	 * �̶�, ����� �Ӻ���� �ý����� ���α׷� ���ᰡ ����� �ϹǷ�
	 * bl main�� �� ���ƿͼ� ���ѷ����� �������� �ʴ� ���� ������ �����ؾ� �Ѵ�
	 */
		
	bl	Main	
	b	.

	/* IRQ Handler ����
	 * IRQ�� �߻� �ҽ��� �پ��ϱ� ������ �ش� C��ƾ�� �ּҸ� ȹ���Ͽ��� �Ѵ�
	 */
	.globl IsrIRQ
IsrIRQ:
	sub	sp,sp,#4			/* reserved for PC */
	stmfd	sp!,{r8-r9}
	
	ldr	r9,=INTOFFSET1
	ldr	r9,[r9]
	ldr	r8,=HandleEINT0
	add	r8,r8,r9,lsl #2
	ldr	r8,[r8]
	str	r8,[sp,#8]
	ldmfd	sp!,{r8-r9,pc}	
	
	/* function initializing stacks */
InitStacks:
	/* Don't use DRAM,such as stmfd,ldmfd......
	 * SVCstack is initialized before
	 * Under toolkit ver 2.5, 'msr cpsr,r1' can be used instead of 'msr cpsr_cxsf,r1'
	 */
	mrs	r0,cpsr
	bic	r0,r0,#Mode_MASK
	orr	r1,r0,#Mode_UND|NOINT
	msr	cpsr_cxsf,r1		/* UndefMode */
	ldr	sp,=UndefStack
	
	orr	r1,r0,#Mode_ABT|NOINT
	msr	cpsr_cxsf,r1		/* AbortMode */
	ldr	sp,=AbortStack

	orr	r1,r0,#Mode_IRQ|NOINT
	msr	cpsr_cxsf,r1		/* IRQMode */
	ldr	sp,=IRQStack
    
	orr	r1,r0,#Mode_FIQ|NOINT
	msr	cpsr_cxsf,r1		/* FIQMode */
	ldr	sp,=FIQStack

	orr	r1,r0,#Mode_SYS|NOINT
	msr	cpsr_cxsf,r1		/* SystemMode */
	ldr	sp,=UserStack
	
	bic	r0,r0,#Mode_MASK|NOINT
	orr	r1,r0,#Mode_SVC
	msr	cpsr_cxsf,r1		/* SVCMode */
	ldr	sp,=SVCStack
	
	/* USER mode has not be initialized. */
	
	mov	pc,lr 
	/* The LR register won't be valid if the current mode is not SVC mode. */
	
	/* MEMORY ������ ���� Table
	 * �����ϰ��� �ϴ� �ý��ۿ� ���� ������ �޶�����
	 */
	
;	.pool
;	.align 2
	.ltorg
SMRDATA:
	.word	0x0004920d
	.word	0x04000040
	.word	0x00570039
	.word	0x80000030
	.word	0x00000313	


	/* C���� ó���� Exception�� Interrupt �Լ� �ּҸ� ������ ��ġ
	 * startup�� �� �ּҿ��� �Լ��ּҸ� ȹ���Ͽ� �б��Ѵ�.
	 */

/*
	//    	ALIGN

	//    	AREA RamData, DATA, READWRITE
	//        ^   _ISR_STARTADDRESS
//; .=0x33ffff00
//HandleReset 	#   4
//HandleUndef 	#   4
//HandleSWI		#   4
//HandlePabort    #   4
//; .=0x33ffff10
//HandleDabort    #   4
//HandleReserved  #   4
//HandleIRQ		#   4
//HandleFIQ		#   4
//
//; .=0x33ffff20
//;IntVectorTable
//HandleEINT0		#   4
//HandleEINT1		#   4
//HandleEINT2		#   4
//HandleEINT3		#   4
//; .=0x33ffff30
//HandleEINT4_7	#   4
//HandleEINT8_23	#   4
//HandleCAM		#   4
//HandleBATFLT	#   4
//; .=0x33ffff40
//HandleTICK		#   4
//HandleWDT		#   4
//HandleTIMER0 	#   4
//HandleTIMER1 	#   4
//; .=0x33ffff50
//HandleTIMER2 	#   4
//HandleTIMER3 	#   4
//HandleTIMER4 	#   4
//HandleUART2  	#   4
//; .=0x33ffff60
//HandleLCD 		#   4
//HandleDMA0		#   4
//HandleDMA1		#   4
//HandleDMA2		#   4
//; .=0x33ffff70
//HandleDMA3		#   4
//HandleMMC		#   4
//HandleSPI0		#   4
//HandleUART1	#   4
//; .=0x33ffff80
//HandleNFCON	#   4
//HandleUSBD		#   4
//HandleUSBH		#   4
//HandleIIC		#   4
//; .=0x33ffff90
//HandleUART0 	#   4
//HandleSPI1 		#   4
//HandleRTC 		#   4
//HandleADC 		#   4
//; .=0x33ffffa0
//
//	END 
*/

#if 1
	.ltorg

	.align 4
	.section int_vect, "w"

HandleReset:		.long   (_ISR_STARTADDRESS + 0x0)
HandleUndef:		.long   (_ISR_STARTADDRESS + 0x4)
HandleSWI:			.long   (_ISR_STARTADDRESS + 0x8)
HandlePabort:		.long   (_ISR_STARTADDRESS + 0xc)
HandleDabort:		.long   (_ISR_STARTADDRESS + 0x10)
HandleReserved:		.long   (_ISR_STARTADDRESS + 0x14)
HandleIRQ:			.long   (_ISR_STARTADDRESS + 0x18)
HandleFIQ:			.long   (_ISR_STARTADDRESS + 0x1c)

HandleEINT0:		.long	(_ISR_STARTADDRESS + 0x20)
#endif 