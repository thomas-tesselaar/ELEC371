#-----------------------------------------------------------------------------
# This template source file for ELEC 371 Lab 2 experimentation with interrupts
# also serves as the template for all assembly-language-level coding for
# Nios II interrupt-based programs in this course. DO NOT USE the approach
# shown in the vendor documentation for the DE0 Basic (or Media) Computer.
# The approach illustrated in this template file is far simpler for learning.
#
# Dr. N. Manjikian, Dept. of Elec. and Comp. Eng., Queen's University
#-----------------------------------------------------------------------------

	.text		# start a code segment (and we will also have data in it)

	.global	_start	# export _start symbol for linker 

#-----------------------------------------------------------------------------
# Define symbols for memory-mapped I/O register addresses and use them in code
#-----------------------------------------------------------------------------

# mask/edge registers for pushbutton parallel port

	.equ	BUTTON_MASK, 0x10000058
	.equ	BUTTON_EDGE, 0x1000005C

# pattern corresponding to the bit assigned to button1 in the registers above

	.equ	BUTTON, 0b10

# data register for LED parallel port

	.equ	LED_DATA, 0x10000010
	
# timer stuff
	
	.equ	TIMER_STATUS, 0x10002000
	.equ	TIMER_CONTROL, 0x10002004
	.equ	TIMER_START_LO, 0x10002008
	.equ	TIMER_START_HI, 0x1000200C

#-----------------------------------------------------------------------------
# Define two branch instructions in specific locations at the start of memory
#-----------------------------------------------------------------------------

	.org	0x0000 # this is the _reset_ address 
_start:
	br	main # branch to actual start of main() routine 

	.org	0x0020	# this is the _exception/interrupt_ address
 
	br	isr	# branch to start of interrupt service routine 
			#   (rather than placing all of the service code here) 

#-----------------------------------------------------------------------------
# The actual program code (incl. service routine) can be placed immediately
# after the second branch above, or another .org directive could be used
# to place the program code at a desired address (e.g., 0x0080). It does not
# matter because the _start symbol defines where execution begins, and the
# branch at that location simply forces execution to continue where desired.
#-----------------------------------------------------------------------------

main:
	movia   sp, 0x007FFFFC # initialize stack pointer

	call Init # call hw/sw initialization subroutine

	movia   r2, LAB2
    	call    PrintString

	mov     r2, r0 # perform any local initialization of gen.-purpose regs.
			#   before entering main loop 
			
	#Test place
	call UpperLEDs

main_loop:

	ldw     r2, COUNT(r0) # body of main loop (reflecting typical embedded
	addi    r2, r2, 1 #   software organization where execution does not
	stw     r2, COUNT(r0) #   terminate)

	br main_loop

#-----------------------------------------------------------------------------
# This subroutine should encompass preparation of I/O registers as well as
# special processor registers for recognition and processing of interrupt
# requests. Initialization of data variables in memory can also be done here.
#-----------------------------------------------------------------------------

Init:				# make it modular -- save/restore registers

	subi    sp, sp, 8 # body of Init() subroutine
	stw     r2, 4(sp)
	stw     r3, 0(sp)

	movia   r2, BUTTON_MASK
	movi    r3, BUTTON
	stwio   r3, 0(r2)
	
	movi	r2, 0x4C
	movia	r3, TIMER_START_HI
	stwio	r2, 0(r3)
	
	movi	r2, 0x4B40
	movia	r3, TIMER_START_LO
	stwio	r2, 0(r3)
	
	movi	r2, 0b111
	movia	r3, TIMER_CONTROL
	stwio	r2, 0(r3)

	movia   r2, 0b0011
	wrctl   ienable, r2
	movia   r2, 1
	wrctl   status, r2

	ldw     r2, 4(sp)
	ldw     r3, 0(sp)
	addi    sp, sp, 8

	ret

#-----------------------------------------------------------------------------
# The code for the interrupt service routine is below. Note that the branch
# instruction at 0x0020 is executed first upon recognition of interrupts,
# and that branch brings the flow of execution to the code below. Therefore,
# the actual code for this routine can be anywhere in memory for convenience.
# This template involves only hardware-generated interrupts. Therefore, the
# return-address adjustment on the ea register is performed unconditionally.
# Programs with software-generated interrupts must check for hardware sources
# to conditionally adjust the ea register (no adjustment for s/w interrupts).
#-----------------------------------------------------------------------------

isr:
	subi    sp, sp, 12 # save register values, except ea which
	stw		ra, 8(sp)
	stw     r2, 4(sp)		# must be modified for hardware interrupts
	stw     r3, 0(sp)
	

	subi	ea, ea, 4	# ea adjustment required for h/w interrupts

	rdctl   r2, ipending # body of interrupt service routine
				#   (use the proper approach for checking
				#    the different interrupt sources)
	andi    r3, r2, 2
	beq     r3, r0, nobutton
	movia   r2, LED_DATA
	ldwio   r3, 0(r2)
	xori    r3, r3, 0b0000011111
	stwio   r3, 0(r2)
	movia   r2, BUTTON_EDGE
	movia   r3, 0xF
	stwio   r3, 0(r2)

nobutton:
	andi    r3, r2, 1
	beq     r3, r0, notimer
	
	movia   r2, TIMER_STATUS
	movia   r3, 0b0
	stwio   r3, 0(r2)
	
	call	UpperLEDs
	
notimer:
	
	ldw		ra, 8(sp)
	ldw		r2, 4(sp) # restore register values
	ldw		r3, 0(sp)
	addi	sp, sp, 12
	
	eret			# interrupt service routines end _differently_
				#   than subroutines; execution must return to
				#   to point in main program where interrupt
				#   request invoked service routine
	
#-----------------------------------------------------------------------------

PrintChar:
    subi    sp, sp, 12
    stw     ra, 8(sp)
    stw     r3, 4(sp)
    stw     r4, 0(sp)
    movia   r3, 0x10001000
pc_loop:
    ldwio   r4, 4(r3)
    andhi   r4, r4, 0xFFFF
    beq     r4, r0, pc_loop
    stwio   r2, 0(r3)
    ldw     ra, 8(sp)
    ldw     r3, 4(sp)
    ldw     r4, 0(sp)
    addi    sp, sp, 12
    ret

PrintString:
    subi    sp, sp, 12
    stw     ra, 8(sp)
    stw     r3, 4(sp)
    stw     r4, 0(sp)
    mov     r4, r2
ps_loop:
    ldb		r2, 0(r4)
    beq     r2, r0, ps_end_loop
    call    PrintChar
    addi    r4, r4, 1
    beq     r0, r0, ps_loop
ps_end_loop:
    ldw     ra, 8(sp)
    ldw     r3, 4(sp)
    ldw     r4, 0(sp)
    addi    sp, sp, 12
    ret
	
#-----------------------------------------------------------------------------
# UpperLEDs modular code ;)
#-----------------------------------------------------------------------------

UpperLEDs:
	subi	sp, sp, 16
	stw		ra, 12(sp)
	stw		r2, 8(sp)
	stw		r3, 4(sp)
	stw		r4, 0(sp)
	
	movia   r2, LED_DATA
	ldwio   r3, 0(r2)
	mov		r4, r3
	andi	r3, r3, 0b1111100000
	
	xori	r3, r3, 0b1111111111
	slli	r3, r3, 1
	andi	r3, r3, 0b1111100000
	xori	r3, r3, 0b1111100000
	andi	r4, r4, 0b0000011111
	or		r3, r3, r4
	
ul_if:
	bne		r3, r4, ul_then
	movi	r3, 0b100000
	or		r3, r3, r4
	
ul_then:
	stwio   r3, 0(r2)
	
	ldw		ra, 12(sp)
	ldw		r2, 8(sp)
	ldw		r3, 4(sp)
	ldw		r4, 0(sp)
	addi	sp, sp, 16
	ret

#-----------------------------------------------------------------------------
# Definitions for program data, incl. anything shared between main/isr code
#-----------------------------------------------------------------------------

	.org	0x1000 # start should be fine for most small programs
COUNT: .word 0 # define/reserve storage for program data
LAB2: .asciz "ELEC371 Lab 2 by Kevin Liang, Liyi Ma, and Thomas Tesselaar \n"		
	.end
