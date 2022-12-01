#include "nios2_control.h"
#include "chario.h"
#include "adc.h"

/* place additional #define macros here */
#ifndef _TIMER_H_
#define _TIMER_H_

#define HEX_DISPLAY (volatile unsigned int *) 0x10000020

#define TIMER0_STATUS	((volatile unsigned int *) 0x10004000)
#define TIMER0_CONTROL	((volatile unsigned int *) 0x10004004)
#define TIMER0_START_LO	((volatile unsigned int *) 0x10004008)
#define TIMER0_START_HI	((volatile unsigned int *) 0x1000400C)

#define TIMER1_STATUS	((volatile unsigned int *) 0x10004020)
#define TIMER1_CONTROL	((volatile unsigned int *) 0x10004024)
#define TIMER1_START_LO	((volatile unsigned int *) 0x10004028)
#define TIMER1_START_HI	((volatile unsigned int *) 0x1000402C)

#define TIMER_TO_BIT 0x1


#endif /* _TIMER_H_ */

#define SWITCHES (volatile unsigned int *) 0x10000040

#define LEDS	((volatile unsigned int *) 0x10000010)



/* define global program variables here */
unsigned int HexTable[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F};
unsigned int LedsTable[] = {0x303, 0x186, 0xCC, 0x78, 0x30, 
	0x78, 0xCC, 0x186};
int leds_num;
unsigned int timer_0_flag;
unsigned int timer_1_flag;



/* place additional functions here */
void DisplayDigit(int which_hex_display, int value);
void handleTimer0();
void handleTimer1();

/*-----------------------------------------------------------------*/

/* this routine is called from the_exception() in exception_handler.c */

void interrupt_handler(void)
{
	unsigned int ipending;
	
	/* read current value in ipending register */
	ipending = NIOS2_READ_IPENDING();
	
	/* do one or more checks for different sources using ipending value */
	if ((ipending & 0x2000) == 0x2000)
	{
		*TIMER0_STATUS = 0;
		handleTimer0();
	}
	
	if ((ipending & 0x4000) == 0x4000)
	{
		*TIMER1_STATUS = 0;
		handleTimer1();
	}
	
	
	/* remember to clear interrupt sources */
}

/*-----------------------------------------------------------------*/

void Init (void)
{
	/* initialize software variables */
	leds_num = 0;

	/* set up each hardware interface */
	
	// TIMER0
	*TIMER0_START_LO = 6250000 & 0xFFFF;
	*TIMER0_START_HI = 6250000 >> 16;
	*TIMER0_CONTROL = 0x7;
	*TIMER0_STATUS = 0;
	
	// TIMER1
	*TIMER1_START_LO = 25000000 & 0xFFFF;
	*TIMER1_START_HI = 25000000 >> 16;
	*TIMER1_CONTROL = 0x7;
	*TIMER1_STATUS = 0;
	
	*LEDS = 0x303;

	/* set up ienable */
	NIOS2_WRITE_STATUS(1); // enables processor interrupts
	NIOS2_WRITE_IENABLE(0x6000); // enables IRQ 0, our timer

	/* enable global recognition of interrupts in procr. status reg. */
}

/*-----------------------------------------------------------------*/

int main (void)
{
	Init ();	/* perform software/hardware initialization */
	InitADC(2, 2);
	
	unsigned int potentReading;
	unsigned int switchReading;
	unsigned int charReading;
	
	PrintChar('\n');
	PrintString("ELEC 371 Lab 4 by Liyi Ma, Thomas Tesselaar, and Kevin Liang");
	PrintChar('\n');
	PrintString("Result from A/D conversion: 0x??");

	while (1)
	{
		/* fill in body of infinite loop */
		if (timer_0_flag)
		{
			timer_0_flag = 0;
			switchReading = *SWITCHES;
			DisplayDigit(3, (switchReading >> 7));
		}
		
		if (timer_1_flag)
		{
			timer_1_flag = 0;
			
			potentReading = ADConvert();
			
			PrintChar('\b');
			PrintChar('\b');
			
			PrintHexDigit((potentReading & 0xF0) >> 4);
			PrintHexDigit(potentReading & 0xF);
		}
		
		charReading = CheckChar();
		if (charReading != 0)
		{
			DisplayDigit(2, (charReading / 100) % 10);
			DisplayDigit(1, (charReading / 10) % 10);
			DisplayDigit(0, charReading % 10);
		}
	}

	return 0;	/* never reached, but main() must return a value */
}

/*-----------------------------------------------------------------*/

void DisplayDigit(int which_hex_display, int value) {
    unsigned int shift = 8 * which_hex_display;
    unsigned int display = *HEX_DISPLAY;
    display = display & (display ^ (0xFF << shift));
    *HEX_DISPLAY = display | (HexTable[value] << shift);
}

/* place additional functions here */
void handleTimer0()
{
	leds_num++;
	leds_num = leds_num % 8;
	*LEDS = LedsTable[leds_num];
	timer_0_flag = 1;
}

void handleTimer1()
{
	timer_1_flag = 1;
}

/*-----------------------------------------------------------------*/
/*              end of application-specific code                   */
/*-----------------------------------------------------------------*/



/*-----------------------------------------------------------------*/


/* 
   exception_handler.c

   This file is a portion of the original code supplied by Altera.

   It has been adapted by N. Manjikian for use in ELEC 371 laboratory work.

   Various unnecessary or extraneous elements have been excluded. For
   example, declarations in C for external functions called from asm()
   instructions are not required because any reference to external names
   in asm() instructions is embedded directly in the output written to
   the assembly-language .s file without any other checks by the C compiler.

   There is one particularly important change: on _reset_, the jump must be
   to the >> _start << location in order to properly initialize the stack
   pointer and to perform other crucial initialization tasks that ensure
   proper C semantics for variable initialization are enforced. The Altera
   version of the code jumped to main(), which will _not_ perform these
   crucial initialization tasks correctly.

   Finally, a reference to control register 'ctl4' in the asm() sequence
   has been replaced with the more meaningful alias 'ipending' for clarity.

   Other than the changes described above, the file contents have also been
   reformatted to fit in 80 columns of text, and comments have been edited.
*/


/* The assembly language code below handles processor reset */
void the_reset (void) __attribute__ ((section (".reset")));

/*****************************************************************************
 * Reset code. By giving the code a section attribute with the name ".reset" *
 * we allow the linker program to locate this code at the proper reset vector*
 * address. This code jumps to _startup_ code for C program, _not_ main().   *
 *****************************************************************************/

void the_reset (void)
{
  asm (".set noat");         /* the .set commands are included to prevent */
  asm (".set nobreak");      /* warning messages from the assembler */
  asm ("movia r2, _start");  /* jump to the C language _startup_ code */
  asm ("jmp r2");            /* (_not_ main, as in the original Altera file) */
}

/* The assembly language code below handles exception processing. This
 * code should not be modified; instead, the C language code in the normal
 * function interrupt_handler() [which is called from the code below]
 * can be modified as needed for a given application.
 */

void the_exception (void) __attribute__ ((section (".exceptions")));

/*****************************************************************************
 * Exceptions code. By giving the code a section attribute with the name     *
 * ".exceptions" we allow the linker program to locate this code at the      *
 * proper exceptions vector address. This code calls the interrupt handler   *
 * and later returns from the exception to the main program.                 *
 *****************************************************************************/

void the_exception (void)
{
  asm (".set noat");         /* the .set commands are included to prevent */
  asm (".set nobreak");      /* warning messages from the assembler */
  asm ("subi sp, sp, 128");
  asm ("stw  et, 96(sp)");
  asm ("rdctl et, ipending"); /* changed 'ctl4' to 'ipending' for clarity */
  asm ("beq  et, r0, SKIP_EA_DEC");   /* Not a hardware interrupt, */
  asm ("subi ea, ea, 4");             /* so decrement ea by one instruction */ 
  asm ("SKIP_EA_DEC:");
  asm ("stw	r1,  4(sp)"); /* Save all registers */
  asm ("stw	r2,  8(sp)");
  asm ("stw	r3,  12(sp)");
  asm ("stw	r4,  16(sp)");
  asm ("stw	r5,  20(sp)");
  asm ("stw	r6,  24(sp)");
  asm ("stw	r7,  28(sp)");
  asm ("stw	r8,  32(sp)");
  asm ("stw	r9,  36(sp)");
  asm ("stw	r10, 40(sp)");
  asm ("stw	r11, 44(sp)");
  asm ("stw	r12, 48(sp)");
  asm ("stw	r13, 52(sp)");
  asm ("stw	r14, 56(sp)");
  asm ("stw	r15, 60(sp)");
  asm ("stw	r16, 64(sp)");
  asm ("stw	r17, 68(sp)");
  asm ("stw	r18, 72(sp)");
  asm ("stw	r19, 76(sp)");
  asm ("stw	r20, 80(sp)");
  asm ("stw	r21, 84(sp)");
  asm ("stw	r22, 88(sp)");
  asm ("stw	r23, 92(sp)");
  asm ("stw	r25, 100(sp)"); /* r25 = bt (r24 = et, saved above) */
  asm ("stw	r26, 104(sp)"); /* r26 = gp */
  /* skip saving r27 because it is sp, and there is no point in saving sp */
  asm ("stw	r28, 112(sp)"); /* r28 = fp */
  asm ("stw	r29, 116(sp)"); /* r29 = ea */
  asm ("stw	r30, 120(sp)"); /* r30 = ba */
  asm ("stw	r31, 124(sp)"); /* r31 = ra */
  asm ("addi	fp,  sp, 128"); /* frame pointer adjustment */

  asm ("call	interrupt_handler"); /* call normal function */

  asm ("ldw	r1,  4(sp)"); /* Restore all registers */
  asm ("ldw	r2,  8(sp)");
  asm ("ldw	r3,  12(sp)");
  asm ("ldw	r4,  16(sp)");
  asm ("ldw	r5,  20(sp)");
  asm ("ldw	r6,  24(sp)");
  asm ("ldw	r7,  28(sp)");
  asm ("ldw	r8,  32(sp)");
  asm ("ldw	r9,  36(sp)");
  asm ("ldw	r10, 40(sp)");
  asm ("ldw	r11, 44(sp)");
  asm ("ldw	r12, 48(sp)");
  asm ("ldw	r13, 52(sp)");
  asm ("ldw	r14, 56(sp)");
  asm ("ldw	r15, 60(sp)");
  asm ("ldw	r16, 64(sp)");
  asm ("ldw	r17, 68(sp)");
  asm ("ldw	r18, 72(sp)");
  asm ("ldw	r19, 76(sp)");
  asm ("ldw	r20, 80(sp)");
  asm ("ldw	r21, 84(sp)");
  asm ("ldw	r22, 88(sp)");
  asm ("ldw	r23, 92(sp)");
  asm ("ldw	r24, 96(sp)");
  asm ("ldw	r25, 100(sp)");
  asm ("ldw	r26, 104(sp)");
  /* skip r27 because it is sp, and we did not save this on the stack */
  asm ("ldw	r28, 112(sp)");
  asm ("ldw	r29, 116(sp)");
  asm ("ldw	r30, 120(sp)");
  asm ("ldw	r31, 124(sp)");

  asm ("addi	sp,  sp, 128");

  asm ("eret"); /* return from exception */

  /* Note that the C compiler will still generate the 'standard'
     end-of-normal-function code with a normal return-from-subroutine
     instruction. But with the above eret instruction embedded
     in the final output from the compiler, that end-of-function code
     will never be executed.
   */ 
}