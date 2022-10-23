.text
.global _start
.org 0x0000
_start:
	movi 	sp, 0x7ffc
	movia r2, Z
	movia r3, X
	movia r4, Y
	ldw 	r5,	N(r0)
	
	call SelectLarger
	
_end:
	break

SelectLarger:
	subi	sp, sp, 32
	stw		ra, 28(sp)
	stw		r2, 24(sp)
	stw		r3, 20(sp)
	stw		r4, 16(sp)
	stw		r5, 8(sp)
	stw		r6, 4(sp)
	stw		r7, 0(sp)
	
lc_loop:
	ldw		r6, 0(r3)
	ldw		r7, 0(r4)
	
	bgt		r7, r6, lc_then
	stw		r6, 0(r2)
	br		lc_loop_end
	
lc_then:
	stw		r7, 0(r2)
	
lc_loop_end:
	addi	r2, r2, 4
	addi	r3, r3, 4
	addi	r4, r4, 4
	subi	r5, r5, 1
	bne 	r5, r0, lc_loop
	
lc_end:
	ldw		ra, 28(sp)
	ldw		r2, 24(sp)
	ldw		r3, 20(sp)
	ldw		r4, 16(sp)
	ldw		r5, 8(sp)
	ldw		r6, 4(sp)
	ldw		r7, 0(sp)
	addi	sp, sp, 32

	ret

.org 0x1000
X:		.word -9, 13, 0xFF, 167
Y:		.word 24, -5, 0, 192
Z: 		.skip 16
N:		.word 4
